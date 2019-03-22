
#include "latch.h"

using namespace gtc;

/********************************************************
   Func Name: CountDownLatch
Date Created: 2019-1-14
 Description: 构造函数
	   Input:
	  Output:
	  Return: 
	 Caution: 
*********************************************************/
CountDownLatch::CountDownLatch(int count):condition_(mutex_)
{
	this->count_ = count;
}

/********************************************************
   Func Name: wait
Date Created: 2019-1-14
 Description: wait
	   Input:
	  Output:
	  Return: 
	 Caution: 
*********************************************************/
void CountDownLatch::wait()
{
	this->mutex_.lock();

	while (this->count_ > 0)
	{
		this->condition_.wait();
	}

	this->mutex_.unlock();
}

/********************************************************
   Func Name: countDown
Date Created: 2019-1-14
 Description: 发送信号
	   Input:
	  Output:
	  Return: 
	 Caution: 倒计时数减一
*********************************************************/
void CountDownLatch::countDown()
{
	this->mutex_.lock();

	--this->count_;
	if (0 == this->count_)
	{
		this->condition_.signal();
	}

	this->mutex_.unlock();
}

/********************************************************
   Func Name: getCount
Date Created: 2019-1-14
 Description: 获取倒计时
	   Input:
	  Output:
	  Return: 
	 Caution: 倒计时数减一
*********************************************************/
int CountDownLatch::getCount() const
{
	Guard_Mutex_Lock lock(this->mutex_);
	return count_;
}

