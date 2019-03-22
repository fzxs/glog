
#ifndef __GTL_STRINGBUILDER_H_
#define __GTL_STRINGBUILDER_H_

#include <vector>
#include <string>

/*
注意：模板类型有要求(并非任意类型，这是由std::basic_string类模板决定的):
    与类型定义的专用化字符串类型元素的char， wstring，为wchar_t， u16string为char16_t，和u32string为char32_t。
*/

namespace gtl 
{
	/* 字符串拼接类 */
	template <typename chr>
	class TLStringBuilder {
	private:
		typedef std::basic_string<chr> string_t;
		typedef std::vector<string_t> container_t;
		typedef typename string_t::size_type size_type;
		container_t _container;
		size_type _total_size;
		void append(const string_t &src)
		{
			_container.push_back(src);
			_total_size += src.size();
		}

		void revoke()
		{
			string_t tmp = _container.back();
			_container.pop_back();
			_total_size -= tmp.size();
		}

	private:
		// No copy constructor, no assignment.
		TLStringBuilder(const TLStringBuilder &);
		TLStringBuilder & operator= (const TLStringBuilder &);

	public:
		TLStringBuilder(const string_t &src)
		{
			if (!src.empty()) {
				_container.push_back(src);
			}
			_total_size = src.size();
		}

		TLStringBuilder()
		{
			_total_size = 0;
		}

		~TLStringBuilder()
		{
			this->_container.clear();
			//std::list<string_t>().swap(_container);
			_total_size = 0;
		}

	public:
		TLStringBuilder & Append(const string_t &src) {
			append(src);
			return *this; 
		}

		TLStringBuilder & Revoke() 
		{
			revoke();
			return *this;
		}

		void clear(void)
		{
			this->_container.clear();
			//std::list<string_t>().swap(_container);
			_total_size = 0;
		}

		size_type Length()
		{
			return _total_size;
		}

		template<class inputIterator>
		TLStringBuilder & Add(const inputIterator &first, const inputIterator &afterLast) {
			for (inputIterator f = first; f != afterLast; ++f) {
				append(*f);
			}
			return *this; 
		}

		TLStringBuilder & AppendLine(const string_t &src) {
			static chr lineFeed[]{ 10, 0 }; // C++ 11
			_container.push_back(src + lineFeed);
			_total_size += 1 + src.size();
			return *this; 
		}
		TLStringBuilder & AppendLine() {
			static chr lineFeed[]{ 10, 0 };
			_container.push_back(lineFeed);
			_total_size++;
			return *this; 
		}

		string_t ToString() const {
			string_t result;

			result.reserve(_total_size + 1);
			typename std::vector<string_t>::const_iterator iter;
			for (iter = _container.begin(); iter != _container.end(); ++iter) {
				result += *iter;
			}
			return result;
		}

	};
}

#endif



