
#include "rotater.h"
#include "macros.h"
#include "StringBuilder.h"
#include "profile.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace glog;

/********************************************************
FileNameCreater
*********************************************************/

/********************************************************
   Func Name: open
Date Created: 2019-3-19
 Description: 初始化
	   Input: 
	  Output: 
	  Return: 
	 Caution: 
*********************************************************/
void FileNameCreater::open(const std::string &strPattern)
{
	assert(!strPattern.empty());

	uint8_t mask = 0;
	const char *pcIndex = NULL;
	char basePath[MAXLEN_PATH] = { 0 };
	char extend[32] = { 0 };
	char date[32] = { 0 };
	char connector[32] = { 0 };
	int seqLen = 0;
	const char *pattern = strPattern.c_str();

	this->_cacheDate = CurrentTime::nowDate();

	pcIndex = strchr(pattern, '%');
	if (pcIndex)
	{
		//时间滚动类型
		SET_BITS(mask, 0x010);
	}
	pcIndex = strchr(pattern, '#');
	if (pcIndex)
	{
		//序号滚动类型
		SET_BITS(mask, 0x100);
	}

	if (BIT_ENABLED(mask,0x110))
	{
		//联合滚动
		sscanf(pattern, "%[^%%]%%d(%[^)])%[^#]#%ds%s", basePath, date, connector, &seqLen, extend);
		this->_basePath = basePath;
		this->_extendName = extend;
		this->_seqLen = seqLen;
		this->_connector = connector;
		this->_type = UNITE_ROLL;
	}else if (BIT_ENABLED(mask,0x010))
	{
		//时间滚动
		sscanf(pattern, "%[^%%]%%d(%[^)])%s", basePath, date, extend);
		this->_basePath = basePath;
		this->_extendName = extend;
		this->_type = DATE_ROLL;
	}else if (BIT_ENABLED(mask,0x100))
	{
		//序号滚动
		sscanf(pattern, "%[^#]#%ds%s", basePath, &seqLen, extend);
		this->_basePath = basePath;
		this->_extendName = extend;
		this->_seqLen = seqLen;
		this->_type = SEQ_ROLL;
	}
	else
	{
		//无滚动
		this->_basePath = pattern;
		this->_type = NON_ROLL;
	}
}

/********************************************************
   Func Name: name
Date Created: 2019-3-19
 Description: 初始化
	   Input: 
	  Output: 
	  Return: 
	 Caution: 
*********************************************************/
std::string FileNameCreater::name(void)
{
	gtl::TLStringBuilder<char> builder;
	switch (this->_type)
	{
	case NON_ROLL:
		builder.Append(this->_basePath);
		break;
	case DATE_ROLL:
		builder.Append(this->_basePath);
		builder.Append(this->_cacheDate);
		builder.Append(this->_extendName);
		break;
	case SEQ_ROLL:
	{
		char formatStr[32] = { 0 };
		char resPath[MAXLEN_PATH] = { 0 };
		sprintf(formatStr, "%%s%%0%du%%s", this->_seqLen);
		sprintf(resPath, formatStr, this->_basePath.c_str(), this->_count++, this->_extendName.c_str());
		builder.Append(resPath);
	}
	break;
	case UNITE_ROLL:
	{
		char formatStr[32] = { 0 };
		char resPath[MAXLEN_PATH] = { 0 };
		sprintf(formatStr, "%%s%%s%%s%%0%du%%s", this->_seqLen);
		sprintf(resPath, formatStr, this->_basePath.c_str(), this->_cacheDate.c_str(), this->_connector.c_str(), this->_count++, this->_extendName.c_str());
		builder.Append(resPath);
	}
	break;
	default:
		break;
	}

	return builder.ToString();
}

/********************************************************
Rotater
*********************************************************/

/********************************************************
   Func Name: open
Date Created: 2019-3-20
 Description: 初始化
	   Input: 
	  Output: 
	  Return: error code
	 Caution: 
*********************************************************/
int Rotater::open(const std::string &strPattern, uint32_t rollCount)
{
	assert(!strPattern.empty());

	this->_nameCreater.open(strPattern);
	this->_rollCount = rollCount;

	return 0;

}

/********************************************************
   Func Name: rotaterName
Date Created: 2019-3-20
 Description: 滚动文件名
	   Input: 
	  Output: 
	  Return: 滚动文件名
	 Caution: 
*********************************************************/
std::string Rotater::rotaterName(const std::string &date)
{
	std::string fileName;

	//1.删除无效文件
	if (this->_rollCount)
	{
		while (this->_rollNames.size() > this->_rollCount)
		{
			fileName = this->_rollNames.front();

			assert(!fileName.empty());

			::unlink(fileName.c_str());

			this->_rollNames.pop();
		}
	}

	//2.创建新的文件名
	if (!date.empty())
	{
		this->_nameCreater.reset(date);
	}

	fileName = this->_nameCreater.name();

	//3.存储新文件名
	this->_rollNames.push(fileName);

	return fileName;

}












