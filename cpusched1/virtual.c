#include "oslabs.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>  

int get_earliest_arrival_idx(struct PTE page_table[TABLEMAX], int table_cnt)
{
	int idx = 0;
	bool firstpass = true;

	for (int i = 0; i < table_cnt; i++)
	{
		if (page_table[i].is_valid == 1 && firstpass)
		{
			firstpass = false;
			idx = i;
		}
		else if (page_table[i].is_valid && page_table[i].arrival_timestamp < page_table[idx].arrival_timestamp)
		{
			idx = i;
		}
	}

	return idx;
}

int process_page_access_fifo(
	struct PTE page_table[TABLEMAX], int* table_cnt, int page_number,
	int frame_pool[POOLMAX], int* frame_cnt, int current_timestamp)
{
	if (page_table[page_number].is_valid == 1)
	{
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].reference_count += 1;
		return page_table[page_number].frame_number;
	}

	// if the page being reference is not in memory
	// checks if there are any free frames -- process frame pool is not empty
	// the frame_cnt tells us that it is not empty if greater than 0
	if (*frame_cnt > 0)
	{
		int next_frame_idx = *frame_cnt - 1;
		int next_frame = frame_pool[next_frame_idx];

		page_table[page_number].frame_number = next_frame;
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].arrival_timestamp = current_timestamp;
		page_table[page_number].reference_count = +1;
		page_table[page_number].is_valid = 1;

		*frame_cnt -= 1;

		return next_frame;
	}

	// smallest arrival_timestamp; earliest
	// marks the page table entry as invalid, reference count to -1, sets frame number
	// to the newly freed frame

	int index = get_earliest_arrival_idx(page_table, *table_cnt);

	// evict the earliest idx
	int frnum = page_table[index].frame_number; 
	struct PTE temp_pte = { 0,-1,-1,-1,-1 };
	page_table[index] = temp_pte;

	page_table[page_number].arrival_timestamp = current_timestamp;
	page_table[page_number].last_access_timestamp = current_timestamp;
	page_table[page_number].reference_count = 1;
	page_table[page_number].frame_number = frnum;
	page_table[page_number].is_valid = 1;
	return frnum;	
};


int count_page_faults_fifo(
	struct PTE page_table[TABLEMAX], int table_cnt, int refrence_string[REFERENCEMAX],
	int reference_cnt, int frame_pool[POOLMAX], int frame_cnt)
{
	int faults = 0;
	int timestamp = 1000;
	for (int i = 0; i < reference_cnt; i++)
	{
		int page_number = refrence_string[i];

		if (page_table[page_number].is_valid)
		{
			page_table[page_number].last_access_timestamp = timestamp + i;
			page_table[page_number].reference_count++;
			continue;
		}

		faults++;

		if (frame_cnt > 0)
		{
			page_table[page_number].is_valid = 1;
			page_table[page_number].frame_number = frame_pool[--frame_cnt];
			page_table[page_number].arrival_timestamp = timestamp + i;
			page_table[page_number].last_access_timestamp = timestamp + i;
			page_table[page_number].reference_count = 1;
			continue;
		}

		int index = get_earliest_arrival_idx(page_table, table_cnt);
		page_table[page_number].frame_number = page_table[index].frame_number;
		page_table[page_number].is_valid = 1;
		page_table[page_number].arrival_timestamp = timestamp + i;
		page_table[page_number].last_access_timestamp = timestamp + i;
		page_table[page_number].reference_count = 1;
		struct PTE temp_pte = { 0,-1,-1,-1,-1 };
		page_table[index] = temp_pte;
	}

	return faults;
};


int process_page_access_lru(
	struct PTE page_table[TABLEMAX], int* table_cnt, int page_number,
	int frame_pool[POOLMAX], int* frame_cnt, int current_timestamp)
{
	if (page_table[page_number].is_valid)
	{
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].reference_count++;
		return page_table[page_number].frame_number;
	}

	if (*frame_cnt > 0)
	{
		page_table[page_number].is_valid = 1;
		page_table[page_number].frame_number = frame_pool[--(*frame_cnt)];
		page_table[page_number].arrival_timestamp = current_timestamp;
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].reference_count = 1;
		return page_table[page_number].frame_number;
	}

	int index = -1;
	int last_access_timestamp = 99999;
	for (int i = 0; i < *table_cnt; i++)
	{
		if (page_table[i].is_valid &&
			page_table[i].last_access_timestamp < last_access_timestamp)
		{
			last_access_timestamp = page_table[i].last_access_timestamp;
			index = i;
		}
	}
	page_table[page_number].frame_number = page_table[index].frame_number;
	page_table[page_number].is_valid = 1;
	page_table[page_number].arrival_timestamp = current_timestamp;
	page_table[page_number].last_access_timestamp = current_timestamp;
	page_table[page_number].reference_count = 1;
	struct PTE temp_pte = { 0,-1,-1,-1,-1 };
	page_table[index] = temp_pte;

	return page_table[page_number].frame_number;
};


