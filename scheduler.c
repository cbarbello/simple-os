#include "simulator.h"
#include "dllist.h"

Scheduler() {
	PCB* pcb = dll_first(readyq);
	
	if (pcb == NULL)
		noop();
	else {
		printf("Running user code.\n");
		run_user_code(pcb->registers);
	}
}