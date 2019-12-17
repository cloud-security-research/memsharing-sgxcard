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
 
#ifndef _VCA_COM_H
#define _VCA_COM_H

#ifdef __cpluscplus
extern "C" {
#endif

#include <vca_com_ds.h>
#include <vca_mem.h>

#define VCA_COM_MAX_CHANNELS MAX_CHANNELS

  // initialize communication to host specified by ip and port
  // Communcation over MAX_CHANNELS channels in parallel
  // returns 0 on success
  int init_vca_com(vca_com_t * com, 
		   const char * host_ip, 
		   const char * host_port, 
		   vca_com_addr * self,
		   vca_com_type type);

  int init_vca_com_repeat(vca_com_t * com,
		   const char * host_ip,
		   const char * host_port);

  // disconnect from host and free memory of com
  // returns 0 on success
  int deinit_vca_com(vca_com_t * com);

  // sends msg on com without adding any header
  // is mainly used in gateway implementaiton
  // which doesn't alter the message (only passthrough)
  int vca_com_send_hdrless_msg(vca_com_t * com,
			       char * msg,
			       unsigned long long length,
			       unsigned int c);

  // send message msg to dst via host com on channel c
  // returns -1 or number of bytes send otherwise
  int vca_com_send_msg(vca_com_t * com, 
		       vca_com_addr * dst,
		       char * msg,
		       unsigned long long length,
		       unsigned int c);

  // receive next message from src via com on channel c with max length
  // returns -1 on failure or number of bytes received otherweise
  int vca_com_recv_msg(vca_com_t * com, 
		       vca_com_addr * src, 
		       char * msg,
		       unsigned long long * length,
		       unsigned int * c);

#ifdef COML_DBG
#define COML_DBM(...)                         \
  do {                                        \
  fprintf(stderr, __VA_ARGS__);             \
  fprintf(stderr, "\n");                    \
  } while(0)
#else // disable debug
#define COML_DBM(...)
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif //!_VCA_COM_H
