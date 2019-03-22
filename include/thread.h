
#ifndef LHN_BASE_THREAD_H
#define LHN_BASE_THREAD_H

#include "macros.h"

#include "latch.h"

#include <functional>
#include <memory>
#include <pthread.h>
#include <string>

/*
知识储备

tid:
	在linux下每一个进程都一个进程id，类型pid_t，可以由getpid（）获取。
POSIX线程也有线程id，类型pthread_t，可以由pthread_self（）获取，线程id由线程库维护。
但是各个进程独立，所以会有不同进程中线程号相同节的情况。
那么这样就会存在一个问题，我的进程p1中的线程pt1要与进程p2中的线程pt2通信怎么办，进程id不可以，
线程id又可能重复，所以这里会有一个真实的线程id唯一标识 tid。
glibc没有实现gettid的函数，所以我们可以通过linux下的系统调用syscall(SYS_gettid)来获得。
*/

namespace gtc
{
	class CountDownLatch;

	class Thread
	{
	public:
		typedef std::function<void()> ThreadFunc;

		//构造函数
		explicit Thread(ThreadFunc, const std::string& name = std::string());

		//析构函数--线程默认分离
		~Thread();

		//启动线程
		void start();

		//设置线程join
		int join();

		//线程是否启动
		bool started() const { return started_; }

		//返回线程名字
		const std::string& name() const { return name_; }

	private:
		//设置默认线程名
		void setDefaultName();

	private:
		bool       started_;
		bool       joined_;
		pthread_t  pthreadId_;
		ThreadFunc func_;
		std::string name_;
		CountDownLatch latch_;
	};

}  // namespace gtc
#endif  // LHN_BASE_THREAD_H
