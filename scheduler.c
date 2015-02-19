#include "scheduler.h"
#include "kos.h"
#include "jval.h"
#include "dllist.h"
#include "console_buf.h"

#include <stdlib.h>

void Scheduler() {
	kt_joinall();
	if (dll_empty(readyq)) {
		currentRunningProcess = NULL;		
		noop();
	}
	else {
		Jval jval = dll_first(readyq)->val;
		PCB* pcb = (PCB*)jval.v;
		currentRunningProcess = pcb;
		dll_delete_node(dll_first(readyq));

		User_Base  = currentRunningProcess->User_Base;
		User_Limit = currentRunningProcess->User_Limit;

		run_user_code(currentRunningProcess->regs);
		currentRunningProcess = NULL;
	}
}