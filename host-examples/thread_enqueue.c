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
#define __USE_GNU
#include <sched.h>
#include <pthread.h>

#include "../mem-sharing-library/vca_mem.h"

queue_object *q_obj;

void *enqueue(void *channel) 
{
	unsigned long buffer[32] = {0,};
	unsigned long count = 0;
	int i = *((int *)channel);
	int j;
	cpu_set_t cpuset;
	const pthread_t pid = pthread_self();
	CPU_ZERO(&cpuset);
	CPU_SET(i, &cpuset);

	const int set_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
  	if (set_result != 0) {
    		printf("pthread_setaffinity_np error setting on id %d\n",i);
  	}

        printf("enqueueing on channel %d\n\n",i);      
        while (1)
        {
	 for (j = 0; j < 32; j++) {
		buffer[j] = count++;
	 }	

         while(s_variable_multi_enqueue(q_obj, buffer, 32, i) != 32); 

         if ((count % 32000032) == 0) {
                printf("sent %ld items on channel %d\n\n",count,i);
         }

	}
       return NULL;
}

int main()
{
        int i, status, sock=0;
	initialize_system("*", "5555", &sock);
	q_obj = init_enqueue(VCA_SOCKET_0);
        pthread_t t[8];
	int c[8] = {0,1,2,3,4,5,6,7};

	for (i=0;i<8;i++) {
        pthread_create(&t[i],NULL,enqueue,&c[i]);
	printf("Created thread %d \n", i);
	}

	for (i=0;i<8;i++) {
        pthread_join(t[i],NULL);
	printf("exited thread %d \n",i);
	}
  return 0;
}
