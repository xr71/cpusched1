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
	int closest_idx = -1;
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
			// immediately return the first block that fits
			if (block.segment_size > request_size) 
			{
                closest_idx = i;

				// split and return
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
};


struct MEMORY_BLOCK worst_fit_allocate(
	int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int* map_cnt, int process_id)
{
	int worst_idx = -1;
	int diff = -1;
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

			if (req_size_diff_from_memmap > diff) 
			{
				diff = req_size_diff_from_memmap;
                best_fit_size = block.segment_size;
                worst_idx = i;
            }

			if (req_size_diff_from_memmap < 0)
			{
				worst_idx = -1;
			}
		}
	}

	// if nothing is found (no free block) then just return the nullblock
	if (worst_idx == -1)
	{
		return null_memblock;
	}

	// if previous conditions fail; we need to split the memory map into two
	struct MEMORY_BLOCK split1, split2;
	//split 1 becomes the closest idx of the memory map
	split1.start_address = memory_map[worst_idx].start_address;
	split1.end_address = memory_map[worst_idx].start_address + request_size - 1;
	split1.process_id = process_id;
	split1.segment_size = request_size;

	// split 2 now needs to be shifted and marked as 0
	split2.segment_size = memory_map[worst_idx].segment_size - split1.segment_size;
    split2.process_id = 0;
    split2.start_address = split1.end_address + 1;
    split2.end_address = split2.start_address + split2.segment_size - 1;

	// finallly update the memory map, counter, and return
	memory_map[*map_cnt] = split2;
	memory_map[worst_idx] = split1;
	*map_cnt += 1;
	
	return split1;
};


struct MEMORY_BLOCK next_fit_allocate(
	int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int* map_cnt, int process_id, 
	int last_address)
{
	int next_fit_idx = -1;
    int last_address_idx = 0;

	// find the index position of the last_address
    for (int i = 0; i < *map_cnt; i++){
        if(memory_map[i].start_address >= last_address){
            last_address_idx = i;
			break;
        }
    }

    for (int i = 0; i < *map_cnt; i++)
    {
        int j = (i + last_address) % *map_cnt;
        struct MEMORY_BLOCK block = memory_map[j];
        if (block.process_id == 0) {
            if (block.segment_size == request_size)
            {
                block.process_id = process_id;
                memory_map[j] = block;
                return block;
            } else if (block.segment_size > request_size) {
                next_fit_idx = j;
                break;
            }
        }
    }

    if (next_fit_idx == -1) {
        return null_memblock;
    }


	// if previous conditions fail; we need to split the memory map into two
	struct MEMORY_BLOCK split1, split2;
	//split 1 becomes the closest idx of the memory map
	split1.start_address = memory_map[next_fit_idx].start_address;
	split1.end_address = memory_map[next_fit_idx].start_address + request_size - 1;
	split1.process_id = process_id;
	split1.segment_size = request_size;

	// split 2 now needs to be shifted and marked as 0
	split2.segment_size = memory_map[next_fit_idx].segment_size - split1.segment_size;
    split2.process_id = 0;
    split2.start_address = split1.end_address + 1;
    split2.end_address = split2.start_address + split2.segment_size - 1;

	// finallly update the memory map, counter, and return
	memory_map[*map_cnt] = split2;
	memory_map[next_fit_idx] = split1;
	*map_cnt += 1;
	
	return split1;
};


int free_block_idx (struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX], int* map_cnt)
{
	for (int i = 0; i < *map_cnt; i++){
        struct MEMORY_BLOCK curr = memory_map[i];
        if (curr.start_address == freed_block.start_address 
			&& curr.end_address == freed_block.end_address) return i;
    }

	return -1;
}

void release_memory(
	struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX], int* map_cnt)
{
	int block_to_free_idx = free_block_idx(freed_block, memory_map, map_cnt);

	if (block_to_free_idx >= 0) {
        int leftblock_idx = block_to_free_idx - 1;
		struct MEMORY_BLOCK leftblock = memory_map[leftblock_idx];

        int rightblock_idx = block_to_free_idx + 1;
		struct MEMORY_BLOCK rightblock = memory_map[rightblock_idx];

        int start_address = freed_block.start_address;
        int end_address = freed_block.end_address;
        int segment_size = freed_block.segment_size;
        int start_merge_idx = block_to_free_idx;
        int end_merge_idx = block_to_free_idx;

        if(leftblock_idx >= 0 && leftblock.process_id == 0){
            start_address = leftblock.start_address;
            segment_size += leftblock.segment_size;
            start_merge_idx = leftblock_idx;
        }

        if(rightblock_idx < *map_cnt && rightblock.process_id == 0){
            end_address = rightblock.end_address;
            segment_size += rightblock.segment_size;
            end_merge_idx = rightblock_idx;
        }

        int blocks_to_merge_cnt = end_merge_idx - start_merge_idx;
        struct MEMORY_BLOCK merged_block = {
            .start_address = start_address,
            .end_address = end_address,
            .segment_size = segment_size,
            .process_id = 0
        };

		// insert the newly created merged block back into memory map
        memory_map[start_merge_idx] = merged_block;

        *map_cnt -= blocks_to_merge_cnt;

		// shift the remaining blocks in the array
        for(int i = start_merge_idx + 1; i < *map_cnt; i++){
            memory_map[i] = memory_map[i + blocks_to_merge_cnt];
        }
    }
};
