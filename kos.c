/*
 * jos.c -- starting point for student's os.
 * 
 */

#include <stdlib.h>

#define INIT_GLOBALS
#include "simulator.h"
#include "kos.h"
#include "errno.h"
#include "kt.h"

void initialize_user_process(char *filename) {
	int i;
	int argc = 0;

	if (load_user_program(filename) < 0) {
		fprintf(stderr, "Can't load program.\n");
		exit(1);
	}
	load_user_program(filename);

	PCB *pcb = (PCB*)malloc(sizeof(PCB));

	for (i=0; i < NumTotalRegs; i++)
		pcb->regs[i] = 0;

	/* set up the program counters and the stack register */
	pcb->regs[PCReg] = 0;
	pcb->regs[NextPCReg] = 4;
	pcb->regs[StackReg] = MemorySize - 24;

	for (i = 0; kos_argv[i] != NULL; i++)
		argc++;

	// if (argc > 1) {
	// 	registers[5] = atoi(kos_argv[1]);
	// }
	// if (argc > 2) {
	// 	registers[6] = atoi(kos_argv[2]);
	// }
	// if (argc > 3) {
	// 	registers[7] = atoi(kos_argv[3]);
	// }
	// if (argc > 4) {
	// 	free(pcb);
	// 	fprintf(stderr, "Too many arguments.\n");
	// 	exit(1);
	//}

	//main_memory[StackReg + 12] = argc;
	//main_memory[StackReg + 16] = kos_argv;
	
	dll_append(readyq, new_jval_v((void*)(pcb)));
	kt_exit();
}

void initialize_semaphores() {
	writeok = make_kt_sem(0);
	writers = make_kt_sem(1);
	readers = make_kt_sem(1);
	kt_exit();
}

void console_buf_read() {
	kt_exit();
}

KOS()
{
	bzero(main_memory, MemorySize);
	readyq = new_dllist();

	kt_fork((void*(*)(void *))initialize_user_process, kos_argv[0]);
	kt_fork((void*(*)(void *))initialize_semaphores, NULL);
	kt_fork((void*(*)(void *))console_buf_read, NULL);
	kt_joinall();

	Scheduler();
}
