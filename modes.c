#include "config.h"
#include "cpu.h"
#include "health.h"
#include "json.h"
#include "log.h"
#include "main_loop.h"
#include "terminal.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void run_cpu_mode() {
  Cpu cpu = {.usage = 0, .nb_cores = 0, .model_name = {0}};
  if (!init_cpu(&cpu)) {
    log_msg(LOG_ERROR, "Failed to initialize the cpu", -1);
    exit(EXIT_FAILURE);
  }
  pthread_t cpu_pthread;
  if (pthread_create(&cpu_pthread, NULL, cpu_thread, &cpu.usage) != 0) {
    log_msg(LOG_ERROR, "cpu pthread_create", errno);
    exit(EXIT_FAILURE);
  }
  init_terminal();
  run_main_loop(&cpu, NULL, NULL);
}

void run_ram_mode() {
  _Atomic Ram ram;
  if (!init_ram(&ram)) {
    log_msg(LOG_ERROR, "Failed to initialize the ram", -1);
    exit(EXIT_FAILURE);
  }
  pthread_t ram_pthread;
  if (pthread_create(&ram_pthread, NULL, ram_thread, &ram) != 0) {
    log_msg(LOG_ERROR, "ram pthread_create", errno);
    exit(EXIT_FAILURE);
  }
  init_terminal();
  run_main_loop(NULL, &ram, NULL);
}

void run_disk_mode() {
  _Atomic Disk disk;
  if (!init_disk(&disk)) {
    log_msg(LOG_ERROR, "Failed to initialize the disk", -1);
    exit(EXIT_FAILURE);
  }
  pthread_t disk_pthread;
  if (pthread_create(&disk_pthread, NULL, disk_thread, &disk) != 0) {
    log_msg(LOG_ERROR, "disk pthread_create", errno);
    exit(EXIT_FAILURE);
  }
  init_terminal();
  run_main_loop(NULL, NULL, &disk);
}

void run_json_mode() {
  Config conf = {.cpu_threshold = DEFAULT_CPU_THRESHOLD,
                 .ram_threshold = DEFAULT_RAM_THRESHOLD,
                 .disk_threshold = DEFAULT_DISK_THRESHOLD,
                 .refresh_interval = DEFAULT_REFRESH_INTERVAL};
  load_conf(&conf);
  Cpu cpu = {.usage = 0, .nb_cores = 0, .model_name = {0}};
  if (!init_cpu(&cpu)) {
    log_msg(LOG_ERROR, "Failed to initialize the cpu", -1);
    exit(EXIT_FAILURE);
  }
  if (!set_cpu_usage(&cpu.usage, 0)) {
    log_msg(LOG_ERROR, "Failed to fetch the cpu usage", -1);
    exit(EXIT_FAILURE);
  }
  _Atomic Ram ram;
  if (!init_ram(&ram)) {
    log_msg(LOG_ERROR, "Failed to initialize the ram", -1);
    exit(EXIT_FAILURE);
  }
  _Atomic Disk disk;
  if (!init_disk(&disk)) {
    log_msg(LOG_ERROR, "Failed to initialize the disk", -1);
    exit(EXIT_FAILURE);
  }
  Ram snap_ram = ram;
  Disk snap_disk = disk;
  int score = get_health_score(&cpu, &snap_ram, &snap_disk, &conf);
  if (!export_json(&cpu, &snap_ram, &snap_disk, score)) {
    exit(EXIT_FAILURE);
  }
}

void run_main_mode() {
  Cpu cpu = {.usage = 0, .nb_cores = 0, .model_name = {0}};
  if (!init_cpu(&cpu)) {
    log_msg(LOG_ERROR, "Failed to initialize the cpu", -1);
    exit(EXIT_FAILURE);
  }
  pthread_t cpu_pthread;
  if (pthread_create(&cpu_pthread, NULL, cpu_thread, &cpu.usage) != 0) {
    log_msg(LOG_ERROR, "cpu pthread_create", errno);
    exit(EXIT_FAILURE);
  }

  _Atomic Ram ram;
  if (!init_ram(&ram)) {
    log_msg(LOG_ERROR, "Failed to initialize the ram", -1);
    exit(EXIT_FAILURE);
  }
  pthread_t ram_pthread;
  if (pthread_create(&ram_pthread, NULL, ram_thread, &ram) != 0) {
    log_msg(LOG_ERROR, "ram pthread_create", errno);
    exit(EXIT_FAILURE);
  }

  _Atomic Disk disk;
  if (!init_disk(&disk)) {
    log_msg(LOG_ERROR, "Failed to initialize the disk", -1);
    exit(EXIT_FAILURE);
  }
  pthread_t disk_pthread;
  if (pthread_create(&disk_pthread, NULL, disk_thread, &disk) != 0) {
    log_msg(LOG_ERROR, "disk pthread_create", errno);
    exit(EXIT_FAILURE);
  }
  init_terminal();
  run_main_loop(&cpu, &ram, &disk);
}

void run_compact_mode() {}

void print_help() {
  printf("Usage: ztop [OPTION]\n\n");
  printf("Options:\n");
  printf("  ztop            Launch full dashboard (CPU, RAM, disk)\n");
  printf("  ztop --cpu      Show CPU only\n");
  printf("  ztop --ram      Show RAM only\n");
  printf("  ztop --disk     Show disk only\n");
  printf("  ztop --json     Print system status as JSON and exit\n");
  printf("  ztop --help     Show this help message\n\n");
  exit(EXIT_SUCCESS);
}
