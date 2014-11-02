/*
 * memory.h
 * Jake Indursky and Alex Salvatore
 * November 12, 2013
 * Interface for segmented memory -- Maximum 2^32 segments
 */



#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "assert.h"

typedef uint32_t Um_word;
typedef uint32_t Seg_id;

#define T Mem_T

struct T {
        int64_t num_segments;
    	int last_mapped; /*index of last_mapped segment */
    	int *unmapped; /*array of indices of any unmapped segments */
    	int last_unmapped; /*index for end of unmapped array */
    	int *lengths; /*array for lengths of segments */
        Um_word **segments;
};

typedef struct T *T;

/* initializes a new memory with the 0 segment of inputed length*/
//T Mem_new(int length);

/* returns a word from memory given an index and offset
 * unchecked run time error to access an unmapped space in memory */        
static inline Um_word get_word(Seg_id index, int offset, T mem)
{
        return mem->segments[index][offset];
}
/* returns the length of a given segment */
//int get_length(Seg_id index, T mem);

/* stores a word into a certain spot in memory 
 * unchecked run time error to access an unmapped space in memory */
//void put_word(Seg_id index, int offset, Um_word word, T mem);

/* 
 * maps a segment in memory of a given length, and returns the index of that
 * segment
 */
//Seg_id map_seg(int seg_length, T mem);

/* unmaps a particular segment */
//void unmap_seg(Seg_id index, T mem);

/* loads an instruction set from a given segment into segment 0 */
//void load_program(Seg_id to_copy, T mem);

/* frees the memory */
//void Memory_free(T *mem);

#define MAX_VAL_32_BIT 4294967296
#define EMPTY -1
#define INIT_VAL 10000



/*
 * Initializes a new Mem_T, and takes in the amount of instructions read
 * in by the program_loader, in order to allocate memory for segment 0
 * Allocates 1000 segments in memory to start te program
 */
static inline T Mem_new(int length)
{
        T mem = malloc(sizeof(*mem));

        mem->num_segments = INIT_VAL;
        mem->last_unmapped = EMPTY;             
        mem->last_mapped = 1;

        mem->lengths =  malloc(sizeof(int) * INIT_VAL);
        assert(mem->lengths);

        mem->segments = malloc(sizeof(Um_word *) * INIT_VAL);
        assert(mem->segments);

        mem->unmapped = malloc(sizeof(int) * INIT_VAL);
        assert(mem->unmapped);

        mem->segments[0] = malloc(sizeof(Um_word) * length);
        assert(mem->segments[0]);

        mem->lengths[0] = length;
        return mem;
}



/* returns length of a given segment */
static inline int get_length(Seg_id index, T mem)
{
        return mem->lengths[index];
}


static inline void put_word(Seg_id index, int offset, Um_word word, T mem)
{
        mem->segments[index][offset] = word;
}

/*
 * checks to see if there should be more space allocated for segments to be
 * mapped. This is the case if the last mapped segment is equal to the number
 * of segments that have been malloced and if there are no unmapped segments
 * in the middle of memory
 * If true, the function allocates memory for 1000 more segments, or up to 2^32
 */
static inline void mem_check(T mem) 
{
        int64_t num_segments = mem->num_segments;
         
          if (mem->last_unmapped == EMPTY && 
	      mem->last_mapped == num_segments) { 
	      		num_segments = num_segments * 2;
			if (num_segments > MAX_VAL_32_BIT) {
			    	num_segments = MAX_VAL_32_BIT;
			}
			mem->segments = realloc(mem->segments,
						sizeof(Um_word *) * 
						num_segments);
                        assert(mem->segments);
			mem->lengths = realloc(mem->lengths,
					       sizeof(int) * 
					       num_segments);
                        assert(mem->lengths);
			mem->unmapped = realloc(mem->unmapped,
						sizeof(int) * 
						num_segments);
                        assert(mem->unmapped);
                        mem->num_segments = num_segments;
        }
}

/* 
 * maps a segment in memory of a given length, and sets the words in that 
 * segment to 0
 */
static inline Seg_id map_seg(int seg_length, T mem)
{
        int i;
        int index = mem->last_mapped;
        mem_check(mem);
      
/*
 * if there is an unmapped segment in the middle of memory, that will be the 
 * segment to be mapped
 */
        if (mem->last_unmapped != EMPTY) {
      
                index = mem->unmapped[mem->last_unmapped];
                mem->last_unmapped--;
      
        } else {
        	mem->last_mapped++;
        }
	
	mem->segments[index] = malloc(sizeof(Um_word) * seg_length);
        assert(mem->segments[index]);

        for (i = 0; i < seg_length; i++) {
                mem->segments[index][i] = 0;
        }

        mem->lengths[index] = seg_length;
        return index;
}



/* Unmaps a specific segment and puts that index in the unmapped array */
static inline void unmap_seg(Seg_id index, T mem)
{
        free(mem->segments[index]);
	mem->segments[index] = NULL;
        mem->lengths[index] = 0;
        mem->last_unmapped++;
        mem->unmapped[mem->last_unmapped] = index;
}

/* loads a set of 32 bits from a given segment into segment 0 */
static inline void load_program(Seg_id to_copy, T mem)
{
    	/* if it loads itself, just return */  
    	if (to_copy == 0) {
            	return;
    	}
/*
 * if the segment to be loaded is larger than the 0 segment, allocate necessary
 * memory
 */
    	int i;
        int length = mem->lengths[to_copy];

    	if (length > mem->lengths[0]) {

		mem->segments[0] = realloc(mem->segments[0], sizeof(Um_word) 
				   	   * length);
		assert(mem->segments[0] != NULL);
        }

    	for (i = 0; i < length; i++) {
		mem->segments[0][i] = mem->segments[to_copy][i];
    	}
    	mem->lengths[0] = length;
}

/* 
 *frees each mapped segment in memory, and any other dynamically allocated
 * memory
 */
static inline void Memory_free(T *mem)
{
        int i;
        for(i = 0; i < (*mem)->last_mapped; i++) {
                if((*mem)->segments[i] != NULL) {
                        free((*mem)->segments[i]);
                }
        }
        free((*mem)->segments);
        free((*mem)->lengths);
        free((*mem)->unmapped);
        free(*mem);
}



#undef T


#endif
