/*
 * jos.c -- starting point for student's os.
 * 
 */

#include <stdlib.h>

#define INIT_GLOBALS
#include "simulator_lab2.h"
#include "kos.h"
#include "console_buf.h"
#include "errno.h"
#include "kt.h"


void initialize_user_process(char *argv[]) {
	int i;
	char *filename = argv[0];
	PCB *pcb       = (PCB*)malloc(sizeof(PCB));

	User_Base       = 0;
	User_Limit      = MemorySize;
	pcb->User_Base  = User_Base;
	pcb->User_Limit = User_Limit;

	int errors = perform_execve(pcb, filename, argv);

	if(errors == 0){
		dll_append(readyq, new_jval_v((void*)(pcb)));
		kt_exit();
	}
	else{
		exit(1);
	}
}

int perform_execve(PCB *pcb, char *filename, char *argv[]) {
	int argc  = 0;
	int i;
	int errors = load_user_program(filename);

	if (errors < 0) {
		return -ENOENT;
	}
	//printf("%s\n", argv[1]);

	pcb->sbrk = errors;

	for (i = 0; argv[i] != NULL; i++){	//get the number of arguments
		argc++;
	}
	for (i=0; i < NumTotalRegs; i++){ 	//initialize the regs to zero
		pcb->regs[i] = 0;
	}

	//set up program counters and stack register
	pcb->regs[PCReg]     = 0;
	pcb->regs[NextPCReg] = 4;

	int argv_space = 0;
	for (i = 0; i < argc; i++){
		int length = strlen(argv[i]) + 1;
		argv_space += length + (4 - (length % 4)); //must be a multiple of four
	}

	//set the stack pointer and argv pointer and make sure ap is 24 bytes away from sp
	//also subtract another 40 bytes to give more space
	int sp = MemorySize - argv_space - 64;
	int ap = sp + 24;
	pcb->regs[StackReg] = sp;

	memcpy(&main_memory[pcb->regs[StackReg] + 12], &argc, sizeof(argc));
	memcpy(&main_memory[sp + 16], &ap, sizeof(ap));

	int address = ap + (argc + 1) * 4; //address for argv[i]
	for (i = argc-1; i >= 0; i--){
		memcpy(&main_memory[ap + 4*i], &address, sizeof(address));
		strcpy(&main_memory[address], argv[i]);
		int length = strlen(argv[i]) + 1;
		address += length;
	}
	return 0;
}

void initialize_semaphores() {
	writeok = make_kt_sem(0);
	writers = make_kt_sem(1);
	readers = make_kt_sem(1);
	nelem   = make_kt_sem(0);
	consoleWait = make_kt_sem(0);
	kt_exit();
}


KOS()
{
	bzero(main_memory, MemorySize);
	readyq = new_dllist();
	console_read_buf = new_dllist();
	console_read_buf_length = 0;

	kt_fork((void*(*)(void *))initialize_user_process, kos_argv);
	kt_fork((void*(*)(void *))initialize_semaphores, NULL);
	kt_fork((void*(*)(void *))console_buf_read, NULL);
	kt_joinall();

	Scheduler();
}
