/*
 * exception.c -- stub to handle user mode exceptions, including system calls
 * 
 * Everything else core dumps.
 * 
 * Copyright (c) 1992 The Regents of the University of California. All rights
 * reserved.  See copyright.h for copyright notice and limitation of
 * liability and disclaimer of warranty provisions.
 */

#include "simulator.h"
#include "kos.h"
#include "scheduler.h"
#include "syscall.h"
#include "console_buf.h"

#include <stdlib.h>

void
exceptionHandler(ExceptionType which)
{
	int             i, type, r5, r6, r7, newPC;
	int             buf[NumTotalRegs];

	examine_registers(buf);
	type = buf[4];
	r5 = buf[5];
	r6 = buf[6];
	r7 = buf[7];
	newPC = buf[NextPCReg];

	
	/*
	 * for system calls type is in r4, arg1 is in r5, arg2 is in r6, and
	 * arg3 is in r7 put result in r2 and don't forget to increment the
	 * pc before returning!
	 */

	for (i=0; i<NumTotalRegs; i++){
		currentRunningProcess->regs[i] = buf[i];
	}

	switch (which) {
	case SyscallException:
		/* the numbers for system calls is in <sys/syscall.h> */
		switch (type) {
		case 0:
			/* 0 is our halt system call number */
			DEBUG('e', "Halt initiated by user program\n");
			SYSHalt();
		case SYS_exit:
			/* this is the _exit() system call */
			DEBUG('e', "_exit() system call\n");
			printf("Program exited with value %d.\n", r5);
			//Dllist ptr = new_dllist();
			//dll_traverse(ptr, console_read_buf){
			//	printf("%c",ptr->val.c);
			//}
			//printf("\n");
			SYSHalt();
		case SYS_write:
			kt_fork((void*(*)(void *))WriteCall, currentRunningProcess);
		 	DEBUG('e', "SYS_write system call\n");
		 	break;
		case SYS_read:
		 	kt_fork((void*(*)(void *))ReadCall, currentRunningProcess);
		 	DEBUG('e', "SYS_read system call\n");
		 	break;
		default:
			DEBUG('e', "Unknown system call\n");
			SYSHalt();
			break;
		}
		break;
	case PageFaultException:
		DEBUG('e', "Exception PageFaultException\n");
		break;
	case BusErrorException:
		DEBUG('e', "Exception BusErrorException\n");
		break;
	case AddressErrorException:
		DEBUG('e', "Exception AddressErrorException\n");
		break;
	case OverflowException:
		DEBUG('e', "Exception OverflowException\n");
		break;
	case IllegalInstrException:
		DEBUG('e', "Exception IllegalInstrException\n");
		break;
	default:
		printf("Unexpected user mode exception %d %d\n", which, type);
		exit(1);
	}

	Scheduler();
}

void
interruptHandler(IntType which)
{

	switch (which) {
	case ConsoleReadInt:
		V_kt_sem(consoleWait);
		DEBUG('e', "ConsoleReadInt interrupt\n");
		break;
	case ConsoleWriteInt:
		V_kt_sem(writeok);
		DEBUG('e', "ConsoleWriteInt interrupt\n");
		break;
	default:
		DEBUG('e', "Unknown interrupt\n");
		break;
	}

	if (currentRunningProcess != NULL) {
		examine_registers(currentRunningProcess->regs);
		dll_prepend(readyq, new_jval_v((void*)(currentRunningProcess)));
	}

	Scheduler();
}
