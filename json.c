#include "health.h"
#include "log.h"
#include <errno.h>
#include <stdio.h>

// ztop --json simply export the json and exit the program
// ztop will dynamically export jsons every loop iterration
int export_json(Cpu *cpu, Ram *ram, Disk *disk, int score) {
  FILE *json = fopen("ztop.json", "w");
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
