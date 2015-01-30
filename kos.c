/*
 * kos.c -- starting point for student's os.
 * 
 */

#include "simulator.h"
#include <stdlib.h>

#define INIT_GLOBALS
#include "kos.h"


void initialize_user_process(char *filename) {
	int registers[NumTotalRegs];
	int i;
	int argc = 0;

	if (load_user_program(filename) < 0) {
		fprintf(stderr, "Can't load program.\n");
		exit(1);
	}
	load_user_program(filename);

	PCB *pcb = (PCB)malloc(sizeof(PCB));

	for (i=0; i < NumTotalRegs; i++)
		registers[i] = 0;

	/* set up the program counters and the stack register */
	registers[PCReg] = 0;
	registers[NextPCReg] = 4;
	registers[StackReg] = MemorySize - 24;

	for (i = 0; kos_argv[i] != NULL; i++)
		argc++;

	if (argc > 1) {
		registers[5] = atoi(kos_argv[1]);
	}
	if (argc > 2) {
		registers[6] = atoi(kos_argv[2]);
	}
	if (argc > 3) {
		registers[7] = atpo(kos_argv[3]);
	}
	if (argc > 4) {
		free(pcb);
		fprintf(stderr, "Too many arguments.\n");
		exit(1);
	}
	pcb->registers = registers;

	main_memory[StackReg + 12] = argc;
	main_memory[StackReg + 16] = kos_argv;
	
	dll_append(readyq, (void*)(pcb));
	kt_exit();
}

void initialize_semaphores() {
	kt_exit();
}

KOS() 
{	
	bzero(main_memory, MemorySize);
	readyq = new_dllist();

	kt_fork(initialize_user_process, kos_argv[0]);
	kt_fork(initialize_semaphores);
	kt_fork(console_buf_read);
	kt_joinall();

	Scheduler();
}

typedef struct pcb {
	int* registers;
} *PCB;

