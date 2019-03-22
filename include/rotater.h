

#ifndef __GLOG_ROTATER_H_
#define __GLOG_ROTATER_H_

#include <string>
#include <queue>
#include <stdint.h>

namespace glog
{
	/* 日志文件名生成器 */
	class FileNameCreater
	{
	public:
		enum RotaterType
		{
			NON_ROLL,     //无滚动
			DATE_ROLL,    //日期滚动
			SEQ_ROLL,     //序号滚动
			UNITE_ROLL,   //联合滚动
		};

		//构造函数
		FileNameCreater() :_basePath(""), _extendName(""), _type(NON_ROLL), _seqLen(0), _connector(""), _count(0), _cacheDate("") {}

		//析构函数
		~FileNameCreater() {}

	public:
		//初始化
		void open(const std::string &strPattern);

		//获取文件名
		std::string name(void);

		//重置
		void reset(const std::string &date)
		{
			this->_count = 0;
			this->_cacheDate = date;
		}

	private:
		std::string _basePath;        //主路径
		std::string _extendName;      //扩展名
		RotaterType _type;            //滚动类型
		uint32_t _seqLen;             //序号长度
		std::string _connector;       //连接符
		uint32_t _count;              //计数器
		std::string _cacheDate;       //缓存日期
	};

	/* 滚动器 */
	class Rotater
	{
	public:
		//初始化
		int open(const std::string &strPattern, uint32_t rollCount);

		//滚动文件名
		std::string rotaterName(const std::string &date = "");

		//重置
		void reset(const std::string &date)
		{
			this->_nameCreater.reset(date);
		}

	private:
		FileNameCreater _nameCreater;        //滚动文件名生成器
		uint32_t _rollCount;                 //滚动文件个数
		std::queue<std::string> _rollNames;  //激活文件名集合
	};
}

#endif
