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

#ifndef _HOST_GATEWAY_H_
#define _HOST_GATEWAY_H_

#ifdef __cplusplus
extern "C" {
#endif

  #include <host-node-gateway.h>
  #include <host-host-gateway.h>

  extern vca_com_hng_t hng;
  extern vca_com_hhg_t hhg;

#ifdef __cplusplus
}
#endif


#endif // _HOST_GATEWAY_H_
