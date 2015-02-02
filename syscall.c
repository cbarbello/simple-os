#include "scheduler.h"
#include "kos.h"
#include "jval.h"
#include "dllist.h"
#include "syscall.h"

#include <stdlib.h>

void SyscallReturn(PCB* pcb, int i) {
	pcb->regs[PCReg] = pcb->regs[NextPCReg];
	pcb->regs[2] = i;
	dll_append(readyq, new_jval_v((void*)(pcb)));
	kt_exit();
}

void ReadCall(PCB* pcb) {
	//printf("read\n");
	SyscallReturn(pcb, 0);
}

void WriteCall(PCB* pcb) {
	//printf("write\n");
	SyscallReturn(pcb, 1);
}