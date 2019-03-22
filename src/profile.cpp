
#include "profile.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#endif

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <pthread.h>
#include <sys/time.h>

using namespace glog;

/********************************************************
CurrentTime
*********************************************************/

/********************************************************
   Func Name: nowDate
Date Created: 2019-3-19
 Description: 获取当前日期
	   Input: 
	  Output: 
	  Return: 日期字符串
	 Caution: 
*********************************************************/
std::string CurrentTime::nowDate(void)
{
	time_t tt;
	struct tm local_time;
	char date[32] = { 0 };

	time(&tt);
	localtime_r(&tt, &local_time);
	strftime(date, 32, "%Y-%m-%d", &local_time);

	return date;
}

/********************************************************
   Func Name: nowDate
Date Created: 2019-3-19
 Description: 获取当前日期
	   Input: 
	  Output: 
	  Return: 日期字符串
	 Caution: 
*********************************************************/
std::string CurrentTime::nowTime(void)
{
	time_t tt;
	struct tm local_time;
	char tv[32] = { 0 };

	time(&tt);
	localtime_r(&tt, &local_time);
	strftime(tv, 32, "%I:%M:%S", &local_time);

	return tv;
}

/********************************************************
   Func Name: now
Date Created: 2019-3-19
 Description: 获取当前时间
	   Input: 
	  Output: 
	  Return: 日期字符串
	 Caution: 
*********************************************************/
void CurrentTime::now(std::string &date, std::string &tv, uint32_t &usec)
{
	struct timeval td;
	struct tm local_time;
	char gDate[32] = { 0 };
	char gTime[32] = { 0 };

	::gettimeofday(&td, NULL);

	::localtime_r(&td.tv_sec, &local_time);

	strftime(gDate, 32, "%Y-%m-%d", &local_time);
	strftime(gTime, 32, "%I:%M:%S", &local_time);

	date = gDate;
	tv = gTime;
	usec = td.tv_usec;

}


__thread int CurrentThread::t_cachedTid = 0;
__thread int CurrentThread::t_cachedPid = 0;

/********************************************************
   Func Name: cacheTid
Date Created: 2019-1-14
 Description: 缓存当前线程的tid
	   Input:
	  Output:
	  Return:
	 Caution: 
*********************************************************/
void CurrentThread::cacheTid()
{
	if (0 == t_cachedTid)
	{
		t_cachedTid =  static_cast<pid_t>(::syscall(SYS_gettid));
	}
}

/********************************************************
   Func Name: cachePid
Date Created: 2019-1-14
 Description: 缓存当前线程的pid
	   Input:
	  Output:
	  Return:
	 Caution: 
*********************************************************/
void CurrentThread::cachePid()
{
	if (0 == t_cachedTid)
	{
		t_cachedPid =  static_cast<pthread_t>(::pthread_self());
	}
}


