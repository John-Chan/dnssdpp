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
	static	void	logingInit(bool consolOnly,const std::string& unque_name,const std::string& logLevel="INFOR")
	{

		std::string	logingFilter="%Severity% >= " +logLevel;
		logV::logging::settings setts;
		setts["Core"]["DisableLogging"] = false;

		setts["Sinks.Console"]["Asynchronous"] = "false";
		setts["Sinks.Console"]["Destination"] = "Console";
		setts["Sinks.Console"]["Filter"] = logingFilter;
		setts["Sinks.Console"]["AutoFlush"] = true;
		setts["Sinks.Console"]["Format"] = "%RecordID% - <%Severity%> - <%TimeStamp%> - <%ThreadID%> - %Message%";


		if(!consolOnly){
			setts["Sinks.File"]["Asynchronous"] = "false";
			setts["Sinks.File.Destination"] = "TextFile";
			setts["Sinks.File"]["Filter"] = logingFilter;
			setts["Sinks.File.FileName"] = unque_name+"_%3N.log";
			setts["Sinks.File.AutoFlush"] = true;
			setts["Sinks.File"]["Format"] = "%RecordID% - <%Severity%> - <%TimeStamp%> - <%ThreadID%> - %Message%";
			setts["Sinks.File.RotationSize"] = 1024*1024; // 1024  KB
			setts["Sinks.File.RotationInterval"] = 5; //or 5 seconds
			setts["Sinks.File.Target"] = unque_name+"-log-archive"; 

		}
		logV::init_log_env(setts);
	}

}}
#endif // AIRSERVER_INIT_FOR_LOGING_HPP__