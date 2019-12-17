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
  COML_DBM("sendHello -ni <node ip> -np <node port> -hi <host ip> -hp <host port> -di <desitnation ip> -dp <destination port> [-s]");
  COML_DBM(" -ni - ip of this node");
  COML_DBM(" -np - port of this node");
  COML_DBM(" -hi - ip of the host gateway to connect to");
  COML_DBM(" -hp - port of the host gateway to connect to");
  COML_DBM(" -s - use socket instead of memory sharing");
}


int main(int argc, char * argv[]) {
  
  int opt = 0;
  int sock = VCA_COM_MEM_SHARING;
  char * node_ip = NULL;
  char * host_ip = NULL;
  char * destination_ip = NULL;
  char ** ip = &node_ip;
  const char * node_port = NULL;
  const char * host_port = NULL;
  const char * destination_port = NULL;
  const char * destination_socket = 0;
  const char ** port = &node_port;


  while((opt = getopt(argc, argv, "i:p:nhdsc:")) != -1) {
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
    case 'd':
      port = &destination_port;
      ip = &destination_ip; 
      break;
    case 's':
      sock = VCA_COM_ZMQ_SOCKET;
      break;
    case 'c':
      destination_socket = optarg;
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

  int missing_arguments = !node_ip || !node_port || !destination_ip || !destination_port || !destination_socket;
  missing_arguments |= (sock == VCA_COM_MEM_SHARING && (!host_ip || ! host_port));
  if(missing_arguments) {
    COML_DBM("Missing arguments");
    usage();
    exit(EXIT_FAILURE);
  }

  vca_com_t c;
  vca_com_addr self;
  vca_com_addr dest;
  char msg_buf[4096];
  
  if(sock == VCA_COM_MEM_SHARING) {
    node_ip[7] -= 1; // make socket id 
    vca_com_init_addr_from_string(&self, "0.0.0.0", "0", host_ip, host_port, &node_ip[7]);
    node_ip[7] += 1; // make ip address again
  } else {
    vca_com_init_addr_from_string(&self, "0.0.0.0", "0", node_ip, node_port, "0");
    host_ip = destination_ip;
    host_port = destination_port;
  }

  vca_com_init_addr_from_string(&dest, "0.0.0.0", "0", destination_ip, destination_port, destination_socket);

  if(init_vca_com(&c, host_ip, host_port, &self, sock)) {

    COML_DBM("failed to init vca");
    exit(EXIT_FAILURE);
  }

  strncpy(msg_buf, "Hello to you.", 4096);
  msg_buf[4096-1] = '\0';

  if(vca_com_send_msg(&c, &dest, msg_buf, 14, 0) < 0) {
    COML_DBM("send failed to %s:%s", destination_ip, destination_port);
    exit(EXIT_FAILURE);
  }

  printf("send msg to %hhu.%hhu.%hhu.%hhu:%d:%d saying %s\n", dest.host[0], 
	 dest.host[1], dest.host[2], dest.host[3], dest.host_port, dest.socket, msg_buf);

  if(deinit_vca_com(&c)) {
    COML_DBM("deinit failed");
    exit(EXIT_FAILURE);
  }
  

  return EXIT_SUCCESS;
}
