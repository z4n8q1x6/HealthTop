#include "cpu.h"
#include "disk.h"
#include "main_loop.h"
#include "ram.h"
#include "terminal.h"
#include <bits/time.h>
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
  char *args[] = {"--cpu", "--ram", "--disk"};
  if (argc > 1) {
    if (strncmp(argv[1], args[0], strlen(args[0])) == 0) {
      Cpu cpu = {.usage = 0, .nb_cores = 0, .model_name = {0}};
      init_cpu(&cpu);
      pthread_t cpu_pthread;
      if (pthread_create(&cpu_pthread, NULL, cpu_thread, &cpu.usage) != 0) {
        perror("cpu pthread_create");
        return 1;
      }
      run_main_loop(&cpu, NULL, NULL);
    } else if (strncmp(argv[1], args[1], strlen(args[1])) == 0) {
      _Atomic Ram ram;
      init_ram(&ram);
      pthread_t ram_pthread;
      if (pthread_create(&ram_pthread, NULL, ram_thread, &ram) != 0) {
        perror("ram pthread_create");
        return 1;
      }
      run_main_loop(NULL, &ram, NULL);
    } else if (strncmp(argv[1], args[2], strlen(args[2])) == 0) {
      _Atomic Disk disk;
      init_disk(&disk);
      pthread_t disk_pthread;
      if (pthread_create(&disk_pthread, NULL, disk_thread, &disk) != 0) {
        perror("disk pthread_create");
        return 1;
      }
      run_main_loop(NULL, NULL, &disk);
    } else {
      printf("Invalid arg\n");
      reset_terminal();
      return 1;
    }
  } else {

    Cpu cpu = {.usage = 0, .nb_cores = 0, .model_name = {0}};
    init_cpu(&cpu);
    pthread_t cpu_pthread;
    if (pthread_create(&cpu_pthread, NULL, cpu_thread, &cpu.usage) != 0) {
      perror("cpu pthread_create");
      return 1;
    }

    _Atomic Ram ram;
    init_ram(&ram);
    pthread_t ram_pthread;
    if (pthread_create(&ram_pthread, NULL, ram_thread, &ram) != 0) {
      perror("ram pthread_create");
      return 1;
    }

    _Atomic Disk disk;
    init_disk(&disk);
    pthread_t disk_pthread;
    if (pthread_create(&disk_pthread, NULL, disk_thread, &disk) != 0) {
      perror("disk pthread_create");
      return 1;
    }
    run_main_loop(&cpu, &ram, &disk);
  }
}

// TODO:  improve ui,  score de santé, historique des
// mesures, alerte intelligente ,implement process
//
// ./monitor           # Lancement du dashboard principal
// ./monitor --cpu     # Affichage ciblé du CPU
// ./monitor --ram     # Affichage ciblé de la RAM
// ./monitor --disk    # Affichage ciblé du disque
// ./monitor --process # Liste des processus
// ./monitor --json    # Export au format JSON
// ./monitor --help    # Liste de l'ensemble des commandes
