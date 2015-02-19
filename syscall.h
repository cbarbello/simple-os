#ifndef SYSCALL_H
#define SYSCALL_H

#include "simulator_lab2.h"
#include "dllist.h"
#include "scheduler.h"
#include "console_buf.h"

#include <stdlib.h>

void SyscallReturn(PCB* pcb, int i);
void WriteCall(PCB* pcb);
void ReadCall(PCB* pcb);
void IoctlCall(PCB* pcb);
void FstatCall(PCB* pcb);
void GetPageSizeCall(PCB* pcb);
void SbrkCall(PCB* pcb);
void ExecveCall(PCB* pcb);

bool ValidAddress(PCB* pcb, int address);

#endif