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

#include "../mem-sharing-library/vca_mem.h"

int
main(int argc, char *argv[])
{
	void *ptr = memalign(4096,4096);
	char buffer[32] = {0,};
	int sock = WITH_HOST;
	mlock(ptr,4096);

	initialize_system("172.31.1.254", "5555", &sock);
	
	share_local_memory(WITH_HOST, ptr, 4096, REMOTE_WILL_WRITE);

	while(1) {
		printf("\n\nPress enter to read the next message\n");
		getchar();
	 	memcpy(buffer,ptr,32);
	 	printf("%s\n",buffer);
	}
	  
	
	return 0;
}
