#ifndef KOS_H
#define KOS_H

#ifdef INIT_GLOBALS
#define EXTERN
#else
#define EXTERN EXTERN
#endif

EXTERN Dllist* readyq;
EXTERN PCB* currentRunningProcess;
