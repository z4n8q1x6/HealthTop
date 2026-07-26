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

const char *get_health_status(int score) {
  // this system is naive and may need to be reworked at some point
  if (score >= 90) {
    return "Excellent";
  } else if (score >= 70 && score < 90) {
    return "Good";
  } else if (score >= 50 && score < 70) {
    return "Medium";
  }
  return "Critical";
}

void print_health(int score) {
  const char *status = get_health_status(score);
  printf("--------------------- HEALTH ---------------------\n");
  printf("System health: %d / 100\n", score);
  printf("Status: %s\n", status);
}
