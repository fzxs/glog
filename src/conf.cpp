
#include "conf.h"
#include "tinyxml2.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace glog;
using namespace tinyxml2;

/********************************************************
TConfigure
*********************************************************/

/********************************************************
   Func Name: load
Date Created: 2019-3-21
 Description: 加载配置文件
	   Input: 
	  Output: 
	  Return: error code
	 Caution: 
*********************************************************/
int TConfigure::load(const std::string &configName, const std::string &category)
{
	assert(!configName.empty());
	assert(!category.empty());

	XMLDocument doc;
	XMLElement* root = NULL;
	XMLElement* paramNode = NULL;

	if (doc.LoadFile(configName.c_str()))
	{
		return -1;
	}

	root = doc.FirstChildElement();
	if (NULL == root)
	{
		return -1;
	}

	//获取全局配置
	{
		XMLElement* globalNode = NULL;
		globalNode = root->FirstChildElement("global");
		if (globalNode)
		{
			//获取缓冲区配置
			XMLElement* bufferNode = NULL;
			const char *pName = NULL;
			const char *pValue = NULL;
			bufferNode = globalNode->FirstChildElement("buffer");
			if (bufferNode)
			{
				paramNode = bufferNode->FirstChildElement("param");
				while (paramNode)
				{
					pName = paramNode->Attribute("name");
					pValue = paramNode->Attribute("value");
					if (NULL == pName || NULL == pValue)
					{
						continue;
					}

					if (0 == strcmp(pName, "MinSize"))
					{
						this->_minBufferSize = this->convertBytes(pValue);
					}else if (0 == strcmp(pName, "MaxSize"))
					{
						this->_maxBufferSize = this->convertBytes(pValue);
					}

					paramNode = paramNode->NextSiblingElement("param");
				}
			}
		}
	}

	//获取类型设置
	{
		XMLElement* ruleNode = NULL;
		ruleNode = root->FirstChildElement("rules");
		if (NULL == ruleNode)
		{
			return 0;
		}

		XMLElement* categoryNode = NULL;
		categoryNode = ruleNode->FirstChildElement("category");
		if (NULL == categoryNode)
		{
			return 0;
		}
		const char *pCategoryName = NULL;
		while (categoryNode)
		{
			pCategoryName = categoryNode->Attribute("name");
			if (NULL == pCategoryName)
			{
				continue;
			}
			if (0 != strcmp(pCategoryName, category.c_str()))
			{
				continue;
			}
			{
				//滚动策略
				XMLElement* rollNode = NULL;
				rollNode = categoryNode->FirstChildElement("rollingPolicy");
				if (rollNode)
				{
					const char *pName = NULL;
					const char *pValue = NULL;
					paramNode = rollNode->FirstChildElement("param");
					while (paramNode)
					{
						pName = paramNode->Attribute("name");
						pValue = paramNode->Attribute("value");
						if (NULL == pName || NULL == pValue)
						{
							continue;
						}
						if (0 == strcmp(pName, "FileNamePattern"))
						{
							this->_fileNamePattern = pValue;
						}
						else if (0 == strcmp(pName, "FileCountPattern"))
						{
							this->_fileCountPattern = ::atoi(pValue);
						}
						else if (0 == strcmp(pName, "FileSizePattern"))
						{
							this->_fileSizePattern = this->convertBytes(pValue);
						}

						paramNode = paramNode->NextSiblingElement("param");
					}
				}
			}

			{
				//布局
				XMLElement* layoutNode = NULL;
				layoutNode = categoryNode->FirstChildElement("layout");
				if (layoutNode)
				{
					const char *pName = NULL;
					const char *pValue = NULL;
					paramNode = layoutNode->FirstChildElement("param");
					while (paramNode)
					{
						pName = paramNode->Attribute("name");
						pValue = paramNode->Attribute("value");
						if (NULL == pName || NULL == pValue)
						{
							continue;
						}
						if (0 == strcmp(pName, "ConversionPattern"))
						{
							this->_specPattern = pValue;
						}

						paramNode = paramNode->NextSiblingElement("param");
					}
				}
			}

			{
				//过滤配置
				XMLElement* filterNode = NULL;
				filterNode = categoryNode->FirstChildElement("filter");
				if (filterNode)
				{
					const char *pName = NULL;
					const char *pValue = NULL;
					const char *pTag = NULL;
					int limit = 0;
					paramNode = filterNode->FirstChildElement("param");
					while (paramNode)
					{
						pName = paramNode->Attribute("name");
						pValue = paramNode->Attribute("value");
						pTag = paramNode->Attribute("value");
						if (NULL == pName || NULL == pValue)
						{
							continue;
						}
						if (pTag)
						{
							switch (toupper(pTag[0]))
							{
							case 'N':
								limit = 20;//NOTICE
								break;
							case 'D':
								limit = 40;//DEBUG
								break;
							case 'I':
								limit = 60;//INFO
								break;
							case 'W':
								limit = 80;//WARN
								break;
							case 'E':
								limit = 100;//ERROR
								break;
							case 'F':
								limit = 120;//FATAL
								break;
							default:
								break;
							}
						}

						if (0 == strcmp(pName, "LevelOperate"))
						{
							//this->_specPattern = pValue;
							switch (*pValue)
							{
							case '*':
							{
								for (int i = 20; i < 140; i += 20)
								{
									this->_level.push_back(i);
								}
							}
							break;
							case '>':
							{
								for (int i = 20; i < 140; i += 20)
								{
									if (i > limit)
									{
										this->_level.push_back(i);
									}
								}
							}
							break;
							case '<':
							{
								for (int i = 20; i < 140; i += 20)
								{
									if (i < limit)
									{
										this->_level.push_back(i);
									}
								}
							}
							break;
							case '!':
							{
								for (int i = 20; i < 140; i += 20)
								{
									if (i != limit)
									{
										this->_level.push_back(i);
									}
								}
							}
							break;
							case '=':
							{
								for (int i = 20; i < 140; i += 20)
								{
									if (i == limit)
									{
										this->_level.push_back(i);
										break;
									}
								}
							}
							break;
							default:
								break;
							}
						}

						paramNode = paramNode->NextSiblingElement("param");
					}
				}
			}
			
			categoryNode = categoryNode->NextSiblingElement("param");
		}
	}
	
	return 0;
}

/********************************************************
   Func Name: convertBytes
Date Created: 2019-3-21
 Description: 转化成字节
	   Input: 
	  Output: 
	  Return: error code
	 Caution: 
*********************************************************/
int TConfigure::convertBytes(const char *pattern)
{
	assert(pattern);

	//提取限制大小
	char num[32] = { 0 };
	char unit[32] = { 0 };
	int numCode = 0;
	sscanf(pattern, "%[0-9]%s", num, unit);
	numCode = ::atoi(num);
	switch (unit[0])
	{
	case 'M':
	case 'm':
		numCode = numCode * 1024 * 1024;
		break;
	case 'K':
	case 'k':
		numCode = numCode * 1024;
		break;
	case 'G':
	case 'g':
		numCode = numCode * 1024 * 1024 * 1024;
		break;
	default:
		break;
	}

	return numCode;
}

