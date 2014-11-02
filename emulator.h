/*
 * emulator.c
 * Jake Indursky and Alex Salvatore
 * November 12, 2013
 * interface that runs a program.
 * expects halt instruction to end the program.
 */

#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include <stdint.h>
#include "instructions.h"
#include "seq.h"
#include "bitpack.h"
#include "memory.h"



#define HALT 7

#define OP_LENGTH 4
#define OP_LSB 28
#define REG_LENGTH 3
#define RC_LSB 0
#define RB_LSB 3
#define RA_LSB 6
#define LOAD_VAL 13
#define LOAD_PRGRM 12
#define RA_LSB_LOAD 25
#define LOAD_VAL_LEN 25
#define MAX_VAL_32_BIT 4294967296




static inline int get_regA(Um_opcode instr, Um_word word);
static inline int get_B(Um_opcode instr, Um_word word);
static inline int get_C (Um_word word);
static inline int get_instr(Um_word word);

/*
 * Runs the emulator loop and creates the registers. Reads a word from segment
 * 0 of memory, unpacks it using bitpack, and sends the resulting data to the
 * instruction interface. If the instruction is halt, the loop is exited
 * and if the instruction is load program, the program counter is set to the 
 * value in regC
 */
static inline void emulator (Mem_T memory)
{
	Um_word reg[8] = {0,0,0,0,0,0,0,0};
        uint32_t program_counter = 0;
        uint32_t word;
        Um_opcode instr = 0;
	int A, B, C;
        int check;
	
        
	while (instr != HALT) {
		word = get_word(0, program_counter, memory);
	     	instr = get_instr(word);
		A = get_regA(instr, word);
		B = get_B(instr, word);
		C = get_C(word);
                program_counter++;
        
                switch (instr) {
                        case 0:
                            if (reg[C] != 0) {
                                        reg[A] = reg[B];
                                }
                                break;
                        case 1:
                              

                                reg[A] = get_word(reg[B], reg[C], memory);
                                break;
                        case 2:
                               
                                put_word(reg[A], reg[B], reg[C], memory);
                                break;
                        case 3:
                               

                                reg[A] = (reg[B] + reg[C]) % MAX_VAL_32_BIT;
                                break;
                        case 4:
                               

                                reg[A] = (reg[B] * reg[C]) % MAX_VAL_32_BIT;
                                break;
                        case 5:
                               

                                reg[A] = reg[B] / reg[C];
                                break;
                        case 6:
                               

                                reg[A] = ~(reg[B] & reg[C]);
                                break;
                        case 7:
                                return; 
                        case 8:
                              

                                reg[B] = map_seg(reg[C], memory);
                   
                                break;
                        case 9:
                               

                                unmap_seg(reg[C], memory);
                                break;
                        case 10:
                         

                                putc(reg[C], stdout);
                                break;
                        case 11:
                         

                                check = fgetc(stdin);
                                if(check == EOF) {
                                        reg[C] = ~0;
	                        } else {
	                                reg[C] = check;
	                        }
                                break;
                        case 12:
                         

                                load_program(reg[B], memory);
                                program_counter = reg[C];
                                break;
                        case 13:
                               
                                reg[A] = B;
                                break;
                        default:
                                exit(1);
                }
        }
}

/*
 * unpacks part of the word to get RegA. Unpacks a different area if it is 
 * loadval instruction
 */

static inline int get_regA(Um_opcode instr, Um_word word)
{   /*
        if (instr != LOAD_VAL) {
                return Bitpack_getu(word, REG_LENGTH, RA_LSB); 
        } else {
                return Bitpack_getu(word, REG_LENGTH, RA_LSB_LOAD);
        }*/


        uint32_t mask = ~0;
        if (instr != 13) {
	        mask = mask >> (32 - REG_LENGTH) << RA_LSB;
	        return ((word & mask) >> RA_LSB);
        } else {
	        mask = mask >> (32 - REG_LENGTH) << RA_LSB_LOAD;
	        return ((word & mask) >> RA_LSB_LOAD);
        }
}

/* 
 * Unpacks part of the word to get RegB. If the instruction is loadvalue,
 * it returns the value instead of the register index
 */
static inline int get_B(Um_opcode instr, Um_word word)
{
      /*  if (instr != LOAD_VAL) {
                return Bitpack_getu(word, REG_LENGTH, RB_LSB);
        } else {
                return Bitpack_getu(word, LOAD_VAL_LEN, 0);
        }*/
        uint32_t mask = ~0;
        if(instr != 13) {
	        mask = mask >> (32 - REG_LENGTH) << RB_LSB;
	        return ((word & mask) >> RB_LSB);
        } else {
	        mask = mask >> (32 - LOAD_VAL_LEN);
	        return ((word & mask));
        }
}

static inline int get_C(Um_word word)         
{
        uint32_t mask = ~0;
        mask = mask >> (32 - REG_LENGTH) << RC_LSB;
        return ((word & mask) >> RC_LSB);
}

static inline int get_instr(Um_word word) 
{
        uint32_t mask = ~0;
        mask = mask >> (32 - OP_LENGTH) << OP_LSB;
        return ((word & mask) >> OP_LSB);
}
