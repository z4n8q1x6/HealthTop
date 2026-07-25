#ifndef ALERT_H
#define ALERT_H

#include "conf.h"
#include "cpu.h"
#include "disk.h"
#include "ram.h"

void print_alerts(Cpu *cpu, Ram *ram, Disk *disk, Config *conf);

#endif // ALERT_H
