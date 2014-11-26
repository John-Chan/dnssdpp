#ifndef LOGV_EZ_SETUP_HPP__
#define LOGV_EZ_SETUP_HPP__

#include "log_init_from_setting.hpp"
#include <string>
#include <boost/filesystem.hpp>

namespace logV{

	enum LogerType{ 
		LOGER_CONSOL,
		LOGER_TXT_FILE
	};

	class SimpleSeting
	{
	public:
		enum{MAX_UNEQUE_NAME=32};
	protected:
		log_severity_level	lv;
		std::string	uneque_name;
		std::string	log_msg_fmt;
		std::string	log_msg_filter;
		bool		async_commit;
		bool		auto_flush;
	public:
		virtual void		put_setting(logging::settings& setts)=0;
		//virtual void		reset_default()=0;


		SimpleSeting()
			:lv(logV::info),uneque_name("Loger")
		{
			//reset_default();
		}
		SimpleSeting(logV::log_severity_level log_lvl,const std::string& unequeName)
			:lv(log_lvl),uneque_name(unequeName)
		{
			//reset_default();
		}

		virtual ~SimpleSeting()
		{
			//
		}
		log_severity_level	getLogLv()const
		{
			return lv;
		}
		const std::string&	getUnequeName()const{
			return uneque_name;
		}
		const std::string&	getMsgFmt()const{
			return log_msg_fmt;
		}
		const std::string&	getMsgFilter()const{
			return log_msg_filter;
		}
		const bool&	getAsyncCommit()const{
			return async_commit;
		}
		const bool&	getAutoFlush()const{
			return auto_flush;
		}

		void	setLogLv(log_severity_level v)
		{
			lv=v;
		}
		// unequeName must [1~32] characters
		void	setUnequeName(const std::string& v){
			uneque_name=v;
			if(uneque_name.length() > MAX_UNEQUE_NAME )
				uneque_name=uneque_name.substr(0,MAX_UNEQUE_NAME);
		}
		void	setMsgFmt(const std::string& v){
			log_msg_fmt=v;
		}
		void	setMsgFilter(const std::string& v){
			log_msg_filter=v;
		}
		void	setAsyncCommit(bool v){
			async_commit=v;
		}
		void	setAutoFlush(bool v){
			auto_flush=v;
		}

	};

