#include "scheduler.h"
//#include "kos.h"
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

	if(!ValidAddress(pcb, address)){
		SyscallReturn(pcb, -EFAULT);
	}
	else if (syscall != 0) {
		SyscallReturn(pcb, -EBADF);
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

	if(!ValidAddress(pcb, address + count)){
		SyscallReturn(pcb, -EFAULT);
	}
	else if (syscall != 1 && syscall != 2) {
		SyscallReturn(pcb, -EBADF);
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

void IoctlCall(PCB* pcb) {
	int fd      = pcb->regs[5];
	int request = pcb->regs[6];
	int termios = pcb->regs[7];

	if(!ValidAddress(pcb, termios)){
		SyscallReturn(pcb, -EFAULT);
	}

	termios = (int)main_memory + termios;

	if(fd == 1 && request == KOS_TCGETP){
		ioctl_console_fill(termios);
		SyscallReturn(pcb, 0);
	}
	else{
		SyscallReturn(pcb, -EINVAL);
	}
}

void FstatCall(PCB* pcb) {
	int fd     = pcb->regs[5];
	int buffer = pcb->regs[6];

	if(!ValidAddress(pcb, buffer)){
		SyscallReturn(pcb, -EFAULT);
	}

	buffer = (int)main_memory + buffer;

	if (fd == 0){
		stat_buf_fill(buffer, 1);
		SyscallReturn(pcb, 0);
	}
	else if (fd == 1 || fd == 2){
		stat_buf_fill(buffer, 256);
		SyscallReturn(pcb, 0);
	}
}

void GetPageSizeCall(PCB* pcb) {
	SyscallReturn(pcb, PageSize);
}

void SbrkCall(PCB* pcb) {
	int increment = pcb->regs[5];
	int sbrk = pcb->sbrk;
	int inc_sbrk = sbrk + increment;

	if(ValidAddress(pcb, inc_sbrk)) {
		SyscallReturn(pcb, -ENOMEM);
	}
	else{
		pcb->sbrk = inc_sbrk;
		SyscallReturn(pcb, sbrk);
	}

}

void ExecveCall(PCB* pcb) {
	int filename_int = pcb->regs[5];
	int argv_int     = pcb->regs[6];
	int envp_int     = pcb->regs[7];

	if (!ValidAddress(pcb, filename_int) || !ValidAddress(pcb, argv_int)){
		SyscallReturn(pcb, -EFAULT);
	}

	filename_int   = (int)main_memory + pcb->User_Base + filename_int;
	char* filename = (char*)filename_int;

	int argc = 0;
	int i = 0;
	int address = *(int *)(main_memory + pcb->User_Base + argv_int + argc*4);

	while (address != 0){ 													  //get the argument count
		address = *(int *)(main_memory + pcb->User_Base + argv_int + argc*4);
		argc++;
	}
	char* arg_tmp;
	char** argv = malloc((argc + 1) * sizeof(char*));
	for (i = 0; i < argc; i++){
		address = *(int *)(main_memory + pcb->User_Base + argv_int + i*4);	//go to the address or agrv[i]
		arg_tmp = (char *)(address + main_memory + pcb->User_Base);			//put value into tmp
		argv[i] = malloc(strlen(arg_tmp) );								//malloc space in argv[i] for tmp
		strcpy(argv[i], arg_tmp);										
	}
	argv[i] = '\0';

	for (i=0; i < NumTotalRegs; i++){
		pcb->regs[i] = 0;
	}
	pcb->regs[NextPCReg] = 0;
	
	int errors = perform_execve(pcb, filename, argv);

	for(i = 0; i < argc  ; i++){
		free(argv[i]);
	}	
	free(argv);

	if(errors == 0){
		SyscallReturn(pcb, 0);
	}
	else{
		SyscallReturn(pcb, errors);
	}
}



bool ValidAddress(PCB* pcb, int address) {
	if (address > 0 && address < pcb->User_Limit){
		return TRUE;
	}
	else{
		return FALSE;
	}
}












