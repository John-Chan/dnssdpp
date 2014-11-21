
/**
 * 
 * @file       loging_ez.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-1-17
 *  
 * @brief      简单日志记录封装
 * @remark     适用于简单使用场景  
 * @note    
 * @e<somthing important>
 * 
 * @e Copyright (c) <year> <copyright holders>
 */


#ifndef	MURADIN_LOGING_EASY_HPP__
#define MURADIN_LOGING_EASY_HPP__

#include <muradin/loging/log/log_generater.hpp>
namespace muradin
{
namespace loging
{
	static log_sink_console global_consol_output_;
}
}
// 向控制台输出日志,不保证线程安全，也不保证异常安全
// 用法 
// CONSOLE_LOG(log_debug).get_line()<<"this is log msg"<<" "<<"这是日志消息";
#define CONSOLE_LOG(LOG_LEVEL) ( muradin::loging::ConsolLoger((LOG_CREATE_ATTRIB_OBJ((LOG_LEVEL))),global_consol_output_) )

// 向文件输出日志,不保证线程安全，也不保证异常安全
// 用法 
// muradin::loging::file_writer file("log.txt","w");
// EZ_FILE_LOG(log_debug,file).get_line()<<"this is log msg"<<" "<<"这是日志消息";
// 备注:文件的打开和关闭由使用方维护
#define EZ_FILE_LOG(LOG_LEVEL,FILE_OBJ) ( muradin::loging::FilelLoger((LOG_CREATE_ATTRIB_OBJ((LOG_LEVEL))),FILE_OBJ) )

#endif//MURADIN_LOGING_EASY_HPP__