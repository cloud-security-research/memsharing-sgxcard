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
 * VCA_COM_DS.h
 *
 * Defines message format for node to node communication including
 * the address of each node and message format. This is used by 
 * the gateway as well as by the node library.
 *
 */

#ifndef _VCA_COM_DS_H
#define _VCA_COM_DS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

  // address of a vca card
  typedef struct __attribute__((__packed__)) vca_com_addr_s {
    unsigned char gateway[4]; // gateway to network of host
    unsigned short gateway_port; // port of gateway port
    unsigned char host[4]; // host ip address behind gateway
    unsigned short host_port; // port of host ip
    unsigned char socket; // socket identifier
  } vca_com_addr;

  typedef enum vca_com_msg_type {
    VCA_COM_MSG_SEND,
    VCA_COM_MSG_DELIVERY_FAILED
  } vca_com_msg_type;

  // message header struct (message typically follows immediately in the byte stream)
  typedef struct __attribute__((__packed__)) {
    // source and destination of message
    vca_com_addr src;
    vca_com_addr dst;
    
    unsigned long long length;
    
    enum vca_com_msg_type type;
  } vca_com_msg_hdr;

  typedef enum vca_com_type {
    VCA_COM_MEM_SHARING = 0,
    VCA_COM_MEM_SHARING_HOST = 1,
    VCA_COM_ZMQ_SOCKET = 2
  } vca_com_type;

  // vca com structure
  typedef struct {
    // self identify card address
    vca_com_addr self;

    // send/recv to host
    void * com;
    
    // type of connection 
    vca_com_type type;

  } vca_com_t;

  #define VCA_COM_ZMQ_ID_SIZE 256

  typedef struct {
    //socket
    void * socket;

    // id
    char id[VCA_COM_ZMQ_ID_SIZE];

    size_t id_size;
  } vca_com_zmqs;
  
  // sets fields in naddr to the specified values and performs simple checks on the inputs
  int vca_com_init_addr(vca_com_addr * addr,
			     unsigned char gateway[4],
			     unsigned short gateway_port,
			     unsigned char host[4],
			     unsigned short host_port,
			     unsigned char socket);

  int vca_com_init_addr_from_string(vca_com_addr * addr,
			const char * gateway_ip,
			const char * gateway_port,
			const char * host_ip,
			const char * host_port,
			const char * socket);

  int vca_com_cpy_addr(vca_com_addr * src,
		       vca_com_addr * dst);

  // sets fields in mHdr to the specified values and performs simple checks
  int vca_com_init_msg(vca_com_msg_hdr * mHdr,
		       vca_com_addr * src,
		       vca_com_addr * dst,
		       unsigned long long length);

  // retrieve hdr from msg
  vca_com_msg_hdr * vca_com_msg_get_hdr(char * msg);

  // retrieve msg from msgs (removing hdr)
  char * vca_com_msg_get_msg(char * msg);


#ifdef __cplusplus
}
#endif
  
#endif // !_VCA_COM_DS_H
