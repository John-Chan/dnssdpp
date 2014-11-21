
/**
 * 
 * @file       log_record.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-1-17
 *  
 * @brief      日志消息记录
 */


#ifndef	MURADIN_LOG_RECORD_ID_HPP__
#define MURADIN_LOG_RECORD_ID_HPP__

#include <string>
#include <vector>

#include "log_attrib.hpp"

namespace muradin
{
namespace loging
{
	class	log_string_msg
	{
	public:	
		log_string_msg&	operator<<(const std::string& msg)
		{
			msg_+=msg;
			return *this;
		}
		log_string_msg&	operator<<(const char* msg)
		{
			msg_+=msg;
			return *this;
		}

		const std::string& get()const{return msg_;};
	private:
		std::string msg_;
	};

	template <typename T>
	class record_vec
	{
	public:
		typedef	std::vector<T>	ContainerType;
	public:
		T&	new_record()
		{
			//T tmp;
			vals_.push_back(T());
			return vals_.back();
		}
		record_vec<T>&	operator<<(const T& val)
		{
			vals_.push_back(val);
			return *this;
		}
		void	clear()
		{
			vals_.clear();
		}
		size_t count()const{return vals_.size();}
	private:
		ContainerType	vals_;
	};

	typedef record_vec<log_string_msg>	LogLine;
}
}
#endif//MURADIN_LOG_RECORD_ID_HPP__