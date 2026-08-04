#include "cpu.h"
#include "colors.h"
#include "log.h"
#include "util.h"
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum {
  USER = 1,
  NICE = 2,
  SYSTEM = 3,
  IDLE = 4,
  IOWAIT = 5,
  IRQ = 6,
  SOFTIRQ = 7,
  STEAL = 8
};

static int get_current_cpu_ticks(unsigned long long *active,
                                 unsigned long long *idle) {
  char line[LINE_MAX];
  FILE *cpu_stat = fopen("/proc/stat", "r");
  if (cpu_stat == NULL) {
    log_msg(LOG_ERROR, "get_current_cpu_ticks: fopen", errno);
    return 0;
  }
  if (fgets(line, sizeof(line), cpu_stat) != NULL) {
    char **tokens = get_tokens(line);
    if (tokens == NULL || tokens[8] == NULL) {
      log_msg(LOG_ERROR, "get_current_cpu_ticks: get_tokens", -1);
      return 0;
    }
    *active = strtoll(tokens[USER], NULL, 0) + strtoll(tokens[NICE], NULL, 0) +
              strtoll(tokens[SYSTEM], NULL, 0) + strtoll(tokens[IRQ], NULL, 0) +
              strtoll(tokens[STEAL], NULL, 0) +
              strtoll(tokens[SOFTIRQ], NULL, 0);
    *idle = strtoll(tokens[IDLE], NULL, 0) + strtoll(tokens[IOWAIT], NULL, 0);
    free(tokens);
    fclose(cpu_stat);
    return 1;
  }
  return 0;
}

int init_cpu(Cpu *cpu) {
  FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
  if (cpuinfo == NULL) {
    log_msg(LOG_ERROR, "init_cpu: fopen", errno);
    return 0;
  }

  char line[LINE_MAX];
  int fields_found = 0;
  char *cpu_infos[] = {"model name", "cpu cores"};
  int n = 2;
  while (fgets(line, sizeof(line), cpuinfo) != NULL && fields_found < n) {
    char *colon = strchr(line, ':');
    if (colon == NULL) {
      continue;
    }

    if (cpu->model_name[0] == '\0' && strstr(line, cpu_infos[0]) != NULL) {
      char *val = colon + 1;

      while (*val == ' ' || *val == '\t')
        val++;
      val[strcspn(val, "\n")] = '\0';

      snprintf(cpu->model_name, sizeof(cpu->model_name), "%s", val);
      fields_found++;
    } else if (cpu->nb_cores == 0 && strstr(line, cpu_infos[1]) != NULL) {
      cpu->nb_cores = atoi(colon + 1);
      fields_found++;
    }
  }

  return 1;
  fclose(cpuinfo);
}

int set_cpu_usage(_Atomic unsigned long long *usage, int looping) {
  unsigned long long active_t1, active_t2, idle_t1, idle_t2, active, idle;
  if (!get_current_cpu_ticks(&active_t1, &idle_t1))
    return 0;
  do {
    sleep(1);
    if (get_current_cpu_ticks(&active_t2, &idle_t2)) {
      active = active_t2 - active_t1;
      idle = idle_t2 - idle_t1;
      if (active != 0) {
        *usage = (active * 100) / (active + idle);
      } else {
        *usage = 0;
      }
      active_t1 = active_t2;
      idle_t1 = idle_t2;
    } else {
      return 0;
    }
  } while (looping);
  return 1;
}

void *cpu_thread(void *usage) {
  if (!set_cpu_usage((_Atomic unsigned long long *)usage, 1)) {
    return NULL;
  }
  return NULL;
}

void print_cpu(Cpu *cpu) {
  printf(BLUE);
  printf("===================\n");
  printf("  ____ ____  _   _ \n"
         " / ___|  _ \\| | | |\n"
         "| |   | |_) | | | |\n"
         "| |___|  __/| |_| |\n"
         " \\____|_|    \\___/ \n\n");
  printf("===================\n");
  printf("\n");
  printf("Model : " WHITE "%s\n" BLUE, cpu->model_name);
  printf("Cores : " WHITE "%d\n" BLUE, cpu->nb_cores);
  printf("Usage : " WHITE "%u%%\n" BLUE, (unsigned int)cpu->usage);
  printf("\n");
  print_usage_bar(cpu->usage);
  printf(RESET);
}
