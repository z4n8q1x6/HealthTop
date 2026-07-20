#include "main_loop.h"
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

  clock_gettime(CLOCK_MONOTONIC, &t1);
  clock_gettime(CLOCK_MONOTONIC, &t2);
  while (1) {
    elapsed =
        (t2.tv_sec - t1.tv_sec) * 1000000000ULL + (t2.tv_nsec - t1.tv_nsec);
    remaining = (timeout - elapsed) / 1000000ULL;
    int retval = poll(fds, 1, (int)remaining);
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
      Ram ram_snap = *ram;
      print_ram(&ram_snap);
    }
    if (disk != NULL) {
      Disk disk_snap = *disk;
      print_disk(&disk_snap);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
  }
}
