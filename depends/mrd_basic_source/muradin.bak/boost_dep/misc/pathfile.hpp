#ifndef	MURADIN_MISC_PATHFILE_HPP__
#define MURADIN_MISC_PATHFILE_HPP__

#include <boost/filesystem.hpp>

#include <string>

namespace muradin{namespace misc{
	
	// Got file basename and extension
	// e.g.
	// get_basename_ext("c:\hello.log",basename_and_ext,err_msg) => basename_and_ext.first is "hello",basename_and_ext.second is ".log"
	// get_basename_ext("../.hello.log",basename_and_ext,err_msg) => basename_and_ext.first is ".hello",basename_and_ext.second is ".log"
	// get_basename_ext("../.hello",basename_and_ext,err_msg) => basename_and_ext.first is ".hello",basename_and_ext.second is ""(empty)
	static bool	get_basename_ext(const std::string& pach_txt,std::pair<std::string,std::string>& basename_and_ext,std::string& what_wrong)
	{
		try{
			boost::filesystem::path pth(pach_txt);
			basename_and_ext.first = boost::filesystem::basename (pth);  
			basename_and_ext.second = boost::filesystem::extension(pth); 
			return true;
		}catch(const boost::filesystem::filesystem_error& e){
			what_wrong=e.what();
			return false;
		}catch(const std::exception& e){
			what_wrong=e.what();
			return false;
		}catch(...){
			what_wrong="get_basename_ext() got unknow exception";
			return false;
		}
	}
}}
#endif//MURADIN_MISC_PATHFILE_HPP__