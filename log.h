#ifndef LOG_H
#define LOG_H

#include <unistd.h>

enum { LOG_ERROR, LOG_INFO, LOG_WARNING };

int init_logging();
int log_msg(int flag, const char *msg, int error);
int get_time(char *buffer, size_t n);

#endif // LOG_H
