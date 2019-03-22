
#include "Lock.h"
#include <time.h>

using namespace gtc;

/********************************************************
   Func Name: Mutex_Lock
Date Created: 2018-7-5
 Description: 构造函数
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
Mutex_Lock::Mutex_Lock()
{
	//暂时不需要设计mutexattr，以后有需要会让外部传参设置mutexattr
	pthread_mutexattr_t *attr = NULL;
	//初始化锁
	::pthread_mutex_init(&this->_mtx, attr);
}

/********************************************************
   Func Name: ~Mutex_Lock
Date Created: 2018-7-5
 Description: 析构函数
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
Mutex_Lock::~Mutex_Lock()
{
	::pthread_mutex_destroy(&this->_mtx);
}

/********************************************************
   Func Name: lock
Date Created: 2018-7-5
 Description: 加锁
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
void Mutex_Lock::lock()
{
	::pthread_mutex_lock(&this->_mtx);
}

/********************************************************
   Func Name: unlock
Date Created: 2018-7-5
 Description: 解锁
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
void Mutex_Lock::unlock()
{
	::pthread_mutex_unlock(&this->_mtx);
}

/********************************************************
Cond_Lock
*********************************************************/

/********************************************************
   Func Name: Cond_Lock
Date Created: 2018-7-5
 Description: 构造函数
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
Cond_Lock::Cond_Lock(Mutex_Lock &mutex):_mutex(mutex)
{
	pthread_condattr_t *attr = NULL;
	::pthread_cond_init(&this->_cond, attr);
}

/********************************************************
   Func Name: ~Cond_Lock
Date Created: 2018-7-5
 Description: 析构函数
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
Cond_Lock::~Cond_Lock()
{
	::pthread_cond_destroy(&this->_cond);
}

/********************************************************
   Func Name: wait
Date Created: 2018-7-5
 Description: 条件等待
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
int Cond_Lock::wait()
{
	int result = 0;

	result = ::pthread_cond_wait(&this->_cond, &(this->_mutex._mtx));

	return result;
}

/********************************************************
   Func Name: timedwait
Date Created: 2018-7-5
 Description: 超时条件等待
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
int Cond_Lock::timedwait(long sec, long nsec)
{
	struct timespec ts;
	int result = 0;

	clock_gettime(CLOCK_REALTIME, &ts);
	/*
		pthread_cond_timedwait的第三个参数获取的是绝对时间，并非相对时间
	*/
	ts.tv_sec += sec;
	ts.tv_nsec += nsec;
	result = ::pthread_cond_timedwait(&this->_cond, &(this->_mutex._mtx), &ts);

	return result;

}

/********************************************************
   Func Name: signal
Date Created: 2018-7-5
 Description: 发送信号
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
int Cond_Lock::signal()
{
	int result = 0;

	//防止信号丢失
	result = ::pthread_cond_signal(&this->_cond);

	return result;
}

/********************************************************
   Func Name: broadcast
Date Created: 2018-9-21
 Description: 广播信号
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
int Cond_Lock::broadcast()
{
	return ::pthread_cond_broadcast(&this->_cond);
}

/********************************************************
RW_Lock
*********************************************************/

/********************************************************
   Func Name: RW_Lock
Date Created: 2018-7-5
 Description: 构造函数
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
RW_Lock::RW_Lock()
{
	pthread_rwlockattr_t *attr = NULL;
	::pthread_rwlock_init(&this->_rwlock, attr);
}

/********************************************************
   Func Name: ~RW_Lock
Date Created: 2018-7-5
 Description: 析构函数
       Input: 
      Output: 
      Return: 
     Caution: 
*********************************************************/
RW_Lock::~RW_Lock()
{
	::pthread_rwlock_destroy(&this->_rwlock);
}

/********************************************************
   Func Name: readLock
Date Created: 2018-7-5
 Description: 获取读锁
       Input: 
      Output: 
      Return: 
     Caution:     如果其它一个线程占有写锁，则当前线程必须
	          等待写锁被释放，才能对保护资源进行访问 
*********************************************************/
void RW_Lock::readLock()
{
	::pthread_rwlock_rdlock(&this->_rwlock);
}

/********************************************************
   Func Name: writeLock
Date Created: 2018-7-5
 Description: 获取写锁
       Input: 
      Output: 
      Return: 
     Caution:  
*********************************************************/
void RW_Lock::writeLock()
{
	::pthread_rwlock_wrlock(&this->_rwlock);
}

/********************************************************
   Func Name: unLock
Date Created: 2018-7-5
 Description: 释放写锁
       Input: 
      Output: 
      Return: 
     Caution:  
*********************************************************/
void RW_Lock::unLock()
{
	::pthread_rwlock_unlock(&this->_rwlock);
}






































