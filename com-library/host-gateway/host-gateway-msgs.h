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

#ifndef _HOST_GATEWAY_MSGS_H_
#define _HOST_GATEWAY_MSGS_H_

#ifdef __cplusplus
extern "C" {
#endif

  #include <vca_com_ds.h>

  #define MAX_MSG_SIZE 1024*1024

  int vca_com_hng_dst_is_node_on_same_host(vca_com_addr * dst, vca_com_addr * src);

  int vca_com_gate_deliver_msg(char * msg, unsigned long long len, vca_com_addr * self);

  int vca_com_gate_deliver_failure(vca_com_t * com, char * msg, unsigned long long len);

#ifdef __cplusplus
    }
#endif

#endif // HOST_GATEWAY_MSGS
