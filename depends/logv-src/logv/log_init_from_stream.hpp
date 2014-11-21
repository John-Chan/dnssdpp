
#ifndef LOGV_LOG_INIT_FROM_STREAM_H__
#define LOGV_LOG_INIT_FROM_STREAM_H__


#include "boost_lg.hpp"
#include "log_constant.h"
#include "log_severity_level.hpp"
#include "log_more_functional.hpp"

#include <fstream>

namespace logV{


	static bool	file_open_no_throw(const std::string& conf_file_name,std::ifstream& file,std::string& what_wrong)
	{
		std::ifstream::iostate stat=file.exceptions();
		file.exceptions ( std::ifstream::goodbit );
		file.open (conf_file_name.c_str());
		file.exceptions ( stat);
		if (!file.good()){
			what_wrong="fail in opening or reading file";
			return false;
		}else{
			return true;
		}
	}

	/// open,prase config file,and create global log-core object automatically 
	// @note
	// - this function is thread-safe
	// - this function throw std::exception based exception. @see init_log_env_no_throw
	static void	init_log_env(const std::string& conf_file_name)
	{
		user_keyword_support();
		std::ifstream file;
		std::string wrong_msg;
		if(!file_open_no_throw(conf_file_name,file,wrong_msg))
			throw std::runtime_error(wrong_msg.c_str());

		logging::init_from_stream(file);
		add_more_attrib_support();
	}


	/// no-throw version for init_log_env
	static bool	init_log_env_no_throw(const std::string& conf_file_name,std::string& what_wrong)
	{
		try{
			init_log_env(conf_file_name);
			return true;
		}catch (const std::exception& e){
			what_wrong=e.what();
			return false;
		}catch (...){
			what_wrong="got unknow exception";
			return false;
		}
	}

}

#endif//LOGV_LOG_INIT_FROM_STREAM_H__