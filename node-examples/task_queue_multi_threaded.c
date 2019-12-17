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

task_queue_opaque *opq = NULL;

void *enqueue(void *channel) 
{
        unsigned long buffer[2048] = {0x1234567890,};
        unsigned long ret = -1, total_recvd = 0;
	int i = *((int *)channel);
	cpu_set_t cpuset;
	const pthread_t pid = pthread_self();
	CPU_ZERO(&cpuset);
	CPU_SET(i, &cpuset);

	const int set_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
  	if (set_result != 0) {
    		printf("pthread_setaffinity_np error setting on id %d\n",i);
  	}
        printf("Running Thread on Channel %d \n",i);

        while (1)
        {
	  ret = vca_recv_task(opq, &total_recvd, buffer, i);
	  printf("Recv Task ret %ld total recvd %ld channel %d %lx\n",ret,total_recvd, i, buffer[0]);
	  buffer[0] = 0xdeadbeef;
	  buffer[2047] = 0xbeefdead;
	  ret = vca_submit_task(opq, sizeof(buffer),  buffer, i);
	  printf("Submit Task ret %ld \n",ret);

	}
       return NULL;
}

int main()
{
        int i, status,sock=WITH_HOST;
        pthread_t t[8];
	int c[8] = {0,1,2,3,4,5,6,7};

        opq = init_vca_task_system("172.31.1.254", "5555", &sock);

        printf("Opaque is %p\n",opq);
        assert(opq != NULL);

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
