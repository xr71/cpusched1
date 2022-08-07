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


void test_case2() 
{
	struct PCB ready_queue[QUEUEMAX];

	int queue_cnt = 3;
	int timestamp = 2;

	memset(ready_queue, 0, sizeof(ready_queue[QUEUEMAX]));

	struct PCB process = { 1,1,4,0,0,4,23 };
	ready_queue[0] = process;
	struct PCB process1 = { 2,1,4,0,0,4,22 };
	ready_queue[1] = process1;
	struct PCB process2 = { 3,1,4,0,0,4,24 };
	ready_queue[2] = process2;

	struct PCB res;
	res = handle_process_completion_pp(ready_queue, &queue_cnt, timestamp);

	print_process(res);
}


int main(void) {
	// test_case1();
	// test_case2();

	/*
	Test Failed: 'Expecting test 3 Memory Map Count value of 4 and received 5 You 
	returned too many memory bloks.  Here is your memory map 
	[start_address: 0 end_address: 19 segment_size: 20 process_id: 10]
	[start_address: 20 end_address: 49 segment_size: 30 process_id: 40]
	[start_address: 40 end_address: 49 segment_size: 10 process_id: 20]
	[start_address: 50 end_address: 1023 segment_size: 974 process_id: 30]
	[start_address: 50 end_address: 39 segment_size: -10 process_id: 0]' != 'Passed All Tests'
	*/
}