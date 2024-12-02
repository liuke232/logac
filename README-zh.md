# logac
一个日志库，使用专门的线程来输出日志，避免多线程输出冲突。

### 简单使用

确保您的程序链接了 liblogac.so 动态库：

```cmake
target_link_libraries(your_target_name PRIVATE logac)
```

并且可以找到 logac.h 头文件。

您的程序可以像这样使用logac库：

```c
#include "logac.h"

int main()
{
    // 初始化，传入配置文件路径
    logInit("./log_config.ini");

    debugLog("This is a debug log.");
    infoLog("This is a info log, %d.", 123);
    warnLog("This is a warn log, %f.", 123.0f);
    errorLog("This is an error log, %s.", "Hello, World!");
    fatalLog("This is a fatal log.");

    // 释放日志
    logRelease();
    return 0;
}
```

### 日志配置
日志文件使用ini格式，初始化时传递文件名给logInit函数。
配置文件支持以下参数：
- logfilepatten: 日志文件名模板，默认为{host}\_{program}\_{date}\_{time}\_{num}.log
- maxsize: 日志文件最大大小，默认为500MB，支持单位为B、K/KB、M/MB、G/GB，如1024KB
- interval: 日志轮转时间间隔，默认为24小时，支持单位为s、m、h、d，如30m表示30分钟
- loglevel: 日志级别，默认为INFO，可选择DEBUG、INFO、WARN、ERROR、FATAL

示例（logconfig.ini）：
```ini
logfilepatten={host}_{program}_{date}_{time}_{num}.log
maxsize=1000KB
interval=30m
loglevel=DEBUG
```
