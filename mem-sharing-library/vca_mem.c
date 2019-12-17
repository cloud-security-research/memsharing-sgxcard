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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/queue.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <assert.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/param.h>
#include <ctype.h>
#include <zmq.h>

#include "vca_mem.h"

#ifdef ENCLAVE

void __assert_fail(const char * assertion, const char * file, unsigned int line, const char * function) {
}

#endif

#ifndef ENCLAVE
#define ZMQ_SEND ((int (*) (void *, void *, size_t, int)) zmq_send)
#define ZMQ_RECV zmq_recv //((int (*) (void *, void *, size_t, int)) zmq_recv)

const char *host_vca_ip_array[2][VCA_SOCKETS] = {
                                                { //Host side IPs       
                                                "172.31.1.254",
                                                "172.31.2.254",
                                                "172.31.3.254",
                                                "172.31.4.254",
                                                "172.31.5.254",
                                                "172.31.6.254"
                                                },

                                                { //VCA side IPs
                                                "172.31.1.1",
                                                "172.31.2.1",
                                                "172.31.3.1",
                                                "172.31.4.1",
                                                "172.31.5.1",
                                                "172.31.6.1"
                                                }
                                          };


void *context = NULL;
void *c = NULL;


int get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr)
{
    size_t nread;
    ssize_t ret;
    uint64_t data;

    nread = 0;
    while (nread < sizeof(data)) {
        ret = pread(pagemap_fd, &data, sizeof(data),
                (vaddr / sysconf(_SC_PAGE_SIZE)) * sizeof(data) + nread);
        nread += ret;
        if (ret <= 0) {
            return 1;
        }
    }
    entry->pfn = data & (((uint64_t)1 << 54) - 1);
    entry->soft_dirty = (data >> 54) & 1;
    entry->file_page = (data >> 61) & 1;
    entry->swapped = (data >> 62) & 1;
    entry->present = (data >> 63) & 1;
    return 0;
}


unsigned long  virt_to_phys_user(uintptr_t vaddr)
{
    char pagemap_file[BUFSIZ];
    int pagemap_fd;
    pid_t pid = getpid();

    snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%ju/pagemap", (uintmax_t)pid);
    pagemap_fd = open(pagemap_file, O_RDONLY);
    if (pagemap_fd < 0) {
        return -1;
    }
    PagemapEntry entry;
    if (get_entry(&entry, pagemap_fd, vaddr)) {
        return -1;
    }
    close(pagemap_fd);
    //printf("pa base 0x%lx vaddr % 4096 ix 0x%lx\n",entry.pfn * sysconf(_SC_PAGE_SIZE),(vaddr % sysconf(_SC_PAGE_SIZE)));
    return ((unsigned long)((entry.pfn * sysconf(_SC_PAGE_SIZE)) + (vaddr % sysconf(_SC_PAGE_SIZE))));
}



void* get_contiguous__2MB(void)
{
   int offset;
   unsigned long phys = 0, base_phys = 0;
   void *vaddr;
   vaddr = memalign(_2MB,_2MB);
   assert(madvise(vaddr,_2MB, MADV_HUGEPAGE) == 0);
   assert(mlock(vaddr,_2MB) == 0);
   base_phys = virt_to_phys_user((unsigned long)vaddr);
   for (offset=0;offset<_2MB;offset += PAGE_SIZE)
   {  
      phys = virt_to_phys_user((unsigned long)vaddr + offset);
      if  (phys != (base_phys + offset ))
           {
		free(vaddr);
		return NULL;
          }
   }
   assert(vaddr != NULL);
   memset((void *)vaddr,0x00,REMOTE_RING_SIZE);
   
   return vaddr;
}

void * get_contiguous__4KB(void)
{
  void * vaddr = NULL;
   
  vaddr = memalign(PAGE_SIZE,PAGE_SIZE);
  assert(vaddr != NULL);
  assert(mlock(vaddr,PAGE_SIZE) == 0);
  memset(vaddr,0x00,PAGE_SIZE);

  return vaddr;
}
   

int get_local_platform_type(void)
{
   return system("lsmod | grep 'vca_mgr ' > /dev/null");	
}

