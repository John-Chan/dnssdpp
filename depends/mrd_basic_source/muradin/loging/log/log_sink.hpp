
/**
 * 
 * @file       log_sink.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-1-7
 *  
 * @brief      日志槽
 * @remark     表示日志的输出对象 
 * @note    
 * 
 * @e Copyright (c) <year> <copyright holders>
 */


#ifndef	MURADIN_LOG_SINK_HPP__
#define MURADIN_LOG_SINK_HPP__

#include <string>
#include <iostream>

namespace muradin
{
namespace loging
{

	/// 日志槽-控制台
	class	log_sink_console
	{
	public:
		log_sink_console(){};
		~log_sink_console(){};

		/// 立即刷新(即将缓冲区数据立即输出)
		int	flush()
		{
			std::cout<<std::flush;
			return 0;
		}

		///提交消息
		log_sink_console&	operator<<(const std::string& msg)
		{
			do_output(msg);
			return *this;
		}
		///提交消息
		log_sink_console&	operator<<(const char* msg)
		{
			do_output(msg);
			return *this;
		}
	private:
		void	do_output(const std::string& msg)
		{
			std::cout<< msg;
		}
		void	do_output(const char* msg)
		{
			std::cout<< msg;
		}
	};

}
}
#include "log_txt_file_writer.hpp"

#endif//MURADIN_LOG_SINK_HPP__