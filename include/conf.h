
#ifndef __GLOG_CONF_H_
#define __GLOG_CONF_H_

#include <string>
#include <stdint.h>
#include <vector>

namespace glog
{
	class TConfigure
	{
	public:
		//加载日志配置
		int load(const std::string &configName, const std::string &category);

		//获取单个缓存的最小值
		uint32_t minBufferSize(void) const
		{
			return this->_minBufferSize;
		}

		//获取单个缓存的最大值
		uint32_t maxBufferSize(void) const
		{
			return this->_maxBufferSize;
		}

		//获取滚动策略
		std::string rotaterPattern(void) const
		{
			return this->_fileNamePattern;
		}

		//获取滚动文件个数
		uint32_t rotaterCount(void) const
		{
			return this->_fileCountPattern;
		}

		//获取滚动文件限制大小
		uint32_t rotaterFileSize(void) const
		{
			return this->_fileSizePattern;
		}

		//获取日志格式
		std::string specPattern(void) const
		{
			return this->_specPattern;
		}

		//获取日志级别
		std::vector<uint32_t> limitLevel(void) const
		{
			return this->_level;
		}

	private:
		//转化成字节
		int convertBytes(const char *pattern);

	private:
		uint32_t _minBufferSize;
		uint32_t _maxBufferSize;
		std::string _fileNamePattern;
		uint32_t _fileCountPattern;
		uint32_t _fileSizePattern;
		std::string _specPattern;
		std::vector<uint32_t> _level;
	};
}

#endif


