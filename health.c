#include "health.h"
#include <stdio.h>

int get_health_score(Cpu *cpu, Ram *ram, Disk *disk, Config *conf) {
  int score = 100;
  if (cpu->usage > conf->cpu_threshold) {
    score -= 20;
  }
  if (ram->usage > conf->ram_threshold) {
    score -= 20;
  }
  if (disk->usage > conf->disk_threshold) {
    score -= 20;
  }
  return score;
}

void print_health(int score) {
  const char *status;
  // this system is naive and may need to be reworked at some point
  if (score >= 90) {
    status = "Excellent";
  } else if (score >= 70 && score < 90) {
    status = "Good";
  } else if (score >= 50 && score < 70) {
    status = "Medium";
  } else {
    status = "Critical";
  }

  printf("--------------------- HEALTH ---------------------\n");
  printf("System health: %d / 100\n", score);
  printf("Status: %s\n", status);
}
