
#ifndef __GLOG_PROFILE_H_
#define __GLOG_PROFILE_H_

#include "macros.h"

#include <stdint.h>
#include <string>

namespace glog
{
	class CurrentTime
	{
	public:
		//获取日期
		static std::string nowDate(void);

		//获取时间
		static std::string nowTime(void);

		//获取时间
		static void now(std::string &date, std::string &tv, uint32_t &usec);
	};

	namespace LevelCode
	{
		inline std::string level(uint32_t code)
		{
			std::string strCode;
			switch (code)
			{
			case 20:
				strCode = "NOTICE";
				break;
			case 40:
				strCode = "DEBUG";
				break;
			case 60:
				strCode = "INFO";
				break;
			case 80:
				strCode = "WARN";
				break;
			case 100:
				strCode = "ERROR";
				break;
			case 120:
				strCode = "FATAL";
				break;
			default:
				break;
			}

			return strCode;
		}
	}

	namespace CurrentThread
	{
		extern __thread int t_cachedTid;
		extern __thread int t_cachedPid;

		//缓存tid
		void cacheTid();

		//缓存pid
		void cachePid();

		//获取tid
		inline int tid()
		{
			if (UNLIKELY(0 == t_cachedTid))
			{
				cacheTid();
			}
			return t_cachedTid;
		}

		//获取tid
		inline int pid()
		{
			if (UNLIKELY(0 == t_cachedPid))
			{
				cachePid();
			}
			return t_cachedPid;
		}
	}
}


#endif


