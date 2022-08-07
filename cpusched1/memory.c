#include "oslabs.h"
#include <stdio.h>
#include <stdbool.h>


struct MEMORY_BLOCK null_memblock = {0,0,0,0};

bool isNullMemBlock(struct MEMORY_BLOCK mb)
{
	if (mb.start_address == 0 &&
		mb.end_address == 0 &&
		mb.segment_size == 0 &&
		mb.process_id == 0)
	{
		return true;
	}

	return false;
}


struct MEMORY_BLOCK best_fit_allocate(
	int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int* map_cnt, int process_id)
{

	/*
		whose size is closest to the requested size; request size = 10; address = [0, 1023]
	*/

	int closest_idx = -1;
	int diff = 16384;
	int best_fit_size = 0;

	for (int i = 0; i < *map_cnt; i++)
	{
		int req_size_diff_from_memmap = memory_map[i].segment_size - request_size;
		struct MEMORY_BLOCK block = memory_map[i];	

		if (block.process_id == 0) {
			// the free block is found to be exactly the same size as the request size
			if (block.segment_size == request_size)
            {
				// simply update the process id and return this mem block
                block.process_id = process_id;
                memory_map[i] = block;
                return block;
            } 

			if (req_size_diff_from_memmap < diff) 
			{
				diff = req_size_diff_from_memmap;
                best_fit_size = block.segment_size;
                closest_idx = i;
            }

			if (req_size_diff_from_memmap < 0)
			{
				closest_idx = -1;
			}
		}
	}

	// if nothing is found (no free block) then just return the nullblock
	if (closest_idx == -1)
	{
		return null_memblock;
	}


	// if previous conditions fail; we need to split the memory map into two
	struct MEMORY_BLOCK split1, split2;
	//split 1 becomes the closest idx of the memory map
	split1.start_address = memory_map[closest_idx].start_address;
	split1.end_address = memory_map[closest_idx].start_address + request_size - 1;
	split1.process_id = process_id;
	split1.segment_size = request_size;

	// split 2 now needs to be shifted and marked as 0
	split2.segment_size = memory_map[closest_idx].segment_size - split1.segment_size;
    split2.process_id = 0;
    split2.start_address = split1.end_address + 1;
    split2.end_address = split2.start_address + split2.segment_size - 1;

	// finallly update the memory map, counter, and return
	memory_map[*map_cnt] = split2;
	memory_map[closest_idx] = split1;
	*map_cnt += 1;
	
	return split1;
};


struct MEMORY_BLOCK first_fit_allocate(
	int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int* map_cnt, int process_id)
{

};


struct MEMORY_BLOCK worst_fit_allocate(
	int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int* map_cnt, int process_id)
{

};


struct MEMORY_BLOCK next_fit_allocate(
	int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int* map_cnt, int process_id, int last_address)
{

};


void release_memory(
	struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX], int* map_cnt)
{

};
