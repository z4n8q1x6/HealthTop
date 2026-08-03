#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H

#include "cpu.h"
#include "disk.h"
#include "process.h"
#include "ram.h"

enum {
  VIEW_CPU,
  VIEW_RAM,
  VIEW_DISK,
  VIEW_PROCESS,
};

#define SCROLL_STEP 10

void run_main_loop(Cpu *cpu, _Atomic Ram *ram, _Atomic Disk *disk,
                   _Atomic ProcessList *ps);

#endif // MAIN_LOOP_H
