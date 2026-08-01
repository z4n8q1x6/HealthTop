#include "terminal.h"
#include "log.h"
#include <bits/posix2_lim.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void init_terminal() {
  struct termios t;
  if (tcgetattr(STDIN_FILENO, &t) == -1) {
    log_msg(LOG_ERROR, "init_terminal: tcgetattr", errno);
    exit(EXIT_FAILURE);
  }
  t.c_lflag &= ~(ICANON | ECHO);
  if (tcsetattr(STDIN_FILENO, TCSANOW, &t) != 0) {
    log_msg(LOG_ERROR, "init_terminal: tcsetattr", errno);
    exit(EXIT_FAILURE);
  }
}

void handle_input(int main_mode, int *view) {
  char c;
  ssize_t n = read(STDIN_FILENO, &c, 1);
  if (n == -1) {
    log_msg(LOG_ERROR, "handle_input: read", errno);
    return;
  } else if (n == 0) {
    return;
  }
  switch (c) {
  case 'q':
    reset_terminal();
    exit(EXIT_SUCCESS);
    break;
    if (main_mode) {
    case '1':
      *view = VIEW_CPU;
      break;
    case '2':
      *view = VIEW_RAM;
      break;
    case '3':
      *view = VIEW_DISK;
      break;
    case '4':
      *view = VIEW_PROCESS;
      break;
    }
  }
}

void reset_terminal() {
  struct termios orig;
  if (tcgetattr(STDIN_FILENO, &orig) == -1) {
    log_msg(LOG_ERROR, "reset_terminal: tcgetattr", errno);
    exit(EXIT_FAILURE);
  }
  orig.c_lflag |= (ICANON | ECHO);
  if (tcsetattr(STDIN_FILENO, TCSANOW, &orig) != 0) {
    log_msg(LOG_ERROR, "reset_terminal: tcsetattr", errno);
    exit(EXIT_FAILURE);
  }
}
