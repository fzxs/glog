
#include "rule.h"
#include "conf.h"
#include "profile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <algorithm>

using namespace glog;

/********************************************************
RuleController
*********************************************************/
RuleController * RuleController::_instance = NULL;
gtc::Mutex_Lock RuleController::_mutex;

/********************************************************
   Func Name: getInstance
Date Created: 2019-3-21
 Description: 创建实例对象
	   Input: 
	  Output: 
	  Return: 实例对象
	 Caution: 
*********************************************************/
RuleController * RuleController::getInstance()
{
	RuleController *tmp = NULL;

	if (NULL == _instance)
	{
		{
			//double lock
			gtc::Guard_Mutex_Lock lock(RuleController::_mutex);
			if (NULL == _instance)
			{
				tmp = new RuleController();
				_instance = tmp;
			}
		}
	}

	return _instance;
}

/********************************************************
   Func Name: ~RuleController
Date Created: 2019-3-21
 Description: 析构函数
	   Input: 
	  Output: 
	  Return: 
	 Caution: 
*********************************************************/
RuleController::~RuleController()
{
	gtc::Guard_Mutex_Lock lock(this->_queueMutex);
	this->_exiting = true;
	this->_emptyCond.signal();
}

/********************************************************
   Func Name: open
Date Created: 2019-3-21
 Description: 初始化
	   Input: 
	  Output: 
	  Return: error code
	 Caution: 
*********************************************************/
int RuleController::open(const std::string &configName, const std::string &category)
{
	if (configName.empty() || category.empty())
	{
		return -1;
	}
	//检测配置文件是否存在
	if (::access(configName.c_str(), R_OK | F_OK))
	{
		return -1;
	}

	TConfigure configParser;
	configParser.load(configName, category);

	//初始化成员属性
	this->_minBufferSize = configParser.minBufferSize();
	this->_maxBufferSize = configParser.maxBufferSize();
	this->_limitLevel = configParser.limitLevel();
	this->_spec.open(configParser.specPattern());

	//启动写日志线程
	this->_logThread.start(configParser.rotaterPattern(), configParser.rotaterCount(), configParser.rotaterFileSize());

	this->_latch.wait();

	return 0;
}

/********************************************************
   Func Name: outputLog
Date Created: 2019-3-21
 Description: 输出日志
	   Input: 
	  Output: 
	  Return: error code
	 Caution: 多线程使用
*********************************************************/
void RuleController::outputLog(const std::string &srcFile, uint32_t lineNo, const std::string &srcFunc, int level, const char *fmt, ...)
{
	SpecInfo specInfo;
	
	if (srcFile.empty() || srcFunc.empty() || NULL == fmt)
	{
		return;
	}

	//不需要输出的日志
	if (std::find(this->_limitLevel.begin(), this->_limitLevel.end(), level) == this->_limitLevel.end())
	{
		return;
	}

	//消息参数组装

	//获取当前时间
	uint32_t usec = 0;
	char gUsec[16] = { 0 };
	char gMsec[16] = { 0 };
	std::string date;
	std::string tv;
	CurrentTime::now(date, tv, usec);
	specInfo.dateTime = date + " " + tv;

	sprintf(gUsec, "%u", usec);
	sprintf(gMsec, "%u", usec/1000);

	specInfo.msTime = gMsec;
	specInfo.usTime = gUsec;
	specInfo.level = LevelCode::level(level);

	char gLine[16] = { 0 };
	sprintf(gLine, "%d", lineNo);
	specInfo.lineNo = gLine;

	specInfo.newLine = "\n";
	char gPid[32] = { 0 };
	sprintf(gPid, "%d", CurrentThread::pid());
	specInfo.pid = gPid;

	char gTid[32] = { 0 };
	sprintf(gTid, "%d", CurrentThread::tid());
	specInfo.tid = gTid;

	specInfo.srcFile = srcFile;
	specInfo.srcFunc = srcFunc;

	va_list args;
	int realLen = 0;
	char *tmpBuffer = NULL;

	tmpBuffer = (char *)malloc(this->_minBufferSize);
	if (NULL == tmpBuffer)
	{
		return;
	}
	memset(tmpBuffer, 0, this->_minBufferSize);

	va_start(args, fmt);
	realLen = vsnprintf(tmpBuffer, this->_minBufferSize, fmt, args);
	if (realLen < 0)
	{
		return;
	}
	if (realLen >= (int)this->_minBufferSize)
	{
		if (realLen >= (int)this->_maxBufferSize)
		{
			//日志超过最大限制
			return;
		}
		free(tmpBuffer);
		tmpBuffer = (char *)malloc(this->_maxBufferSize);
		if (NULL == tmpBuffer)
		{
			return;
		}
		memset(tmpBuffer, 0, this->_maxBufferSize);
		vsnprintf(tmpBuffer, this->_maxBufferSize, fmt, args);
	}
	va_end(args);

	specInfo.userMsg = tmpBuffer;

	free(tmpBuffer);
	tmpBuffer = NULL;

	MsgInfo msgInfo;

	msgInfo.msg = this->_spec.toMessage(specInfo);
	msgInfo.date = date;

	this->inputQueye(msgInfo);

}

/********************************************************
   Func Name: push
Date Created: 2019-3-21
 Description: 插入数据
	   Input: 
	  Output: 
	  Return: 
	 Caution: 
*********************************************************/
void RuleController::inputQueye(const MsgInfo &info)
{
	gtc::Guard_Mutex_Lock lock(this->_queueMutex);
	this->_queue.push_back(info);
	this->_emptyCond.signal();
}

/********************************************************
   Func Name: pop
Date Created: 2019-3-21
 Description: 取出数据
	   Input: 
	  Output: 
	  Return: 数据
	 Caution: 
*********************************************************/
std::deque<MsgInfo> RuleController::outputQueye(void)
{
	std::deque<MsgInfo> infos;

	this->_latch.countDown();
	{
		gtc::Guard_Mutex_Lock lock(this->_queueMutex);

		while (0 == this->_queue.size())
		{
			this->_callingWait = true;

			this->_emptyCond.wait();

			this->_callingWait = false;

			if (this->_exiting)
			{
				return infos;
			}
		}
		infos = this->_queue;
		this->_queue.clear();
		
	}

	return infos;
}
