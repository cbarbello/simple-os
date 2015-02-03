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
#include "kt.h"
#include "errno.h"

typedef struct {
	int regs[NumTotalRegs];
} PCB;

EXTERN Dllist readyq;
EXTERN PCB* currentRunningProcess;
EXTERN kt_sem writeok;
EXTERN kt_sem writers;
EXTERN kt_sem readers;

#endif