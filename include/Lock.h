
#ifndef __GTC_LOCK_H_
#define __GTC_LOCK_H_

#include<pthread.h>

/* 锁模块 */

/*
	Thread Safety Annotations 可以方便阅读代码，而且在编译的时候会检查锁的状态。
*/

#if defined(__SUPPORT_TS_ANNOTATION__) || defined(__clang__)
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   // no-op
#endif

/*
	必须定义了__SUPPORT_TS_ANNOTATION__或者__clang__ Thread Safety Annotations 才起作用，否则是no-op
*/

#define CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define SCOPED_CAPABILITY \
  THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define PT_GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define ACQUIRED_BEFORE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define ACQUIRED_AFTER(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define REQUIRES(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define REQUIRES_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))

#define ACQUIRE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))

#define RELEASE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define RELEASE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define TRY_ACQUIRE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define TRY_ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))

#define EXCLUDES(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define ASSERT_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define ASSERT_SHARED_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define RETURN_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define NO_THREAD_SAFETY_ANALYSIS \
  THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)



namespace gtc 
{
	//互斥锁
	class Cond_Lock;
	class Mutex_Lock
	{
	public:
		friend class Cond_Lock;
	public:
		Mutex_Lock();
		~Mutex_Lock();

	public:
		void lock();
		void unlock();

	protected:
		pthread_mutex_t _mtx;

	};

	//条件锁
	/*
		注意点:
			使用条件锁时，需要由用户自己确保wait()方法会在signal()之前调用
	*/
	class Cond_Lock
	{
	public:
		Cond_Lock(Mutex_Lock &mutex);
		virtual ~Cond_Lock();

	public:
		//条件等待
		int wait();

		//条件等待
		int timedwait(long sec, long nsec = 0);

		//发送信号
		int signal();

		//广播信号
		int broadcast();

	private:
		pthread_cond_t _cond;
		Mutex_Lock &_mutex;
	};

	//读写锁
	class RW_Lock
	{
	public:
		RW_Lock();
		~RW_Lock();

	public:
		//获取读锁
		void readLock();

		//获取写锁
		void writeLock();

		//释放锁
		void unLock();

	private:
		pthread_rwlock_t _rwlock;
	};

	//范围读锁
	class Guard_Mutex_Lock
	{
	public:
		Guard_Mutex_Lock(Mutex_Lock &mutex) :_mtx(mutex)
		{
			this->_mtx.lock();
		}
		~Guard_Mutex_Lock()
		{
			this->_mtx.unlock();
		}
	private:
		Mutex_Lock &_mtx;
	};
}




#endif
