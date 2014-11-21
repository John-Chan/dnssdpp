#ifndef MURADIN_DBG_HELPER_MODULE_INFO_HPP__
#define MURADIN_DBG_HELPER_MODULE_INFO_HPP__


#include <muradin/boost_dep/debug_helper/stackworker_win/api_waper.hpp>


namespace	muradin{namespace dbg{
	class	ModuleInfo
	{
	public:
		ModuleInfo()
		{
			clear();
		}
		enum		{kVersionStrCount=16,kTimeStrCount=64,kMaxPathLen=512};	// calced by character,not bytes
		PSAPI::MODULEINFO			module_info_from_psapi;
		Tool32API::MODULEENTRY32	module_info_from_tl32;
		TCHAR		full_file_name[kMaxPathLen];
		TCHAR		base_file_name[kMaxPathLen];
		TCHAR		time_create[kTimeStrCount];
		TCHAR		time_last_access[kTimeStrCount];
		TCHAR		time_last_write[kTimeStrCount];
		TCHAR		file_version[kVersionStrCount];
		//V_IMAGEHLP_MODULE64	img_module;
		bool		have_psapi_module_info;
		bool		have_tl32_module_info;
		void	clear()
		{
			::memset(&module_info_from_psapi,0x0,sizeof module_info_from_psapi);
			::memset(&module_info_from_tl32,0x0,sizeof module_info_from_tl32);
			::memset(&full_file_name,0x0,sizeof full_file_name);
			::memset(&base_file_name,0x0,sizeof base_file_name);
			::memset(&time_create,0x0,sizeof time_create);
			::memset(&time_last_access,0x0,sizeof time_last_access);
			::memset(&time_last_write,0x0,sizeof time_last_write);
			::memset(&file_version,0x0,sizeof file_version);
			//::memset(&img_module,0x0,sizeof img_module);
			have_psapi_module_info=false;
			have_tl32_module_info=false;
		}

	};

}}
#endif