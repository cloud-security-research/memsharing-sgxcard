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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <vca_com.h>

void usage() {
  COML_DBM("halloCmd -ni <node ip> -np <node port> -hi <host ip> -hp <host port> -s ");
  COML_DBM(" -ni - ip of this node");
  COML_DBM(" -np - port of this node");
  COML_DBM(" -hi - ip of the host gateway to connect to");
  COML_DBM(" -hp - port of the host gateway to connect to");
  COML_DBM(" -s - use socket instead of memory sharing");
}


int main(int argc, char * argv[]) {
  
  int opt = 0;
  int sock = 0;
  char * node_ip = NULL;
  char * host_ip = NULL;
  char ** ip = &node_ip;
  const char * node_port = NULL;
  const char * host_port = NULL;
  const char ** port = &node_port;


  while((opt = getopt(argc, argv, "i:p:nhs")) != -1) {
    switch (opt) {
    case 'i':
      if(ip) {
	*ip = optarg;
      } else {
	COML_DBM("specify node or host ip");
	usage();
	exit(EXIT_FAILURE);
      }
      break;
    case 'n':
      port = &node_port;
      ip = &node_ip;
      break;
    case 'h':
      port = &host_port;
      ip = &host_ip;
      break;
    case 's':
      sock = 2;
      break;
    case '?':
      COML_DBM("argument %c requires a parameter", opt);
      usage();
      exit(EXIT_FAILURE);
    case 'p':
      if(port) {
	*port = optarg;
	break;
      } 
      // fall through to the default error message
    default:
      COML_DBM("Wrong argument");
      usage();
      exit(EXIT_FAILURE);
    }
  }

  if(!host_ip || !host_port || !node_ip || !node_port) { 
    COML_DBM("Missing arguments");
    usage();
    exit(EXIT_FAILURE);
  }

  vca_com_t c;
  vca_com_addr self;
  vca_com_addr src;
  char msg_buf[4096];
  
  if(!sock) {
	node_ip[7] -= 1; // make socket id 
	vca_com_init_addr_from_string(&self, "0.0.0.0", "0", host_ip, host_port, &node_ip[7]);
	node_ip[7] += 1; // make ip address again
  } else {
	vca_com_init_addr_from_string(&self, "0.0.0.0", "0", node_ip, node_port, "0");
  }

  if(init_vca_com(&c, host_ip, host_port, &self, sock)) {

    COML_DBM("failed to init vca");
    exit(EXIT_FAILURE);
  }

  int rc = 0;
  unsigned int channel = 0;
  unsigned long long len = 4096;
  do {
    rc = vca_com_recv_msg(&c, &src, msg_buf, &len, &channel); // recv and go over all channels
    channel = (channel + 1) % 8;
  } while ( rc < 0 );

  printf("msg received from %hhu.%hhu.%hhu.%hhu:%d:%d saying %s (%llu)\n", src.host[0], 
	 src.host[1], src.host[2], src.host[3], src.host_port, src.socket, msg_buf, len);

  if(deinit_vca_com(&c)) {
    COML_DBM("deinit failed");
    exit(EXIT_FAILURE);
  }
  

  return EXIT_SUCCESS;
}
