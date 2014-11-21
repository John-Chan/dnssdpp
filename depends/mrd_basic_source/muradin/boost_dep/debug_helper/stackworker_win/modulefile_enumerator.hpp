#ifndef MURADIN_DBG_HELPER_MODULEFILE_ENUMERATOR_HPP__
#define MURADIN_DBG_HELPER_MODULEFILE_ENUMERATOR_HPP__


#include <muradin/boost_dep/debug_helper/stackworker_win/api_waper.hpp>
#include <muradin/boost_dep/debug_helper/stackworker_win/misc.hpp>
#include <muradin/boost_dep/debug_helper/stackworker_win/printer.hpp>
#include <muradin/boost_dep/debug_helper/stackworker_win/module_info.hpp>
#include <muradin/boost_dep/debug_helper/stackworker_win/system_helper.hpp>


#include <list>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#pragma comment(lib, "version.lib")
namespace	muradin{namespace dbg{

	
	/// ModuleLoadCallback
	/// @para
	/// _1 index,start with 0
	/// _2 ModuleInfo class
	/// _3 shoudld skip
	typedef	boost::function<void(size_t,const ModuleInfo*,bool*)>	ModuleEnumCallback;

	typedef std::list<ModuleInfo>		ModuleInfoList;

	class	ModuleFileEnumerator
	{
	public:
		ModuleFileEnumerator(const ModuleEnumCallback& on_module_enum)
			:on_module_enum_(on_module_enum)
		{
			if(NULL==on_module_enum_)
				on_module_enum_=boost::bind(&ModuleFileEnumerator::print,this,_1,_2,_3);
			th32api_.load();
			psapi_.load();
			//dbg_helper_api_.load();
		}
		bool	enum_proc_modules(HANDLE process,DWORD process_id,int& system_error)
		{
			// try toolhelp32 api first ,then psapi
			if(!get_modules_by_th32api(process,process_id,system_error)){
				return get_modules_by_psapi(process,system_error);
			}else{
				return true;
			}
		}
	private:
		void		save_version_info(const TCHAR* file_name,ModuleInfo& saver)
		{
			const ULONGLONG	no_version=0xFFFFFFFF;
			ULONGLONG file_version=get_file_version(file_name,no_version);
			if(file_version == no_version){
				_sntprintf_s(saver.file_version,(sizeof(saver.file_version)/sizeof(saver.file_version[0])),_T("%s"),_T("NULL"));
			}else{

				DWORD v4 = (DWORD) file_version & 0xFFFF;
				DWORD v3 = (DWORD) (file_version>>16) & 0xFFFF;
				DWORD v2 = (DWORD) (file_version>>32) & 0xFFFF;
				DWORD v1 = (DWORD) (file_version>>48) & 0xFFFF;
				_sntprintf_s(saver.file_version,(sizeof(saver.file_version)/sizeof(saver.file_version[0])),_T("%d.%d.%d.%d"),v1,v2,v3,v4);
			}
		}
		void		save_filetime_info(const TCHAR* file_name,ModuleInfo& saver)
		{
			SYSTEMTIME creat_time={0};
			SYSTEMTIME access_time={0};
			SYSTEMTIME write_time={0};
			/// FIXME: check is_NTFS
			bool is_NTFS=true;
			get_file_time(file_name,creat_time,access_time,write_time,is_NTFS); // igrone fail

			_sntprintf_s(saver.time_create,(sizeof(saver.time_create)/sizeof(saver.time_create[0])),_T("%04d-%02d-%02d_%02d-%02d-%02d"),
				creat_time.wYear,creat_time.wMonth,creat_time.wDay,creat_time.wHour,creat_time.wMinute,creat_time.wSecond);
			_sntprintf_s(saver.time_last_access,(sizeof(saver.time_last_access)/sizeof(saver.time_last_access[0])),_T("%04d-%02d-%02d_%02d-%02d-%02d"),
				access_time.wYear,access_time.wMonth,access_time.wDay,access_time.wHour,access_time.wMinute,access_time.wSecond);
			_sntprintf_s(saver.time_last_write,(sizeof(saver.time_last_write)/sizeof(saver.time_last_write[0])),_T("%04d-%02d-%02d_%02d-%02d-%02d"),
				write_time.wYear,write_time.wMonth,write_time.wDay,write_time.wHour,write_time.wMinute,write_time.wSecond);
		}

