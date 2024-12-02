#ifndef __HEADER_DATETIME_H
#define __HEADER_DATETIME_H

#include <stdint.h>
#include <stdbool.h>

typedef struct DateTime {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
    uint16_t microsecond;
    uint16_t nanosecond;
    bool is_valid;
} DateTime;

typedef struct TimeStamp {
    uint64_t sec;
    uint64_t nsec;
} TimeStamp;

DateTime dateTimeNowTime();

void dateTimeClear(DateTime* ret);

bool dateTimeFromStr(DateTime* ret, const char* str, const char* fmt);

int64_t dateTimeToStr(char* ret, DateTime time, const char* fmt);

int64_t dateTimeSecsTo(DateTime t1, DateTime t2);

TimeStamp timeStampNowTime();

TimeStamp dateTimeToTimeStamp(DateTime dt);

DateTime timeStampToDateTime(TimeStamp ts);

int64_t timeStampSecsTo(TimeStamp t1, TimeStamp t2);

#endif