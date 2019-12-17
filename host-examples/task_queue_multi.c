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
	task_queue_opaque *opq = NULL;
	unsigned long buffer[2048] = {0xcafeface,};
        unsigned long ret = -1;
	int task_id = 9;
        long total_recvd;
	int i=0, sock=0;

	opq = init_host_task_system(NULL, "*", "5555", &sock); //vca_socket 0
	printf("Opaque is %p\n",opq);
	assert(opq != NULL);
    while (1) {
		getchar();
	  	ret = host_submit_task(opq, 16384, buffer, -1); //to any destination
	  	//	  printf("Submit Task ret %ld \n",ret);
	  	ret = host_recv_task(opq, &total_recvd, buffer, &task_id);
	  	//	  printf("recv Task ret %ld total recvd %ld task_id %d 0x%lx 0x%lx\n",ret,total_recvd,task_id,buffer[0],buffer[2047]);
	  	i++;
    }

//	deinit_vca_task_system(opq, 1);
      

        return 0;
}
