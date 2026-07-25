#include "alerts.h"
#include <stdio.h>

void print_alerts(Cpu *cpu, Ram *ram, Disk *disk, Config *conf) {
  printf("--------------------- ALERTS ---------------------\n");
  if (cpu != NULL) {
    if (cpu->usage >= conf->cpu_threshold) {
      int cpu_usage = cpu->usage;
      printf("Cpu Usage: %d%% !!!\n", cpu_usage);
    }
  }
  if (ram != NULL && ram->total != 0) {
    if (ram->usage >= conf->ram_threshold) {
      printf("Ram Usage: %d%% !!!\n", ram->usage);
    }
  }

  if (disk != NULL && disk->total != 0) {
    if (disk->usage >= conf->disk_threshold) {
      printf("Disk Usage : %d%% !!!\n", disk->usage);
    }
  }
}