	/************************************************************************
	//  ConsolLogCfg
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	class ConsolLogCfg:public SimpleSeting
	{
	private:
		void	consol_log_default()
		{
			setMsgFmt("%RecordID% - <%Severity%> - <%TimeStamp%> - <%ThreadID%> - %Message%");
			std::string def_filter=std::string("%Severity% >= ")+std::string(log_severity_level_name(lv));
			setMsgFilter(def_filter);
			setAsyncCommit(false);
			setAsyncCommit(true);
		}
	public:
		virtual void		put_setting(logging::settings& setts)
		{
			std::string logerName=uneque_name+"_Console";
			setts["Sinks"][logerName]["Destination"]="Console";
			setts["Sinks"][logerName]["Asynchronous"]=async_commit;
			setts["Sinks"][logerName]["Filter"]=log_msg_filter;
			setts["Sinks"][logerName]["AutoFlush"]=auto_flush;
			setts["Sinks"][logerName]["Format"]=log_msg_fmt;
		}
		virtual void		reset_default()
		{
			consol_log_default();
		}

		ConsolLogCfg()
			:SimpleSeting()
		{
			reset_default();
		}
		ConsolLogCfg(logV::log_severity_level log_lvl,const std::string& unequeName)
			:SimpleSeting(log_lvl,unequeName)
		{
			reset_default();
		}
		
	};

	/************************************************************************
	//  TxtLogCfg
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	class TxtLogCfg:public SimpleSeting
	{
	private:
		std::string	archive_log_path;
		std::string	temp_log_path;
		size_t		rotation_size_kb;
		size_t		rotation_interval_sec;
	private:
		void	txtlog_default()
		{
			setMsgFmt("%RecordID% - <%Severity%> - <%TimeStamp%> - <%ThreadID%> - %Message%");
			std::string def_filter=std::string("%Severity% >= ")+std::string(log_severity_level_name(lv));
			setMsgFilter(def_filter);
			setAsyncCommit(false);
			setAsyncCommit(true);
			archive_log_path="";

			temp_log_path="";
			rotation_size_kb=4096;
			rotation_interval_sec=60;
		}

	public:
		/// return "" if cant get parent path
		static std::string	get_parent_path(const std::string& path)
		{
			boost::system::error_code ec;
			boost::filesystem::path pt(path);
			if(pt.has_parent_path())
				return pt.parent_path().string();
			return "";
		}
		virtual void		put_setting(logging::settings& setts)
		{
			std::string logerName=uneque_name+"_File";

			setts["Sinks"][logerName]["Destination"]="TextFile";
			setts["Sinks"][logerName]["Asynchronous"]=async_commit;
			setts["Sinks"][logerName]["Filter"]=log_msg_filter;
			setts["Sinks"][logerName]["AutoFlush"]=auto_flush;
			setts["Sinks"][logerName]["Format"]=log_msg_fmt;

			std::string tmp_log_name=uneque_name+"_%3N.log";
			if(temp_log_path.length() >0){
				tmp_log_name=temp_log_path;
			}
			setts["Sinks"][logerName]["FileName"]=tmp_log_name;

			if(archive_log_path.length() > 0){
				setts["Sinks"][logerName]["Target"]=archive_log_path;
				setts["Sinks"][logerName]["RotationSize"]=rotation_size_kb*1024;
				setts["Sinks"][logerName]["RotationInterval"]=rotation_interval_sec;
			}
		}
		virtual void		reset_default()
		{
			txtlog_default();
		}

		TxtLogCfg()
			:SimpleSeting()
		{
			reset_default();
		}
		TxtLogCfg(logV::log_severity_level log_lvl,const std::string& unequeName)
			:SimpleSeting(log_lvl,unequeName)
		{
			reset_default();
		}

		// return empty string means LogArchive disabled
		const std::string&	getArchivePath()const
		{
			return archive_log_path;
		}
		const std::string&	getTmpLogPath()const
		{
			return temp_log_path;
		}
		size_t				getRotationSizeKB()const
		{
			return rotation_size_kb;
		}
		size_t				getRotationIntervalSec()const
		{
			return rotation_interval_sec;
		}

		bool	enableArchive(const std::string& dir_path,size_t RotationSizeKB=4096,size_t RotationIntervalSec=300 )
		{
			if(dir_path.length() == 0 ){
				archive_log_path="log";
				temp_log_path=uneque_name+"_%3N.log";
			}else{
				boost::system::error_code ec;
				boost::filesystem::path pt(dir_path);
				bool is_exists=boost::filesystem::exists(pt,ec);
				bool is_file=boost::filesystem::is_regular_file(pt,ec);
				bool is_dir=boost::filesystem::is_directory(pt,ec);
				if(is_dir && !ec){
					boost::filesystem::path pt2(pt);
					boost::filesystem::path pt3(pt);
					pt2/=uneque_name+"_%3N.log";
					pt3/="log";
					temp_log_path=pt2.string();
					archive_log_path=pt3.string();
				}else{
					archive_log_path="log";
					temp_log_path=uneque_name+"_%3N.log";
				}
			}
			rotation_size_kb=RotationSizeKB;
			rotation_interval_sec=RotationIntervalSec;
			
			return true;
		}

		void				setRotationSizeKB(size_t v)
		{
			rotation_size_kb=v;
		}
		void				setRotationIntervalSec(size_t v)
		{
			rotation_interval_sec=v;
		}
	};

}
#endif // LOGV_EZ_SETUP_HPP__