#ifndef LAB_RTC_H
#define LAB_RTC_H

#include "stm32f10x_rtc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"
#include "misc.h"
#include "stm32f10x.h"

#define RTC_BKP_DRX             BKP_DR1
#define RTC_BKP_DATA            0xA5A5
#define TIME_ZOOM               (8*60*60)


#define LAB_RTC                 0

struct rtc_time
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
};

void RTC_Init(void);
void GregorianDay(struct rtc_time * tm);
uint32_t mktimev(struct rtc_time *tm);
void to_tm(uint32_t tim, struct rtc_time * tm);


#endif