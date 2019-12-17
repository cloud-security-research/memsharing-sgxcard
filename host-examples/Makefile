executables=read write enqueue dequeue thread_enqueue thread_dequeue task_queue_multi

CFLAGS=`pkg-config libzmq --cflags --libs`

target: $(executables)

% : %.c
	@echo "BUILD " $@  
	@gcc -g -O0 $< -lpthread -L../mem-sharing-library -lvca_mem  $(CFLAGS) -o $@

% : %.cpp
	@echo "BUILD " $@  
	@g++ -g -O0 -std=c++11 $< -lpthread -L../mem-sharing-library -lvca_mem $(CFLAGS) -o $@

.PHONY: clean

clean :
	@echo "CLEANING UP "
	@rm -rf $(executables) 
