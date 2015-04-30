#ifndef AIRSERVER_INIT_FOR_LOGING_HPP__
#define AIRSERVER_INIT_FOR_LOGING_HPP__
#include <string>
#include <logv/logv.hpp>

namespace air{namespace loging {

	

	/// loging level (string)
	/// "TRACE"
	/// "DEBUG"
	/// "INFOR"
	/// "WARNG"
	/// "ERROR"
	/// "FATAL"

	//static	void	logingInit(bool consolOnly,const std::string& unque_name,const std::string& exe_name,logV::log_severity_level lv=logV::info)
	//{
	//	//std::string	logingFilter="%Severity% >= " +logLevel;
	//	logV::logging::settings setts;
	//	setts["Core"]["DisableLogging"] = false;

	//	logV::ConsolLogCfg	consolCfg(lv,unque_name); 
	//	consolCfg.put_setting(setts);
	//	if(!consolOnly){

	//		logV::TxtLogCfg txtcfg(lv,unque_name);
	//		//txtcfg.enableArchive(txtcfg.get_parent_path(exe_name),1024,60 );
	//		txtcfg.setTmplogPath(txtcfg.get_parent_path(exe_name));
	//		txtcfg.put_setting(setts);

	//	}
	//	logV::init_log_env(setts);
	//}

	
}}
#endif // AIRSERVER_INIT_FOR_LOGING_HPP__