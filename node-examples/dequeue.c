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


int main()
{
	queue_object *q_obj;
	int channel = 0; // Channel number can be from 0 to 7. Each queue object supports upto 8 channels
	int items_recvd,j;
	unsigned long buffer[32];
	int sock = WITH_HOST;
	initialize_system("172.31.1.254", "5555", &sock);
	q_obj = init_dequeue(WITH_HOST);

        while (1)
        {
         items_recvd = s_variable_multi_dequeue(q_obj, buffer, 32, channel);
	 if (items_recvd != 0) {
 	 	for (j=0 ; j<items_recvd ; j++) {
		   printf("0x%lx ",buffer[j]);
	 	} 	  
	 	printf("\n\n");
         }	
	}
        free_queue(q_obj);
        return 0;
}
