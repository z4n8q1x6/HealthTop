#include "health.h"
#include <stdio.h>

int get_health_score(Cpu *cpu, Ram *ram, Disk *disk, ProcessList *ps,
                     Config *conf) {
  int score = 100;
  if (cpu->usage > conf->cpu_threshold) {
    score -= 25;
  }
  if (ram->usage > conf->ram_threshold) {
    score -= 25;
  }
  if (disk->usage > conf->disk_threshold) {
    score -= 25;
  }
  if (ps->count > 1000) {
    score -= 25;
  }
  return score;
}

const char *get_health_status(int score) {
  if (score == 100) {
    return "Perfect";
  } else if (score <= 75 && score > 50) {
    return "Good";
  } else if (score <= 50 && score > 25) {
    return "Moderate";
  } else if (score <= 25 && score > 0) {
    return "Severe";
  }
  return "Critical";
}

void print_health(int score) {
  const char *status = get_health_status(score);
  printf("--------------------- HEALTH ---------------------\n");
  printf("System health: %d / 100\n", score);
  printf("Status: %s\n", status);
}
