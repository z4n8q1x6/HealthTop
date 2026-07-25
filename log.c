#include "log.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char log_path[PATH_MAX] = {0};

int init_logging() {
  char *home = getenv("HOME");
  if (home == NULL) {
    perror("Failed to get $HOME global variable, getenv");
    return 0;
  }
  snprintf(log_path, sizeof(log_path), "%s/.config/ztop/ztop.log", home);
  FILE *log = fopen(log_path, "a");
  if (log == NULL) {
    perror("Failed to load the log file, fopen");
    return 0;
  }
  fclose(log);
  return 1;
}

// NOTE: We are opening and closing the log file each time, perhaps we should
// just let it open and only close it when program finishes that way we only
// need 1 open and 1 close over a bunch of syscalls, this version of the
// function does have an upside tho because if we use the same open log file in
// mutiple threads it leads to race condition beacuse the "a" mode in
// fopen (which mirrors the O_APPEND flag in open) gurantee atomicity only for
// different fds.
//
// ABOUT: use -1 or any negative number if no errno
int log_msg(int flag, const char *msg, int error) {
  if (log_path[0] == '\0')
    return 0;
  FILE *log = fopen(log_path, "a");
  if (log == NULL) {
    perror("Failed to open the log file, fopen");
    return 0;
  }
  char time_buffer[256] = {0};
  get_time(time_buffer, sizeof(time_buffer));
  const char *str_flag;
  switch (flag) {
  case LOG_ERROR:
    str_flag = "Error";
    break;
  case LOG_INFO:
    str_flag = "Info";
    break;
  case LOG_WARNING:
    str_flag = "Warning";
    break;
  default:
    str_flag = "Unknown";
    break;
  }
  if (error < 0) {
    char error_buffer[256];
    strerror_r(error, error_buffer, sizeof(error_buffer));
    if (fprintf(log, "%s | %s: %s | %s\n", time_buffer, str_flag, msg,
                error_buffer) < 0) {
      perror("Failed to write to the log file, fprintf");
      fclose(log);
      return 0;
    }
  } else {
    if (fprintf(log, "%s | %s: %s\n", time_buffer, str_flag, msg) < 0) {
      perror("Failed to write to the log file, fprintf");
      fclose(log);
      return 0;
    }
  }
  fclose(log);
  return 1;
}

int get_time(char *buffer, size_t n) {
  time_t current = time(NULL);
  struct tm *local = localtime(&current);
  if (strftime(buffer, n, "%Y-%m-%d (%H:%M:%S)", local) == 0)
    return 0;
  return 1;
}
