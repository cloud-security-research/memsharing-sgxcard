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
	unsigned long buffer[2048] = {0x1234567890,};
        unsigned long ret = -1, total_recvd = 0;
	int task_id = 9;
	int sock = WITH_HOST;

	opq = init_vca_task_system(NULL, 0, &sock); //Slave, host socket

	printf("Opaque is %p\n",opq);
	assert(opq != NULL);
//	while (ret != NUM_ITEMS) {
//        ret = s_variable_multi_dequeue(opq->rx_q_objs[opq->num_socket], buffer, NUM_ITEMS, 0);  
//	}
        ret = recv_task(opq, &total_recvd, buffer, &task_id);
	printf("Recv Task ret %ld total recvd %ld task_id %d %lx\n",ret,total_recvd,task_id,buffer[0]);
        buffer[0] = 0xdeadbeef;
	buffer[2047] = 0xbeefdead;
        ret = submit_task(opq, 16384, buffer, 0);
        printf("Submit Task ret %ld \n",ret);


//	deinit_vca_task_system(opq, 0);
        return 0;
}