		/// return should skip work
		bool	more_module_info(HANDLE owner_process,size_t index,const Tool32API::MODULEENTRY32& entry,int& system_error)
		{
			ModuleInfo	info;
			memcpy(&info.module_info_from_tl32,&entry,sizeof(info.module_info_from_tl32));
			info.have_tl32_module_info=true;
			_tcsncpy_s(info.base_file_name,entry.szModule,_tcslen(entry.szModule));
			_tcsncpy_s(info.full_file_name,entry.szExePath,_tcslen(entry.szExePath));
			save_version_info(entry.szExePath,info);
			save_filetime_info(entry.szExePath,info);
			bool skip=false;
			do_callback(index,&info,&skip);
			return skip;
		}
		/// return should skip work
		bool	more_module_info(
			HANDLE owner_process,
			size_t index,
			const TCHAR* exe_name, 
			const TCHAR* base_name,
			const PSAPI::MODULEINFO& mod_info,
			int& system_error)
		{
			ModuleInfo	info;
			memcpy(&info.module_info_from_psapi,&mod_info,sizeof(info.module_info_from_psapi));
			info.have_psapi_module_info=true;
			_tcsncpy_s(info.base_file_name,base_name,_tcslen(base_name));
			_tcsncpy_s(info.full_file_name,exe_name,_tcslen(exe_name));
			
			save_version_info(exe_name,info);
			save_filetime_info(exe_name,info);
			bool skip=false;
			do_callback(index,&info,&skip);
			return skip;
		}
		bool	qry_modules_count(HANDLE process ,DWORD& count,int& system_error)
		{
			if(psapi_.all_func_good()){
				auto_buff_c<HMODULE> modules(1);
				if(modules.is_null()){
					system_error=ERROR_NOT_ENOUGH_MEMORY;
					return false;
				}
				DWORD		bytes= static_cast<DWORD>(modules.bytes());
				DWORD		need_bytes=0;
				if(psapi_.MrdEnumProcessModules(process,modules.ptr(),bytes,&need_bytes)){
					count= need_bytes/sizeof(HMODULE);
					return true;
				}else{
					retrieve_sys_error(system_error);
					return false;
				}	
			}else{
				system_error=ERROR_INVALID_FUNCTION;
				return false;
			}
		}
		/// The handle must have the PROCESS_QUERY_INFORMATION and PROCESS_VM_READ access rights
		bool	get_modules_by_psapi(HANDLE process ,int& system_error)
		{
			static const unsigned short kMaxModuleCount=2048;
			if(!psapi_.all_func_good()){
				system_error=ERROR_INVALID_FUNCTION;
				return false;
			}
			DWORD		module_count=0;
			if(!qry_modules_count(process,module_count,system_error)){
				return false;
			}
			/// FIXME: module_count == 0 ?
			auto_buff_c<HMODULE> modules(module_count);
			auto_buff_c<TCHAR> exe_name(MAX_PATH+1);
			auto_buff_c<TCHAR> base_name(MAX_PATH+1);
			if(modules.is_null() ||exe_name.is_null() ||base_name.is_null()  ){
				system_error=ERROR_NOT_ENOUGH_MEMORY;
				return false;
			}
			DWORD		alloced_bytes= static_cast<DWORD>(modules.bytes());
			DWORD		need_bytes=0;
			if(!psapi_.MrdEnumProcessModules(process,modules.ptr(),alloced_bytes,&need_bytes)){
				retrieve_sys_error(system_error);
				return false;
			}
			if(need_bytes > alloced_bytes){
				/// maybe need_bytes has been changed after call qry_modules_count
				system_error=ERROR_NOT_ENOUGH_MEMORY;
				return false;
			}else{
				size_t	index=0;
				size_t	count=need_bytes/sizeof(HMODULE);
				for ( ; index < count; ++index){
					PSAPI::MODULEINFO module_info={0};

					if(!pspi_get_module_info(process,modules.ptr()[index],module_info,exe_name,base_name,system_error)){
						return false;
					}

					if(more_module_info(process,index,exe_name.ptr(),base_name.ptr(),module_info,system_error)){
						// usercallback  skiped
						return true;
					}
				}
				return true;
			}
		}

