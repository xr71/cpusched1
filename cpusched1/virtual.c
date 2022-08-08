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

	int earliest_idx = get_earliest_arrival_idx(page_table, *table_cnt);

	// evict the earliest idx
	page_table[earliest_idx].is_valid = 0;
	page_table[earliest_idx].arrival_timestamp = -1;
	page_table[earliest_idx].last_access_timestamp = -1;
	page_table[earliest_idx].reference_count = -1;

	int frnum = page_table[earliest_idx].frame_number; 

	page_table[earliest_idx].frame_number = -1;
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

};


int process_page_access_lru(
	struct PTE page_table[TABLEMAX], int* table_cnt, int page_number,
	int frame_pool[POOLMAX], int* frame_cnt, int current_timestamp)
{

};


int count_page_faults_lru(
	struct PTE page_table[TABLEMAX], int table_cnt,
	int refrence_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt)
{

};


int process_page_access_lfu(
	struct PTE page_table[TABLEMAX], int* table_cnt,
	int page_number, int frame_pool[POOLMAX], int* frame_cnt, int current_timestamp)
{

};


int count_page_faults_lfu(
	struct PTE page_table[TABLEMAX], int table_cnt,
	int refrence_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt)
{

};



