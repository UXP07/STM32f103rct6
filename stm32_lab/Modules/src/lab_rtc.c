#include "lab_rtc.h"

#if LAB_RTC

#define SECDAY                  (24*60*60)
#define STARTOFTIME             1970
#define FEBRUARY                2
#define leapyear(year)          (((year) % 4 == 0 && (year) % 100 != 0) || (year) % 400 == 0)
#define days_in_year(year)         (leapyear(year) ? 366 : 365)

static int month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#define days_in_month(m)        (month_days[(m)-1])

void RTC_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP|RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    BKP_DeInit();

    RCC_LSEConfig(RCC_LSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);

    RTC_WaitForSynchro();
    RTC_WaitForLastTask();

    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    RTC_WaitForLastTask();

    RTC_SetPrescaler(32767);
    RTC_WaitForLastTask();

}

void GregorianDay(struct rtc_time * tm)
{
	int year = tm->tm_year;
    int month = tm->tm_mon;
    int day = tm->tm_mday;
    
    // 算法规定：1月2月要看作上一年的13、14月
    if (month < 3) {
        month += 12;
        year--;
    }
    
    // 格里高利历计算公式 (蔡勒公式变种)
    // W = (d + 2*m + 3*(m+1)/5 + y + y/4 - y/100 + y/400 + 1) % 7
    tm->tm_wday = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400 + 1) % 7;

}

uint32_t mktimev(struct rtc_time *tm)
{
    if (0 >= (int) (tm->tm_mon -= 2)) { /* 1..12 -> 11,12,1..10 */
        tm->tm_mon += 12;   /* Puts Feb last since it has leap day */
        tm->tm_year -= 1;
    }

    return (((
            (uint32_t) (tm->tm_year/4 - tm->tm_year/100 + tm->tm_year/400 +
            367*tm->tm_mon/12 + tm->tm_mday) + tm->tm_year*365 - 719499
            )*24 + tm->tm_hour /* now have hours */
            )*60 + tm->tm_min /* now have minutes */
        )*60 + tm->tm_sec; /* finally seconds */
}



void to_tm(uint32_t tim, struct rtc_time * tm)
{
    register uint32_t    i;
    register long   hms, day;

    day = tim / SECDAY;     /* 有多少天 */
    hms = tim % SECDAY;     /* 今天的时间，单位s */

    /* Hours, minutes, seconds are easy */
    tm->tm_hour = hms / 3600;
    tm->tm_min = (hms % 3600) / 60;
    tm->tm_sec = (hms % 3600) % 60;

    /* Number of years in days */ /*算出当前年份，起始的计数年份为1970年*/
    for (i = STARTOFTIME; day >= days_in_year(i); i++) {
        day -= days_in_year(i);
    }
    tm->tm_year = i;

    /* Number of months in days left */ /*计算当前的月份*/
    if (leapyear(tm->tm_year)) {
        days_in_month(FEBRUARY) = 29;
    }
    for (i = 1; day >= days_in_month(i); i++) {
        day -= days_in_month(i);
    }
    days_in_month(FEBRUARY) = 28;
    tm->tm_mon = i;

    /* Days are what is left over (+1) from all that. *//*计算当前日期*/
    tm->tm_mday = day + 1;

    /*
    * Determine the day of week
    */
    GregorianDay(tm);
}

#endif