// TODO: assumes single digit number of CPUs (max 3 SGX cards)
int get_card_self_socket_number(void)
{
        char hostname[256];
	int hn_len = 0;
        int card_cpu = 0, card_id = 0;
        hostname[255] = '\0';
        gethostname(hostname,255);
	hn_len = strnlen(hostname, 256);
	card_cpu =  isdigit(hostname[hn_len-1]) ? hostname[hn_len-1] - '0' : 0;
	card_id =  isdigit(hostname[hn_len-2]) ? hostname[hn_len-2] - '0' : 0;
//        return(((card_cpu/10)*3) + (card_cpu%10)); 
        return card_id * 3 + card_cpu; //Card socket can only see 1 host socket so /tmp/pipe_0 will always be used
}


void execute(const char *fmt, ...) {
    char command[MAX_COMMAND_LEN] = {0,};
    va_list args;
    va_start(args, fmt);
    vsnprintf(command,MAX_COMMAND_LEN, fmt, args);
    va_end(args);
    //printf("Executing %s\n",command);
    int i = system(command);
}

void initialize_system(const char * ip, const char * port, int * socket) 
{
  int rc = 0;
  char ipname[256];

  // is the system already initialized?
  if(!c && !context) {

    // form the ip name and port as a string
    if(ip && port > 0) {
      snprintf(ipname, 256, "tcp://%s:%s", ip, port);
    } else {
      // no ip or port specified, use defaults from the table
      if(*socket > -1 && *socket < 6)
	ip = host_vca_ip_array[get_local_platform_type()][*socket];
      else
	ip = host_vca_ip_array[get_local_platform_type()][get_card_self_socket_number()];				      
      
      snprintf(ipname, 256, "tcp//%s:%s", ip, (port) ? port : VCA_HOST_PORT);
    }
    
    // create zmq context
    context = zmq_ctx_new ();
    if(*socket == -1 || *socket == -3) {
      // running as client -> connect to host server
      c = zmq_socket (context, ZMQ_REQ);
      rc = zmq_connect (c, ipname);
      if (rc != 0) {
	perror("Failed to connect to server");
	exit(1);
    }
    } else {
      // running as host -> bind on port clients connect to
      c = zmq_socket (context, ZMQ_REP);
      rc = zmq_bind (c, ipname);
      if (rc != 0) {
	perror("Failed to bind port on host");
	exit(1);
      }
    }
  }
  
  // exchange socket numbers when needed
  if (*socket == -3) {
    uint32_t s = get_card_self_socket_number();
    rc = zmq_send(c, &s, sizeof(s), 0);
    if(rc != sizeof(s)) {
      perror("Failed to exchange socket numbers");
      exit(1);
      }
    rc = zmq_recv(c, &s, sizeof(s), 0);
    if(rc != sizeof(s) && s != get_card_self_socket_number()) {
      perror("Failed to get ack from host for sock num");
      exit(1);
    }
  } else if (*socket == -2) {
    // accept arbitrary socket number -> fix it for this run
    uint32_t s = 0;
    rc = zmq_recv(c, &s, sizeof(s), 0);
    if(rc != sizeof(s)) {
      perror("Failed to exchange socket numbers");
      exit(1);
    }
    *socket = s;
    rc = zmq_send(c, &s, sizeof(s), 0);
    if(rc != sizeof(s)) {
      perror("Failed to send ack to node for sock num");
      exit(1);
      }
  }
  
  
}

int request_sharing(transfer_mapping * map, int (*send_recv)(void *, void *, size_t, int)) {
  int rc = 0;
  
  rc = send_recv(c, map, sizeof(transfer_mapping), 0);
  if(rc != sizeof(transfer_mapping)) {
    perror("Failed to share memory");
    return rc;
  }

  return 0;
}

int send_recv_mapping(transfer_mapping * in, transfer_mapping * out, int socket) {

  int rc = 0;
  transfer_mapping error_map = { .socket = -255 };
  
  if (get_local_platform_type() == HOST) {
    rc = request_sharing(out, ZMQ_RECV);
    // ensure we talk to the correct socket
    if (out->socket != socket) {
      request_sharing(&error_map, ZMQ_SEND);
      return 1; // retry wrong socket connected
    }
    
    rc |= request_sharing(in, ZMQ_SEND);
    
  } else { // for enclaves (clients)
    
    rc = request_sharing(in, ZMQ_SEND); // first send then recv
    rc |= request_sharing(out, ZMQ_RECV);

    if (out->socket == -255) {
      printf("Host decliened sharing memory\n");
      exit(1);
    }
  }

  return rc;
}

