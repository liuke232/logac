#include "core.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>

static char hostname[128];
static char username[128];
static char program[128];
static pthread_once_t host_inited;
static pthread_once_t user_inited;
static pthread_once_t program_inited;

static void hostInit()
{
    gethostname(hostname, sizeof(hostname));
}

static void userInit()
{
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);

    if (pw != NULL) {
        strcpy(username, pw->pw_name);
    } else {
        strcpy(username, "unknown");
    }
}

static void programInit()
{
    char path[512] = {0};
    char process_name[512] = {0};
    FILE *fp;

    // 获取当前进程的PID
    pid_t pid = getpid();

    // 构建cmdline文件的路径
    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);

    // 打开文件
    fp = fopen(path, "r");
    if (!fp) {
        // 找不到文件
        perror("Failed to open /proc/<pid>/cmdline");
        goto error_handle;
    }

    // 读取文件内容到process_name中
    if (fgets(process_name, sizeof(process_name), fp) == NULL) {
        // 文件没有内容
        perror("file /proc/<pid>/cmdline is empty");
        goto error_handle;
    }

    // 去除字符串末尾的换行符
    process_name[strcspn(process_name, "\n")] = 0;
    // 找到最后一个/符号的下一位
    char* p = strrchr(process_name, '/') + 1;
    strcpy(program, p);
    fclose(fp);
    return;

error_handle:
    // 使用pid+进程id代替进程名
    sprintf(program, "pid%d", pid);
    // 关闭文件
    fclose(fp);
    return;
}

void coreInit()
{
    memset(hostname, 0, sizeof(hostname));
    memset(username, 0, sizeof(username));
    memset(program, 0, sizeof(program));
    host_inited = PTHREAD_ONCE_INIT;
    user_inited = PTHREAD_ONCE_INIT;
    program_inited = PTHREAD_ONCE_INIT;
}

const char* getHostName()
{
    pthread_once(&host_inited, hostInit);
    return hostname;
}

const char* getUserName()
{
    pthread_once(&user_inited, userInit);
    return username;
}
const char* getProgramName()
{
    pthread_once(&program_inited, programInit);
    return program;
}

