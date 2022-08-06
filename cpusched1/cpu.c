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
	if (*queue_cnt == 0)
	{
		return null_pcb;
	}

	// find next highest priority - which is lowest int for priority
	int highest_priority = ready_queue[0].process_priority;
	int highest_index = 0;
	for (int i = 1; i < *queue_cnt; i++)
	{
		if (ready_queue[i].process_priority < highest_priority)
		{
			highest_priority = ready_queue[i].process_priority;
			highest_index = i;
		}
	}
	// printf("highest index is:  %i \n", highest_index);

	struct PCB next_highest_process = ready_queue[highest_index];
	for (int i = highest_index; i < *queue_cnt - 1; i++)
	{
		ready_queue[i] = ready_queue[i + 1];
	}

	*queue_cnt -= 1;
	next_highest_process.execution_starttime = timestamp;
	next_highest_process.execution_endtime = timestamp + next_highest_process.remaining_bursttime;

	return next_highest_process;
}


struct PCB handle_process_arrival_srtp(
	struct PCB ready_queue[QUEUEMAX], 
	int* queue_cnt, 
	struct PCB current_process, 
	struct PCB new_process, 
	int time_stamp)
{
	/*
	If there is no currently-running process (i.e., the third argument is the NULLPCB), 
	then the method returns the PCB of the newly-arriving process, indicating that 
	it is the process to execute next. In this case, the PCB of the new process 
	is modified so that the execution start time is set to the current timestamp, 
	the execution end time is set to the sum of the current timestamp and the 
	total burst time and the remaining burst time is set to the total burst time. 
	*/
	if (isNullProcess(current_process))
	{
		new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
	}

	/*
	the method compares the remaining burst time of the currently-running 
	process and the total burst time of the newly-arriving process. If the new 
	process does not have a shorter burst time, then its PCB is simply added to 
	the ready queue and the return value is the PCB of the currently running process. 
	As the newly-arriving process is added to the ready queue, its execution start 
	time and execution end time are set to 0, and the remaining burst time is set 
	to the total burst time. 
	*/
	if (current_process.remaining_bursttime <= new_process.total_bursttime)
	{

		new_process.execution_starttime = 0;
		new_process.execution_endtime = 0;
		new_process.remaining_bursttime = new_process.total_bursttime;
		ready_queue[*queue_cnt] = new_process;
		*queue_cnt += 1;
		return current_process;
	}

	new_process.execution_starttime = time_stamp;
	new_process.execution_endtime = time_stamp + new_process.total_bursttime;
	new_process.remaining_bursttime = new_process.total_bursttime;

	current_process.remaining_bursttime -= (time_stamp - current_process.execution_starttime);
	current_process.execution_starttime = 0;
	current_process.execution_endtime = 0;
	ready_queue[*queue_cnt] = current_process;
	*queue_cnt += 1;

	return new_process;
}


struct PCB handle_process_completion_srtp(
	struct PCB ready_queue[QUEUEMAX], 
	int* queue_cnt, 
	int timestamp)
{
	
	if (*queue_cnt == 0)
	{
		return null_pcb;
	}

	/*
	Otherwise, the method finds the PCB of the process in the ready queue with the 
	smallest remaining burst time, removes this PCB from the ready queue and returns 
	it. Before returning the PCB of the next process to execute, it is modified to 
	set the execution start time as the current timestamp and the execution end time 
	as the sum of the current timestamp and the remaining burst time. 
	*/
	int lowest_remaining_bursttime_index = 0;
	int lowest_remaining_bursttime = ready_queue[0].remaining_bursttime;
	for (int i = 1; i < *queue_cnt; i++)
	{
		if (ready_queue[i].remaining_bursttime < lowest_remaining_bursttime)
		{
			lowest_remaining_bursttime = ready_queue[i].remaining_bursttime;
			lowest_remaining_bursttime_index = i;
		}
	}

	struct PCB next_lowest_bursttime_proc = ready_queue[lowest_remaining_bursttime_index];
	for (int i = lowest_remaining_bursttime_index; i < *queue_cnt - 1; i++)
	{
		ready_queue[i] = ready_queue[i + 1];
	}

	*queue_cnt -= 1;
	next_lowest_bursttime_proc.execution_starttime = timestamp;
	next_lowest_bursttime_proc.execution_endtime = timestamp + next_lowest_bursttime_proc.remaining_bursttime;

	return next_lowest_bursttime_proc;
}


struct PCB handle_process_arrival_rr(
	struct PCB ready_queue[QUEUEMAX], 
	int* queue_cnt, 
	struct PCB current_process, 
	struct PCB new_process, 
	int timestamp, 
	int time_quantum)
{
	/*
	If there is no currently-running process (i.e., the third argument is the NULLPCB), 
	then the method returns the PCB of the newly-arriving process, indicating that 
	it is the process to execute next. In this case, the PCB of the new process is 
	modified so that the execution start time is set to the current timestamp, the 
	execution end time is set to the sum of the current timestamp and the smaller of 
	the time quantum and the total burst time. The remaining burst time is set to the 
	total burst time. 
	*/
	if (isNullProcess(current_process))
	{
		new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + (new_process.total_bursttime > time_quantum ? time_quantum : new_process.total_bursttime);
        new_process.remaining_bursttime = new_process.total_bursttime;

        return new_process;
	}

	/*
	If there is a currently-running process, the method simply adds the PCB of the 
	newly-arriving process to the ready queue and the return value is the PCB of the 
	currently running process. As the newly-arriving process is added to the ready 
	queue, its execution start time and execution end time are set to 0, and the 
	remaining burst time is set to the total burst time. 
	*/
	new_process.execution_starttime = 0;
    new_process.execution_endtime = 0;
    new_process.remaining_bursttime = new_process.total_bursttime;
	
	ready_queue[*queue_cnt] = new_process;
	*queue_cnt += 1;

    return current_process;
}


struct PCB handle_process_completion_rr(
	struct PCB ready_queue[QUEUEMAX], 
	int* queue_cnt, 
	int timestamp, 
	int time_quantum)
{
	/*
	If the ready queue is empty, the method returns the NULLPCB, indicating that there 
	is no process to execute next. 
	*/
	if (*queue_cnt == 0)
	{
		return null_pcb;
	}


	/*
	Otherwise, the method finds the PCB of the process 
	in the ready queue with the earliest arrival time, removes this PCB from the ready 
	queue and returns it. Before returning this PCB, it is modified to set the execution 
	start time as the current timestamp and the execution end time as the sum of the 
	current timestamp and the smaller of the time quantum and the remaining burst time. 
	*/
	int earliest_arr_time_index = 0;
	int earliest_arr_time = ready_queue[0].arrival_timestamp;
	for (int i = 1; i < *queue_cnt; i++)
	{
		if (ready_queue[i].arrival_timestamp < earliest_arr_time)
		{
			earliest_arr_time = ready_queue[i].arrival_timestamp;
			earliest_arr_time_index = i;
		}
	}

	struct PCB next_earliest_arr_proc = ready_queue[earliest_arr_time_index];
	for (int i = earliest_arr_time_index; i < *queue_cnt - 1; i++)
	{
		ready_queue[i] = ready_queue[i + 1];
	}

	*queue_cnt -= 1;
	next_earliest_arr_proc.execution_starttime = timestamp;
	next_earliest_arr_proc.execution_endtime = timestamp + (
		next_earliest_arr_proc.remaining_bursttime > time_quantum ? 
		time_quantum : next_earliest_arr_proc.remaining_bursttime);
    return next_earliest_arr_proc;
}