int share_local_memory(int socket, void *ptr, unsigned long size, int mapping_type)
{
  unsigned long physical_base = (unsigned long)virt_to_phys_user((unsigned long)ptr);
  transfer_mapping tq, out;
  int rc = 0;

  if(c == NULL) {
    printf("No connection, initialize system first\n");
    exit(1);
  } 
  
  assert(physical_base != 0);
  if (socket == -1) 
    socket = get_card_self_socket_number();
  assert(host_vca_ip_array[!get_local_platform_type()][socket] != NULL);
  assert(ptr != NULL);	
  assert(size % PAGE_SIZE == 0);
  assert((mapping_type == READ) || (mapping_type == WRITE));

  tq.physical_addr = physical_base;
  tq.size = size;
  tq.mapping_type = mapping_type;
  tq.socket = socket;

  do {
    rc = send_recv_mapping(&tq, &out, socket);
  } while (rc);
  
  printf("Channel Established..channel phys addr 0x%lx size 0x%lx type %d socket %d\n",physical_base,size,mapping_type, socket);
  
  return 0;
}

int recv_remote_data(int * socket, unsigned long *remote_physical, unsigned long *size, int *mapping_type) 
{
  char command[MAX_COMMAND_LEN] = {0,};
  char result[BUFSIZ] = {0,};
  transfer_mapping in, map;
  FILE *fp;
  int rc = 0;

  if(c == NULL) {
    printf("No connection, initialize system first\n");
    exit(1);
  } 
  
  if (*socket == -1) 
    *socket = get_card_self_socket_number();

  do {
    rc = send_recv_mapping(&in, &map, *socket);
  } while (rc);
  
  *remote_physical = map.physical_addr;
  *size = map.size;
  *mapping_type = map.mapping_type;
  *socket = map.socket;
  
  printf("Received \"0x%lx 0x%lx %d\" on socket %d\n",*remote_physical,*size,*mapping_type, *socket);	
  return 0;
}

unsigned long get_local_mapping(int socket, int mapping_number ) {
    char command[MAX_COMMAND_LEN] = {0,};    
    char result[BUFSIZ] = {0,};
    unsigned long remote_base, size, local_base;
    FILE *fp;

    
    if(get_local_platform_type() != HOST)
      socket = 0;
      
    snprintf(command,MAX_COMMAND_LEN, "cat /sys/kernel/sgx5_mapper_%d/map%d",socket,mapping_number);

    printf("local mapping %s\n", command);
    
    if ((fp = popen(command, "r")) == NULL) {
        printf("Error opening pipe!\n");
        return -1;
    }

    assert(fgets(result, BUFSIZ, fp)); 
    sscanf(result,"0x%lx 0x%lx 0x%lx\n",&remote_base, &size, &local_base);
    assert((remote_base % PAGE_SIZE) == 0);
    assert((size % PAGE_SIZE) == 0);
    assert((local_base % PAGE_SIZE) == 0);
    return local_base;
}
	


int setup_mtrr_mappings(unsigned long base, unsigned long size, int remote_access_type) 
{
        unsigned long i = PAGE_SIZE;
        char caching_type[BUFSIZ] = {0,};

        assert((base % PAGE_SIZE) == 0);
        assert((size % PAGE_SIZE) == 0);
        assert((remote_access_type == READ) || (remote_access_type == WRITE));

        while(i < size)
                i = i<<1;
        size = i;
        base = base & ((~size)+1); //Find the MTRR base that is a multiple of size

        if (remote_access_type == READ)
                 strncpy(caching_type, "write-through", BUFSIZ); //OR WRITE-BACK based on access requirement
        if (remote_access_type == WRITE)
                 strncpy(caching_type, "write-combining", BUFSIZ);

        printf("Calculated base=0x%lx size=0x%lx type=%s mapping for MTRR\n",base,size,caching_type);
        execute("echo \"base=0x%lx size=0x%lx type=%s\" > /proc/mtrr",base,size,caching_type);
        return 0;
}


