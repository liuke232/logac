#include "datetime.h"
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

DateTime dateTimeNowTime()
{
    struct timespec tms;
    DateTime ret;
    if (0 != clock_gettime(CLOCK_REALTIME, &tms)) {
        ret.is_valid = false;
        return ret;
    }
    struct tm ltm;
    localtime_r(&tms.tv_sec, &ltm);
    ret.is_valid = true;
    ret.year = (uint16_t)ltm.tm_year + 1900;
    ret.month = (uint8_t)ltm.tm_mon + 1;
    ret.day = (uint8_t)ltm.tm_mday;
    ret.hour = (uint8_t)ltm.tm_hour;
    ret.minute = (uint8_t)ltm.tm_min;
    ret.second = (uint8_t)ltm.tm_sec;
    ret.millisecond = (uint16_t)(tms.tv_nsec / 1000000);
    ret.microsecond = (uint16_t)((tms.tv_nsec % 1000000) / 1000);
    ret.nanosecond = (uint16_t)(tms.tv_nsec % 1000);
    return ret;
}

void dateTimeClear(DateTime* ret)
{
    memset(ret, 0, sizeof(DateTime));
}

bool dateTimeFromStr(DateTime *ret, const char *str, const char *fmt)
{
    dateTimeClear(ret);
    const char* p = str;
    const char* q = fmt;
    int flag = 0;
    int cnt = 0;
    while (*p && *q) {
        switch (*q) {
        case 'y':case 'Y':
            cnt = (flag == 1) ? cnt + 1 : 1;
            flag = 1;
            if (cnt == 4 && isdigit(p[0]) && isdigit(p[-1]) && isdigit(p[-2]) && isdigit(p[-3])) {
                ret->year = (p[0] - '0') + (p[-1] - '0') * 10 + (p[-2] - '0') * 100 + (p[-3] - '0') * 1000;
                ret->is_valid = true;
            }
            break;
        case 'M':
            cnt = (flag == 2) ? cnt + 1 : 1;
            flag = 2;
            if (cnt == 2 && isdigit(p[0]) && isdigit(p[-1])) {
                ret->month = (p[0] - '0') + (p[-1] - '0') * 10;
                ret->is_valid = true;
            }
            break;
        case 'd':case 'D':
            cnt = (flag == 3) ? cnt + 1 : 1;
            flag = 3;
            if (cnt == 2 && isdigit(p[0]) && isdigit(p[-1])) {
                ret->day = (p[0] - '0') + (p[-1] - '0') * 10;
                ret->is_valid = true;
            }
            break;
        case 'H':case 'h':
            cnt = (flag == 4) ? cnt + 1 : 1;
            flag = 4;
            if (cnt == 2 && isdigit(p[0]) && isdigit(p[-1])) {
                ret->hour = (p[0] - '0') + (p[-1] - '0') * 10;
                ret->is_valid = true;
            }
            break;
        case 'm':
            cnt = (flag == 5) ? cnt + 1 : 1;
            flag = 5;
            if (cnt == 2 && isdigit(p[0]) && isdigit(p[-1])) {
                ret->minute = (p[0] - '0') + (p[-1] - '0') * 10;
                ret->is_valid = true;
            }
            break;
        case 's':case 'S':
            cnt = (flag == 6) ? cnt + 1 : 1;
            flag = 6;
            if (cnt == 2 && isdigit(p[0]) && isdigit(p[-1])) {
                ret->second = (p[0] - '0') + (p[-1] - '0') * 10;
                ret->is_valid = true;
            }
            break;
        case 'z':case 'Z':
            cnt = (flag == 7) ? cnt + 1 : 1;
            flag = 7;
            if (cnt == 3 && isdigit(p[0]) && isdigit(p[-1]) && isdigit(p[-2])) {
                ret->millisecond = (p[0] - '0') + (p[-1] - '0') * 10 + (p[-2] - '0') * 100;
                ret->is_valid = true;
            }
            break;
        case 'v':case 'V':
            cnt = (flag == 8) ? cnt + 1 : 1;
            flag = 8;
            if (cnt == 3 && isdigit(p[0]) && isdigit(p[-1]) && isdigit(p[-2])) {
                ret->microsecond = (p[0] - '0') + (p[-1] - '0') * 10 + (p[-2] - '0') * 100;
                ret->is_valid = true;
            }
            break;
        case 'n':case 'N':
            cnt = (flag == 9) ? cnt + 1 : 1;
            flag = 9;
            if (cnt == 3 && isdigit(p[0]) && isdigit(p[-1]) && isdigit(p[-2])) {
                ret->nanosecond = (p[0] - '0') + (p[-1] - '0') * 10 + (p[-2] - '0') * 100;
                ret->is_valid = true;
            }
            break;
        default:
            cnt = 0;
            flag = 0;
            break;
        }
        ++p;
        ++q;
    }
    return ret->is_valid;
}

/**
 *  @brief 将DateTime转化为字符串
 *  @param ret: 返回的字符串
 *  @param time: 输入时间
 *  @param fmt: 字符串格式，通常是"yyyy-MM-dd HH:mm:ss"
 *  @return 返回的时间字符串长度，一般与fmt长度相等，返回-1表示转化失败
 */
