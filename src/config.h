#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

extern enum LogLevel write_log_level;
extern char logfile_namepat[];
extern int64_t logfile_maxsize;
extern int64_t logfile_interval;

bool loadLogConfig(const char* config_file);

#endif