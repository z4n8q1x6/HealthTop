#ifndef JSON_H
#define JSON_H

#include "cpu.h"
#include "disk.h"
#include "ram.h"

int export_json(Cpu *cpu, Ram *ram, Disk *disk, int score);

#endif // JSON_H
