
/**
 * 
 * @file       log_generater.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-1-17
 *  
 * @brief      简易日志生成器,简化生成日志消息的操作
 */

#ifndef	MURADIN_LOG_GENERATER_HPP__
#define MURADIN_LOG_GENERATER_HPP__

#include "log_attrib.hpp"
#include "log_record.hpp"
#include "log_sink.hpp"

#include <iostream>

namespace muradin
{
namespace loging
{

	template<class TSink>
	class generic_loger
	{
	public:
		generic_loger(const log_attrib& attr,TSink& msg_sink)
			:msg_sink_(msg_sink),attr_(attr)
		{
			one_line_<<attr_.attrib_to_str();
		}
		~generic_loger()
		{
			one_line_<<std::string("\r\n");
			msg_sink_<<one_line_.get();
			msg_sink_.flush();
		}
		log_string_msg&	get_line()
		{
			return one_line_;
		}
	private:
		TSink& msg_sink_;
		log_attrib	attr_;
		log_string_msg		one_line_;
	};

	typedef generic_loger<log_sink_console> ConsolLoger;
	typedef generic_loger<file_writer>		FilelLoger;


}
}



#endif//MURADIN_LOG_GENERATER_HPP__