unsigned long setup_local_mappings(int socket, int mapping_number, unsigned long base, unsigned long size)
{
  
  
  assert((base % PAGE_SIZE) == 0);
  assert((size % PAGE_SIZE) == 0);
  if (socket == -1) 
    socket = get_card_self_socket_number();
  
  if(get_local_platform_type() != HOST)
    socket = 0;

  printf("sgx5_mapper_%d/map%d init\n", socket, mapping_number);
  
  execute("echo \"0x00 0x00\" > /sys/kernel/sgx5_mapper_%d/map%d",socket,mapping_number);
  
  execute("echo \"0x%lx 0x%lx\" > /sys/kernel/sgx5_mapper_%d/map%d",base,size,socket,mapping_number);
	
  base = get_local_mapping(socket, mapping_number); 
  return base;	
}	
	 	


void*  map_phys_memory(unsigned long phys, unsigned long size) {
        int fd;
 	void* ptr;
        if ((fd = open("/dev/mem", O_RDWR )) == -1) { 
        perror("open");
        exit(1);
        }
        
        ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, phys);
       	assert(ptr != NULL); 

	close(fd);
   	return ptr;
}



void* map_remote_memory(transfer_mapping * map, int socket, unsigned long request_size, int mapping_number) 
{
  unsigned long local_physical, local_size, page_offset;
  
  if (get_local_platform_type() != HOST) //Workaround for bug in detectig CARD 
    socket = -1;
  
  assert(map->size % PAGE_SIZE == 0);
  assert(map->size == request_size);
  assert(mapping_number < MAX_MAPPINGS_PER_VCA_SOCKET);
  assert((map->mapping_type == READ) || (map->mapping_type == WRITE));
  
  page_offset = map->physical_addr & 0xFFF;
  if (page_offset != 0) {
    map->physical_addr = map->physical_addr - page_offset;
    local_size = map->size + PAGE_SIZE;
  } else {
    local_size = map->size;
  }	
  
  local_physical = setup_local_mappings(socket,mapping_number, map->physical_addr, local_size);

//	printf("Channel Established... channel phys addr 0x%lx size 0x%lx type %d socket %d\n",local_physical + page_offset, local_size, mapping_type, get_card_self_socket_number());
  
  return (void *)((unsigned long)map_phys_memory(local_physical, local_size) + page_offset);
}


int allocate_ring(void ** addr_ptr, void * (*alloc) (), int size, transfer_mapping * map, int socket, int rem_prod) {
  int i = 0;
  
  *addr_ptr = alloc();
  
  for (i=0; i< MAX_CHANNELS_PER_VCA_SOCKET; i++) {
    ((unsigned long *)*addr_ptr)[rem_prod + 2*i] = (i * MAX_ITEMS);
    ((unsigned long *)*addr_ptr)[rem_prod + (2*i) + 1] = i * MAX_ITEMS;
  }

  map->physical_addr = (unsigned long)virt_to_phys_user((unsigned long) *addr_ptr);
  map->size = size;
  map->mapping_type = REMOTE_WILL_WRITE;
  map->socket = socket;

  assert(map->physical_addr != 0);
  assert(map->size % PAGE_SIZE == 0);
  assert(map->mapping_type == READ || map->mapping_type == WRITE);

  return 0;
}

queue_object *init_dequeue(int socket) {
  int i = 0, rc = 0;
  transfer_mapping in, out;
  
  if(c == NULL) {
    printf("No connection, initialize system first\n");
    exit(1);
  }
  
  if (get_local_platform_type() == HOST) {
    printf("Local Platform is HOST\n");
    assert(socket < VCA_SOCKETS);
  } else {
    printf("Local Platform is CARD\n");
    socket = get_card_self_socket_number();
  }

  assert(socket >= 0 && host_vca_ip_array[!get_local_platform_type()][socket] != NULL);
  
  queue_object *q = malloc(sizeof(queue_object));
  assert(q != NULL);

  if (allocate_ring(&q->ring_2mb, get_contiguous__2MB, _2MB, &in, socket, REMOTE_PRODUCER)) {
    free(q);
    return NULL;
  }
  
  do {
    rc = send_recv_mapping(&in, &out, socket);
  } while(rc);
  
  q->ring_4kb = map_remote_memory(&out, socket, PAGE_SIZE, DEQUEUE_MAP_NUMBER);
  q->queue_type = DEQUEUE_MAP_NUMBER;
  q->socket = socket;

  return q;
}

