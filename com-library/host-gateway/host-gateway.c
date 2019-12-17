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
 *
 * host-gateway.c
 *
 * Main entry point to the executable host-gateway.
 * Implements communication between VCA card nodes using
 * memory sharing when communicating to other nodes within
 * the same host or TCP when communicating to VCA cards outside
 * of the host.
 * 
 * Program arguments:
 *  1)  n - # of VCA sockets in the system
 */

#include <stdio.h>  
#include <unistd.h>  
#include <stdlib.h>
#include <assert.h>

#include <vca_com.h>
#include <host-host-gateway.h>
#include <host-node-gateway.h>
#include <host-gateway.h>
#include <host-gateway-connection-store.h>

vca_com_hng_t hng;
vca_com_hhg_t hhg;

static void usage() {
  COML_DBM("./host-gateway -v <num> -i <ip> -np <port> -hp <port>");
  COML_DBM(" -v - number of vca cards in machine that will connect to host gateway");
  COML_DBM(" -i - ip of this host gateway");
  COML_DBM(" -np - port of this host gateway accepting new node connections");
  COML_DBM(" -hp - port of this host gateway accepting new host connections");
}

int main(int argc, char * argv[]) {
  
  int opt = 0;
  unsigned int num_vcacards = 0;
  const char * ip = NULL;
  pthread_t node_control;
  void * tret = NULL;
  const char * node_port = NULL;
  const char * host_port = NULL;
  const char ** port = &node_port;

  while((opt = getopt(argc, argv, "i:p:v:nh")) != -1) {
    switch (opt) {
    case 'v':
      num_vcacards = strtoul(optarg, NULL, 10);
      break;
    case 'i':
      ip = optarg;
      break;
    case 'n':
      port = &node_port;
      break;
    case 'h':
      port = &host_port;
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

  if(!(num_vcacards > 0) || !ip || !node_port || !host_port) {
    COML_DBM("Missing arguments");
    usage();
    exit(EXIT_FAILURE);
  }

  assert(!vca_com_hng_init(&hng, num_vcacards * 3, ip, node_port));
  (void) vca_com_hng_set_global_host_port(&hng, host_port);
  assert(!vca_com_hhg_init(&hhg, ip, host_port));
  connection_store = vca_com_cons_table_init(0);

  // start new thread accepting vcacards
  pthread_create(&node_control, NULL, (void * (*) (void *)) vca_com_hng_accept_new_nodes, &hng);

  // cycle through vca cards and incomming sockets for msgs to be routed
  do {
    
    if(vca_com_hng_spin_and_deliver(&hng)) {
      //COML_DBM("Failed to spin over vca queue");
    }
    
    if(vca_com_hhg_accept_and_deliver(&hhg)) {
      //COML_DBM("Failed to spint over zmq socket");
    }
    
  } while(1);

  if(!pthread_join(node_control, &tret)) {
    COML_DBM("finished accepting node connections");
  } else {
    perror("Failed to join thread");
  }

  assert(!vca_com_hng_deinit(&hng));
  assert(!vca_com_hhg_deinit(&hhg));
}
