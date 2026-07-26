#include "config.h"
#include "cpu.h"
#include "disk.h"
#include "health.h"
#include "log.h"
#include "main_loop.h"
#include "ram.h"
#include "terminal.h"
#include <bits/time.h>
#include <errno.h>
#include <limits.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <unistd.h>

void handle_sigint(int sig) {
  reset_terminal();
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  signal(SIGINT, handle_sigint);
  init_terminal();
#include <string.h>
  char *args[] = {"--cpu", "--ram", "--disk", "--process", "--json", "--help"};
  if (argc > 1) {
    if (strncmp(argv[1], args[0], strlen(args[0])) == 0) {
      Cpu cpu = {.usage = 0, .nb_cores = 0, .model_name = {0}};
      if (!init_cpu(&cpu)) {
        reset_terminal();
        return 1;
      }
      pthread_t cpu_pthread;
      if (pthread_create(&cpu_pthread, NULL, cpu_thread, &cpu.usage) != 0) {
        log_msg(LOG_ERROR, "cpu pthread_create", errno);
        reset_terminal();
        return 1;
      }
      run_main_loop(&cpu, NULL, NULL);
    } else if (strncmp(argv[1], args[1], strlen(args[1])) == 0) {
      _Atomic Ram ram;
      if (!init_ram(&ram)) {
        reset_terminal();
        return 1;
      }
      pthread_t ram_pthread;
      if (pthread_create(&ram_pthread, NULL, ram_thread, &ram) != 0) {
        log_msg(LOG_ERROR, "ram pthread_create", errno);
        reset_terminal();
        return 1;
      }
      run_main_loop(NULL, &ram, NULL);
    } else if (strncmp(argv[1], args[2], strlen(args[2])) == 0) {
      _Atomic Disk disk;
      if (!init_disk(&disk)) {
        reset_terminal();
        return 1;
      }
      pthread_t disk_pthread;
      if (pthread_create(&disk_pthread, NULL, disk_thread, &disk) != 0) {
        log_msg(LOG_ERROR, "disk pthread_create", errno);
        reset_terminal();
        return 1;
      }
      run_main_loop(NULL, NULL, &disk);
    } else if (strncmp(argv[1], args[4], strlen(args[4])) == 0) {
      reset_terminal();
      Config conf = {.cpu_threshold = DEFAULT_CPU_THRESHOLD,
                     .ram_threshold = DEFAULT_RAM_THRESHOLD,
                     .disk_threshold = DEFAULT_DISK_THRESHOLD,
                     .refresh_interval = DEFAULT_REFRESH_INTERVAL};
      init_logging();
      load_conf(&conf);
      Cpu cpu = {.usage = 0, .nb_cores = 0, .model_name = {0}};
      if (!init_cpu(&cpu)) {
        reset_terminal();
        return 1;
      }
      set_cpu_usage(&cpu.usage, 0);
      _Atomic Ram ram;
      if (!init_ram(&ram)) {
        reset_terminal();
        return 1;
      }
      _Atomic Disk disk;
      if (!init_disk(&disk)) {
        reset_terminal();
        return 1;
      }
#include "json.h"
      Ram snap_ram = ram;
      Disk snap_disk = disk;
      int score = get_health_score(&cpu, &snap_ram, &snap_disk, &conf);
      if (!export_json(&cpu, &snap_ram, &snap_disk, score)) {
        return 1;
      }
      return 0;
    } else {
      reset_terminal();
      printf("Invalid arg\n");
      return 1;
    }
  } else {
    Cpu cpu = {.usage = 0, .nb_cores = 0, .model_name = {0}};
    if (!init_cpu(&cpu)) {
      reset_terminal();
      return 1;
    }
    pthread_t cpu_pthread;
    if (pthread_create(&cpu_pthread, NULL, cpu_thread, &cpu.usage) != 0) {
      log_msg(LOG_ERROR, "cpu pthread_create", errno);
      reset_terminal();
      return 1;
    }

    _Atomic Ram ram;
    if (!init_ram(&ram)) {
      reset_terminal();
      return 1;
    }
    pthread_t ram_pthread;
    if (pthread_create(&ram_pthread, NULL, ram_thread, &ram) != 0) {
      log_msg(LOG_ERROR, "ram pthread_create", errno);
      reset_terminal();
      return 1;
    }

    _Atomic Disk disk;
    if (!init_disk(&disk)) {
      reset_terminal();
      return 1;
    }
    pthread_t disk_pthread;
    if (pthread_create(&disk_pthread, NULL, disk_thread, &disk) != 0) {
      log_msg(LOG_ERROR, "disk pthread_create", errno);
      reset_terminal();
      return 1;
    }
    run_main_loop(&cpu, &ram, &disk);
  }
}

// TODO:  finish json, refactor main, implement process then  improve ui +
// refactoring
// ./ztop
// ./ztop --cpu
// ./ztop --ram
// ./ztop --disk
// ./ztop --process
// ./ztop --json
// ./ztop --help