queue_object * init_enqueue(int socket) {
  int i = 0, rc = 0;
  transfer_mapping in, out;

  if(c == NULL) {
    printf("No connection, initialize system first\n");
    exit(1);
  }
  
  if (get_local_platform_type() == HOST) {
    printf("Local Platform is HOST\n");
    assert(socket < VCA_SOCKETS);
  } else {
    printf("Local Platform is CARD\n");
    socket = get_card_self_socket_number();
  }
  
  queue_object *q = malloc(sizeof(queue_object));
  assert(q != NULL);

  if (allocate_ring(&q->ring_4kb, get_contiguous__4KB, PAGE_SIZE, &in, socket, 0)) {
    free(q);
    return NULL;
  }
  
  do { 
    rc = send_recv_mapping(&in, &out, socket);
  } while (rc);
    
  q->ring_2mb = map_remote_memory(&out,socket, _2MB, ENQUEUE_MAP_NUMBER);
  q->queue_type = ENQUEUE_MAP_NUMBER;
  q->socket = socket;
  
  printf("Init split enqueue done : _2MB pointer %p 4KB pointer %p \n",q->ring_2mb,
	 q->ring_4kb);
  
  return q;
}

void free_queue(queue_object *q)
{
	assert(q != NULL);
	if (q->queue_type == DEQUEUE_MAP_NUMBER) {
		assert(q->ring_2mb != NULL);
		free(q->ring_2mb);
	} else if (q->queue_type == ENQUEUE_MAP_NUMBER) {
		assert(q->ring_4kb != NULL);
		free(q->ring_4kb);
	}

	free(q);
}



void *init_host_task_system(void *opq, const char * ip, const char * port, int * socket)
{
   task_queue_opaque *opaque = opq;
   unsigned int i = 0;
   assert(*socket < VCA_SOCKETS);
   if (opaque == NULL) {
   	opaque = (task_queue_opaque *) malloc(sizeof(task_queue_opaque));  
	assert(opaque != NULL);
	memset(opaque, 0, sizeof(task_queue_opaque));
   	opaque->next_recv_channel = 0;
   	opaque->next_submit_channel = 0;
        opaque->next_recv_socket = 0;
        opaque->next_submit_socket = 0;
	opaque->total_sockets = 0;
	for(i = 0; i < VCA_SOCKETS; i++)
	  opaque->active_sockets[i] = -1;
   } 

   initialize_system(ip, port, socket); // may update the socket, depending on who connects
   opaque->active_sockets[opaque->total_sockets++] = *socket;
   printf("\nInitializing communication with worker on socket %d\n",*socket);
   opaque->tx_q_objs[*socket] = init_enqueue(*socket);
   assert(opaque->tx_q_objs[*socket] != NULL);
   opaque->rx_q_objs[*socket] = init_dequeue(*socket);
   assert(opaque->rx_q_objs[*socket] != NULL);
   return opaque;
}

void *init_vca_task_system(const char * ip, const char * port, int * socket)
{
  task_queue_opaque *opaque;
  opaque = (task_queue_opaque *) malloc(sizeof(task_queue_opaque));
  memset(opaque, 0, sizeof(task_queue_opaque));
  assert(opaque != NULL);
  initialize_system(ip, port, socket);
  opaque->rx_q_objs[0] = init_dequeue(-1);
  assert(opaque->rx_q_objs[0] != NULL);
  opaque->tx_q_objs[0] = init_enqueue(-1);
  assert(opaque->tx_q_objs[0] != NULL);
  return opaque;
}


void deinit_vca_task_system(void *opq)
{
 int i = 0;  
 task_queue_opaque *opaque = opq;
 assert(opaque != NULL);
 
 printf("\nTearing down communication with workers\n");
 for (i = 0; i < VCA_SOCKETS; i++) {
   if(opaque->rx_q_objs[i])
     free_queue(opaque->rx_q_objs[i]);
   if(opaque->tx_q_objs[i])
     free_queue(opaque->tx_q_objs[i]);
 }
 
 free(opaque);

 zmq_close(c);
 zmq_ctx_destroy(context);
}

