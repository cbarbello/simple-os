#include "scheduler.h"
#include "kos.h"
#include "jval.h"
#include "dllist.h"
#include "syscall.h"
#include "console_buf.h"

#include <stdlib.h>

void SyscallReturn(PCB* pcb, int i) {
	pcb->regs[PCReg] = pcb->regs[NextPCReg];
	pcb->regs[NextPCReg] = pcb->regs[PCReg] + 4;
	pcb->regs[2] = i;
	dll_prepend(readyq, new_jval_v((void*)(pcb)));
	kt_exit();
}

void ReadCall(PCB* pcb) {
	int syscall = pcb->regs[5];
	int address = pcb->regs[6];
	int count   = pcb->regs[7];

	if (syscall != 0) {
		SyscallReturn(pcb, -EBADF);
	}
	else if (address < 0) {
		SyscallReturn(pcb, -EFAULT);
	}
	else if (count < 0) {
		SyscallReturn(pcb, -EINVAL);
	}
	else {
		int i, c;

		P_kt_sem(readers);
		for (i = 0; i < count; i++){
			P_kt_sem(nelem);
			Jval jval = dll_first(console_read_buf)->val;
			c = jval_i(jval);
			dll_delete_node(dll_first(console_read_buf));
			console_read_buf_length -= 1;

			if(c == -1){
				break; 
			}

			main_memory[address + i] = c;
		}
		V_kt_sem(readers);
		SyscallReturn(pcb, i);
	}
}

void WriteCall(PCB* pcb) {
	int syscall = pcb->regs[5];
	int address = pcb->regs[6];
	int count   = pcb->regs[7];

	if (syscall != 1 && syscall != 2) {
		SyscallReturn(pcb, -EBADF);
	}
	else if (address < 0 || address + count > MemorySize) {
		SyscallReturn(pcb, -EFAULT);
	}
	else if (count < 0) {
		SyscallReturn(pcb, -EINVAL);
	}
	else {
		int i;
		char c;

		P_kt_sem(writers);
		for (i=0; i < count; i++){
			c = main_memory[address+i];
			console_write(c);
			P_kt_sem(writeok);
		}
		V_kt_sem(writers);
		SyscallReturn(pcb, count);
	}
}