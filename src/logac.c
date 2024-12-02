#include "logac.h"
#include "config.h"
#include "cstr.h"
#include "msgqueue.h"
#include "datetime.h"
#include "core.h"
#include <unistd.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

const char* log_levels[LOGLEVEL_SIZE] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
};

static const char* const ENDFILE_FLAG = "$----END FILE----$";

static char log_filename[128];
static FILE* fp_logfile;
static int32_t logfile_count;
static TimeStamp logfile_lasttime;
static MsgQueue* msg_queue;
static pthread_t log_thread;

/**
 *  \brief 根据日志文件名模式，更新日志文件名
 */
static bool checkAndFormatLogFileName()
{
    const char* errmsg;
    char tmp[128] = {0};
    bool in_bar = false;
    char* left_bar_idx = logfile_namepat;
    char* p = logfile_namepat;
    size_t top = 0;
    DateTime now_time;
    bool now_time_valid = false;
    int64_t field_len = 0;
    char field[16] = {0};
    const char* field_ptr = NULL;

    while (*p && top < 127) {
        if (*p == '{') {
            if (in_bar) {
                errmsg = "Bar cannot nest";
                goto error_handle;
            } else {
                in_bar = true;
                left_bar_idx = p + 1;
            }
        } else if (*p == '}') {
            if (in_bar) {
                in_bar = false;
                field_ptr = NULL;
                if (p - left_bar_idx == 4 && strncmp(left_bar_idx, "host", 4) == 0) {
                    field_ptr = getHostName();
                    field_len = strlen(field_ptr);
                } else if (p - left_bar_idx == 7 && strncmp(left_bar_idx, "program", 7) == 0) {
                    field_ptr = getProgramName();
                    field_len = strlen(field_ptr);
                } else if (p - left_bar_idx == 4 && strncmp(left_bar_idx, "date", 4) == 0) {
                    if (!now_time_valid) {
                        now_time = timeStampToDateTime(logfile_lasttime);
                        now_time_valid = true;
                    }
                    field_len = dateTimeToStr(field, now_time, "yyyyMMdd");
                    if (field_len <= 0) {
                        errmsg = "Parse date failed";
                        goto error_handle;
                    }
                    field_ptr = field;
                } else if (p - left_bar_idx == 4 && strncmp(left_bar_idx, "time", 4) == 0) {
                    if (!now_time_valid) {
                        now_time = timeStampToDateTime(logfile_lasttime);
                        now_time_valid = true;
                    }
                    field_len = dateTimeToStr(field, now_time, "HHmm");
                    if (field_len <= 0) {
                        errmsg = "Parse time failed";
                        goto error_handle;
                    }
                    field_ptr = field;
                } else if (p - left_bar_idx == 3 && strncmp(left_bar_idx, "num", 3) == 0) {
                    field_len = snprintf(field,  10,"%d", logfile_count);
                    field_ptr = field;
                }

                if (field_ptr != NULL) {
                    strcpy(tmp + top, field_ptr);
                    top += field_len;
                }
            } else {
                errmsg = "No Left Bar Before";
                goto error_handle;
            }
        } else {
            if (!in_bar) {
                tmp[top++] = *p;
            }
        }
        ++p;
    }

    strcpy(log_filename, tmp);
    return true;

error_handle:
    fprintf(stderr, "checkAndFormatLogFileName error, reason: %s.\n", errmsg);
    return false;
}

