#include "log.h"
#include "modes.h"
#include "terminal.h"
#include <bits/time.h>
#include <limits.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

void handle_sigint(int sig) {
  reset_terminal();
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  signal(SIGINT, handle_sigint);
  init_logging();
  const char *args[] = {"--cpu",     "--ram",  "--disk",
                        "--process", "--json", "--help"};
  if (argc > 1) {
    if (strncmp(argv[1], args[0], strlen(args[0])) == 0) {
      run_cpu_mode();
    } else if (strncmp(argv[1], args[1], strlen(args[1])) == 0) {
      run_ram_mode();
    } else if (strncmp(argv[1], args[2], strlen(args[2])) == 0) {
      run_disk_mode();
    } else if (strncmp(argv[1], args[3], strlen(args[3])) == 0) {
      // run_process_mode();
    } else if (strncmp(argv[1], args[4], strlen(args[4])) == 0) {
      run_json_mode();
    } else if (strncmp(argv[1], args[5], strlen(args[5])) == 0) {
      print_help();
    } else {
      reset_terminal();
      printf("Invalid argument: %s\n"
             "Use --help to see available options.",
             argv[1]);
      return 1;
    }
  } else {
    run_main_mode();
  }
  return 0;
}

// TODO: compact modes, refactor main, implement process then  improve ui +
// refactoring
// ./ztop
// ./ztop --cpu
// ./ztop --ram
// ./ztop --disk
// ./ztop --process
// ./ztop --json
// ./ztop --help
