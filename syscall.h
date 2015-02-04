#ifndef SYSCALL_H
#define SYSCALL_H

#include "simulator.h"
#include "dllist.h"
#include "scheduler.h"
#include "console_buf.h"

#include <stdlib.h>

void SyscallReturn(PCB* pcb, int i);
void WriteCall(PCB* pcb);
void ReadCall(PCB* pcb);

#endif