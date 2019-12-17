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
	void *ptr = NULL;
	transfer_mapping map;
	int socket = WITH_HOST, mapping_type = 0;
	unsigned long remote_physical = 0, size = 0;

	initialize_system("172.31.1.254", "5555", &socket);

	recv_remote_data(&socket, &remote_physical, &size, &mapping_type);
	
	map.physical_addr = remote_physical;
	map.size = size;
	map.socket = socket;
	map.mapping_type = mapping_type;
	
	ptr = map_remote_memory(&map, WITH_HOST,4096,SHMEM_MAP_NUMBER_1);
	assert(ptr != NULL);
	while(1) {
		printf("\n\nType a short message and press enter\n");
		fgets(ptr,32,stdin);	
        }
	
	return 0;
}
