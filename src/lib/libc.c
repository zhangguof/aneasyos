#include "MyOs.h"
#define DAYTIME (24*60*60)
#define YEARTIME (365*24*60*60)

#define IS_LEAP_YEAR(year) (((year)%4==0 && (year)%100!=0)||((year)%400==0))

static int month_days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
static int month_day_sum[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

int get_leap_year_cnt(int year1,int year2)
{
	assert(IS_LEAP_YEAR(year1));
	int cnt = (year2-year1)/4 - (year2/100 - year1/100)/4;
	return cnt;
}

u32 mktime(DATE* pdate)
{
	//change date to unix time
	//since 1970-01-01 0:0:0 utc-8
	const int first_leap_year = 1972;
	const int first_leap_year100 = 2000;
	u32 leap_year_cnt = 0;
	u32 year_cnt = 0;
	u32 month_cnt = 0;
	u32 day_cnt = 0;
	u32 hour_cnt = 0;
	u32 min_cnt = 0;
	u32 sec_cnt = 0;

	u32 year = pdate->year;
	u32 month = pdate->month;
	u32 day = pdate->day;
	u32 hour = pdate->hour;
	u32 minute = pdate->minute;
	u32 second = pdate->second;

	u32 seconds = 0;

	assert(month>0 && month<13);
	assert(day>0 && day<32);

	if(year > first_leap_year)
	{
		leap_year_cnt = ((year-1 - first_leap_year)/4 + 1);
		if(year > first_leap_year100)
		{
			leap_year_cnt = leap_year_cnt - (year-1 - first_leap_year100)/100 + \
							(year-1 - first_leap_year100)/400;
		}
	}
	year_cnt = (year - 1970) * YEARTIME + leap_year_cnt * DAYTIME;
	month_cnt = month_day_sum[month-1] * DAYTIME;
	if(IS_LEAP_YEAR(year) && month > 2)
	{
		month_cnt += DAYTIME;
	}
	day_cnt = (day-1)*DAYTIME;
	hour_cnt = hour * 60 * 60;
	min_cnt = minute * 60;
	sec_cnt = second;

	seconds = year_cnt + month_cnt + day_cnt + hour_cnt + min_cnt + sec_cnt;
	return seconds;
}