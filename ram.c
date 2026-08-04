#include "ram.h"
#include "log.h"
#include "util.h"
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { TOTAL, FREE, AVAILABLE, CACHED };
static const char ram_infos[4][13] = {"MemTotal", "MemFree", "MemAvailable",
                                      "Cached"};
static const size_t n_ram_infos = sizeof(ram_infos) / sizeof(ram_infos[0]);
static int read_ram(Ram *ram);

int init_ram(_Atomic Ram *ram) {
  Ram ram_snap;
  if (read_ram(&ram_snap)) {
    *ram = ram_snap;
    return 1;
  }
  return 0;
}

static int read_ram(Ram *ram) {
  char line[LINE_MAX];
  size_t fields_found = 0;
  FILE *meminfo = fopen("/proc/meminfo", "r");
  if (meminfo == NULL) {
    log_msg(LOG_ERROR, "read_ram: fopen", errno);
    return 0;
  }
  while (fgets(line, sizeof(line), meminfo) != NULL &&
         fields_found < n_ram_infos) {
    for (size_t i = 0; i < n_ram_infos; i++) {
      if (strncmp(line, ram_infos[i], strlen(ram_infos[i])) == 0) { // check
        char *colon = strchr(line, ':');
        if (colon == NULL)
          continue;

        char *val = ++colon;
        unsigned long long tmp = strtoull(val, NULL, 0);
        switch (i) {
        case TOTAL:
          ram->total = tmp;
          break;
        case AVAILABLE:
          ram->available = tmp;
          break;
        case FREE:
          ram->free = tmp;
          break;
        case CACHED:
          ram->cached = tmp;
          break;
        }
        fields_found++;
      }
    }
  }
  ram->used = ram->total - ram->available;
  if (ram->total != 0)
    ram->usage = (ram->used * 100) / ram->total;
  else
    ram->usage = 0;
  fclose(meminfo);
  return 1;
}

void *ram_thread(void *arg) {
  Ram ram;

  while (1) {
    if (read_ram(&ram)) {
      *(_Atomic Ram *)arg = ram;
    }
    sleep(1);
  }
  return NULL;
}

void print_ram(Ram *snap) {
  printf("\033[31m");
  printf("======================\n");
  printf(" ____     _    __  __ \n"
         "|  _ \\   / \\  |  \\/  |\n"
         "| |_) | / _ \\ | |\\/| |\n"
         "|  _ < / ___ \\| |  | |\n"
         "|_| \\_/_/   \\_\\_|  |_|\n\n");
  printf("======================\n");
  printf("\n");
  printf("Total    : \033[37m%zu\033[31m\n", snap->total);
  printf("Available: \033[37m%zu\033[31m\n", snap->available);
  printf("Free     : \033[37m%zu\033[31m\n", snap->free);
  printf("Cached   : \033[37m%zu\033[31m\n", snap->cached);
  printf("Used     : \033[37m%zu\033[31m\n", snap->used);
  printf("Usage    : \033[37m%d%%\033[31m\n", snap->usage);
  printf("\n");
  print_usage_bar(snap->usage);
  printf("\033[0m");
}
