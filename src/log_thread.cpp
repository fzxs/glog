
#include "log_thread.h"
#include "rotater.h"
#include "thread.h"
#include "StringBuilder.h"
#include "profile.h"
#include "rule.h"

#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace glog;

/********************************************************
   Func Name: LogThread
Date Created: 2019-3-21
 Description: 构造函数
	   Input:
	  Output:
	  Return:
	 Caution:
*********************************************************/
LogThread::LogThread() :_fdLog(-1), _maxFileSize(0), _cacheTime(""), _exiting(false), _thread(std::bind(&LogThread::threadFunc, this))
{
}

/********************************************************
   Func Name: EventLoopThread
Date Created: 2019-1-28
 Description: 析构函数
	   Input:
	  Output:
	  Return:
	 Caution:
*********************************************************/
LogThread::~LogThread()
{
	this->_exiting = true;

	if (this->_fdLog >= 0)
	{
		::close(this->_fdLog);
		this->_fdLog = -1;
		this->_thread.join();
	}
}

/********************************************************
   Func Name: startLoop
Date Created: 2019-1-28
 Description: 开启线程
	   Input:
	  Output:
	  Return: EventLoop实例
	 Caution:
*********************************************************/
int LogThread::start(const std::string &pattern, uint32_t rollCount, uint32_t maxFileSize)
{
	assert(!this->_thread.started());
	assert(!pattern.empty());
	assert(maxFileSize);

	int result = 0;

	//1.初始化滚动器
	result = this->_rotater.open(pattern, rollCount);
	if (result)
	{
		return -1;
	}

	//2.初始化成员属性
	this->_maxFileSize = maxFileSize;
	this->_cacheTime = CurrentTime::nowDate();

	//3.创建日志文件
	this->createFile();

	//4.开启子线程，start()方法会等待子线程启动后才会返回
	this->_thread.start();

	return 0;
}

/********************************************************
   Func Name: threadFunc
Date Created: 2019-1-28
 Description: 线程函数
	   Input:
	  Output:
	  Return: EventLoop实例
	 Caution: 该方法是在子线程中运行
*********************************************************/
void LogThread::createFile()
{
	//1.关闭旧的日志文件
	if (this->_fdLog >= 0)
	{
		::close(this->_fdLog);
		this->_fdLog = -1;
	}

	//2.打开新的日志文件
	this->_activeFileName = this->_rotater.rotaterName();
	if (!this->_activeFileName.empty())
	{
		this->_fdLog = open(this->_activeFileName.c_str(),O_WRONLY | O_APPEND | O_CREAT);
	}
	
}

/********************************************************
   Func Name: threadFunc
Date Created: 2019-1-28
 Description: 线程函数
	   Input:
	  Output:
	  Return: EventLoop实例
	 Caution: 该方法是在子线程中运行
*********************************************************/
void LogThread::threadFunc()
{
	int result = 0;
	std::deque<MsgInfo> infos;
	uint32_t restLen = 0;
	uint32_t activeFileSize = 0;

	while (!this->_exiting)
	{
		infos = RuleController::getInstance()->outputQueye();
		if (0 == infos.size())
		{
			break;
		}

		//1.获取当前文件信息
		struct stat fileStat;
		result = stat(this->_activeFileName.c_str(), &fileStat);
		if (result)
		{
			//文件已损坏，重新创建文件
			this->createFile();
			activeFileSize = 0;
		}
		else
		{
			activeFileSize = fileStat.st_size;
		}

		//2.校验当前文件信息
		if (activeFileSize >= this->_maxFileSize)
		{
			this->createFile();
			activeFileSize = 0;
		}

		//3.获取可写文件大小
		restLen = this->_maxFileSize - activeFileSize;

		gtl::TLStringBuilder<char> builder;

		//4.将数据写入缓冲区
		while (!infos.empty())
		{
			//4.1校验当前时间，确认是否需要滚动文件
			if (infos.front().date != this->_cacheTime)
			{
				//刷新缓冲区数据到磁盘上
				this->flush(builder.ToString());
				builder.clear();
				//更新缓存时间
				this->_cacheTime = infos.front().date;
				this->_rotater.reset(this->_cacheTime);
				this->createFile();
				restLen = this->_maxFileSize;
			}

			//4.2 填充数据
			if ((builder.Length() + infos.front().msg.length()) >= restLen)
			{
				//刷新缓冲区数据到磁盘上
				this->flush(builder.ToString());
				builder.clear();
				this->createFile();
				restLen = this->_maxFileSize;
			}

			builder.Append(infos.front().msg);

			infos.pop_front();
		}

		if (builder.Length())
		{
			//刷新缓冲区数据到磁盘上
			this->flush(builder.ToString());
		}
	}

}

/********************************************************
   Func Name: flush
Date Created: 2019-1-28
 Description: 将文件刷新到磁盘上
	   Input:
	  Output:
	  Return: 
	 Caution: 
*********************************************************/
void LogThread::flush(const std::string &strData)
{
	::write(this->_fdLog, strData.c_str(), strData.length());
	::fsync(this->_fdLog);
}

