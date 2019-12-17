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
 * vca_com_ds.c
 *
 * VCA COM data structures (address and message) initialization
 * and copy functions.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vca_com_ds.h>

// sets fields in naddr to the specified values and performs simple checks on the inputs
int vca_com_init_addr(vca_com_addr * addr,
			   unsigned char gateway[4],
			   unsigned short gateway_port,
			   unsigned char host[4],
			   unsigned short host_port,
			   unsigned char socket) {

  if(!addr) {
    return -1;
  }

  memcpy(addr->gateway, gateway, sizeof(unsigned char) * 4);
  addr->gateway_port = gateway_port;
  memcpy(addr->host, host, sizeof(unsigned char) * 4);
  addr->host_port = host_port;
  addr->socket = socket;
  
  return 0;
}

// scan for 4 unsigned char values
// returns != 0 in case didn't match all 4
static int scan_ip_to_char(const char * ip, unsigned char * iparr) {
	
  return sscanf(ip, "%hhu.%hhu.%hhu.%hhu", &iparr[0], &iparr[1], &iparr[2], &iparr[3]) != 4;
}

int vca_com_init_addr_from_string(vca_com_addr * addr,
			const char * gateway_ip,
			const char * gateway_port,
			const char * host_ip,
			const char * host_port,
			const char * socket) {

  if(!addr || !gateway_ip || !gateway_port || !host_ip || !host_port || !socket)
	return -1;

  int ret = scan_ip_to_char(gateway_ip, addr->gateway);
  addr->gateway_port = (unsigned short) strtoul(gateway_port, NULL, 0);
  ret |= scan_ip_to_char(host_ip, addr->host);
  addr->host_port = (unsigned short) strtoul(host_port, NULL, 0);
  addr->socket = (unsigned char) strtoul(socket, NULL, 0);

  return ret;
}

int vca_com_cpy_addr(vca_com_addr * src,
		     vca_com_addr * dst) {

  if(!src || !dst) {
    return -1;
  }

  return dst == memcpy(dst, src, sizeof(vca_com_addr));
  
}

// sets fields in mHdr to the specified values and performs simple checks
int vca_com_init_msg(vca_com_msg_hdr * mHdr,
		     vca_com_addr * src,
		     vca_com_addr * dst,
		     unsigned long long length) {
  
  if(!mHdr || !src || !dst || length < 1) {
    return -1;
  }

  if(vca_com_cpy_addr(&mHdr->src, src) 
     && vca_com_cpy_addr(&mHdr->dst, dst)
     && (mHdr->length = length)
     && (mHdr->type = VCA_COM_MSG_SEND) ==  0) {
    return 0;  } else {
    return -1;
  }
}

vca_com_msg_hdr * vca_com_msg_get_hdr(char * msg) {
  return (vca_com_msg_hdr *)msg;
}

char * vca_com_msg_get_msg(char * msg) {
  return msg + sizeof(vca_com_msg_hdr);
}