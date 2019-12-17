/*
 * Copyright 2019 Intel(R) Corporation (http://www.intel.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * vca_com.c
 *
 * Implementation of the vca communication library interface
 * for both applications running on vca cards and non-vca hosts.
 * Clients are either connected via memory sharing (for vca nodes)
 * or libzmq sockets to the host gateway. The host gateway routes
 * all incomming messages to the respective vca cards directly via 
 * memory sharing, if the host is the same, or libzmq sockets.
 *
 * General Interface Lifecycle:
 * 1) init_vca_com to initialize the connection to the host gateway
 * 2) vca_com_send/recv_msgs to send/recv messages to/from the host gateway
 * 3) deinit_vca_com gracefully disconnects the connection to the host gateway
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vca_com.h>
#include <zmq.h>

void * vca_com_zmq_ctx = NULL;

static void* init_zmq_socket_to_host(const char * host_ip,
			    const char * host_port) {
  // setup zmq socket and store it in com->com
  vca_com_zmqs * zmqs = malloc(sizeof(vca_com_zmqs));
  void * socket = NULL;
  int rc = 0;
  char endpoint[256];

  memset(&zmqs->id[0], 0, sizeof(char) * VCA_COM_ZMQ_ID_SIZE);
  zmqs->id_size = 256;

  if(vca_com_zmq_ctx == NULL) {
    vca_com_zmq_ctx = zmq_ctx_new();
  }

  socket = zmq_socket(vca_com_zmq_ctx, ZMQ_STREAM);

  // prepare endpoint format in zmq language
  if(strnlen(host_ip, 256) + strnlen(host_port, 256) + 7 < 255) {
    snprintf(endpoint, 256, "tcp://%s:%s", host_ip, host_port);
  }

  COML_DBM("connecting to %s", endpoint);

  rc = zmq_connect(socket, endpoint);

  if(rc != 0) {
    free(zmqs);
    return NULL;
  }
  
  zmqs->socket = socket;
  // get id of socket
  rc = zmq_getsockopt(socket, ZMQ_IDENTITY, (void *) &(zmqs->id[0]), &(zmqs->id_size));
  if(rc != 0 || zmqs->id_size > VCA_COM_ZMQ_ID_SIZE) {
    perror("failed to get id");
    free(zmqs);
    return NULL;
  }

  return zmqs;
}

static void deinit_zmq_socket_to_host(void * s) {

  vca_com_zmqs * zmqs = (vca_com_zmqs*) s;
  int rc = 0;

  if(!zmqs || !zmqs->socket) 
    return;

  rc = zmq_send(zmqs->socket, zmqs->id, zmqs->id_size, ZMQ_SNDMORE);
  if(rc < 0) {
    COML_DBM("Failed to close zmq socket send id");
    perror("failed to close zmq");
    return;
  }
  
  rc = zmq_send(zmqs->socket, "", 0, ZMQ_SNDMORE);
  if(rc < 0) {
    COML_DBM("Failed to close zmq socket send null");
    perror("failed to close zmq socket");
    return;
  }  

  zmq_close(zmqs->socket);

  free(zmqs);

  if(vca_com_zmq_ctx) {
    void * tmp = vca_com_zmq_ctx;
    vca_com_zmq_ctx = NULL;
    zmq_ctx_destroy(tmp);
  } 
  
  return;
}

// initialize communication to host specified by ip and port
int init_vca_com(vca_com_t * com, 
		 const char * host_ip, 
		 const char * host_port, 
		 vca_com_addr * self,
		 vca_com_type type) {
  
  int socket = -3; // with host publishing socketid

  if(!com || !host_ip || !host_port) {
    return -1;
  }
  
  // initialize com data structure
  com->com = NULL;
  com->type = type;
  vca_com_cpy_addr(self, &com->self);

  // connect to host via memsharing library
  switch (type) {
  case VCA_COM_MEM_SHARING:
    com->com = init_vca_task_system(host_ip, host_port, &socket);
    break;
  case VCA_COM_MEM_SHARING_HOST:
    com->com = init_host_task_system(com->com, host_ip, host_port, &socket);
    break;
  case VCA_COM_ZMQ_SOCKET: 
    {
      vca_com_addr dst;
      memset(&dst, 0, sizeof(vca_com_addr));
      com->com = init_zmq_socket_to_host(host_ip, host_port);
      // send initial msg to establish socket
      vca_com_send_msg(com, &dst, NULL, 0, 0);
      break;
    }
  default:
    return -1;
  }
    
  return (com->com) ? 0 : -1;
}

int init_vca_com_repeat(vca_com_t * com,
			const char * host_ip,
			const char * host_port) {
  int socket = -3;
  
  if(!com || !host_ip || !host_port || !com->com) {
     return -1;
  }

  switch (com->type) {
  case VCA_COM_MEM_SHARING_HOST:
     (void) init_host_task_system(com->com, host_ip, host_port, &socket);
     break;
  default: // should only be used for hosts with multiple connections to VCA cards
     return -1;
  }

  return 0;
}

// disconnect from host and free memory of com
int deinit_vca_com(vca_com_t * com) {
  
  if(!com) {
    return -1;
  }

  switch (com->type) {
  case VCA_COM_MEM_SHARING:
  case VCA_COM_MEM_SHARING_HOST:
    deinit_vca_task_system(com->com);   
    break;
  case VCA_COM_ZMQ_SOCKET: 
    deinit_zmq_socket_to_host(com->com);
    break;
  default:
    return -1;
  }

  return 0;
}

int vca_com_send_hdrless_msg(vca_com_t * com,
			     char * msg,
			     unsigned long long length,
			     unsigned int c) {

  switch (com->type) {
  case VCA_COM_MEM_SHARING:
  // send msg through com round robin over channels
    vca_submit_task(com->com, length, msg, c);
    break;
  case VCA_COM_MEM_SHARING_HOST:
    host_submit_task(com->com, length, msg, com->self.socket);
    break;
  case VCA_COM_ZMQ_SOCKET:
    {
      vca_com_zmqs * zmqs = (vca_com_zmqs*) com->com;
      int rc = 0;
      //zmq_sock_send
      rc = zmq_send(zmqs->socket, zmqs->id, zmqs->id_size, ZMQ_SNDMORE);
      if(rc < zmqs->id_size) {
	perror("failed to send id for msg");
	return rc;
      }
      rc = zmq_send(zmqs->socket, msg, length, ZMQ_SNDMORE);
      if(rc < length) {
	perror("failed to send msg");
	return rc;
      }
      break;
    }
  default:
    return -1;
  }

  return 0;
}

// send message msg to dst via com
int vca_com_send_msg(vca_com_t * com, 
		     vca_com_addr * dst, 
		     char * msg,
		     unsigned long long length,
		     unsigned int c) {
  
  char sspace[4096];
  void * buffer = (void*) sspace;
  vca_com_msg_hdr * hdr = NULL;
  unsigned long long full_length = length + sizeof(vca_com_msg_hdr);
  int ret = 0;

  // for short messages rely on the stack allocated array, otherwise default ot dyn. mem alloc
  if(full_length > 4096) {
    buffer = malloc(full_length);
  }
  
  // setup msg hdr hdr
  hdr = vca_com_msg_get_hdr(buffer);
  memset(hdr, 0, sizeof(vca_com_msg_hdr));
  vca_com_cpy_addr(&com->self, &hdr->src);
  vca_com_cpy_addr(dst, &hdr->dst);
  hdr->length = length;

  // copy message
  if(length > 0) {
    memcpy(vca_com_msg_get_msg(buffer), msg, length);
  }

  ret = vca_com_send_hdrless_msg(com, buffer, full_length, c);
  
  // free dyn. allocated msg after sending
  if(buffer != sspace) {
    free(buffer);
  }

  return ret;
}

// receive next message from src via com on channel c
int vca_com_recv_msg(vca_com_t * com, 
		     vca_com_addr * src, 
		     char * msg,
		     unsigned long long * length,
		     unsigned int * c) {
  
  char sspace[4096];
  void * buffer = (void*) sspace;
  unsigned long long task_len = 0;
  vca_com_msg_hdr * hdr = NULL;
  int rc = 0;


  if(!com || !src || !msg || !c || length == 0) {
    return -1;
  }

  if (*length > 4096) {
    buffer = malloc(*length + sizeof(vca_com_msg_hdr));
  }

  switch(com->type) {
  case VCA_COM_MEM_SHARING:
     rc = vca_recv_task(com->com, (long*) &task_len, buffer, *c); 
     break;
  case VCA_COM_MEM_SHARING_HOST:
     rc = host_recv_task(com->com, (long*) &task_len, buffer, (int*) c);
     break;
  case VCA_COM_ZMQ_SOCKET:
    {
      vca_com_zmqs * zmqs = (vca_com_zmqs*)com->com;
      char tmpid[sizeof(zmqs->id)];
      do {
        rc = zmq_recv(zmqs->socket, tmpid, sizeof(tmpid), 0);
        if(rc < 0 || memcmp(zmqs->id, tmpid, zmqs->id_size))
  	  break;
        rc = zmq_recv(zmqs->socket, buffer, *length, 0);
        if(rc < 0)
	  break;
        task_len = (unsigned long long) rc; // set length of request
      } while (rc < 1); // to step over connection establishment
      if (rc > 0)
	rc = 0; // to emulate shared memory return semantics
      break;
    }
  }
 
  if(rc == 0) {
    // received a message
    hdr = vca_com_msg_get_hdr(buffer);
    vca_com_cpy_addr(&hdr->src, src);

    // check that task fits
    if(*length < task_len) {
	return -1;
    }
    
    *length = task_len - sizeof(vca_com_msg_hdr);
    memcpy(msg, vca_com_msg_get_msg(buffer), *length);
  }

  if(buffer != sspace) {
    free(buffer); // free if previously allocated
  }

  return rc;
}
