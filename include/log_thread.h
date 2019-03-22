
#ifndef LHN_NET_EVENTLOOPTHREAD_H
#define LHN_NET_EVENTLOOPTHREAD_H

#include "Lock.h"
#include "rotater.h"
#include "thread.h"

#include <string>
#include <stdint.h>
#include <stdio.h>

namespace glog
{
	class LogThread
	{
	public:
		//构造函数
		LogThread();

		//析构函数
		~LogThread();

	public:
		//启动线程
		int start(const std::string &pattern, uint32_t rollCount, uint32_t maxFileSize);

	private:
		//子线程执行函数
		void threadFunc();

		//重新生成日志文件
		void createFile();

		//将文件刷新到磁盘上
		void flush(const std::string &strData);

	private:
		//FILE * _fpLog;    //日志文件描述符
		int _fdLog;
		std::string _activeFileName;    //当前日志文件名
		uint32_t _maxFileSize;   //滚动文件大小
		std::string _cacheTime;  //前一次日志日期
		bool _exiting;     //是否退出当前线程
		gtc::Thread _thread;
		Rotater _rotater;    //滚动器
	};
}

#endif  // LHN_NET_EVENTLOOPTHREAD_H
