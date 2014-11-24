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
		std::string	log_archive_dir;
		std::string	log_file_tmp_dir;
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
			log_archive_dir="";

			log_file_tmp_dir="";
			rotation_size_kb=4096;
			rotation_interval_sec=60;
		}

	public:
		virtual void		put_setting(logging::settings& setts)
		{
			std::string logerName=uneque_name+"_File";

			setts["Sinks"][logerName]["Destination"]="TextFile";
			setts["Sinks"][logerName]["Asynchronous"]=async_commit;
			setts["Sinks"][logerName]["Filter"]=log_msg_filter;
			setts["Sinks"][logerName]["AutoFlush"]=auto_flush;
			setts["Sinks"][logerName]["Format"]=log_msg_fmt;

			boost::system::error_code ec;
			std::string tmp_log_name=uneque_name+"_%3N.log";
			if(log_file_tmp_dir.length() >0){
				boost::filesystem::path tmplog_path(log_file_tmp_dir);
				if(boost::filesystem::is_directory(tmplog_path,ec) && boost::filesystem::exists(tmplog_path,ec)){
					tmplog_path/= uneque_name+"_%3N.log";
					tmp_log_name=tmplog_path.string();
				}
			}
			setts["Sinks"][logerName]["FileName"]=tmp_log_name;

			if(log_archive_dir.length() > 0){
				std::string archive_path_name="log";
				boost::filesystem::path archive_path(log_archive_dir);
				if(boost::filesystem::is_directory(archive_path,ec)){
					archive_path_name=archive_path.string();
				}
				setts["Sinks"][logerName]["Target"]=archive_path_name;
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
		const std::string&	getLogArchiveDir()const
		{
			return log_archive_dir;
		}
		const std::string&	getLogTmpDir()const
		{
			return log_file_tmp_dir;
		}
		size_t				getRotationSizeKB()const
		{
			return rotation_size_kb;
		}
		size_t				getRotationIntervalSec()const
		{
			return rotation_interval_sec;
		}

		/// enable or disabled archive log  
		/// a_asb_path is a EXISTS dir or file name(abs path)
		/// if a_asb_path point to a file,logs save in it parent dir
		/// if a_asb_path point to a dir,logs save in this dir
		/// e.g. setLogArchive(argv[0])
		void	setLogArchive(const std::string& a_asb_path)
		{
			boost::system::error_code ec;
			boost::filesystem::path store_path(a_asb_path);
			if(boost::filesystem::exists(store_path,ec)){

				if(boost::filesystem::is_regular_file(store_path,ec) && store_path.has_parent_path()){
					store_path=store_path.parent_path();
				}
				if(boost::filesystem::is_directory(store_path,ec)  ){
					log_file_tmp_dir=store_path.string();
					boost::filesystem::path archive_path(store_path);
					archive_path/="log";
					boost::filesystem::create_directories(archive_path,ec);
					log_archive_dir=archive_path.string();
				}
			}
			
		}

		/// enable or disabled archive log  
		/// archive_dir is a dir
		/// tmp_dir is a EXISTS dir  
		/// e.g.
		/// archive_dir is empty,means disable archive log
		void	setLogArchive(const std::string& archive_dir,const std::string& tmp_dir)
		{
			log_archive_dir=archive_dir;
			log_file_tmp_dir=tmp_dir;
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