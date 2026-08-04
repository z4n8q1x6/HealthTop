#include "health.h"
#include "colors.h"
#include <stdio.h>

#define PROCESS_WARNING 500
#define PROCESS_CRITICAL 1000

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
  if (ps->count > PROCESS_WARNING) {
    score -= 10;
  } else if (ps->count > PROCESS_CRITICAL) {
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

// void print_health(int score) {
//   const char *status = get_health_status(score);
//   printf("--------------------- HEALTH ---------------------\n");
//   printf("System health: %d / 100\n", score);
//   printf("Status: %s\n", status);
// }

void print_health(int score) {
  const char *status = get_health_status(score);
  printf(MAGENTA);
  printf("======================================\n");
  printf(" ____ __   __ ____ _____ _____ __  __ \n"
         "/ ___|\\ \\ / // ___|_   _| ____|  \\/  |\n"
         "\\___ \\ \\ V / \\___ \\ | | |  _| | |\\/| |\n"
         " ___) | | |   ___) || | | |___| |  | |\n"
         "|____/  |_|  |____/ |_| |_____|_|  |_|\n");
  printf("\n======================================\n\n");

  printf(MAGENTA);
  printf("System health : " WHITE "%d/100\n", score);
  printf(MAGENTA);
  printf("Status        : " WHITE "%s\n", status);
  printf(RESET);
  printf("\n");
}
