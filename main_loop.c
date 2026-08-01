#include "main_loop.h"
#include "alerts.h"
#include "config.h"
#include "health.h"
#include "json.h"
#include "log.h"
#include "process.h"
#include "terminal.h"
#include <bits/time.h>
#include <errno.h>
#include <limits.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>

void run_main_loop(Cpu *cpu, _Atomic Ram *ram, _Atomic Disk *disk,
                   _Atomic ProcessList *ps) {
  struct pollfd fds[1];
  fds[0].events = POLLIN;
  fds[0].fd = STDIN_FILENO;
  nfds_t nfds = 1;
  struct timespec t1;
  struct timespec t2;
  unsigned long long elapsed;
  unsigned long long timeout =
      33333333; // 30fps ( 1 refresh each 1/30s = (1/30)*10^9 in nanosec)
  unsigned long long remaining;
  struct timespec last_log = {0};

  Ram ram_snap = {0};
  Disk disk_snap = {0};
  ProcessList ps_snap = {0};

  Config conf = {.cpu_threshold = DEFAULT_CPU_THRESHOLD,
                 .ram_threshold = DEFAULT_RAM_THRESHOLD,
                 .disk_threshold = DEFAULT_DISK_THRESHOLD,
                 .refresh_interval = DEFAULT_REFRESH_INTERVAL};
  load_conf(&conf);

  int view = -1;
  int main_mode = disk != NULL && ram != NULL && cpu != NULL && ps != NULL;
  if (main_mode || cpu != NULL) {
    view = VIEW_CPU;
  } else if (ram != NULL) {
    view = VIEW_RAM;
  } else if (disk != NULL) {
    view = VIEW_DISK;
  } else {
    view = VIEW_PROCESS;
  }

  clock_gettime(CLOCK_MONOTONIC, &t1);
  clock_gettime(CLOCK_MONOTONIC, &t2);
  while (1) {
    elapsed =
        (t2.tv_sec - t1.tv_sec) * 1000000000ULL + (t2.tv_nsec - t1.tv_nsec);
    if (elapsed < timeout) {
      remaining = (timeout - elapsed) / 1000000ULL;
    } else {
      remaining = 0;
    }
    int retval = poll(fds, nfds, (int)remaining);
    if (retval > 0) {
      if (fds[0].revents & POLLIN) {
        handle_input(main_mode, &view);
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("\033[2J\033[H");
    if (cpu != NULL && view == VIEW_CPU) {
      print_cpu(cpu);
    }
    if (ram != NULL) {
      ram_snap = *ram;
      if (view == VIEW_RAM)
        print_ram(&ram_snap);
    }
    if (disk != NULL) {
      disk_snap = *disk;
      if (view == VIEW_DISK)
        print_disk(&disk_snap);
    }
    if (ps != NULL) {
      ps_snap = *ps;
      ProcessInfo *owned = malloc(sizeof(*owned) * ps_snap.count);
      if (owned == NULL) {
        log_msg(LOG_ERROR, "main_loop_processes: malloc", errno);
        exit(EXIT_FAILURE);
      }
      memcpy(owned, ps_snap.items, sizeof(*owned) * ps_snap.count);
      ps_snap.items = owned;
      if (view == VIEW_PROCESS)
        print_processes(&ps_snap);
    }

    if (main_mode) {
      int score = get_health_score(cpu, &ram_snap, &disk_snap, &conf);
      print_health(score);
      clock_gettime(CLOCK_MONOTONIC, &t2);
      unsigned long long elapsed_logging =
          (t2.tv_sec * 1000000000ULL + t2.tv_nsec) -
          (last_log.tv_sec * 1000000000ULL + last_log.tv_nsec);
      if (elapsed_logging >= 1000000000ULL) {
        log_mesures(cpu, &ram_snap, &disk_snap, score);
        clock_gettime(CLOCK_MONOTONIC, &last_log);
      }
      print_alerts(cpu, &ram_snap, &disk_snap, &conf);
      export_json(cpu, &ram_snap, &disk_snap, score);
      free(ps_snap.items);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
  }
}
