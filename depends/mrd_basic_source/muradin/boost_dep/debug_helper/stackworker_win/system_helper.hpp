#ifndef MURADIN_DBG_HELPER_SYSTEM_HELPER_HPP__
#define MURADIN_DBG_HELPER_SYSTEM_HELPER_HPP__

#include <muradin/boost_dep/debug_helper/stackworker_win/misc.hpp>

#include <Windows.h>
#include <tchar.h>
namespace	muradin{namespace dbg{

	class	ThreadPauser
	{
	public:
		ThreadPauser(HANDLE thread_handle)
			:paused_(false),thread_handle_(thread_handle)
		{
			//
		}
		~ThreadPauser()
		{
			resume();
		}

		void	pause()
		{
			if(!paused_)
				::SuspendThread(thread_handle_);
		}
		void	resume()
		{
			if(paused_)
				::ResumeThread(thread_handle_);
		}
	private:
		bool	paused_;
		HANDLE	thread_handle_;
	};
	class SystemInfo
	{
	public:
		const TCHAR*	current_user()
		{
			DWORD	char_count=(DWORD)(sizeof(current_user_)/sizeof(current_user_[0]));
			if(!::GetUserName(current_user_,&char_count)){
				_sntprintf_s(current_user_,char_count, _T("Unknow"));
				int err=::GetLastError();
			}
			return current_user_;
		}
		const TCHAR*	os_version()
		{
			_sntprintf_s(os_version_, (sizeof(os_version_)/sizeof(os_version_[0])), _T("Unknow"));
#if _MSC_VER <= 1200
			OSVERSIONINFO ver;
			::ZeroMemory(&ver, sizeof(OSVERSIONINFO));
			ver.dwOSVersionInfoSize = sizeof(ver);
			if (::GetVersionEx(&ver)){
				_sntprintf_s(os_version_, (sizeof(os_version_)/sizeof(os_version_[0])), _T("%d.%d.%d (%s)"), 
					ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber,ver.szCSDVersion);
			}
#else
			OSVERSIONINFOEX  ver;
			::ZeroMemory(&ver, sizeof(OSVERSIONINFOEX ));
			ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			//LPOSVERSIONINFOEXW	p_ver=&ver;
			if (::GetVersionEx( (OSVERSIONINFO*)(&ver))){
				_sntprintf_s(os_version_, (sizeof(os_version_)/sizeof(os_version_[0])), _T("%d.%d.%d (%s) 0x%X-0x%X"), 
					ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber,
					ver.szCSDVersion, ver.wSuiteMask, ver.wProductType);
			}
#endif
			return os_version_;
		}

