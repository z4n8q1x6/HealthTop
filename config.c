#include "config.h"
#include "limits.h"
#include "log.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int load_conf(Config *conf) {
  char path[PATH_MAX];
  char *home = getenv("HOME");
  if (home == NULL) {
    log_msg(LOG_WARNING, "Failed to get $HOME global var", -1);
    return 0;
  }
  snprintf(path, sizeof(path), "%s/.config/ztop/ztop.conf", home);
  FILE *conf_file = fopen(path, "r");
  if (conf_file == NULL) {
    log_msg(LOG_INFO, "No config file found, using defaults", errno);
    return 0;
  }
  char line[LINE_MAX];
  Config tmp = {0};
  while (fgets(line, sizeof(line), conf_file) != NULL) {
    if (sscanf(line, "cpu_threshold=%d", &tmp.cpu_threshold)) {
      if (!(tmp.cpu_threshold < 0 || tmp.cpu_threshold > 100)) {
        conf->cpu_threshold = tmp.cpu_threshold;
      }
      continue;
    }

    if (sscanf(line, "ram_threshold=%d", &tmp.ram_threshold)) {
      if (!(tmp.ram_threshold < 0 || tmp.ram_threshold > 100)) {
        conf->ram_threshold = tmp.ram_threshold;
      }
      continue;
    }

    if (sscanf(line, "disk_threshold=%d", &tmp.disk_threshold)) {
      if (!(tmp.disk_threshold < 0 || tmp.disk_threshold > 100)) {
        conf->disk_threshold = tmp.disk_threshold;
      }
      continue;
    }

    if (sscanf(line, "refresh_interval=%lf", &tmp.refresh_interval)) {
      if (!(tmp.refresh_interval <= 0)) {
        conf->refresh_interval = tmp.refresh_interval;
      }
      continue;
    }
  }

  fclose(conf_file);
  return 1;
}
