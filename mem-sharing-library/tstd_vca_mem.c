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

#include "vca_mem.h"

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
  int i = 0;

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
  unsigned int max_available, num_elements, remaining_elements, new_bound_val;
  
#ifdef HOST_MODE
  asm volatile ("mfence" ::: "memory"); 
#endif
  max_available = (RP_VAL-LC_VAL) % MAX_ITEMS;
  num_elements = MIN(UPPER_BOUND-LC_VAL, max_requested);
  remaining_elements = max_requested - num_elements;
  new_bound_val = (unlikely(remaining_elements != 0))
    ? LOWER_BOUND + remaining_elements
    : LP_VAL + num_elements; 
  
  if (likely(max_requested <= max_available)) {
    
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


