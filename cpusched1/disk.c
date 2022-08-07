#include "oslabs.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>  


struct RCB null_rcb = {0,0,0,0,0};

bool isNullRCB(struct RCB rcb)
{
	if (rcb.address == 0 &&
		rcb.arrival_timestamp == 0 &&
		rcb.cylinder == 0 &&
		rcb.process_id == 0 &&
		rcb.request_id == 0)
	{
		return true;
	}

	return false;
}



struct RCB handle_request_arrival_fcfs(
	struct RCB request_queue[QUEUEMAX], 
	int* queue_cnt, 
	struct RCB current_request,
	struct RCB new_request, 
	int timestamp)
{
	if (isNullRCB(current_request)) return new_request;

	request_queue[*queue_cnt] = new_request;
	*queue_cnt += 1;
	return current_request;
};


struct RCB handle_request_completion_fcfs(
	struct RCB request_queue[QUEUEMAX], int* queue_cnt)
{
	if (*queue_cnt <= 0)
	{
		return null_rcb;
	}

	// otherwise find the earliest arrival time
	int earliest_arrival = 9999999;
	int earliest_idx = 0;
	for (int i = 0; i < *queue_cnt; i++)
	{
		if (request_queue[i].arrival_timestamp < earliest_arrival)
		{
			earliest_arrival = request_queue[i].arrival_timestamp;
			earliest_idx = i;
		}
	}

	struct RCB block = request_queue[earliest_idx];

    for (int i = earliest_idx; i < *queue_cnt - 1; i++){
        request_queue[i] = request_queue[i + 1];
	}
	*queue_cnt-=1;
	return block;
};


struct RCB handle_request_arrival_sstf(
	struct RCB request_queue[QUEUEMAX], int* queue_cnt, struct RCB current_request,
	struct RCB new_request, int timestamp)
{
	if (isNullRCB(current_request)) return new_request;

	request_queue[*queue_cnt] = new_request;
	*queue_cnt += 1;
	return current_request;
};


struct RCB handle_request_completion_sstf(
	struct RCB request_queue[QUEUEMAX], int* queue_cnt, int current_cylinder)
{
	if (*queue_cnt <= 0)
	{
         return null_rcb;
    }

	struct RCB block[QUEUEMAX];

	int closest = 9999999;
	int earliest_arrival = 999999;
	int closest_idx = 0;
	int number_of_closest_cyls = 0;

	for (int i = 0; i < *queue_cnt; i++) {
		if (abs(current_cylinder - request_queue[i].cylinder) <= closest)
		{
			if (abs(current_cylinder - request_queue[i].cylinder) < closest)
			{
				closest = abs(current_cylinder - request_queue[i].cylinder);
				earliest_arrival = request_queue[i].arrival_timestamp;
				closest_idx = i;
			}
			else if (request_queue[i].arrival_timestamp < earliest_arrival)
			{
				earliest_arrival = request_queue[i].arrival_timestamp;
				closest_idx = i;
			}
		}
	}

	struct RCB sstf_block = request_queue[closest_idx];

    for (int i = closest_idx; i < *queue_cnt - 1; i++){
        request_queue[i] = request_queue[i + 1];
	}
	*queue_cnt-=1;
	return sstf_block;
};


struct RCB handle_request_arrival_look(
	struct RCB request_queue[QUEUEMAX], int* queue_cnt, struct RCB current_request,
	struct RCB new_request, int timestamp)
{
	if (isNullRCB(current_request)) return new_request;

	request_queue[*queue_cnt] = new_request;
	*queue_cnt += 1;
	return current_request;
};


struct RCB handle_request_completion_look(
	struct RCB request_queue[QUEUEMAX], int* queue_cnt, int current_cylinder, int scan_direction)
{
	if (*queue_cnt <= 0)
	{
		return null_rcb;
	}


};