int count_page_faults_lru(
	struct PTE page_table[TABLEMAX], int table_cnt,
	int refrence_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt)
{
	int faults = 0;
	int timestamp = 1;
	for (int i = 0; i < reference_cnt; i++)
	{
		int page_number = refrence_string[i];

		if (page_table[page_number].is_valid)
		{
			page_table[page_number].last_access_timestamp = timestamp + i;
			page_table[page_number].reference_count++;
			continue;
		}

		faults++;

		if (frame_cnt > 0)
		{
			page_table[page_number].is_valid = 1;
			page_table[page_number].frame_number = frame_pool[--frame_cnt];
			page_table[page_number].arrival_timestamp = timestamp + i;
			page_table[page_number].last_access_timestamp = timestamp + i;
			page_table[page_number].reference_count = 1;
			continue;
		}

		int index = -1;
		int last_access_timestamp = 99;
		for (int i = 0; i < table_cnt; i++)
		{
			if (page_table[i].is_valid &&
				page_table[i].last_access_timestamp < last_access_timestamp)
			{
				last_access_timestamp = page_table[i].last_access_timestamp;
				index = i;
			}
		}
		page_table[page_number].frame_number = page_table[index].frame_number;
		page_table[page_number].is_valid = 1;
		page_table[page_number].arrival_timestamp = timestamp + i;
		page_table[page_number].last_access_timestamp = timestamp + i;
		page_table[page_number].reference_count = 1;

		struct PTE temppte = { 0, -1,-1,-1,-1 };
		page_table[index] = temppte;
	}

	return faults;
};


int process_page_access_lfu(
	struct PTE page_table[TABLEMAX], int* table_cnt,
	int page_number, int frame_pool[POOLMAX], int* frame_cnt, int current_timestamp)
{
	if (page_table[page_number].is_valid)
	{
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].reference_count++;
		return page_table[page_number].frame_number;
	}

	if (*frame_cnt > 0)
	{
		page_table[page_number].is_valid = 1;
		page_table[page_number].frame_number = frame_pool[--(*frame_cnt)];
		page_table[page_number].arrival_timestamp = current_timestamp;
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].reference_count = 1;
		return page_table[page_number].frame_number;
	}

	int index = -1;
	int reference_count = 9999999;
	int arrival_timestamp = 9999999;
	for (int i = 0; i < *table_cnt; i++)
	{
		if (page_table[i].is_valid &&
			page_table[i].reference_count <= reference_count)
		{
			reference_count = page_table[i].reference_count;
			if (page_table[i].arrival_timestamp < arrival_timestamp)
			{
				arrival_timestamp = page_table[i].arrival_timestamp;
				index = i;
			}
		}
	}
	page_table[page_number].frame_number = page_table[index].frame_number;
	page_table[page_number].is_valid = 1;
	page_table[page_number].arrival_timestamp = current_timestamp;
	page_table[page_number].last_access_timestamp = current_timestamp;
	page_table[page_number].reference_count = 1;

	struct PTE temppte = { 0, -1,-1,-1,-1 };
	page_table[index] = temppte;

	return page_table[page_number].frame_number;
};


int count_page_faults_lfu(
	struct PTE page_table[TABLEMAX], int table_cnt,
	int refrence_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt)
{
	int faults = 0;
	int timestamp = 1000;
	for (int i = 0; i < reference_cnt; i++)
	{
		int page_number = refrence_string[i];

		if (page_table[page_number].is_valid)
		{
			page_table[page_number].last_access_timestamp = timestamp + i;
			page_table[page_number].reference_count++;
			continue;
		}

		faults++;

		if (frame_cnt > 0)
		{
			page_table[page_number].is_valid = 1;
			page_table[page_number].frame_number = frame_pool[--frame_cnt];
			page_table[page_number].arrival_timestamp = timestamp + i;
			page_table[page_number].last_access_timestamp = timestamp + i;
			page_table[page_number].reference_count = 1;
			continue;
		}

		int reference_count = 9999999;
		for (int j = 0; j < table_cnt; j++)
		{
			if (page_table[j].is_valid &&
				page_table[j].reference_count < reference_count)
			{
				reference_count = page_table[j].reference_count;
			}
		}
		int index = -1;
		int arrival_timestamp = 9999999;
		for (int j = 0; j < table_cnt; j++)
		{
			if (page_table[j].is_valid &&
				page_table[j].reference_count == reference_count &&
				page_table[j].arrival_timestamp < arrival_timestamp)
			{
				arrival_timestamp = page_table[j].arrival_timestamp;
				index = j;
			}
		}
		page_table[page_number].frame_number = page_table[index].frame_number;
		page_table[page_number].is_valid = 1;
		page_table[page_number].arrival_timestamp = timestamp + i;
		page_table[page_number].last_access_timestamp = timestamp + i;
		page_table[page_number].reference_count = 1;

		struct PTE temppte = { 0, -1,-1,-1,-1 };
		page_table[index] = temppte;
	}

	return faults;
};



