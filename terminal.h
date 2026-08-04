#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

void init_terminal();
void handle_input(int *action);
void reset_terminal();

enum {
  ACTION_QUIT,
  ACTION_VIEW_CPU,
  ACTION_VIEW_RAM,
  ACTION_VIEW_DISK,
  ACTION_VIEW_PROCESS,
  ACTION_VIEW_HEALTH,
  ACTION_CURSOR_DOWN,
  ACTION_CURSOR_UP,
  ACTION_SCROLL_DOWN,
  ACTION_SCROLL_UP,
  ACTION_FIRST_LINE,
  ACTION_LAST_LINE,
};

#endif // TERMINAL_H
