#include "terminal.h"
#include "log.h"
#include <bits/posix2_lim.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
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
  printf("\033[?25l"); // hide cursor
}

void handle_input(int *action) {
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
    *action = ACTION_QUIT;
    break;
  case '1':
    *action = ACTION_VIEW_CPU;
    break;
  case '2':
    *action = ACTION_VIEW_RAM;
    break;
  case '3':
    *action = ACTION_VIEW_DISK;
    break;
  case '4':
    *action = ACTION_VIEW_PROCESS;
    break;
  case '5':
    *action = ACTION_VIEW_HEALTH;
    break;
  case 'j':
    *action = ACTION_CURSOR_DOWN;
    break;
  case 'k':
    *action = ACTION_CURSOR_UP;
    break;
  case 'd':
    *action = ACTION_SCROLL_DOWN;
    break;
  case 'u':
    *action = ACTION_SCROLL_UP;
    break;
  case 'g':
    *action = ACTION_FIRST_LINE;
    break;
  case 'G':
    *action = ACTION_LAST_LINE;
    break;
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
  printf("\033[?25h"); // restore cursor
}
