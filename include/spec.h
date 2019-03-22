
#ifndef __GLOG_SPEC_H_
#define __GLOG_SPEC_H_

#include "StringBuilder.h"

#include <string>
#include <vector>

namespace glog
{

	/*
	支持规则标识
		%D -- 日期+时间
		%ms -- 毫秒
		%us -- 微妙
		%F -- 源文件
		%L -- 行号
		%m -- 用户信息
		%n -- 换行
		%p -- 线程号
		%t -- tid
		%U -- 方法名
		%V -- 错误级别
	*/

	/* 规则信息 */
	struct SpecInfo
	{
		std::string dateTime;     //日期+时间
		std::string msTime;       //毫秒
		std::string usTime;       //微妙
		std::string srcFile;      //源文件
		std::string lineNo;       //源文件行号
		std::string userMsg;      //用户信息
		std::string newLine;      //换行
		std::string pid;          //线程ID
		std::string tid;          //系统tid
		std::string srcFunc;      //方法名
		std::string level;        //错误级别  
	};

	/* 规则基类 */
	class BaseSpec
	{
	public:
		//析构函数
		virtual ~BaseSpec() {}

	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info) = 0;

	protected:
		//基类禁止构造
		BaseSpec() {}
		BaseSpec(const BaseSpec &b) {}

	};

	/* 连接规则 */
	class ConnectorSpec :public BaseSpec
	{
	public:
		//构造函数
		ConnectorSpec(const std::string & connector) :_connector(connector) {}

	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(this->_connector);
		}

	private:
		std::string _connector;
	};

	/* 日期规则 */
	class DateSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(info.dateTime);
		}
	};

	/* 毫秒规则 */
	class MsecSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(info.msTime);
		}
	};

	/* 微妙规则 */
	class UsecSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(info.usTime);
		}
	};

	/* 源文件规则 */
	class FileSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(info.srcFile);
		}
	};

	/* 行号规则 */
	class LineSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(info.lineNo);
		}
	};

	/* 用户规则 */
	class UserSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(info.userMsg);
		}
	};

	/* 换行规则 */
	class NewLineSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(info.newLine);
		}
	};

	/* pid规则 */
	class ProcessSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(info.pid);
		}
	};

	/* tid规则 */
	class ThreadSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(info.tid);
		}
	};

	/* 方法名规则 */
	class FuncSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(info.srcFunc);
		}
	};

	/* 级别规则 */
	class LevelSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append(info.level);
		}
	};

	/* 无效规则 */
	class InvalidSpec :public BaseSpec
	{
	public:
		//拼接规则
		virtual void splitJoin(gtl::TLStringBuilder<char> &builder, const SpecInfo &info)
		{
			builder.Append("");
		}
	};

	/* 规则控制器 */
	class SpecController
	{
	public:
		//析构函数
		~SpecController();

	public:
		//初始化
		int open(const std::string &pattern = "");

		//生成消息
		std::string toMessage(const SpecInfo &info);

	private:
		//生成默认规则
		void defaultSpecs(void);

		//规则解析
		int parseSpec(const std::string &pattern);

		//创建规则
		BaseSpec * createSpec(const std::string &mask);

	private:
		std::vector<BaseSpec *> _specs;    //规则表
	};
}


#endif


