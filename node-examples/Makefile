#executables = task_queue_multi_threaded
executables = read write enqueue dequeue thread_enqueue thread_dequeue task_queue_multi_threaded

target: $(executables)

% : %.c
	@echo "BUILD " $@  
	@gcc -g -O0 $< -lpthread -L../mem-sharing-library -lvca_mem `pkg-config libzmq --cflags --libs` -o $@	

.PHONY: clean

clean :
	@echo "CLEANING UP "
	@rm -rf $(executables) 
