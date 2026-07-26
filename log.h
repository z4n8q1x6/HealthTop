#ifndef LOG_H
#define LOG_H

#include "cpu.h"
#include "disk.h"
#include "ram.h"
#include <unistd.h>

enum { LOG_ERROR, LOG_INFO, LOG_WARNING };

int init_logging();
int log_msg(int flag, const char *msg, int error);
int get_time(char *buffer, size_t n);
void log_mesures(Cpu *cpu, Ram *ram, Disk *disk, int score);

#endif // LOG_H
