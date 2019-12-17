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

#ifndef _HOST_HOST_GATEWAY_H_
#define _HOST_HOST_GATEWAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <vca_com_ds.h>

#define HHG_MAX_CONCURRENT_CONNECTIONS 1024

  typedef struct {
    void * zmq_ctx;
    void * zmq_socket;
    
    uint8_t * msg_buf;

    vca_com_addr self;
  } vca_com_hhg_t;
  
  // starts accepting connections on ip/port from other hosts
  // stores connections in hosts
  int vca_com_hhg_init(vca_com_hhg_t * hhg,
		       const char * ip,
		       const char * port);

  int vca_com_hhg_deinit(vca_com_hhg_t * hhg);

  int vca_com_hhg_accept_and_deliver(vca_com_hhg_t * hhg);

  int vca_com_hhg_create_com(vca_com_hhg_t * hhg,
			     vca_com_addr * dest,
			     vca_com_t ** com);
   

#ifdef __cplusplus
}
#endif

#endif //!_HOST_HOST_GATEWAY_