int64_t dateTimeToStr(char* ret, DateTime time, const char* fmt)
{
    if (!time.is_valid) {
        return -1;
    }
    strcpy(ret, fmt);
    char* p = ret;
    const char* q = fmt;
    int flag = 0, cnt = 0;
    while (*q) {
        switch (*q) {
        case 'y':case 'Y':
            cnt = (flag == 1) ? cnt + 1 : 1;
            flag = 1;
            if (cnt == 4) {
                p[-3] = time.year / 1000 + '0';
                p[-2] = (time.year % 1000) / 100 + '0';
                p[-1] = (time.year % 100) / 10 + '0';
                p[0] = time.year % 10 + '0';
            }
            break;
        case 'M':
            cnt = (flag == 2) ? cnt + 1 : 1;
            flag = 2;
            if (cnt == 2) {
                p[-1] = time.month / 10 + '0';
                p[0] = time.month % 10 + '0';
            }
            break;
        case 'd':case 'D':
            cnt = (flag == 3) ? cnt + 1 : 1;
            flag = 3;
            if (cnt == 2) {
                p[-1] = time.day / 10 + '0';
                p[0] = time.day % 10 + '0';
            }
            break;
        case 'H':case 'h':
            cnt = (flag == 4) ? cnt + 1 : 1;
            flag = 4;
            if (cnt == 2) {
                p[-1] = time.hour / 10 + '0';
                p[0] = time.hour % 10 + '0';
            }
            break;
        case 'm':
            cnt = (flag == 5) ? cnt + 1 : 1;
            flag = 5;
            if (cnt == 2) {
                p[-1] = time.minute / 10 + '0';
                p[0] = time.minute % 10 + '0';
            }
            break;
        case 's':case 'S':
            cnt = (flag == 6) ? cnt + 1 : 1;
            flag = 6;
            if (cnt == 2) {
                p[-1] = time.second / 10 + '0';
                p[0] = time.second % 10 + '0';
            }
            break;
        case 'z':case 'Z':
            cnt = (flag == 7) ? cnt + 1 : 1;
            flag = 7;
            if (cnt == 3) {
                p[-2] = time.millisecond / 100 + '0';
                p[-1] = (time.millisecond % 100) / 10 + '0';
                p[0] = time.millisecond % 10 + '0';
            }
            break;
        case 'v':case 'V':
            cnt = (flag == 8) ? cnt + 1 : 1;
            flag = 8;
            if (cnt == 3) {
                p[-2] = time.microsecond / 100 + '0';
                p[-1] = (time.microsecond % 100) / 10 + '0';
                p[0] = time.microsecond % 10 + '0';
            }
            break;
        case 'n':case 'N':
            cnt = (flag == 9) ? cnt + 1 : 1;
            flag = 9;
            if (cnt == 3) {
                p[-2] = time.nanosecond / 100 + '0';
                p[-1] = (time.nanosecond % 100) / 10 + '0';
                p[0] = time.nanosecond % 10 + '0';
            }
            break;
        default:
            cnt = 0;
            flag = 0;
            break;
        }
        ++p;
        ++q;
    }
    return p - ret;
}

int64_t dateTimeSecsTo(DateTime t1, DateTime t2)
{
    TimeStamp ts1 = dateTimeToTimeStamp(t1);
    TimeStamp ts2 = dateTimeToTimeStamp(t2);
    return timeStampSecsTo(ts1, ts2);
}

TimeStamp timeStampNowTime()
{
    struct timespec tms;
    TimeStamp ret = {0, 0};
    if (0 != clock_gettime(CLOCK_REALTIME, &tms)) {
        return ret;
    }
    ret.sec = tms.tv_sec;
    ret.nsec = tms.tv_nsec;
    return ret;
}

TimeStamp dateTimeToTimeStamp(DateTime dt)
{
    TimeStamp ret = {0, 0};
    if (!dt.is_valid) {
        return ret;
    }

    struct tm t;
    
    // 设置年、月、日、时、分、秒
    t.tm_year = dt.year - 1900;  // 年份需要减去1900
    t.tm_mon = dt.month - 1;         // 月份从0开始，所以9月是8
    t.tm_mday = dt.day;
    t.tm_hour = dt.hour;
    t.tm_min = dt.minute;
    t.tm_sec = dt.second;

    // 设置其他字段为0或默认值
    t.tm_isdst = -1;  // Daylight Saving Time flag, -1 means let mktime figure it out
    t.tm_wday = 0;
    t.tm_yday = 0;

    time_t time = mktime(&t);
    if (time == (time_t)-1) {
        return ret;
    }
    
    ret.sec = (uint64_t)time;
    ret.nsec = dt.millisecond * 1000000UL + dt.microsecond * 1000UL + dt.nanosecond;
    return ret;
}

DateTime timeStampToDateTime(TimeStamp ts)
{
    DateTime ret;
    struct timespec tms = {ts.sec, ts.nsec};
    struct tm ltm;
    if (NULL == localtime_r(&tms.tv_sec, &ltm)) {
        ret.is_valid = false;
        return ret;
    }
    ret.is_valid = true;
    ret.year = (uint16_t)ltm.tm_year + 1900;
    ret.month = (uint8_t)ltm.tm_mon + 1;
    ret.day = (uint8_t)ltm.tm_mday;
    ret.hour = (uint8_t)ltm.tm_hour;
    ret.minute = (uint8_t)ltm.tm_min;
    ret.second = (uint8_t)ltm.tm_sec;
    ret.millisecond = (uint16_t)(tms.tv_nsec / 1000000);
    ret.microsecond = (uint16_t)((tms.tv_nsec % 1000000) / 1000);
    ret.nanosecond = (uint16_t)(tms.tv_nsec % 1000);
    return ret;
}

int64_t timeStampSecsTo(TimeStamp t1, TimeStamp t2)
{
    return t2.sec - t1.sec;
}
