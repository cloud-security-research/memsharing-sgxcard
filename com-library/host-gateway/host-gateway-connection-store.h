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
 
#ifndef _HOST_GATEWAY_CONNECTION_STORE_H_
#define _HOST_GATEWAY_CONNECTION_STORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <vca_com_ds.h>

#define CUCKOO_TABLE_NAME vca_com_cons_table
#define CUCKOO_KEY_TYPE vca_com_addr
#define CUCKOO_MAPPED_TYPE vca_com_t *

#include <cuckoo_table_template.h>

  extern vca_com_cons_table * connection_store;

#ifdef __cplusplus
}
#endif

#endif
