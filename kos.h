// #ifndef KOS_H
// #define KOS_H

// #ifdef INIT_GLOBALS
// #define EXTERN
// #else
// #define EXTERN extern
// #endif

// #include "simulator_lab2.h"
// #include "dllist.h"
// #include "scheduler.h"
// #include "console_buf.h"
// #include "kt.h"
// #include "errno.h"


// typedef struct {
// 	int regs[NumTotalRegs];
// 	int sbrk;
// 	int User_Base;
// 	int User_Limit;
// } PCB;

// EXTERN Dllist readyq;
// EXTERN Dllist console_read_buf;
// EXTERN PCB* currentRunningProcess;
// EXTERN kt_sem writeok;
// EXTERN kt_sem writers;
// EXTERN kt_sem readers;
// EXTERN kt_sem consoleWait;
// EXTERN kt_sem nelem;
// EXTERN int console_read_buf_length;

// #endif