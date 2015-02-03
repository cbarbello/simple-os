#include "scheduler.h"
#include "kos.h"
#include "jval.h"
#include "dllist.h"
#include "syscall.h"

#include <stdlib.h>

void SyscallReturn(PCB* pcb, int i) {
	pcb->regs[PCReg] = pcb->regs[NextPCReg];
	pcb->regs[NextPCReg] = pcb->regs[PCReg] + 4;
	pcb->regs[2] = i;
	dll_prepend(readyq, new_jval_v((void*)(pcb)));
	kt_exit();
}

void ReadCall(PCB* pcb) {
	if (pcb->regs[5] != 0) {
		SyscallReturn(pcb, -EBADF);
	}
	else if (pcb->regs[6] < 0) {
		SyscallReturn(pcb, -EFAULT);
	}
	else if (pcb->regs[7] < 0) {
		SyscallReturn(pcb, -EINVAL);
	}
	else {
		P_kt_sem(readers);
		//V_kt_sem(readers);
		SyscallReturn(pcb, 0);
		V_kt_sem(readers);
	}
}

void WriteCall(PCB* pcb) {
	if (pcb->regs[5] != 1 && pcb->regs[5] != 2) {
		SyscallReturn(pcb, -EBADF);
	}
	else if (pcb->regs[6] < 0 || pcb->regs[6] + pcb->regs[7] > MemorySize) {
		SyscallReturn(pcb, -EFAULT);
	}
	else if (pcb->regs[7] < 0) {
		SyscallReturn(pcb, -EINVAL);
	}
	else {
		int count = pcb->regs[7];
		int i;
		char c;

		P_kt_sem(writers);
		for (i=0; i < count; i++){
			c = main_memory[pcb->regs[6]+i];
			console_write(c);
			P_kt_sem(writeok);
		}
		V_kt_sem(writers);
		SyscallReturn(pcb, 1);
	}
}