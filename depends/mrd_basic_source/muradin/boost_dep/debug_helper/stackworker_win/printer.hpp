#ifndef MURADIN_DBG_HELPER_PRINTER_HPP__
#define MURADIN_DBG_HELPER_PRINTER_HPP__

#include <muradin/boost_dep/debug_helper/stackworker_win/module_info.hpp>
#include <muradin/boost_dep/debug_helper/stackworker_win/callstack_info.hpp>
#include <muradin/boost_dep/debug_helper/stackworker_win/system_helper.hpp>

#include <stdio.h>
#include <string.h>

namespace	muradin{namespace dbg{

	/// you will got 'prefix-TIMESTAMP-PROCESSID-THREADID.LOG'
	/// count_of_char >= 256 is good enough
	/// count_of_char means charactor count,NOT bytes!
	static TCHAR*		make_base_file_name(const TCHAR* prefix,TCHAR* out_buff,size_t count_of_char)
	{
		SystemInfo sys;
		_sntprintf_s(out_buff,count_of_char,count_of_char-1,
			_T("%s-%s-PID%d-TID%d.log"),
			prefix,
			sys.local_time_long(),
			::GetCurrentProcessId(),
			::GetCurrentThreadId());
		return out_buff;

	}
	class SimpleFile
	{
	private:
		enum{kOpenFlagLen=256,kMaxFileNameCount=2048};
		FILE*	fp_;
		TCHAR file_name_[kMaxFileNameCount];
		TCHAR open_flag_[kOpenFlagLen];
	public:

		SimpleFile(const TCHAR* file_name,const TCHAR* open_flag)
			:fp_(NULL)
		{
			_tcsncpy_s(file_name_,kMaxFileNameCount,file_name,kMaxFileNameCount-1);
			_tcsncpy_s(open_flag_,kOpenFlagLen,open_flag,kOpenFlagLen-1);
		}
		~SimpleFile()
		{
			
		}
		const TCHAR*	get_file_name()const
		{
			return file_name_;
		}
		FILE*	get_handle()
		{
			return fp_;
		}
		void	close()
		{
			if(NULL != fp_ ){
				::fclose(fp_);
				fp_=NULL;
			}
		}
		bool	open()
		{
			if(is_open())
				return true;
			int err_no=::_tfopen_s(&fp_,file_name_,open_flag_);
			if(0 != err_no){
				fp_=NULL;
				return false;
			}
			return true;
		}
		bool	is_open()
		{
			return (NULL != fp_ );
		}
	};

	/************************************************************************
	//  EXCEPTION_POINTERS Formator
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/
	

	class	ExceptionPointerFormator:boost::noncopyable
	{
	public:
		ExceptionPointerFormator(size_t	char_count=20480)
			:char_count_(char_count),buff_(char_count_)
		{
			//
		}
		~ExceptionPointerFormator()
		{
		}
		const TCHAR*	fmt(const struct _EXCEPTION_POINTERS* ep)
		{
			if(buff_.is_null()) return	_T("");
			buff_.zero_memory();
			auto_buff_c<TCHAR>	tmp_buf(2048);
			if(tmp_buf.is_null()) return _T("");

			if(NULL == ep) return _T("");

			_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("%s"),_T("*************************************** Exception pointer information **************************************\r\n"));
			_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			if(NULL != ep->ExceptionRecord){
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("Exception Code    : %s\r\n"),exception_code_str(ep->ExceptionRecord->ExceptionCode));
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("Exception Address : %p\r\n"),(LPVOID)(ep->ExceptionRecord->ExceptionAddress));
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
			}

			/*
			/// ep->ContextRecord not portable
			if(NULL != ep->ContextRecord){
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tEax    : 0x%08x\r\n"),ep->ContextRecord->Eax);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tEbp    : 0x%08x\r\n"),ep->ContextRecord->Ebp);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tEbx    : 0x%08x\r\n"),ep->ContextRecord->Ebx);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tEcx    : 0x%08x\r\n"),ep->ContextRecord->Ecx);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tEdi    : 0x%08x\r\n"),ep->ContextRecord->Edi);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tEdx    : 0x%08x\r\n"),ep->ContextRecord->Edx);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tEip    : 0x%08x\r\n"),ep->ContextRecord->Eip);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tEsi    : 0x%08x\r\n"),ep->ContextRecord->Esi);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tEflag  : 0x%08x\r\n"),ep->ContextRecord->Eflag);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tEFlags : 0x%08x\r\n"),ep->ContextRecord->EFlags);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			}
			*/

