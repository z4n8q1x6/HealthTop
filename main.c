#include "log.h"
#include "modes.h"
#include "terminal.h"
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void handle_sigint(int sig) {
  reset_terminal();
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  signal(SIGINT, handle_sigint);
  init_logging();
  const ModeEntry modes[] = {
      {"--cpu", run_cpu_mode},   {"--ram", run_ram_mode},
      {"--disk", run_disk_mode}, {"--process", run_process_mode},
      {"--json", run_json_mode}, {"--help", print_help},
  };
  if (argc <= 1) {
    run_main_mode();
    return 0;
  } else {
    for (size_t i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
      if (strcmp(argv[1], modes[i].flag) == 0) {
        modes[i].handler();
        return 0;
      }
    }
  }
  printf("Invalid argument: %s\n"
         "Use --help to see available options.\n",
         argv[1]);
  return 1;
}

// TODO: implement process, improve ui and refine
// ./ztop
// ./ztop --cpu
// ./ztop --ram
// ./ztop --disk
// ./ztop --process
// ./ztop --json
// ./ztop --help
