#include "process.h"
#include "util.h"
#include <dirent.h>
#include <pthread.h>
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int init_processlist(ProcessList *ps) {
  ps->count = 0;
  ps->capacity = 5000;
  ps->items = malloc(sizeof(*ps->items) * ps->capacity);
  if (ps->items == NULL) {
    perror("malloc");
    return 0;
  }
  return 1;
}

int read_processes(ProcessList *ps) {
  DIR *proc_dir = opendir("/proc/");
  if (proc_dir == NULL) {
    perror("opendir");
    return 0;
  }
  struct dirent *proc_dirent;
  while ((proc_dirent = readdir(proc_dir))) {
    int pid = atoi(proc_dirent->d_name);
    if (pid) {
      char current_path[1024] = "/proc/";
      strcat(current_path, proc_dirent->d_name);
      char status_path[1024] = {0};
      strcpy(status_path, current_path);
      strcat(status_path, "/status");
      FILE *status = fopen(status_path, "r");
      if (status == NULL) {
        perror("status fopen");
        continue;
      }
      if (ps->count >= ps->capacity) {
        ProcessInfo *tmp = realloc(ps->items, sizeof(*tmp) * ps->capacity * 2);
        if (tmp == NULL) {
          perror("realloc");
          return 0;
        }
        ps->items = tmp;
        ps->capacity *= 2;
      }
      char line[512];
      while (fgets(line, sizeof(line), status) != NULL) {
        int count = ps->count;
        char **tokens = get_tokens(line);
        if (tokens == NULL)
          continue;
        if (tokens[1] == NULL) {
          free(tokens);
          continue;
        }
        if (strcmp(tokens[0], "Pid:") == 0) {
          snprintf(ps->items[count].pid, sizeof(ps->items[count].pid), "%s",
                   tokens[1]);

        } else if (strcmp(tokens[0], "Name:") == 0) {
          snprintf(ps->items[count].name, sizeof(ps->items[count].name), "%s",
                   tokens[1]);

        } else if (strcmp(tokens[0], "State:") == 0) {
          snprintf(ps->items[count].state, sizeof(ps->items[count].state), "%s",
                   tokens[1]);

        } else if (strcmp(tokens[0], "Threads:") == 0) {
          ps->items[count].threads = atoi(tokens[1]);

        } else if (strcmp(tokens[0], "Uid:") == 0) {
          uid_t uid = atoi(tokens[1]);
          struct passwd *pw = getpwuid(uid);
          if (pw != NULL) {
            snprintf(ps->items[count].user, sizeof(ps->items[count].user), "%s",
                     pw->pw_name);
          }
        } else if (strcmp(tokens[0], "VmRSS:") == 0) {
          ps->items[count].mem = strtoul(tokens[1], NULL, 0);
        }
        free(tokens);
      }
      fclose(status);
      ps->count++;
    }
  }
  closedir(proc_dir);
  return 1;
}

void *processes_thread(void *arg) {
  _Atomic ProcessList *ps = arg;
  ProcessList buff1 = {0};
  ProcessList buff2 = {0};
  if (!init_processlist(&buff1)) {
    return NULL;
  }
  if (!read_processes(&buff1)) {
    fprintf(stderr, "Failed to read processes\n");
  } else {
    *ps = buff1;
  }
  sleep(1);
  while (1) {
    buff2 = buff1;
    if (!init_processlist(&buff1)) {
      return NULL;
    }
    if (!read_processes(&buff1)) {
      fprintf(stderr, "Failed to read processes\n");
    } else {
      *ps = buff1;
    }
    free(buff2.items);
    buff2.items = NULL;
    sleep(1);
  }
  return NULL;
}

void print_processes(ProcessList *ps) {
  printf("-------------------------------------------------------------------"
         "--\n");
  printf("%-8s %-35s %-10s %-10s %-15s %-10s\n", "PID", "NAME", "STATE",
         "THREADS", "USER", "MEM");

  for (int i = 0; i < ps->count; i++) {
    printf("%-8s %-35s %-10s %-10d %-15s %lu\n", ps->items[i].pid,
           ps->items[i].name, ps->items[i].state, ps->items[i].threads,
           ps->items[i].user, ps->items[i].mem);
  }

  printf("-------------------------------------------------------------------"
         "--\n");
}
