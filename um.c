/*
 * um.c
 * Jake Indursky and Alex Salvatore
 * November 12, 2013
 * Runs the UM by calling the different modules.
 * Expects a binary file from the command line.
 *
 */


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "emulator.h"
#include "memory.h"
#include "programloader.h"


int main (int argc, char **argv)
{
	if(argc != 2) {
	    fprintf(stderr, "invalid command line input\n");
	    exit (1);
	}
        FILE *fp = fopen(argv[1], "rb");
        Mem_T memory = program_load(fp);
        emulator(memory);
        Memory_free(&memory);
	fclose(fp);
}
