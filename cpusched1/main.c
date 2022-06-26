#include <stdio.h>
#include <stdlib.h>
#include "oslabs.h"


void print_process(struct PCB process) {
	printf("{ %d %d %d %d %d %d %d }\n", 
		process.process_id,
		process.arrival_timestamp,
		process.total_bursttime,
		process.execution_starttime,
		process.execution_endtime,
		process.remaining_bursttime,
		process.process_priority
	);
}

void test_case1() 
{
	struct PCB ready_queue[QUEUEMAX];

	struct PCB current_process = { 1, 1, 4, 1, 5, 4, 8 };
	struct PCB new_process     = { 2, 2, 3, 0, 0, 3, 6 };

	struct PCB process;
	int queue_cnt = 0;
	int timestamp = 2;

	memset(ready_queue, 0, sizeof(ready_queue[QUEUEMAX]));

	process = handle_process_arrival_pp(ready_queue, &queue_cnt, current_process, new_process, timestamp);

	print_process(process);

	for (int i = 0; i < queue_cnt; i++)
	{
		print_process(ready_queue[i]);
	}
}



int main(void) {
	test_case1();
}