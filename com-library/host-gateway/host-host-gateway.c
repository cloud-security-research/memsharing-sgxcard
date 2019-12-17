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
 */
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <host-host-gateway.h>
#include <host-gateway-connection-store.h>
#include <host-gateway-msgs.h>
#include <host-gateway.h>
#include <vca_com.h>
#include <vca_mem.h>

int vca_com_hhg_init(vca_com_hhg_t * hhg,
		     const char * ip,
		     const char * port) {

  char ipname[256];
  int rc = -1;

  hhg->zmq_ctx = zmq_ctx_new();
  hhg->zmq_socket = zmq_socket(hhg->zmq_ctx, ZMQ_STREAM); 

  (void) vca_com_init_addr_from_string(&hhg->self, "0.0.0.0", "0", ip, port, "0");
  
  if(strnlen(ip, 256) + strnlen(port, 256) + 8 < 255) {
    snprintf(ipname, 256, "tcp://*:%s", port);
    rc = zmq_bind(hhg->zmq_socket, ipname);
    if(rc == -1) {
      perror("socket bind failed for hhg");
      return rc;
    }
  }

  COML_DBM("Listening to %s", ipname);

  hhg->msg_buf = malloc(MAX_MSG_SIZE);

  return rc;
}

int vca_com_hhg_deinit(vca_com_hhg_t * hhg) {
  
  if(!hhg)
    return -1;

  if(hhg->zmq_socket)
    zmq_close(hhg->zmq_socket);

  if(hhg->zmq_ctx)
    zmq_ctx_destroy(hhg->zmq_socket);

  if(hhg->msg_buf)
    free(hhg->msg_buf);

  return 0;
}

static void create_zmq_vca_com(vca_com_t ** com, uint8_t * id, 
			       int id_size,
			       vca_com_addr * self,
			       void * socket) {	
  vca_com_zmqs * zmqs = NULL;

  (*com) = malloc(sizeof(vca_com_t));
  zmqs = malloc(sizeof(vca_com_zmqs));
  memset(zmqs->id, 0, VCA_COM_ZMQ_ID_SIZE);
  zmqs->id_size = id_size > VCA_COM_ZMQ_ID_SIZE ? VCA_COM_ZMQ_ID_SIZE : id_size;
  memcpy(zmqs->id, id, zmqs->id_size);
  zmqs->socket = socket;
  (*com)->com = zmqs;
  vca_com_cpy_addr(self, &((*com)->self));
  (*com)->type = VCA_COM_ZMQ_SOCKET;
  
  COML_DBM("created zmq socket for %hhu.%hhu.%hhu.%hhu:%hu", (*com)->self.host[0],
	   (*com)->self.host[1], (*com)->self.host[2], (*com)->self.host[3], (*com)->self.host_port);
  
  return;
}

static void store_id_of_client(vca_com_hhg_t * hhg, 
			       uint8_t * id,
			       int id_size,
			       void * socket) {

  vca_com_t * com = NULL;
  // first store the id, if not exists
  vca_com_msg_hdr * hdr = (vca_com_msg_hdr *)hhg->msg_buf;

  COML_DBM("msg from %hhu.%hhu.%hhu.%hhu:%hu:%hu type %d", hdr->src.host[0],
	hdr->src.host[1], hdr->src.host[2], hdr->src.host[3], hdr->src.host_port, hdr->src.socket,
	hdr->type);
  
  if(!vca_com_cons_table_find(connection_store, &hdr->src, &com)) {
    // create connection for socket to client
    create_zmq_vca_com(&com, (uint8_t *) id, id_size, &hdr->src, socket);
    
    // store new socket id in connection store
    vca_com_cons_table_insert(connection_store, &com->self, &com); 
  } else {
    // check that it is the same id
    if(memcmp(((vca_com_zmqs*)com->com)->id, id, VCA_COM_ZMQ_ID_SIZE)) { // something went wrong new ID for same node
      COML_DBM("found mismatching ID, will reinsert");
      vca_com_cons_table_erase(connection_store, &hdr->src);
      
      create_zmq_vca_com(&com, (uint8_t *) id, id_size, &hdr->src, socket);
      
      vca_com_cons_table_insert(connection_store, &com->self, &com);
      // updated id for the same node
      }
  }
}

int vca_com_hhg_accept_and_deliver(vca_com_hhg_t * hhg) {
  
  int rc = 0;
  uint8_t id[VCA_COM_ZMQ_ID_SIZE];
  int id_size = 0;

  
  if(!hhg && hhg->zmq_socket && hhg->msg_buf)
    return -1;
  
  do {
    
    // get id
    rc = zmq_recv(hhg->zmq_socket, id, VCA_COM_ZMQ_ID_SIZE, ZMQ_DONTWAIT);
//    COML_DBM("recv id rc=%d", rc);
    if(rc > 0) { // recv an id
      id_size = rc;

      COML_DBM("recv an id %s", id);

      rc = zmq_recv(hhg->zmq_socket, hhg->msg_buf, MAX_MSG_SIZE, ZMQ_DONTWAIT);
      if(rc >= sizeof(vca_com_msg_hdr)) { // recv a msg with content
        COML_DBM("recv a msg size %d", rc);
    
        // store ID of client for future responses
        store_id_of_client(hhg, (uint8_t *) id, id_size, hhg->zmq_socket);
	
        // deliver msg (if content exists
        if (rc > sizeof(vca_com_msg_hdr)) {
          // handle empty and bulked up msgs
          vca_com_msg_hdr * hdr = vca_com_msg_get_hdr((char *) hhg->msg_buf);
          int hdrId = 0;
          while((hdr + hdrId)->length == 0 && hdrId * sizeof(vca_com_msg_hdr) < rc) {
            hdrId++;
          }

          if(rc > hdrId * sizeof(vca_com_msg_hdr)) // ensure that there is a msg to deliver
            vca_com_gate_deliver_msg((char *) (hdr + hdrId), rc + 
                                  hdrId * sizeof(vca_com_msg_hdr) , &hhg->self);
        }

      } else {
        if(rc == -1) { // failure in recv
            perror("recv msg of zmq socket failed:");
        } else if (rc == 0) { // msg to indicate
          COML_DBM("new connection or connection close request");
          // TODO handle extenral requests to close connection
        } else {
          COML_DBM("wrong message length (less than hdr and not defined as error)");
        }
      }
    } else {
      //      perror("no msg");
      //COML_DBM("nothing received on socket");
      if(rc == -1 && errno != EAGAIN) {
      	perror("something went wrong");
      }
    }
  } while(rc != -1);
  
  return 0;
}

int vca_com_hhg_create_com(vca_com_hhg_t * hhg,
			   vca_com_addr * dest,
			   vca_com_t ** com) {
  
  int rc = 0;
  char endpoint[256];
  size_t id_size = 0;

  if(!hhg || !dest || !*com) {
    return -1;
  }

  snprintf(endpoint, 256, "tcp://%hhu.%hhu.%hhu.%hhu:%hu", dest->host[0],
	  dest->host[1], dest->host[2], dest->host[3], dest->host_port);

  // connect to dest
  rc = zmq_connect(hhg->zmq_socket, endpoint);

  if(rc != 0) {
    return -1;
  }
  
  // get id of socket
  zmq_getsockopt(hhg->zmq_socket, ZMQ_IDENTITY, endpoint, &id_size);
  if(id_size > sizeof(char) * 256)
    return -1;

  // push com into the connection store
  store_id_of_client(hhg, (uint8_t *) endpoint, id_size, hhg->zmq_socket);
  
  return 0;
}
