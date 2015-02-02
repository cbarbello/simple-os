#ifndef KOS_H
#define KOS_H

#ifdef INIT_GLOBALS
#define EXTERN
#else
#define EXTERN extern
#endif

#include "simulator.h"
#include "dllist.h"
#include "scheduler.h"

typedef struct {
	int regs[NumTotalRegs];
} PCB;

EXTERN Dllist readyq;
EXTERN PCB* currentRunningProcess;

#endif