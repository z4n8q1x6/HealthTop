#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

void init_terminal();
void handle_input(int main_mode, int *view);
void reset_terminal();

enum {
  VIEW_CPU,
  VIEW_RAM,
  VIEW_DISK,
  VIEW_PROCESS,
};

#endif // TERMINAL_H
