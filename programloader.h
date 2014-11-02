/*
 * programloader.c
 * Jake Indursky and Alex Salvatore
 * November 12, 2013
 * interface that loads a program from a file into a new memory module
 */



#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include "bitpack.h"
#include "memory.h"

/*  reads a binary file and stores it in memory */

#define MAX_32_BIT_VAL 4294967296

/*
 * Determines the length of the file in terms of 32 bit words, and uses that
 * length to initialize memory, with the length used as the length for segment
 * 0. 
 */
static inline Um_word create_word (uint32_t word, unsigned width, 
                                unsigned lsb, uint32_t value)
{
        uint32_t mask = ~0;
        mask = mask >> (32 - width);
        mask = mask << lsb;
        mask = ~mask;
        word &= mask;
        value = value << (32 - width);
        value = value >> (32 - width);
        return (word | (value << lsb));
}	

/*
 * Determines the length of the file in terms of 32 bit words, and uses that
 * length to initialize memory, with the length used as the length for segment
 * 0. 
 */
static inline Mem_T program_load(FILE *input)
{
        double length;
        int c, i;
        int counter = 0;
        Um_word word = 0;
        Mem_T memory;
        fseek(input, 0, SEEK_END);
        length = ftell(input);
        fseek(input, 0, SEEK_SET);
	length = ceil(length/4);
        assert (length < MAX_32_BIT_VAL);
        memory = Mem_new((int)length);
        assert(memory);

        while(counter < length) {
	        for(i = 24; i >= 0; i = i - 8) {
	                c = getc(input);
	                if(c == EOF) {
		                break;
	                }
	                word = create_word(word, 8, i, c);
	        }
	        if(word != 0 || c != EOF) {
	                put_word(0, counter, word, memory);
	        }
	        counter++;
        } 	
    
            return memory;
}
	 
