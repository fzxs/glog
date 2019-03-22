
#include "thread.h"

#include <type_traits>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

/*
设计方案:
	为什么需要设计ThreadData对象？
		目的一：为了子线程函数startThread()的安全，保证startThread()方法里所有的数据都是有效的(如果传递Thread实例，可能存在Thread实例被释放了，但是子线程仍然在运行的情况，导致程序崩溃)
		目的二:为了实现各种各样的子线程函数，因为子线程的函数签名是 "void * (*func)(void *)" ,这个签名太广泛了，不够定制化
*/

namespace gtc
{
	namespace detail
	{
		/* 线程数据 */
		struct ThreadData
		{
			typedef gtc::Thread::ThreadFunc ThreadFunc;             //线程函数类型

			ThreadFunc func_;              //线程执行函数(通过std::bind方法目前支持各种不同函数签名)
			std::string name_;             //线程名字
			CountDownLatch *latch_;       //倒计时锁
		public:
			/********************************************************
			   Func Name: ThreadData
			Date Created: 2019-1-14
			 Description: 构造函数
				   Input:
				  Output:
				  Return: tid
				 Caution: 系统级调用
			*********************************************************/
			ThreadData(ThreadFunc func
				, const std::string& name
				, CountDownLatch *latch) : func_(std::move(func))
				, name_(name)
				, latch_(latch)
			{
			}

			/********************************************************
			   Func Name: runInThread
			Date Created: 2019-1-14
			 Description: 执行用户方法
				   Input:
				  Output:
				  Return: 
				 Caution: 
			*********************************************************/
			void runInThread()
			{
				//发送信号，线程已经启动
				this->latch_->countDown();
				this->latch_ = NULL;

				this->func_();
			}
		};

		/********************************************************
		   Func Name: startThread
		Date Created: 2019-1-14
		 Description: 线程启动函数
			   Input:
			  Output:
			  Return:
			 Caution:
		*********************************************************/
		void* startThread(void* obj)
		{
			ThreadData* data = static_cast<ThreadData*>(obj);
			data->runInThread();
			delete data;
			return NULL;
		}

	}  // namespace detail



	/********************************************************
	   Func Name: Thread
	Date Created: 2019-1-14
	 Description: 构造函数
		   Input:
		  Output:
		  Return: 
		 Caution: 
	*********************************************************/
	Thread::Thread(ThreadFunc func
		, const std::string& name)
		: started_(false)
		, joined_(false)
		, pthreadId_(0)
		, func_(std::move(func))
		, name_(name)
		, latch_(1)
	{
		setDefaultName();
	}

	/********************************************************
	   Func Name: ~Thread
	Date Created: 2019-1-14
	 Description: 析构函数
		   Input:
		  Output:
		  Return: 
		 Caution: 
	*********************************************************/
	Thread::~Thread()
	{
		if (started_ && !joined_)
		{
			::pthread_detach(pthreadId_);
		}
	}

	/********************************************************
	   Func Name: setDefaultName
	Date Created: 2019-1-14
	 Description: 设置默认线程名
		   Input:
		  Output:
		  Return: 
		 Caution: 
	*********************************************************/
	void Thread::setDefaultName()
	{
		name_ = "test";
	}

	/********************************************************
	   Func Name: start
	Date Created: 2019-1-14
	 Description: 启动线程
		   Input:
		  Output:
		  Return: 
		 Caution: 
	*********************************************************/
	void Thread::start()
	{
		assert(!started_);
		started_ = true;

		detail::ThreadData* data = new detail::ThreadData(this->func_, this->name_, &this->latch_);
		if (::pthread_create(&pthreadId_, NULL, &detail::startThread, data))
		{
			//线程创建失败
			started_ = false;
			delete data;
		}
		else
		{
			/*
			注意点:
				latch_.wait()返回只能表明子线程已经创建成功，并无法说明detail::startThread方法已经执行完毕，这是两个概念
			*/
			//等待线程成功创建再返回
			latch_.wait();
		}
	}

	/********************************************************
	   Func Name: join
	Date Created: 2019-1-14
	 Description: 设置线程join
		   Input:
		  Output:
		  Return: 
		 Caution: 
	*********************************************************/
	int Thread::join()
	{
		assert(this->started_);
		assert(!this->joined_);

		joined_ = true;
		return ::pthread_join(this->pthreadId_, NULL);
	}

}  // namespace gtc
