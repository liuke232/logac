#ifndef __HEADER_LOGAC_H
#define __HEADER_LOGAC_H

/**
 *  @brief 日志等级
 */
enum LogLevel {
    LOGLEVEL_DEBUG = 0,
    LOGLEVEL_INFO = 1,
    LOGLEVEL_WARN = 2,
    LOGLEVEL_ERROR = 3,
    LOGLEVEL_FATAL = 4,
    LOGLEVEL_SIZE,
};

/**
 *  @brief 日志等级的字符串表示
 */
extern const char* log_levels[];

/**
 *  @brief 初始化日志，包括：初始化core，读取日志配置，创建消息队列，创建日志打印线程
 */
void logInit(const char* config_file);

/**
 *  @brief 释放日志，包括：结束日志打印线程，关闭日志输出文件，释放消息队列
 */
void logRelease();

/**
 *  @brief 写日志
 *  @param level: 日志等级
 *  @param func:  当前代码所在函数
 *  @param file:  当前代码所在文件
 *  @param line:  当前代码所在行数
 *  @param fmt:   字符串模板
 *  @param ...:   字符串模板参数列表
 *  @return void
 */
void logWrite(enum LogLevel level, const char* func, const char* file, int line, const char* fmt, ...);

#if defined(_MSC_VER)
#define _FUNC __FUNCTION__ 
#elif defined (__GNUC__) && defined (__GNUC_MINOR__)
#define _FUNC __func__
#else
#define _FUNC ""
#endif

#define _Log(level, fmt, ...) \
    logWrite(level, _FUNC, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

/**
 *  @brief 写debug等级日志
 *  @param fmt:   字符串模板
 *  @param ...:   字符串模板参数列表
 *  @usage: debugLog("Hello, %s, %d.", "World", 20240517);
 */
#define debugLog(fmt, ...) _Log(LOGLEVEL_DEBUG, fmt, ##__VA_ARGS__)

/**
 *  @brief 写info等级日志
 *  @param fmt:   字符串模板
 *  @param ...:   字符串模板参数列表
 *  @usage: infoLog("Hello, %s, %d.", "World", 20240517);
 */
#define infoLog(fmt, ...) _Log(LOGLEVEL_INFO, fmt, ##__VA_ARGS__)

/**
 *  @brief 写warn等级日志
 *  @param fmt:   字符串模板
 *  @param ...:   字符串模板参数列表
 *  @usage: warnLog("Hello, %s, %d.", "World", 20240517);
 */
#define warnLog(fmt, ...) _Log(LOGLEVEL_WARN, fmt, ##__VA_ARGS__)

/**
 *  @brief 写error等级日志
 *  @param fmt:   字符串模板
 *  @param ...:   字符串模板参数列表
 *  @usage: errorLog("Hello, %s, %d.", "World", 20240517);
 */
#define errorLog(fmt, ...) _Log(LOGLEVEL_ERROR, fmt, ##__VA_ARGS__)

/**
 *  @brief 写fatal等级日志
 *  @param fmt:   字符串模板
 *  @param ...:   字符串模板参数列表
 *  @usage: fatalLog("Hello, %s, %d.", "World", 20240517);
 */
#define fatalLog(fmt, ...) _Log(LOGLEVEL_FATAL, fmt, ##__VA_ARGS__)


#endif
