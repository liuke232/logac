#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "logac.h"
#include "datetime.h"

int main()
{
    logInit("../log_config.ini");
    warnLog("Hello, %s", "World");
    int N = 0;

    while (1)
    {
        debugLog("Hello, %s, %d", "World", N++);
        infoLog("Hello, %s, %d", "World", N++);
        warnLog("Hello, %s, %d", "World", N++);
        errorLog("Hello, %s, %d", "World", N++);
        fatalLog("Hello, %s, %d", "World", N++);
        usleep(500000);
    }

    logRelease();

    return 0;
}

