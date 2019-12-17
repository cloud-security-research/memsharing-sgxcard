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
#include <inttypes.h>
#include <sys/types.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <malloc.h>
#include <assert.h>


#include "../mem-sharing-library/vca_mem.h"

int
main(int argc, char *argv[])
{
  int i, items_sent, sock = 0;
  queue_object *q0_obj;
  unsigned long buffer[32] = {0,};

  initialize_system("*", "5555", &sock);
  
  q0_obj = init_enqueue(VCA_SOCKET_0);
  
  int channel = 0; // Channel range can be 0 to 7. Each queue supports upto 8 channels
  while (1) {
    printf("\n\n Press Enter to enqueue burst of items\n\n");      
    getchar();
    for (i=0;i<32;i++) {
      buffer[i] = rand();
      printf("0x%lx ",buffer[i]);
    }
    items_sent = s_variable_multi_enqueue(q0_obj, buffer, 32, channel);
    printf("sent %d items\n\n",items_sent);
  }
}
