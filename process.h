#ifndef PROCESSS_H
#define PROCESSS_H

#include <unistd.h>

typedef struct {
  char pid[32];
  char name[256];
  char user[256];
  char state[32];
  unsigned int threads;
  unsigned long mem;
} ProcessInfo;

typedef struct {
  ProcessInfo *items;
  size_t capacity;
  size_t count;
} ProcessList;

int read_processes(ProcessList *ps);
void *processes_thread(void *arg);
void print_processes(ProcessList *ps, size_t *offset);
int get_processes(ProcessList *ps);

#endif // PROCESSS_H