		// return YYYY-MM-DD_HH-NN-SS-SSS (23 CHARS)
		const TCHAR*	local_time_long()
		{
			SYSTEMTIME st={0};
			::GetLocalTime(&st);
			_sntprintf_s(os_local_time_, (sizeof(os_local_time_)/sizeof(os_local_time_[0])),_T("%04d-%02d-%02d_%02d-%02d-%02d-%03d"),
				st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
			return os_local_time_;
		}
		// return YYYY-MM-DD_HH-NN-SS-SSS (23 CHARS)
		const TCHAR*	utc_time_long()
		{
			SYSTEMTIME st={0};
			::GetSystemTime(&st);
			_sntprintf_s(os_utc_time_, (sizeof(os_utc_time_)/sizeof(os_utc_time_[0])),_T("%04d-%02d-%02d_%02d-%02d-%02d-%03d"),
				st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
			return os_utc_time_;
		}
	private:
		enum	{kMaxCharCount=1024};
		TCHAR	current_user_[kMaxCharCount];
		TCHAR	os_version_[kMaxCharCount];
		TCHAR	os_local_time_[kMaxCharCount];
		TCHAR	os_utc_time_[kMaxCharCount];

	};

	class	Win32Path
	{
	public:
		static	bool	get_abs_path(HMODULE module,TCHAR* path,size_t max_char_count)
		{
			::SetLastError(0);
			DWORD char_count_got=::GetModuleFileName(module,path,(DWORD)max_char_count);
			if(0== char_count_got){
				return false;
			}else{
				int truncated_orr=::GetLastError(); 
				if(ERROR_INSUFFICIENT_BUFFER == truncated_orr){
					//truncated,because buff too small.
					return false;
				}
				return true;
			}
		}
		// moudle can be NULL,that means current module
		static	Win32Path	path_of_module(HMODULE moudle)
		{
			TCHAR	abs_path[kMaxCharCountForPath];
			if(get_abs_path(moudle,abs_path,kMaxCharCountForPath)){
				return Win32Path(abs_path);
			}else{
				return Win32Path(_T(""));
			}
		}


		Win32Path(const TCHAR* abs_path_str)
			:good_(false)
		{
			clear();
			_tcsncpy(abs_path_,abs_path_str,kMaxCharCountForPath);
			extract();
		}

		/// e.g. new_file_path("my-file",".txt")
		Win32Path	new_file_path(const TCHAR* name,const TCHAR* ext)const
		{
			TCHAR	abs_path[kMaxCharCountForPath];
			_sntprintf_s(abs_path, (sizeof(abs_path)/sizeof(abs_path[0])),_T("%s%s%s%s"),
				driver_,dir_,name_,ext_);
			return Win32Path(abs_path);
		}

		/// e.g. new_file_path("my-file.txt")
		Win32Path	new_file_path(const TCHAR* name_with_ext)const
		{
			TCHAR	abs_path[kMaxCharCountForPath];
			_sntprintf_s(abs_path, (sizeof(abs_path)/sizeof(abs_path[0])),_T("%s%s%s"),
				driver_,dir_,name_with_ext);
			return Win32Path(abs_path);
		}

		bool	good()const 
		{
			return good_;
		}
		/// e.g. c:\dir\file.ext => [c:\dir\file.ext] 
		const TCHAR*	full_path(const TCHAR* def_val)const
		{
			return good_?abs_path_:def_val;
		}

		/// e.g. c:\dir\file.ext => [c:]
		const TCHAR*	driver(const TCHAR* def_val)const
		{
			return good_?driver_:def_val;
		}

		/// e.g. c:\dir\file.ext => [\dir\] or [\dir]
		const TCHAR*	dir(const TCHAR* def_val ,bool include_slash)const
		{
			if(!good_)	return def_val;
			return include_slash?dir_:dir2_;
		}

		/// e.g. c:\dir\file.ext => [file]
		const TCHAR*	base_name(const TCHAR* def_val)const
		{
			return good_?name_:def_val;
		}

		/// e.g. c:\dir\file.ext => [.ext]
		const TCHAR*	ext(const TCHAR* def_val)const
		{
			return good_?ext_:def_val;
		}
	private:
		void	clear()
		{
			::ZeroMemory(abs_path_,sizeof(abs_path_));
			::ZeroMemory(driver_,sizeof(driver_));
			::ZeroMemory(dir_,sizeof(dir_));
			::ZeroMemory(dir2_,sizeof(dir2_));
			::ZeroMemory(name_,sizeof(name_));
			::ZeroMemory(ext_,sizeof(ext_));
		}
		
		void	extract()
		{
			// dir_ will including trailing slash.
			errno_t err=_tsplitpath_s(abs_path_,
				driver_,kMaxCharCountForPath,
				dir_,kMaxCharCountForPath,
				name_,kMaxCharCountForPath,
				ext_,kMaxCharCountForPath);
			if(0 != err ){
				good_=false;
				return;
			}
			size_t chars=_tcslen(dir_);
			if (chars > 0){
				_tcsncpy(dir2_,dir_,chars);
				dir2_[chars-1]=0;
			}
			good_=true;
		}
	private:
		enum	{kMaxCharCountForPath=2048};// _MAX_DRIVE, _MAX_DIR, _MAX_FNAME, and _MAX_EXT
		bool	good_;
		TCHAR	abs_path_[kMaxCharCountForPath];
		TCHAR	driver_[kMaxCharCountForPath];
		TCHAR	dir_[kMaxCharCountForPath];
		TCHAR	dir2_[kMaxCharCountForPath];
		TCHAR	name_[kMaxCharCountForPath];
		TCHAR	ext_[kMaxCharCountForPath];
	};
	class	ProcessInfo:boost::noncopyable
	{
	public:
		BOOL	process_mem_counter(HANDLE process,PSAPI::PROCESS_MEMORY_COUNTERS& counters)	
		{
			if(psapi_.load()){
				//counters.cb=sizeof(sizeof(PSAPI::PPROCESS_MEMORY_COUNTERS));
				return psapi_.GetProcessMemoryInfo(process,&counters,sizeof(PSAPI::PROCESS_MEMORY_COUNTERS));
			}
			return FALSE;
		}

		
	private:
		//enum	{kMaxCharCount=1024};
		PSAPI	psapi_;
		//TCHAR	buff_[kMaxCharCount];
	};
	static ULONGLONG	get_file_version(const TCHAR* file_name,ULONGLONG default_val)
	{
		VS_FIXEDFILEINFO *Info_ptr = NULL;
		DWORD handle=0;
		DWORD bytes = ::GetFileVersionInfoSize(file_name, &handle);
		ULONGLONG file_ver=default_val;
		if (bytes > 0){
			auto_buff_c<char> ver_data(bytes);

			if (!ver_data.is_null()){
				if (::GetFileVersionInfo(file_name, handle, (DWORD)ver_data.bytes(), ver_data.ptr()) != 0){
					UINT len=0;
					TCHAR sub_block[] = _T("\\");
					if (::VerQueryValue(ver_data.ptr(), sub_block, (LPVOID*) &Info_ptr, &len) != 0){
						file_ver = ((ULONGLONG)Info_ptr->dwFileVersionLS) + ((ULONGLONG)Info_ptr->dwFileVersionMS << 32);
					}
				}
			}
		}
		return file_ver;
	}

	/// set is_utc_time=true if file stores in a NTFS file system
	/// The NTFS file system stores time values in UTC format
	/// The FAT file system stores time values based on the local time of the computer
	static bool		get_file_time(const TCHAR* file_name,SYSTEMTIME& creat_time,SYSTEMTIME& access_time,SYSTEMTIME& write_time,bool is_utc_time)
	{
		FILETIME file_creat_time;
		FILETIME file_access_time;
		FILETIME file_write_time;
		DWORD dwRet=0;
		Win32Handle file_handle(::CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL,OPEN_EXISTING, 0, NULL));
		if(file_handle.is_invalid_handle_value()) return false;

		// Retrieve the file times for the file.
		if (!::GetFileTime(file_handle.get(), &file_creat_time, &file_access_time, &file_write_time))
			return false;

		if(is_utc_time){
			SYSTEMTIME utc_time;
			::FileTimeToSystemTime(&file_creat_time, &utc_time);
			::SystemTimeToTzSpecificLocalTime(NULL, &utc_time, &creat_time);
			::FileTimeToSystemTime(&file_access_time, &utc_time);
			::SystemTimeToTzSpecificLocalTime(NULL, &utc_time, &access_time);
			::FileTimeToSystemTime(&file_write_time, &utc_time);
			::SystemTimeToTzSpecificLocalTime(NULL, &utc_time, &write_time);
		}else{
			::FileTimeToSystemTime(&file_creat_time, &creat_time);
			::FileTimeToSystemTime(&file_access_time, &access_time);
			::FileTimeToSystemTime(&file_write_time, &write_time);
		}
		return true;
	}

}}
#endif