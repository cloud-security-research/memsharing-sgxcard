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

#ifndef _VCA_MEM_H
#define _VCA_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define VCA_SOCKETS 6
#define MAX_MAPPINGS_PER_VCA_SOCKET 3 
#define VCA_SOCKET_0 0
#define VCA_SOCKET_1 1
#define VCA_SOCKET_2 2
#define VCA_SOCKET_3 3
#define VCA_SOCKET_4 4
#define VCA_SOCKET_5 5
#define WITH_HOST -1

#define VCA_HOST_PORT "5555"

#define READ 0
#define WRITE 1 
#define REMOTE_WILL_WRITE WRITE
#define REMOTE_WILL_READ READ
#define MAX_COMMAND_LEN 256 
#define PAGE_SIZE 4096
#define HOST 0
#define CARD 1 
#define _2MB 0x200000

#define ENQUEUE_MAP_NUMBER 0
#define DEQUEUE_MAP_NUMBER 1
#define SHMEM_MAP_NUMBER_0 0
#define SHMEM_MAP_NUMBER_1 1
#define SHMEM_MAP_NUMBER_2 2


#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define MAX_ITEMS 8192 // For now its fixed number of items per channel;
#define MAX_ITEMS_ORDER 13 // 2^13 is 8192 
#define MAX_CHANNELS_PER_VCA_SOCKET 8
#define REMOTE_PRODUCER 65536 //MAX_ITEMS * MAX_CHANNELS_PER_VCA_SOCKET 
#define LOCAL_CONSUMER 65537 //(MAX_ITEMS * MAX_CHANNELS_PER_VCA_SOCKET + 1) //8193 local consumer

#define LOCAL_PRODUCER 0
#define REMOTE_CONSUMER 1
#define HOST_CNT 2
#define NODE_CNT 3
#define HOST_ARR 4
#define NODE_ARR (4+32)
#define REMOTE_RING_SIZE ((MAX_ITEMS * sizeof(unsigned long) * MAX_CHANNELS_PER_VCA_SOCKET) + PAGE_SIZE)

#define MAGIC 0xdeadbeefcafebabe
#define MAGIC_LEN sizeof(MAGIC)
#define NUM_ITEMS 32
#define BUFF_SIZE_BOUNDARY (NUM_ITEMS*sizeof(long))
#define MAX_CHANNELS 8
#define MAX_RETRY 8
#define GENERAL_ERROR -1

typedef struct {
    unsigned long  pfn : 54;
    unsigned int soft_dirty : 1;
    unsigned int file_page : 1;
    unsigned int swapped : 1;
    unsigned int present : 1;
} PagemapEntry;

typedef struct {
    void *ring_2mb;
    void *ring_4kb;
    int queue_type; 
    int socket; 
} queue_object;

typedef struct {
  int active_sockets[VCA_SOCKETS];
  int next_recv_channel;
  int next_submit_channel;
  int next_recv_socket;
  int next_submit_socket;
  int total_sockets;
  queue_object *tx_q_objs[VCA_SOCKETS];
  queue_object *rx_q_objs[VCA_SOCKETS];
} task_queue_opaque;

typedef struct __attribute__((__packed__)) {
 unsigned long total_bursts;
 unsigned long payload_size;
 unsigned long magic;
 unsigned long padding[29];
 } task_header;


typedef struct __attribute__((__packed__)) transfer_mapping {
  unsigned long physical_addr;
  unsigned long size;
  int mapping_type;
  int socket;
} transfer_mapping;  

// To get pointer to the page map entry in /proc/self/pagemap
int get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr);


// To get pointer to physical address corresponding to a virtual address. Virtual address need not be page aligned
unsigned long  virt_to_phys_user(uintptr_t vaddr);

// To get contigious 2MB physical pages; works nicely on systems with transperant hugepages enabled 
void* get_contiguous__2MB(void);

// Check whether platform is card or host
int get_local_platform_type(void);

// if card then what is the socket number
int get_card_self_socket_number(void);

// Execute something on the local platform. This API also used to execute something on remote using SSH/SSL
void execute(const char *fmt, ...);

// Sets to FIFO pipes and cleans up MTRR if executed on card socket
void initialize_system(const char * ip, const char * port, int * socket);
	
// API to share local memory with remote socket ; is also used by library to setup async queues
int share_local_memory(int socket, void *ptr, unsigned long size, int mapping_type);

// API to request sharing a mapping specified by map
int request_sharing(transfer_mapping * map, int (*send_recv)(void *, void *, size_t, int));

// API to share local memory with remote socket ; is also used by library to setup async queues
int recv_remote_data(int * socket, unsigned long *remote_physical, unsigned long *size, int *mapping_type); 

// API to get local side physical address from /sys/kernel/sgx_mapper_N/map[0-n]
unsigned long get_local_mapping(int socket, int mapping_number ); 
	
// Program base address, size and type in /proc/mtrr. Address and size alignments are adjustments are taken care inside the API
int setup_mtrr_mappings(unsigned long base, unsigned long size, int remote_access_type);

//Given a remote  physical address, size and socket number, programs the DMA controller 
unsigned long setup_local_mappings(int socket, int mapping_number, unsigned long remote_base, unsigned long size);
	 	
// Given a local physical address and size returns a virtual mapping
void*  map_phys_memory(unsigned long phys, unsigned long size); 

// Given remote socket and local mapping index number, returns the virtual address mapped into apps address space. request size will deprecated in future
void* map_remote_memory(transfer_mapping * map, int socket, unsigned long request_size, int mapping_number); 
	
// Initialize and setup a queue with the remote socket for dequeuing data ; each queue with a socket provides 8 channels; Only one socket per queue supported but can be easily extended to any number of queues per socket  	
queue_object *init_dequeue(int socket);


// Initialize and setup a queue with the remote socket for enqueuing data ; each queue with a socket provides 8 channels; Only one socket per queue supported but can be easily extended to any number of queues per socket	
queue_object *init_enqueue(int socket);

void incr_prodcons(unsigned long * prod_cons_array, int real_idx);

// Producer Side of the application will call this to push data into queue into a certain channel idx.
int s_variable_multi_enqueue(queue_object *queue_obj, void *source, unsigned int total_elements, unsigned int idx); 

// Consumer side of the application will call this to take out  data from queue a certain channel idx
int s_variable_multi_dequeue(queue_object *queue_obj, void *source, unsigned int max_requested, unsigned int idx);

// Free up queue object if no longer required
void free_queue(queue_object *q);

void *init_host_task_system(void *opq, const char * ip, const char * port, int * socket);
void *init_vca_task_system(const char * ip, const char * port, int * socket);

void deinit_vca_task_system(void *opq);

long common_submit_task(void *opq, long task_length, void *task_buffer, int channel, int socket);

long common_recv_task(void *opq, long *task_length, void *task_buffer, int channel, int socket);

long host_submit_task(void *opq, long task_length, void *task_buffer, int task_id);
long host_recv_task(void *opq, long *task_length, void *task_buffer, int *task_id);

long vca_submit_task(void *opq, long task_length, void *task_buffer, int channel);
long vca_recv_task(void *opq, long *task_length, void *task_buffer, int channel);

#ifdef __cplusplus
}
#endif
  
#endif // !_VCA_MEM_H
