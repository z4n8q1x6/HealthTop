#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H

#include "cpu.h"
#include "disk.h"
#include "process.h"
#include "ram.h"

void run_main_loop(Cpu *cpu, _Atomic Ram *ram, _Atomic Disk *disk,
                   _Atomic ProcessList *ps);

#endif // MAIN_LOOP_H
