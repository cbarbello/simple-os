//scheduler.c
#include "scheduler.h"
#include "kos.h"
#include "jval.h"
#include "dllist.h"

#include <stdlib.h>

void Scheduler() {
	Jval jval = dll_first(readyq)->val;
	PCB* pcb = (PCB*)jval.v;

	if (dll_empty(readyq)) {
		currentRunningProcess = NULL;		
		noop();
	}
	else {
		currentRunningProcess = pcb;
		printf("Running user code.\n");
		dll_delete_node(dll_first(readyq));
		run_user_code(pcb->regs);
	}
}