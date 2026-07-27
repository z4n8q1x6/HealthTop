#ifndef MODES_H
#define MODES_H

typedef struct {
  const char *flag;
  void (*handler)(void);
} ModeEntry;

void run_cpu_mode();
void run_ram_mode();
void run_disk_mode();
void run_process_mode();
void run_json_mode();
void run_main_mode();
void print_help();

#endif // MODES_H
