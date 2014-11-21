
/**
 * 
 * @file       log_attrib.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-1-17
 *  
 * @brief      日志消息属性(时间戳,线程ID,源代码信息等)
 */


#ifndef	MURADIN_LOG_ATTRIB_HPP__
#define MURADIN_LOG_ATTRIB_HPP__

#include <Windows.h>//for GetLocalTime GetSystemTime SYSTEMTIME GetCurrentThreadId
#include <stdio.h>//for sprintf_s _snprintf_s

#include <string>

namespace muradin
{
namespace loging
{
	enum severity_level
	{
		log_debug,				/* 调试信息 */
		log_normal,				/* 一般信息 */
		log_notification,		/* 通知信息 */
		log_warning,			/* 警告信息 */
		log_error,				/* 错误信息 */
		log_critical			/* 严重错误信息 */
	};

	/// 返回severity_level的字符串描述
	inline const char* severity_level_str(severity_level lv)
	{
		if (log_debug == lv){
			return "debug";
		}else if (log_normal == lv){
			return "normal";
		}else if (log_notification == lv){
			return "notification";
		}else if (log_warning == lv){
			return "warning";
		}else if (log_error == lv){
			return "error";
		}else if (log_critical == lv){
			return "critical";
		}else {
			return "unknow";
		}
	}
	/// 返回severity_level的字符串描述，字符串是用空格填充来对齐的
	inline const char* severity_level_str_fmt1(severity_level lv)
	{
		if (log_debug == lv){
			return "debug        ";
		}else if (log_normal == lv){
			return "normal       ";
		}else if (log_notification == lv){
			return "notification ";
		}else if (log_warning == lv){
			return "warning      ";
		}else if (log_error == lv){
			return "error        ";
		}else if (log_critical == lv){
			return "critical     ";
		}else {
			return "unknow       ";
		}
	}

	class log_attrib
	{
	public:
		severity_level level;
		int	 log_id;
		char timestamp_str[64];
		int	 thread_id;
		char src[256];
		char func[128];
		int line;
		void* cookie;
		log_attrib(severity_level lv,int log_id,void* cookie,const char* src,const char* func,int line)
		{
			make_attrib(lv,log_id,cookie,src,func,line,*this);
		}
		log_attrib()
		{
			make_attrib(log_debug,0,NULL,NULL,NULL,0,*this);
		}
		log_attrib (const log_attrib& rh)
		{
			copy_attrib(*this,rh);
		}
		log_attrib& operator=(const log_attrib& rh)
		{
			if (&rh != this){
				copy_attrib(*this,rh);
			}
			return *this;
		}
		static const char* get_time_format()
		{
			return "%04d-%02d-%02d %02d:%02d:%02d %03d";// time string formating:YYYY-MM-DD HH24:NN:SS MS"
		}
		static log_attrib create_attrib(severity_level lv,int log_id,void* cookie,const char* src,const char* func,int line)
		{
			log_attrib attr;
			make_attrib(lv,log_id,cookie,src,func,line,attr);
			return attr;
		}
		static log_attrib& make_attrib(severity_level lv,int log_id,void* cookie,const char* src,const char* func,int line,log_attrib& attrib)
		{
			static const char* no_src_file="NULL";
			static const char* no_src_func="NULL";

			const char* src_file=((src == NULL)?no_src_file : src);
			const char* src_func=((func == NULL)?no_src_func : func);
			// 
			attrib.thread_id=(int)::GetCurrentThreadId() ;
			local_time_now(attrib.timestamp_str,sizeof(attrib.timestamp_str));
			strncpy_s(attrib.src,sizeof(attrib.src), src_file,_TRUNCATE);
			strncpy_s(attrib.func,sizeof(attrib.func), src_func,_TRUNCATE);
			attrib.line=line;
			attrib.log_id=log_id;
			attrib.cookie=cookie;
			attrib.level=lv;
			return attrib;
		}
		static void copy_attrib(log_attrib& to,const log_attrib& from)
		{
			to.cookie=from.cookie;
			to.level=from.level;
			to.line=from.line;
			to.log_id=from.log_id;
			to.thread_id=from.thread_id;
			memcpy_s(to.src,sizeof to.src,from.src,sizeof to.src);
			memcpy_s(to.timestamp_str,sizeof to.timestamp_str,from.timestamp_str,sizeof to.timestamp_str);
			memcpy_s(to.func,sizeof to.func,from.func,sizeof to.func);
		}
		std::string attrib_to_str()
		{
			char buff_1024[2014]={0};
			return std::string(attrib_to_str(buff_1024,sizeof(buff_1024)));
		}
		char*	attrib_to_str(char* buff,size_t buff_bytes)
		{
			// 类别|时间|线程|日志ID|源代码文件|函数|行号|
			_snprintf_s(buff,buff_bytes,_TRUNCATE ,"%s|%s|tid=%08d|logid=%08d|src=%s|func=%s|line=%08d|",
				severity_level_str_fmt1(this->level),
				this->timestamp_str,
				this->thread_id,
				this->log_id,
				this->src,
				this->func,
				this->line);
			return buff;
		}
	private:
		static char*		format_systemtime_string(const SYSTEMTIME& tm,char* buff,size_t buff_bytes)
		{
			buff[0]=0;
			if(buff_bytes>24)
				sprintf_s(buff,buff_bytes,get_time_format(),tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond,tm.wMilliseconds);
			return buff;
		}
		static char*		local_time_now(char* buff,size_t buff_bytes)
		{
			SYSTEMTIME tm;
			::GetLocalTime(&tm);
			return format_systemtime_string(tm,buff,buff_bytes);
		}
		static char*		utc_time_now(char* buff,size_t buff_bytes)
		{
			SYSTEMTIME tm;
			::GetSystemTime(&tm);
			return format_systemtime_string(tm,buff,buff_bytes);
		}
	};

}
}

#define LOG_MAKE_ATTRIBS_FULL(LOG_LEVEL,LOG_ID,COOKIE_PTR,ATTRIB_PTR) muradin::loging::log_attrib::make_attrib((LOG_LEVEL),(LOG_ID),(COOKIE_PTR),__FILE__,__FUNCTION__,__LINE__,*(ATTRIB_PTR))
#define LOG_MAKE_ATTRIBS_SHORT(LOG_LEVEL,ATTRIB_PTR) muradin::loging::log_attrib::make_attrib((LOG_LEVEL),0,NULL,__FILE__,__FUNCTION__,__LINE__,*(ATTRIB_PTR))
#define LOG_CREATE_ATTRIB_OBJ(LOG_LEVEL) muradin::loging::log_attrib::create_attrib((LOG_LEVEL),0,NULL,__FILE__,__FUNCTION__,__LINE__)
#endif//MURADIN_LOG_ATTRIB_HPP__