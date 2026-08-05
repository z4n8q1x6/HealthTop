#include "log.h"
#include "errno.h"
#include <bits/posix2_lim.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define LOG_SIZE_MAX (2 * 1024 * 1024)      // 2MiB
#define HISTORY_SIZE_MAX (60 * 1024 * 1024) // 60MiB

static char log_path[PATH_MAX] = {0};
static char history_path[PATH_MAX] = {0};
static FILE *history = NULL;

int init_logging() {
  char *home = getenv("HOME");
  if (home == NULL) {
    perror("Failed to get $HOME global variable, getenv");
    return 0;
  }
  char ztop_dir[PATH_MAX];
  snprintf(ztop_dir, sizeof(ztop_dir), "%s/.config/ztop", home);
  if (mkdir(ztop_dir, 0750) == -1 && errno != EEXIST) {
    perror("mkdir");
    return 0;
  }
  snprintf(log_path, sizeof(log_path), "%s/ztop.log", ztop_dir);
  FILE *log = fopen(log_path, "a");
  if (log == NULL) {
    perror("Failed to load ztop.log, fopen");
    return 0;
  }
  snprintf(history_path, sizeof(history_path), "%s/.config/ztop/history.log",
           home);
  history = fopen(history_path, "a");
  if (history == NULL) {
    log_msg(LOG_WARNING, "Failed to load history.log", errno);
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
  if (error > 0) {
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
  fseek(log, 0, SEEK_END);
  long size = ftell(log);
  if (size > LOG_SIZE_MAX) {
    if (!shrink_log(log_path, LOG_SIZE_MAX)) {
      perror("Failed to shrink ztop.log");
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

void log_mesures(Cpu *cpu, Ram *ram, Disk *disk, int score) {
  if (history == NULL)
    return;
  char time_buffer[256];
  get_time(time_buffer, sizeof(time_buffer));
  fprintf(history, "%s | CPU=%d%% | RAM=%d%% | DISK=%d%% | HEALTH=%d%%\n",
          time_buffer, (int)cpu->usage, ram->usage, disk->usage, score);

  fseek(history, 0, SEEK_END);
  long size = ftell(history);
  if (size > HISTORY_SIZE_MAX) {
    fclose(history);
    if (!shrink_log(history_path, HISTORY_SIZE_MAX)) {
      perror("Failed to shrink history.log");
    }
    history = fopen(history_path, "a");
    if (history == NULL) {
      log_msg(LOG_WARNING, "Failed to load history.log", errno);
      return;
    }
  }
}

int shrink_log(char *filename, long size) {
  FILE *log = fopen(filename, "r+");
  if (log == NULL)
    return 0;
  fseek(log, 0, SEEK_END);
  char tmp_path[PATH_MAX];
  snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", filename);
  FILE *tmp = fopen(tmp_path, "w");
  if (tmp == NULL)
    return 0;
  long offset = size;
  fseek(log, -offset, SEEK_END);
  char line[LINE_MAX];
  fgets(line, sizeof(line), log); // eat first line
  while (fgets(line, sizeof(line), log) != NULL) {
    fprintf(tmp, "%s", line);
  }
  fclose(log);
  fclose(tmp);
  remove(filename);
  rename("tmp.log", filename);
  return 1;
}