static void updateLogFile(TimeStamp now)
{
    const char* errmsg;
    bool exist_file = access(log_filename, F_OK | W_OK) == 0;
    bool need_new_file = false;

    // TEST
    DateTime ns = timeStampToDateTime(now);
    char time_str[128] = {0};

    if (*log_filename == 0 || fp_logfile == NULL || !exist_file) {
        // 文件名是空，文件不存在，或文件没有打开，需要重新打开
        need_new_file = true;

        // TEST
        dateTimeToStr(time_str, ns, "yyyy-MM-dd HH:mm:ss");
        printf("At [%s], file is empty, change file.\n", time_str);
        
    } else {
        if (now.sec / logfile_interval > logfile_lasttime.sec / logfile_interval) {
            // 文件时间达到时间间隔整点，需要换文件
            need_new_file = true;

            // TEST
            dateTimeToStr(time_str, ns, "yyyy-MM-dd HH:mm:ss");
            printf("At [%s], time reach interval, change file.\n", time_str);
        }

        long size = ftell(fp_logfile);
        if (size >= logfile_maxsize) {
            // 文件大小达到最大，需要换文件
            need_new_file = true;

            // TEST
            dateTimeToStr(time_str, ns, "yyyy-MM-dd HH:mm:ss");
            printf("At [%s], file reach maxsize, change file.\n", time_str);
        }
    }

    if (need_new_file) {
        // 关闭原有的文件
        if (fp_logfile != NULL) {
            fclose(fp_logfile);
            fp_logfile = NULL;
        }

        // 更新文件时间和编号
        if (!exist_file || now.sec / 60 > logfile_lasttime.sec / 60) {
            logfile_count = 1;
        } else {
            ++logfile_count;
        }
        logfile_lasttime = now;

        // 获取文件名
        if (!checkAndFormatLogFileName()) {
            errmsg = "format logfilename failed";
            goto error_handle;
        }

        printf("switched logfile_name: %s\n", log_filename);

        // 打开文件
        fp_logfile = fopen(log_filename, "w");
        if (fp_logfile == NULL) {
            errmsg = "open logfile failed";
            goto error_handle;
        }
    }

    return;

error_handle:
    fprintf(stderr, "updateLogFile error, reason: %s.\n", errmsg);
    return;
}

static void* threadWriteLog(void* arg)
{
    (void)arg;
    bool thread_run = true;
    while (thread_run)
    {
        // 从消息队列取数据
        CString* item = msgQueuePoll(msg_queue);
        
        // 解析消息时间
        // char time_str[19] = {0};
        // strncpy(time_str, item->data, 18);
        DateTime now_time;
        TimeStamp now;
        if (dateTimeFromStr(&now_time, item->data, "yyyy-MM-dd HH:mm:ss")) {
            now = dateTimeToTimeStamp(now_time);
        } else {
            now = timeStampNowTime();
        }

        // 更新日志文件
        updateLogFile(now);
        
        // 写日志
        fprintf(fp_logfile, "%s\n", item->data);
        fflush(fp_logfile);

        // 读取到结束标记，结束线程
        if (strcmp(ENDFILE_FLAG, item->data) == 0) {
            thread_run = false;
        }
        cstrDelete(item);
    }
    return NULL;
}

void logInit(const char* config_file)
{
    coreInit();
    memset(log_filename, 0, sizeof(log_filename));
    fp_logfile = NULL;
    msg_queue = msgQueueInit();
    logfile_count = 1;
    memset(&logfile_lasttime, 0, sizeof(logfile_lasttime));
    loadLogConfig(config_file); 
    printf("log config:\n");
    printf("  - level: %s\n", log_levels[write_log_level]);
    printf("  - interval: %lds\n", logfile_interval);
    printf("  - maxsize: %ldB\n", logfile_maxsize);
    printf("  - filename: %s\n", logfile_namepat);

    pthread_create(&log_thread, NULL, threadWriteLog, NULL);
}

void logRelease()
{
    // 向消息队列写入结束标记
    CString* end_flag = cstrCreate(ENDFILE_FLAG);
    msgQueuePush(msg_queue, end_flag);

    // 等待线程结束
    pthread_join(log_thread, NULL);

    // 关闭日志文件
    if (fp_logfile != NULL) {
        fclose(fp_logfile);
        fp_logfile = NULL;
    }

    // 释放消息队列
    msgQueueDelete(msg_queue);
}

void logWrite(enum LogLevel level, const char* func, const char* file, int line, const char* fmt, ...)
{
    // 检查日志等级
    if (level < write_log_level) {
        return;
    }

    // 获取日志内容
    va_list args;
    va_start(args, fmt);
    CString* content = cstrVsprintf(fmt, args);
    va_end(args);

    // 获取当前时间
    char time_str[32] = {0};
    DateTime now = dateTimeNowTime();
    int64_t time_str_len = dateTimeToStr(time_str, now, "yyyy-MM-dd HH:mm:ss");
    assert(time_str_len > 0);

    // 获取当前文件名
    const char* pf = strrchr(file, '/'); 
    pf = pf ? pf + 1 : file; 

    // 将日志写入消息队列
    CString* item = cstrSprintf("%s [%s]: %s (%ld:%s:%s:%d)", time_str, log_levels[level], content->data, pthread_self(), func, pf, line);
    msgQueuePush(msg_queue, item);

    // 销毁临时日志内容 
    cstrDelete(content);
}