#endif //NOT ENCLAVE MODE

int s_variable_multi_enqueue(queue_object *queue_obj, void *source, unsigned int total_elements, unsigned int idx)
{

  unsigned long *ring = queue_obj->ring_2mb;
  unsigned long *prod_cons_array = queue_obj->ring_4kb;
  unsigned int real_idx = idx << 1;
  unsigned int LP_VAL = (unsigned int) prod_cons_array[real_idx + LOCAL_PRODUCER];
  unsigned int RC_VAL = prod_cons_array[real_idx + REMOTE_CONSUMER];
  unsigned int LOWER_BOUND = idx << MAX_ITEMS_ORDER;
  unsigned int UPPER_BOUND = LOWER_BOUND + MAX_ITEMS;
  unsigned int max_dist, num_elements, remaining_elements, new_bound_val;
  
  max_dist = (RC_VAL-(LP_VAL+1)) % MAX_ITEMS;
  num_elements = MIN(UPPER_BOUND-LP_VAL, total_elements);
  remaining_elements = total_elements - num_elements;
  new_bound_val = (unlikely(remaining_elements != 0))
    ? LOWER_BOUND + remaining_elements
    : LP_VAL + num_elements;
  
  if (likely(total_elements <= max_dist)) {
    memcpy( (void *)(ring + LP_VAL), (void *)source, num_elements << 3);
    asm volatile ("mfence" ::: "memory");
    if (unlikely(remaining_elements != 0)) {
      memcpy( (void *)(ring + LOWER_BOUND), (void *)((unsigned long)source + (num_elements << 3)), remaining_elements << 3);
      asm volatile ("mfence" ::: "memory");
      ring[real_idx + REMOTE_PRODUCER] = prod_cons_array[real_idx + LOCAL_PRODUCER] = LOWER_BOUND + remaining_elements;
    } else {
      ring[real_idx + REMOTE_PRODUCER] = prod_cons_array[real_idx + LOCAL_PRODUCER] = LP_VAL + num_elements;
    }
    
    return total_elements;
  }
  
  return 0;
}

int s_variable_multi_dequeue(queue_object *queue_obj, void *source, unsigned int max_requested, unsigned int idx)
{

 unsigned long *ring = queue_obj->ring_2mb;
 unsigned long *prod_cons_array = queue_obj->ring_4kb;
 unsigned int real_idx = idx << 1; //2x the idx value
 unsigned int LC_VAL = (unsigned int)ring[real_idx + LOCAL_CONSUMER];
 unsigned int RP_VAL = ring[real_idx + REMOTE_PRODUCER];
 unsigned int LOWER_BOUND = idx << MAX_ITEMS_ORDER;
 unsigned int UPPER_BOUND = LOWER_BOUND + MAX_ITEMS;
 unsigned int max_available, num_elements, remaining_elements, i = 0;

#ifdef HOST_MODE
        asm volatile ("mfence" ::: "memory");
#endif
        max_available = (RP_VAL-LC_VAL) % MAX_ITEMS;
    
        if (likely(max_requested <= max_available)) {
                num_elements = MIN(UPPER_BOUND-LC_VAL, max_requested);
                remaining_elements = max_requested - num_elements;
                memcpy( (void *) source, (void *)(ring + LC_VAL), num_elements << 3);
                if (unlikely(remaining_elements != 0)) {
                        memcpy((void *)((unsigned long)source + (num_elements << 3)), (void *)(ring + LOWER_BOUND), remaining_elements << 3);
                        prod_cons_array[real_idx + REMOTE_CONSUMER] = ring[real_idx + LOCAL_CONSUMER] = LOWER_BOUND + remaining_elements;
                } else {
                        prod_cons_array[real_idx + REMOTE_CONSUMER] = ring[real_idx + LOCAL_CONSUMER] = LC_VAL + num_elements;
                }

                return max_requested;
        }

        return 0;
}



