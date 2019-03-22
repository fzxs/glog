
#ifndef __GLOG_RULE_H_
#define __GLOG_RULE_H_

#include "spec.h"
#include "log_thread.h"
#include "Lock.h"
#include "latch.h"

#include <string>

namespace glog
{
	struct MsgInfo
	{
		std::string date;
		std::string msg;
	};

	class RuleController
	{
	public:
		

	public:
		//创建实例对象
		static RuleController *getInstance();

		//初始化
		int open(const std::string &configName, const std::string &category);

		//输出日志
		void outputLog(const std::string &srcFile, uint32_t lineNo, const std::string &srcFunc, int level, const char *fmt, ...);

		//获取消息
		std::deque<MsgInfo> outputQueye(void);

	private:
		//插入队列
		void inputQueye(const MsgInfo &info);

	private:
		RuleController() :_minBufferSize(0), _maxBufferSize(0), _exiting(false), _callingWait(false), _latch(1), _emptyCond(_queueMutex) {}
		RuleController(const RuleController &r);
		void operator=(const RuleController &r) {}
		~RuleController();    //禁止析构

	private:
		static RuleController * _instance;
		static gtc::Mutex_Lock _mutex;

	private:
		uint32_t _minBufferSize;
		uint32_t _maxBufferSize;
		bool _exiting;
		bool _callingWait;
		gtc::CountDownLatch _latch;
		std::vector<uint32_t> _limitLevel;
		SpecController _spec;
		LogThread _logThread;
		gtc::Mutex_Lock _queueMutex;
		gtc::Cond_Lock _emptyCond GUARDED_BY(_queueMutex);
		std::deque<MsgInfo> _queue GUARDED_BY(_queueMutex);
	};





}

#endif


