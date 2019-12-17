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

#include <host-host-gateway.h>

#include <cstring>
#include <functional>

#include <host-gateway-connection-store.h>

// functions required by hash function

namespace std {
  template <> struct hash<vca_com_addr> {
    size_t operator()(const vca_com_addr &addr) const { 
      size_t result = 2166136261;
      const unsigned char * it = addr.gateway;
      for (; it < addr.gateway + sizeof(vca_com_addr); ++it) {
	result = (result * 16777619) ^ (*it);
      }
 
      return result;
    }
  };

  template <> struct equal_to<vca_com_addr> {
    bool operator()(const vca_com_addr &lhs, const vca_com_addr &rhs) const {
      return memcmp(&lhs, &rhs, sizeof(vca_com_addr)) == 0;
    }
  };
}

#include <cuckoo_table_template.cc>

vca_com_cons_table * connection_store;
