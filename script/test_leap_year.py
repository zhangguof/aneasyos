#-*- coding:utf-8 -*-

def cnt_leap_year(year):
	cnt = 1;
	for y in xrange(1973,year):
		if ((y%4==0 and y%100!=0) or y%400==0):
			cnt+=1
	return cnt

def cnt_leap_year2(year):
	return ((year-1 - 1972)/4 + 1) - (year-1 - 2000)/100 + (year-1 - 2000)/400

print cnt_leap_year(2100)
print cnt_leap_year2(2100)

mons = [31,28,31,30,31,30,31,31,30,31,30,31]
mon_sum = [0] * 12
for i in xrange(1,len(mons)):
	mon_sum[i] = mon_sum[i-1] + mons[i-1]
print mon_sum