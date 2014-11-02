/* 
 * instructions.h
 * Jake Indursky and Alex Salvatore
 * November 12, 2013
 * interface that is used to perform a single instruction
 */


#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "assert.h"
#include "memory.h"

typedef uint32_t Umsegment_id;
typedef uint8_t Um_opcode;
typedef uint8_t Um_register;
typedef uint32_t Um_segment;

/*
 * performs an instruction, given an instruction between 0 and 13, the 
 * registers the instruction uses and memory
 */
unsigned perform_instr(Um_opcode instr, int A, int B, int C, 
		       Um_word *reg, Mem_T memory);

#endif
