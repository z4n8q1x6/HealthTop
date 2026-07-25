#ifndef CONF_H
#define CONF_H

typedef struct {
  int cpu_threshold;
  int ram_threshold;
  int disk_threshold;
  double refresh_interval;
} Config;

int load_conf(Config *conf);

#define DEFAULT_CPU_THRESHOLD 80
#define DEFAULT_RAM_THRESHOLD 80
#define DEFAULT_DISK_THRESHOLD 90
#define DEFAULT_REFRESH_INTERVAL 1.0

#endif // CONF_H
