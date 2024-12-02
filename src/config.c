#include "config.h"
#include "datetime.h"
#include "logac.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum LogLevel write_log_level;
char logfile_namepat[128];
int64_t logfile_maxsize;
int64_t logfile_interval;

static void parseKeyValue(char* line, char** key, char** value)
{
    size_t len = strlen(line);
    if (len == 0) {
        goto error_handle;
    }
    line[len - 1] = 0;
    char* pos = strchr(line, '=');
    if (pos == NULL) {
        goto error_handle;
    }
    *key = line;
    *value = pos + 1;
    *pos = 0;
    return;

error_handle:
    *key = *value = NULL;
    return;
}

static int64_t parseMaxSize(char* value) {
    size_t len = strlen(value);
    char* end = value + len;
    int64_t base = 1;
    
    if (len > 1 && strcmp("K", end - 1) == 0) {
        base = 1024;
        end[-1] = 0;
    } else if (len > 1 && strcmp("M", end - 1) == 0) {
        base = 1024 * 1024;
        end[-1] = 0;
    } else if (len > 1 && strcmp("G", end - 1) == 0) {
        base = 1024 * 1024 * 1024;
        end[-1] = 0;
    } else if (len > 2 && strcmp("KB", end - 2) == 0) {
        base = 1024;
        end[-2] = 0;
    } else if (len > 2 && strcmp("MB", end - 2) == 0) {
        base = 1024 * 1024;
        end[-2] = 0;
    } else if (len > 2 && strcmp("GB", end - 2) == 0) {
        base = 1024 * 1024 * 1024;
        end[-2] = 0;
    } else if (len > 1 && strcmp("B", end - 1) == 0) {
        base = 1;
        end[-1] = 0;
    }

    int64_t ret = strtol(value, NULL, 10);
    return ret > 0 ? ret * base : 0;
}

static int64_t parseInterval(char* value)
{
    size_t len = strlen(value);
    char* end = value + len;
    int64_t base = 1;
    
    if (len > 1 && strcmp("s", end - 1) == 0) {
        base = 1;
        end[-1] = 0;
    } else if (len > 1 && strcmp("m", end - 1) == 0) {
        base = 60;
        end[-1] = 0;
    } else if (len > 1 && strcmp("h", end - 1) == 0) {
        base = 60 * 60;
        end[-1] = 0;
    } else if (len > 1 && strcmp("d", end - 1) == 0) {
        base = 60 * 60 * 24;
        end[-1] = 0;
    }

    int64_t ret = strtol(value, NULL, 10);
    return ret > 0 ? ret * base : 0;
}

bool loadLogConfig(const char* config_file)
{
    write_log_level = LOGLEVEL_INFO;
    strcpy(logfile_namepat, "{host}_{program}_{date}_{time}_{num}.log");
    logfile_maxsize = 500 * 1024 * 1024;
    logfile_interval = 24 * 60 * 60;
    FILE* fp_cfg = NULL;

    if (config_file == NULL) {
        // 使用默认配置
        goto ok;
    }

    fp_cfg = fopen(config_file, "r");
    if (!fp_cfg) {
        fprintf(stderr, "Open Log Config Failed!\n");
        return false;
    }
    char *key, *value;
    char line[256] = {0};
    while (fgets(line, sizeof(line), fp_cfg) != NULL) {
        parseKeyValue(line, &key, &value);
        if (key == NULL || value == NULL) {
            continue;
        }

        if (strcmp(key, "logfilepatten") == 0) {
            strcpy(logfile_namepat, value);
        } else if (strcmp(key, "maxsize") == 0) { 
            int num = parseMaxSize(value);
            if (num > 0) {
                logfile_maxsize = num;
            }
        } else if (strcmp(key, "interval") == 0) {
            int num = parseInterval(value);
            if (num > 0) {
                logfile_interval = num;
            }
        } else if (strcmp(key, "loglevel") == 0) {
            for (size_t i = 0; i < LOGLEVEL_SIZE; ++i) {
                if (strcmp(value, log_levels[i]) == 0) {
                    write_log_level = (enum LogLevel)i;
                    break;
                }
            }
        }
    }

ok:
    printf("loadLogConfig succ.\n");
    fclose(fp_cfg);
    return true;
}