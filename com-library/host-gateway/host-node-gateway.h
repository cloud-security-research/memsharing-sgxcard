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
 *
 * host-node-gateway.h
 *
 * Interface to communicate between nodes of the same host.
 * 
 */

#ifndef _HOST_NODE_GATEWAY_H
#define _HOST_NODE_GATEWAY_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include <vca_com_ds.h>
#include <pthread.h>
    
  typedef struct {
    
    void * vca_task_opq;

    unsigned int num_sockets;
    const char * ip;
    const char * port;
    vca_com_addr self;

    unsigned int * active_sockets;
    unsigned int num_active;

    // protects active_sockets and num_active
    pthread_rwlock_t lock;

    char * msg_buffer;

  } vca_com_hng_t;

  // spins on the spins on dequeue of all known sockets
  // and delivers messages to other sockets or TCP deliver queue
  int vca_com_hng_spin_and_deliver(vca_com_hng_t * hng);  
  
  // accepts new connections via libzmq control planer interface
  int vca_com_hng_accept_new_nodes(vca_com_hng_t * hng);

  // initializes hng data structure to safe values
  int vca_com_hng_init(vca_com_hng_t * hng, unsigned int num_sockets, const char * ip, const char * port);

  // set host port instead of node port
  void vca_com_hng_set_global_host_port(vca_com_hng_t * hng, const char * port);

  // frees all allocated data structures
  int vca_com_hng_deinit(vca_com_hng_t * hng);
  
#ifdef __cplusplus
}
#endif

#endif //!_HOST_NODE_GATEWAY_H
