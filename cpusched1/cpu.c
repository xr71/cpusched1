#include "oslabs.h"
#include <stdio.h>
#include <stdbool.h>


struct PCB null_pcb = { 0,0,0,0,0,0 };

bool isNullProcess(struct PCB process)
{
	if (process.process_id == 0 &&
		process.arrival_timestamp == 0 &&
		process.process_priority == 0 &&
		process.total_bursttime == 0 &&
		process.remaining_bursttime == 0 &&
		process.execution_starttime == 0 &&
		process.execution_endtime ==0)
	{
		return true;
	}

	return false;
}

struct PCB handle_process_arrival_pp(
	struct PCB ready_queue[QUEUEMAX], 
	int* queue_cnt, 
	struct PCB current_process, 
	struct PCB new_process, 
	int timestamp) 
{
	if (isNullProcess(current_process))
	{
		new_process.execution_starttime = timestamp;
		new_process.execution_endtime = timestamp + new_process.total_bursttime;
		new_process.remaining_bursttime = new_process.total_bursttime;

		return new_process;
	}


	// higher priority is lower integer for priority field
	// i.e. priority of 6 is higher than priority of 8
	if (new_process.process_priority < current_process.process_priority)
	{
		new_process.execution_starttime = timestamp;
		new_process.execution_endtime = timestamp + new_process.total_bursttime;
		new_process.remaining_bursttime = new_process.total_bursttime;
		current_process.remaining_bursttime = current_process.execution_endtime - timestamp;
		current_process.execution_endtime = 0;
		ready_queue[*queue_cnt] = current_process;
		*queue_cnt += 1;

		return new_process;
	}
	else
	{
		new_process.execution_endtime = 0;
		new_process.execution_starttime = 0;
		new_process.remaining_bursttime = new_process.total_bursttime;
		ready_queue[*queue_cnt] = new_process;
		*queue_cnt += 1;

		return current_process;
	}


	return new_process;
}


struct PCB handle_process_completion_pp(
	struct PCB ready_queue[QUEUEMAX], 
	int* queue_cnt, 
	int timestamp)
{
	return null_pcb;
}


struct PCB handle_process_arrival_srtp(
	struct PCB ready_queue[QUEUEMAX], 
	int* queue_cnt, 
	struct PCB current_process, 
	struct PCB new_process, 
	int time_stamp)
{
	return new_process;
}


struct PCB handle_process_completion_srtp(
	struct PCB ready_queue[QUEUEMAX], 
	int* queue_cnt, 
	int timestamp)
{
	
	return null_pcb;
}


struct PCB handle_process_arrival_rr(
	struct PCB ready_queue[QUEUEMAX], 
	int* queue_cnt, 
	struct PCB current_process, 
	struct PCB new_process, 
	int timestamp, 
	int time_quantum)
{
	return new_process;
}


struct PCB handle_process_completion_rr(
	struct PCB ready_queue[QUEUEMAX], 
	int* queue_cnt, 
	int timestamp, 
	int time_quantum)
{
	return null_pcb;
}
