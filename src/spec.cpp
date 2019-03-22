
#include "spec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using namespace glog;

/********************************************************
SpecController
*********************************************************/

/********************************************************
   Func Name: ~SpecController
Date Created: 2019-3-21
 Description: 析构
	   Input: 
	  Output: 
	  Return: 
	 Caution: 
*********************************************************/
SpecController::~SpecController()
{
	for (BaseSpec * obj : this->_specs)
	{
		delete obj;
		obj = NULL;
	}
}

/********************************************************
   Func Name: open
Date Created: 2019-3-21
 Description: 初始化
	   Input: 
	  Output: 
	  Return: 
	 Caution: 
*********************************************************/
int SpecController::open(const std::string &pattern)
{
	if (pattern.empty())
	{
		this->defaultSpecs();
		return 0;
	}

	return this->parseSpec(pattern);
}

/********************************************************
   Func Name: parseSpec
Date Created: 2019-3-21
 Description: 规则解析
	   Input: 
	  Output: 
	  Return: error code
	 Caution: 
*********************************************************/
int SpecController::parseSpec(const std::string &pattern)
{
	assert(!pattern.empty());

	const char *p = pattern.c_str();
	int index = 0;
	char linker[32] = { 0 };    //规则间的连接字符
	char mask[8] = { 0 };       //规则标识
	BaseSpec * spec = NULL;

	while (*p)
	{
		memset(linker, 0, 32);
		memset(mask, 0, 8);

		if ('%' == *p)
		{
			//匹配"%m"
			sscanf(p, "%%%[a-zA-Z]%n", mask, &index);
		}
		else
		{
			//匹配"xx%m"
			sscanf(p, "%[^%%]%%%[a-zA-Z]%n", linker, mask, &index);
		}

		if (linker[0])
		{
			//创建连接符规则
			spec = new ConnectorSpec(linker);
			if (NULL == spec)
			{
				return -1;
			}
			this->_specs.push_back(spec);
		}
		if (mask[0])
		{
			//创建规则
			spec = this->createSpec(mask);
			if (NULL == spec)
			{
				return -1;
			}
			this->_specs.push_back(spec);
		}
		p += index;
	}

	return 0;
}

/********************************************************
   Func Name: createSpec
Date Created: 2019-3-21
 Description: 创建规则
	   Input: 
	  Output: 
	  Return: 规则对象
	 Caution: 
*********************************************************/
BaseSpec * SpecController::createSpec(const std::string &mask)
{
	assert(!mask.empty());

	BaseSpec * spec = NULL;
	const char *p = mask.c_str();

	if (mask == "ms")
	{
		spec = new MsecSpec();
		return spec;
	}

	if (mask == "us")
	{
		spec = new UsecSpec();
		return spec;
	}

	switch (*p)
	{
	case 'D':
		spec = new DateSpec();
		break;
	case 'F':
		spec = new FileSpec();
		break;
	case 'L':
		spec = new LineSpec();
		break;
	case 'm':
		spec = new UserSpec();
		break;
	case 'n':
		spec = new NewLineSpec();
		break;
	case 'p':
		spec = new ProcessSpec();
		break;
	case 't':
		spec = new ThreadSpec();
		break;
	case 'U':
		spec = new FuncSpec();
		break;
	case 'V':
		spec = new LevelSpec();
		break;
	default:
		spec = new InvalidSpec();
		break;
	}

	return spec;
}

/********************************************************
   Func Name: defaultSpecs
Date Created: 2019-3-21
 Description: 生成默认规则
	   Input: 
	  Output: 
	  Return: 
	 Caution: 
*********************************************************/
void SpecController::defaultSpecs(void)
{
	const char * pattern = "%D.%ms [%V][%F:%L] %m%n";

	this->parseSpec(pattern);
}

/********************************************************
   Func Name: toMessage
Date Created: 2019-3-21
 Description: 生成消息
	   Input: 
	  Output: 
	  Return: 消息体
	 Caution: 
*********************************************************/
std::string SpecController::toMessage(const SpecInfo &info)
{
	gtl::TLStringBuilder<char> builder;

	for (BaseSpec * spec : this->_specs)
	{
		spec->splitJoin(builder, info);
	}

	return builder.ToString();
}