		bool	pspi_get_module_info(HANDLE process ,HMODULE module,PSAPI::MODULEINFO& module_info,auto_buff_c<TCHAR>& exe_name,auto_buff_c<TCHAR>& base_name,int& system_error)
		{
			BOOST_ASSERT(!exe_name.is_null());
			BOOST_ASSERT(!base_name.is_null());
			BOOST_ASSERT(psapi_.all_func_good());
			// base address, size
			if(!psapi_.MrdGetModuleInformation(process,module,&module_info, sizeof( PSAPI::MODULEINFO) )){
				retrieve_sys_error(system_error);
				return false;
			}
			// image file name
			// truncate when buff too small
			if(0==psapi_.MrdGetModuleFileNameEx(process,module,exe_name.ptr(),(DWORD)exe_name.elem_count())){
				retrieve_sys_error(system_error);
				return false;
			}
			// module name
			// truncate when buff too small
			if(0==psapi_.MrdGetModuleBaseName(process,module,base_name.ptr(),(DWORD)base_name.elem_count())){
				retrieve_sys_error(system_error);
				return false;
			}
			return true;
		}
		bool	get_modules_by_th32api(HANDLE process,DWORD process_id,int& system_error)
		{
			if(th32api_.all_func_good()){
				Win32Handle	process_snap;
				Tool32API::MODULEENTRY32	module_entry;
				process_snap.reset(th32api_.MrdCreateToolhelp32Snapshot(Tool32API::kTH32SnapModule,process_id)) ;
				if(INVALID_HANDLE_VALUE == process_snap.get()){
					retrieve_sys_error(system_error);
					return false;
				}else{
					size_t	index=0;
					zero_mem<Tool32API::MODULEENTRY32>(&module_entry);
					module_entry.dwSize=sizeof(Tool32API::MODULEENTRY32);
					BOOL	got_module=th32api_.MrdModule32First(process_snap.get(),&module_entry);
					while(got_module){
						if(more_module_info(process,index,module_entry,system_error)){
							// usercallback  skiped
							return true;
						}
						++index;

						zero_mem<Tool32API::MODULEENTRY32>(&module_entry);
						module_entry.dwSize=sizeof(Tool32API::MODULEENTRY32);
						got_module=th32api_.MrdModule32Next(process_snap.get(),&module_entry);
					}
					return  (!got_module && ERROR_NO_MORE_FILES ==retrieve_sys_error(system_error) );
				}
			}else{
				system_error=ERROR_INVALID_FUNCTION;
				return false;
			}
		}
	private:
		void				print(size_t index,const ModuleInfo* info,bool* skip)
		{
			ModuleInfoConsolPrinter::print(index,info,skip);
		}
		void				do_callback(size_t index,const ModuleInfo* info,bool* skip)
		{
			if(NULL==on_module_enum_){
				print(index,info,skip);
			}else{
				on_module_enum_(index,info,skip);
			}
		}
	private:
		ModuleEnumCallback	on_module_enum_;
		PSAPI		psapi_;
		Tool32API	th32api_;
		//DbgHelperAPI	dbg_helper_api_;
	};
}}
#endif