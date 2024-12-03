# logac

A logging library for C that uses a dedicated thread to output logs, avoiding conflicts from multi-threaded outputs.

## Simple Usage

Ensure your program links against the liblogac.so dynamic library:

```cmake
target_link_libraries(your_target_name PRIVATE logac)
```
and can find the logac.h header file.

If not, you can build it from source:

```bash
cd logac
mkdir build && cd build
cmake ..
cmake --build .
```

The logac.h header file can be found in the includes directory.

Your program can use the logac library like this:

```c
#include "logac.h"

int main()
{
    // Initialize with the path to the configuration file
    logInit("./log_config.ini");

    debugLog("This is a debug log.");
    infoLog("This is an info log, %d.", 123);
    warnLog("This is a warn log, %f.", 123.0f);
    errorLog("This is an error log, %s.", "Hello, World!");
    fatalLog("This is a fatal log.");

    // Release the logger
    logRelease();
    return 0;
}
```

### Log Configuration
The log configuration file uses the INI format and is passed to the logInit function during initialization. 
The configuration file supports the following parameters:
- logfilepatten: Log file name template. Default is {host}\_{program}\_{date}\_{time}\_{num}.log.
- maxsize: Maximum size of a log file. Default is 500MB. Supported units are B, K/KB, M/MB, G/GB (e.g., 1024KB).
- interval: Log rotation time interval. Default is 24 hours. Supported units are s (seconds), m (minutes), h (hours), d (days) (e.g., 30m for 30 minutes).
- loglevel: Log level. Default is INFO. Available options are DEBUG, INFO, WARN, ERROR, FATAL.

Example Configuration File (log_config.ini):
```ini
logfilepatten={host}_{program}_{date}_{time}_{num}.log
maxsize=1000KB
interval=30m
loglevel=DEBUG
```
