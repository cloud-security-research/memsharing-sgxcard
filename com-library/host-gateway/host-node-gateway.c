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

#include <vca_com.h>
#include <vca_mem.h>
#include <host-node-gateway.h>
#include <host-gateway.h>
#include <host-gateway-connection-store.h>
#include <host-gateway-msgs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int vca_com_hng_spin_and_deliver(vca_com_hng_t * hng) {

  unsigned int s = 0, c = 0;
  long task_len = 0;
  
  if(!hng || !hng->vca_task_opq) {
    //COML_DBM("com not initialzed");
    return -1;
  }

  if(!hng->msg_buffer) {
    hng->msg_buffer = malloc(MAX_MSG_SIZE);
  }

  pthread_rwlock_rdlock(&hng->lock);

  for(s = 0; s < hng->num_active; s++) {
    for(c = 0; c < MAX_CHANNELS; c++) {
      
      if(common_recv_task(hng->vca_task_opq, &task_len, (void*) hng->msg_buffer, c, hng->active_sockets[s])
	 == 0) {
	// received msg

	// set src to host ip
	vca_com_msg_hdr * hdr = (vca_com_msg_hdr *) hng->msg_buffer;
  memcpy(hdr->src.host, hng->self.host, sizeof(char) * 4);
	hdr->src.host_port = hng->self.host_port;
	
	if(vca_com_gate_deliver_msg(hng->msg_buffer, task_len, &hng->self)) {
	  COML_DBM("Failed to deliver msg or failure to either src or dst");
	}
      }
      // continue with next channel
    }
    // continue with next socket
  }

  pthread_rwlock_unlock(&hng->lock);

  return 0;
}

int vca_com_hng_accept_new_nodes(vca_com_hng_t * hng) {
  unsigned int i = 0;
  int socket = -2;
  vca_com_addr addr;
  vca_com_t * com = NULL;
    
  if(!hng)
    return -1;

  for(i = 0; i< hng->num_sockets; i++) {    
    socket = -2;

    COML_DBM("Waiting for next node connection...");
    hng->vca_task_opq = init_host_task_system(hng->vca_task_opq, "*", hng->port, &socket);
    COML_DBM("Accepted node connection %d on socket %d", i, socket);

    // add socket into host-gateway-connection-store
    // addr is key to connection store
    vca_com_cpy_addr(&hng->self, &addr);
    addr.socket = socket;

    // com is value in map
    com = malloc(sizeof(vca_com_t));
    com->self = addr;
    com->com = (hng->vca_task_opq);
    com->type = VCA_COM_MEM_SHARING_HOST;
    
    vca_com_cons_table_insert(connection_store, &addr, &com);

    // insert into active set
    pthread_rwlock_wrlock(&hng->lock);
    hng->active_sockets[hng->num_active++] = socket;
    pthread_rwlock_unlock(&hng->lock);

    COML_DBM("Accepted mem sharing to node %d", socket);
  } 

  COML_DBM("Finished accepting connections to nodes");
  
  return 0;
}

void vca_com_hng_set_global_host_port(vca_com_hng_t * hng, const char * port) {
  if(!hng || ! port)
    return;

  hng->self.host_port = (unsigned short) strtoul(port, NULL, 0);
}

int vca_com_hng_init(vca_com_hng_t * hng, unsigned int num_sockets, const char * ip,
		     const char * port) {

  if (!hng || num_sockets < 1) 
    return -1;

  hng->num_sockets = num_sockets;
  hng->vca_task_opq = NULL;
  hng->ip = ip;
  hng->port = port;
  vca_com_init_addr_from_string(&hng->self, "0.0.0.0", "0", ip, port, "0");
  hng->active_sockets = malloc(sizeof(unsigned int) * num_sockets);
  memset(hng->active_sockets, 0, sizeof(unsigned int) * num_sockets);
  hng->num_active = 0;

  pthread_rwlock_init(&hng->lock, NULL);
  
  return 0;
}

int vca_com_hng_deinit(vca_com_hng_t * hng) {

  if(!hng)
    return -1;

  deinit_vca_task_system(hng->vca_task_opq);

  return 0;
}
