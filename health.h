#ifndef HEALTH_H
#define HEALTH_H

#include "config.h"
#include "cpu.h"
#include "disk.h"
#include "ram.h"

int get_health_score(Cpu *cpu, Ram *ram, Disk *disk, Config *conf);
void print_health(int score);

#endif // HEALTH_H
