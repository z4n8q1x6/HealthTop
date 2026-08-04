#include "alerts.h"
#include "colors.h"
#include <stdio.h>

void print_alerts(Cpu *cpu, Ram *ram, Disk *disk, Config *conf) {
  printf(RED);
  printf("────────────── ALERTS ──────────────\n\n");
  printf(RESET);
  if (cpu != NULL) {
    if (cpu->usage >= conf->cpu_threshold) {
      int cpu_usage = cpu->usage;

      printf(RED);
      printf("Cpu Usage: " WHITE "%d%% !!!\n", cpu_usage);
    }
  }
  if (ram != NULL && ram->total != 0) {
    if (ram->usage >= conf->ram_threshold) {

      printf(RED);
      printf("Ram Usage: " WHITE "%d%% !!!\n", ram->usage);
    }
  }

  if (disk != NULL && disk->total != 0) {
    if (disk->usage >= conf->disk_threshold) {

      printf(RED);
      printf("Disk Usage : " WHITE "%d%% !!!\n", disk->usage);
    }
  }
  printf(RESET);
}