			return buff_.ptr();

		}
		//
	private:
		size_t				char_count_;
		auto_buff_c<TCHAR>	buff_;
	};
	/************************************************************************
	//  ModuleInfoFormator
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	class	ModuleInfoFormator:boost::noncopyable
	{
	public:
		ModuleInfoFormator(size_t	char_count=20480)
			:char_count_(char_count),buff_(char_count_)
		{
			//
		}
		~ModuleInfoFormator()
		{
		}
		const TCHAR*	fmt(size_t index,const ModuleInfo* info)
		{
			if(buff_.is_null()) return	_T("");
			buff_.zero_memory();
			auto_buff_c<TCHAR>	tmp_buf(2048);
			if(tmp_buf.is_null()) return _T("");

			if(index==0){
				SystemInfo sys;

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("******************************* ENV    information [%s] *******************************\r\n"),sys.local_time_long() );
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("OS  :%s\r\n"),sys.os_version());
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("Current User  :%s\r\n"),sys.current_user());
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("%s"),_T("Current Process Info:\r\n"));
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tProcess ID=%d\r\n"),::GetCurrentProcessId());
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());


				ProcessInfo	pi;
				PSAPI::PROCESS_MEMORY_COUNTERS counters={0};
				// igrone fail
				if(!pi.process_mem_counter(::GetCurrentProcess(),counters))
				{
					int err=::GetLastError();
				}

				static const unsigned int kBytesOfKB=1024;
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tCurrent Memory Usage  : %d KB,Max Memory Usage  :%d KB\r\n"),
					(unsigned int)(counters.WorkingSetSize/kBytesOfKB),(unsigned int)(counters.PeakWorkingSetSize/kBytesOfKB));
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tCurrent Pagefile Usage: %d KB,Max Pagefile Usage:%d KB\r\n"),
					(unsigned int)(counters.PagefileUsage/kBytesOfKB),(unsigned int)(counters.PeakPagefileUsage/kBytesOfKB));
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
				//////////////////////////////////////////////////////////////////////////
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("%s"),_T("******************************************** Module information ********************************************\r\n"));
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());


			}
			_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("[#%04d] %s(version:%s)\r\n"),(int)(index+1),info->base_file_name,info->file_version);
			_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("Create time      :%s\r\n"),info->time_create);
			_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("Last access time :%s\r\n"),info->time_last_access);
			_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("Last write time  :%s\r\n"),info->time_last_write);
			_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("path:%s\r\n"),info->full_file_name);
			_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			if(info->have_tl32_module_info){

				/*
				_tprintf_s(_T("ToolHelp32.info:ModuleID=%08d,ProcessID=%08d,GlblcntUsage=%08d,ProccntUsage=%08d,modBaseAddr=%p,modBaseSize=%08d\r\n"),
					info->module_info_from_tl32.th32ModuleID,
					info->module_info_from_tl32.th32ProcessID,
					info->module_info_from_tl32.GlblcntUsage,
					info->module_info_from_tl32.ProccntUsage,
					(const void*)(info->module_info_from_tl32.modBaseAddr),
					info->module_info_from_tl32.modBaseSize);
				*/
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("%s"),_T("ToolHelp32.info:\r\n"));
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tModuleID=%08d\r\n"),info->module_info_from_tl32.th32ModuleID);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tProcessID=%08d\r\n"),info->module_info_from_tl32.th32ProcessID);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tGlblcntUsage=%08d\r\n"),info->module_info_from_tl32.GlblcntUsage);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tProccntUsage=%08d\r\n"),info->module_info_from_tl32.ProccntUsage);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tmodBaseAddr=%p\r\n"),(const void*)(info->module_info_from_tl32.modBaseAddr));
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tmodBaseSize=%08d\r\n"),info->module_info_from_tl32.modBaseSize);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			}
			if(info->have_psapi_module_info){
				/*
				_tprintf_s(_T("PASPI.info: LoadAddress=%p,EntryPoint=%p,SizeOfImage=%08d\r\n"),
					(const void*)(info->module_info_from_psapi.lpBaseOfDll),
					(const void*)(info->module_info_from_psapi.EntryPoint),
					info->module_info_from_psapi.SizeOfImage
					);
				*/
				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("%s"),_T("PASPI.info:\r\n"));
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tLoadAddress=%p,EntryPoint=%p,SizeOfImage=%08d\r\n"),(const void*)(info->module_info_from_psapi.lpBaseOfDll));
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tEntryPoint=%p\r\n"),(const void*)(info->module_info_from_psapi.EntryPoint));
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tSizeOfImage=%08d\r\n"),info->module_info_from_psapi.SizeOfImage);
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			}

			_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("%s"),_T("------------------------------------------------------------------------------------------------------------\r\n"));
			_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
			return buff_.ptr();

		}
		//
	private:
		size_t				char_count_;
		auto_buff_c<TCHAR>	buff_;
	};
	

	/************************************************************************
	//  ModuleInfoPrinter
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	class ModuleInfoConsolPrinter
	{
	public:
		static	void	print_ln(const TCHAR* msg)
		{
			_tprintf_s(_T("%s\r\n"),msg);
		}
		static	void	print(const TCHAR* msg)
		{
			_tprintf_s(_T("%s"),msg);
		}
		static	void	print(size_t index,const ModuleInfo* info,bool* skip)
		{
			ModuleInfoFormator fm;
			_tprintf_s(fm.fmt(index,info));
		}
	};

	class ModuleInfoFilePrinter
	{
	private:
		SimpleFile	file_;
	public:

		ModuleInfoFilePrinter(const TCHAR* file_name,const TCHAR* open_flag)
			:file_(file_name,open_flag)
		{
			
		}
		~ModuleInfoFilePrinter()
		{
			close();
		}

		bool	open(){
			return file_.open();
		}
		void	close()
		{
			file_.close();
		}
		void	print_ln(const TCHAR* msg)
		{
			if(!file_.is_open()){
				_tprintf_s(_T("%s\r\n"),_T("can not open file"));
				return;
			}
			_ftprintf_s(file_.get_handle(),_T("%s\r\n"),msg);
		}
		void	print(const TCHAR* msg)
		{
			if(!file_.is_open()){
				_tprintf_s(_T("%s\r\n"),_T("can not open file"));
				return;
			}
			_ftprintf_s(file_.get_handle(),_T("%s"),msg);
		}
		void	print(size_t index,const ModuleInfo* info,bool* skip)
		{

			if(!file_.is_open()){
				_tprintf_s(_T("%s\r\n"),_T("can not open file"));
				*skip=true;
				return;
			}

			ModuleInfoFormator fm;
			print(fm.fmt(index,info));
		}
	};
	/************************************************************************
	//  CallstackFormator
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	class	CallstackFormator:boost::noncopyable
	{
	public:
		CallstackFormator(size_t	char_count=20480)
			:char_count_(char_count),buff_(char_count_)
		{
			//
		}
		~CallstackFormator()
		{
		}
		const TCHAR*	fmt(size_t index,const CallstackEntry* info)
		{
			if(buff_.is_null()) return	_T("");
			buff_.zero_memory();
			auto_buff_c<TCHAR>	tmp_buf(2048);
			if(tmp_buf.is_null()) return _T("");


			if(index==0){
				SystemInfo sys;

				_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("***************************** CallStack information [%s] ******************************\r\n"),sys.local_time_long());
				_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());
			}
			_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("[#%04d] %s\r\n"),
				(int)(index+1),
				((info->undFuncName[0] == 0 )? (_T("NULL")):info->undFuncName)
				);
			_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tsrc=%s;line=%d\r\n"),
				((info->lineFileName[0] == 0) ? (_T("NULL")):info->lineFileName),
				info->lineNumber 
				);
			_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("\tSymbolType=%s;Image=%s\r\n"),
				((info->symTypeString[0] == 0) ? (_T("Unknow-Symbol-Type")):info->symTypeString),
				((info->loadedImageName[0] == 0) ? (_T("NA")):info->loadedImageName)
				);
			_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());


			_sntprintf_s(tmp_buf.ptr(),tmp_buf.elem_count(),tmp_buf.elem_count()-1,_T("------------------------------------------------------------------------------------------------------------\r\n"));
			_tcscat_s(buff_.ptr(),buff_.elem_count(),tmp_buf.ptr());

			return buff_.ptr();

		}
	private:
		size_t				char_count_;
		auto_buff_c<TCHAR>	buff_;
	};
	
	/************************************************************************
	//  CallstackPrinter
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	class CallstackConsolPrinter
	{
	public:

		static	void	print_ln(const TCHAR* msg)
		{
			_tprintf_s(_T("%s\r\n"),msg);
		}
		static	void	print(const TCHAR* msg)
		{
			_tprintf_s(_T("%s"),msg);
		}
		static	void	print(size_t index,const CallstackEntry* info,bool* skip)
		{

			CallstackFormator fm;
			_tprintf_s(fm.fmt(index,info));
		}
	};

	class CallstackFilePrinter
	{
	private:
		SimpleFile	file_;
	public:

		CallstackFilePrinter(const TCHAR* file_name,const TCHAR* open_flag)
			:file_(file_name,open_flag)
		{
		}
		~CallstackFilePrinter()
		{
			close();
		}

		bool	open(){
			return file_.open();
		}
		void	close()
		{
			file_.close();
		}
		void	print_ln(const TCHAR* msg)
		{
			if(!file_.is_open()){
				_tprintf_s(_T("%s\r\n"),_T("can not open file"));
				return;
			}
			_ftprintf_s(file_.get_handle(),_T("%s\r\n"),msg);
		}
		void	print(const TCHAR* msg)
		{
			if(!file_.is_open()){
				_tprintf_s(_T("%s\r\n"),_T("can not open file"));
				return;
			}
			_ftprintf_s(file_.get_handle(),_T("%s"),msg);
		}
		void	print(size_t index,const CallstackEntry* info,bool* skip)
		{

			if(!file_.is_open()){
				_tprintf_s(_T("%s\r\n"),_T("can not open file"));
				*skip=true;
				return;
			}

			CallstackFormator fm;
			print(fm.fmt(index,info));
		}

	};
}}
#endif