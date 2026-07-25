#include "main_loop.h"
#include "alerts.h"
#include "conf.h"
#include "log.h"
#include "terminal.h"
#include <bits/time.h>
#include <limits.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>

void run_main_loop(Cpu *cpu, _Atomic Ram *ram, _Atomic Disk *disk) {
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

  Ram ram_snap = {0};
  Disk disk_snap = {0};
  Config conf = {.cpu_threshold = DEFAULT_CPU_THRESHOLD,
                 .ram_threshold = DEFAULT_RAM_THRESHOLD,
                 .disk_threshold = DEFAULT_DISK_THRESHOLD,
                 .refresh_interval = DEFAULT_REFRESH_INTERVAL};
  init_logging();
  load_conf(&conf);

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
        handle_input();
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("\033[2J\033[H");
    if (cpu != NULL) {
      print_cpu(cpu);
    }
    if (ram != NULL) {
      ram_snap = *ram;
      print_ram(&ram_snap);
    }
    if (disk != NULL) {
      disk_snap = *disk;
      print_disk(&disk_snap);
    }
    if (disk != NULL && ram != NULL && cpu != NULL) {
      print_alerts(cpu, &ram_snap, &disk_snap, &conf);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
  }
}
