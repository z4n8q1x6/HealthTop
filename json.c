#include "health.h"
#include "log.h"
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>

// ztop --json simply export the json and exit the program
// ztop will dynamically export jsons every loop iterration
int export_json(Cpu *cpu, Ram *ram, Disk *disk, int score) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    log_msg(LOG_ERROR, "export_json: getcwd", errno);
    return 0;
  }
  char path[PATH_MAX];
  snprintf(path, sizeof(path), "%s/ztop.json", cwd);
  FILE *json = fopen(path, "w");
  if (json == NULL) {
    log_msg(LOG_ERROR, "Failed to export json", errno);
    return 0;
  }
  fprintf(json,
          "{\n"
          "\"cpu\": %d,\n"
          "\"ram\": %d,\n"
          "\"disk\": %d,\n"
          "\"health_score\": %d,\n"
          "\"status\": \"%s\"\n"
          "}\n",
          (int)cpu->usage, ram->usage, disk->usage, score,
          get_health_status(score));
  fclose(json);
  return 1;
}
