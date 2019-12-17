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
#include <host-gateway-connection-store.h>
#include <host-host-gateway.h>
#include <host-gateway.h>

#include <stdio.h>

int vca_com_hng_dst_is_node_on_same_host(vca_com_addr * dst, vca_com_addr * src) {

  if(dst->gateway == src->gateway && dst->gateway_port == src->gateway_port
     && dst->host == src->host && dst->host_port == src->host_port) {
    return 1;    
  }

  return 0;
}

int vca_com_gate_deliver_failure(vca_com_t * com, char * msg, unsigned long long len) {
  
  vca_com_msg_hdr * hdr = (vca_com_msg_hdr*) msg;
  vca_com_addr tmp;

  hdr->type = VCA_COM_MSG_DELIVERY_FAILED;
  // swap direction of message (body+length of message stays the same)
  vca_com_cpy_addr(&tmp, &hdr->src);
  vca_com_cpy_addr(&hdr->src, &hdr->dst);
  vca_com_cpy_addr(&hdr->dst, &tmp);

  return vca_com_send_hdrless_msg(com, msg, len, -1);
}

int vca_com_gate_deliver_msg(char * msg, unsigned long long len, vca_com_addr * self) {

  vca_com_msg_hdr * hdr = (vca_com_msg_hdr*) msg;
  vca_com_t * com = NULL;

  COML_DBM("deliver msg to %hhu.%hhu.%hhu.%hhu:%hu:%hu", hdr->dst.host[0],
	hdr->dst.host[1], hdr->dst.host[2], hdr->dst.host[3], hdr->dst.host_port, hdr->dst.socket);

  // deliver msg to dst otherwise send failure to src
  if(vca_com_cons_table_find(connection_store, &hdr->dst, &com)) {
    // destination connection found
    COML_DBM("found host in connection store, send hdrless msg");
    if(!vca_com_send_hdrless_msg(com, msg, len, -1)) {
      return 0;
    }    
  } else {
    // connection does not exist, ask host host gateway to create one (if external)
    COML_DBM("could not find connection to dst, try to create");
    if(!vca_com_hng_dst_is_node_on_same_host(&hdr->dst, self)) {
       COML_DBM("dst is not on same host");
      // try open new connection to remote card/host specified in dst
      int rc = vca_com_hhg_create_com(&hhg, &hdr->dst, &com);
      if(rc == 0 && com) {
        COML_DBM("was able to create socket to dst, send hdrless msg");
	// send msg to com
	if(!vca_com_send_hdrless_msg(com, msg, len, -1)) {
	  return 0;
	}
      }
    } // if msg is should be delivered within host, but doesn't exist, drop it
    else {
      COML_DBM("recipient on host, but not connected, drop message");
    }
  }


  // delivery failed
  if(com) {
    return vca_com_gate_deliver_failure(com, msg, len);
  } else {
    return -1;
  }
}
