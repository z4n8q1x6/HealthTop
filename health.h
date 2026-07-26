#ifndef HEALTH_H
#define HEALTH_H

#include "config.h"
#include "cpu.h"
#include "disk.h"
#include "ram.h"

int get_health_score(Cpu *cpu, Ram *ram, Disk *disk, Config *conf);
const char *get_health_status(int score);
void print_health(int score);

#endif // HEALTH_H
