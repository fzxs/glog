
#ifndef LHN_BASE_COUNTDOWNLATCH_H
#define LHN_BASE_COUNTDOWNLATCH_H

#include "Lock.h"

/*
设计方案:
	条件锁使用时，要求wait()方法必须在signal()方法之前调用，CountDownLatch通过内置计数器的方式确保了这种调用
*/

namespace gtc
{
	class CountDownLatch
	{
	public:
		//构造函数
		explicit CountDownLatch(int count);

		//wait
		void wait();

		//发送信号
		void countDown();

		//获取倒计时
		int getCount() const;

	private:
		mutable Mutex_Lock mutex_;
		Cond_Lock condition_ GUARDED_BY(mutex_);
		int count_ GUARDED_BY(mutex_);

	};

}  // namespace gtc
#endif  // LHN_BASE_COUNTDOWNLATCH_H