long common_submit_task(void *opq, long task_length, void *task_buffer, int channel, int socket)
{
  task_queue_opaque *opaque = opq;
  long ret;
  int burst_num, retries = 0;
  task_header th;
  //  printf("submit task len %d, channel %d socket %d\n", task_length, channel, socket);

  th.total_bursts = ((task_length - 1) / BUFF_SIZE_BOUNDARY) + 1;
  th.payload_size = task_length; 
  th.magic = MAGIC;
  
  do {
    ret = s_variable_multi_enqueue(opaque->tx_q_objs[socket], &th, NUM_ITEMS, channel);
    } while (ret != NUM_ITEMS); //&& (++retries <= MAX_RETRY));

  // Start enqueing 
  for (burst_num = 0; burst_num < th.total_bursts ; burst_num++) {
    do {
        ret = s_variable_multi_enqueue(opaque->tx_q_objs[socket], task_buffer + (burst_num * BUFF_SIZE_BOUNDARY), NUM_ITEMS, channel);
    } while (ret != NUM_ITEMS); //&& (++retries <= MAX_RETRY));

  }
  return task_length;
}

long common_recv_task(void *opq, long *task_length, void *task_buffer, int channel, int socket)
{
  task_queue_opaque *opaque = opq;
  int burst_num=0; //Later use round robin to find from which channel data needs to be acquired
  long ret;
  task_header th;

  assert(opaque && task_buffer);

  //  printf("submit task len %d, channel %d socket %d\n", task_length, channel, socket);
  
  ret = s_variable_multi_dequeue(opaque->rx_q_objs[socket], &th, NUM_ITEMS, channel);

   if (ret != NUM_ITEMS)
        return -1;

  assert ((th.total_bursts != 0) && (th.payload_size != 0) && (th.magic == MAGIC));

  *task_length = th.payload_size;

  // Start dequeing 
  for (burst_num = 0; burst_num < th.total_bursts ; burst_num++) {
    do {
        ret = s_variable_multi_dequeue(opaque->rx_q_objs[socket], task_buffer + (burst_num * BUFF_SIZE_BOUNDARY), NUM_ITEMS, channel);
    } while (ret != NUM_ITEMS);

  }

  return 0;
}



 long host_submit_task(void *opq, long task_length, void *task_buffer, int task_id) 
{
  task_queue_opaque *opaque = opq;
  int channel;
  int socket;

  assert(opaque && task_buffer && task_length);                              

  channel = task_id < 0 ? (opaque->next_submit_channel++) % MAX_CHANNELS : task_id % 10;

  if (channel == 0) 
    opaque->next_submit_socket++;

  socket = task_id < 0 ? opaque->active_sockets[opaque->next_submit_socket % opaque->total_sockets] : task_id / 10; 

  assert((channel < MAX_CHANNELS) && (socket < VCA_SOCKETS));                              
  
  return common_submit_task(opq,task_length,task_buffer,channel,socket);  
}


long host_recv_task(void *opq, long *task_length, void *task_buffer, int *task_id)
{
  task_queue_opaque *opaque = opq;
  int channel,socket;
  int got_data;

  assert(opaque && task_buffer && task_length && task_id);

  do {
  	channel = (opaque->next_recv_channel++) % MAX_CHANNELS;

        if (channel == 0) 
    		opaque->next_recv_socket++;

	socket = opaque->active_sockets[opaque->next_recv_socket % opaque->total_sockets];     

  	assert((channel < MAX_CHANNELS) && (socket < VCA_SOCKETS));                                        

  	*task_id = (socket * 10) + (channel);
  	got_data  = common_recv_task(opq,task_length,task_buffer,channel,socket); 
  } while (got_data != 0);

  return got_data;
  
}

long  vca_submit_task(void *opq, long task_length, void *task_buffer, int channel)
{
  task_queue_opaque *opaque = opq;

  assert(opaque && task_buffer && task_length && (channel < MAX_CHANNELS));

  return common_submit_task(opq,task_length,task_buffer,channel,0);
}


long  vca_recv_task(void *opq, long *task_length, void *task_buffer, int channel)
{
  task_queue_opaque *opaque = opq;
  int got_data = 0;

  assert(opaque && task_buffer && task_length && (channel < MAX_CHANNELS));

  do {
    got_data = common_recv_task(opq,task_length,task_buffer,channel,0);
  } while (got_data != 0);
  
  return got_data;
}


