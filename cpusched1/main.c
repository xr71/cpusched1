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

void test_free_mem_block() 
{

	struct MEMORY_BLOCK map[10];
	struct MEMORY_BLOCK map0 = {0,7, 8, 12};
	struct MEMORY_BLOCK map1 = {8,15,8,0};
	struct MEMORY_BLOCK map2 = {16,23,8,13};
	struct MEMORY_BLOCK map3 = {24,27,4,0};
	struct MEMORY_BLOCK map4 = {28,29,2,11};
	map[0] = map0;
	map[1] = map1;
	map[2] = map2;
	map[3] = map3;
	map[4] = map4;
	int mapcnt = 5;

	release_memory(map[2], map, &mapcnt);

	for (int i = 0; i < mapcnt; i++)
	{
		printf("%d %d %d %d \n",
			map[i].start_address,
			map[i].end_address,
			map[i].process_id,
			map[i].segment_size);
	}
}


void test_next_fit_block()
{
	struct MEMORY_BLOCK map[10];
	struct MEMORY_BLOCK map0 = {0,1023,1024,0};
	map[0] = map0;
	int mapcnt = 1;

	/*
	Test Failed: 'Expecting test 2 Memory Map Count value of 4 and received 5 
	You returned too many memory bloks.  Here is your memory map 
	[start_address: 0 end_address: 19 segment_size: 20 process_id: 10]
	[start_address: 20 end_address: 39 segment_size: 20 process_id: 40]
	[start_address: 40 end_address: 39 segment_size: 0 process_id: 0]
	[start_address: 50 end_address: 1023 segment_size: 974 process_id: 30]
	[start_address: 0 end_address: 0 segment_size: -1976127328 process_id: 32767]' != 'Passed All Tests'
	*/


	/*
	Test Failed: 'Missing Block in memory_map for test 2 [20,39,20,40].  
	Here is the memory map. 
	[start_address: 0 end_address: 19 segment_size: 20 process_id: 10]
	[start_address: 20 end_address: 39 segment_size: 20 process_id: 0]
	[start_address: 40 end_address: 49 segment_size: 10 process_id: 20]
	[start_address: 50 end_address: 1023 segment_size: 974 process_id: 30]' != 'Passed All Tests'
	*/

	next_fit_allocate(10, map, &mapcnt, 32, 0);

	for (int i = 0; i < mapcnt; i++)
	{
		printf("%d %d %d %d %d \n",
			map[i].start_address,
			map[i].end_address,
			map[i].process_id,
			map[i].segment_size,
			i);
	}

}


void test_handle_rc_sstf()
{
	struct RCB request_queue[QUEUEMAX];
	struct RCB r1 = { 1, 72, 45, 45, 1 };
	struct RCB r2 = { 2, 71, 47, 47, 2 };
	struct RCB r3 = { 3, 73, 43, 43, 3 };
	request_queue[0] = r1;
	request_queue[1] = r2;
	request_queue[2] = r3;
	int queue_cnt = 3;
	int current_cylinder = 48;

	handle_request_completion_sstf(request_queue, &queue_cnt, current_cylinder);

}


void test_ppf_fifo()
{
	struct PTE zero = {0,-1,-1,-1,-1};
    struct PTE one = {0,-1,-1,-1,-1};
    struct PTE two = {1,10,3,3,1};
    struct PTE three = {0,-1,-1,-1,-1};
    struct PTE four = {0,-1,-1,-1,-1};
    struct PTE five = {1,20,2,4,2};
    struct PTE six = {0,-1,-1,-1,-1};
    struct PTE seven = {1,30,1,1,1};
    struct PTE page_table[] = {zero, one, two, three, four, five, six, seven};

    // int reference_string[] = {0, 3, 2, 6, 3, 4, 5, 2, 6, 4, 5};
    // int reference_cnt = 11;
    int table_cnt = 8;
    int page_number = 0;
    int frame_pool[POOLMAX];
    int frame_cnt = 0;

    int frame_output = process_page_access_fifo(
		page_table, &table_cnt, 0, frame_pool, &frame_cnt, 12);

    printf("The page table contains the following.\n");
    for (int i = 0; i < table_cnt; i++)
    {
        printf("Page #: %d IV: %d FN: %d ATS: %d LATS: %d RC: %d\n",i, page_table[i].is_valid, page_table[i].frame_number, page_table[i].arrival_timestamp, page_table[i].last_access_timestamp, page_table[i].reference_count);
    }
	printf("Frame %i\n", frame_output);


	/*
	Test Failed: 'Missing entry in page table for test 1 [0,-1,-1,-1,-1] page number 7.  
	Here is the page table. 
	[is_valid:  1 frame_number: 30 arrival_timestamp: 12 last_access_timestamp: 12 reference_count: 1]
	[is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
	[is_valid:  1 frame_number: 10 arrival_timestamp: 3 last_access_timestamp: 3 reference_count: 1]
	[is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
	[is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
	[is_valid:  1 frame_number: 20 arrival_timestamp: 2 last_access_timestamp: 4 reference_count: 2]
	[is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
	[is_valid:  0 frame_number: -1 arrival_timestamp: 0 last_access_timestamp: 0 reference_count: 0]' != 'Passed All Tests'
	*/


	/*
	Test Failed: 'Missing entry in page table for test 3 [1,10,15,15,1] page number 3.  
	Here is the page table. 
	[is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
	[is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
	[is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
	[is_valid:  1 frame_number: 10 arrival_timestamp: -1 last_access_timestamp: 15 reference_count: 1]
	[is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
	[is_valid:  1 frame_number: 20 arrival_timestamp: 2 last_access_timestamp: 4 reference_count: 2]
	[is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
	[is_valid:  1 frame_number: 30 arrival_timestamp: 1 last_access_timestamp: 1 reference_count: 1]' != 'Passed All Tests'

	*/
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

	// test_free_mem_block();

	/*
	Missing Block in memory_map for test 2 [0,19,20,10].  
	Here is the memory map. 
	[start_address: 0 end_address: 19 segment_size: 20 process_id: 40]
	[start_address: 20 end_address: 39 segment_size: 20 process_id: 0]
	[start_address: 40 end_address: 49 segment_size: 10 process_id: 20]
	[start_address: 50 end_address: 1023 segment_size: 974 process_id: 30]' != 'Passed All Tests'
	*/

	// test_next_fit_block();

	test_ppf_fifo();
}