#ifndef DLLOADER_HPP
#define DLLOADER_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define  WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <string.h>
#include <tchar.h>

#include <boost/utility.hpp>

namespace air{namespace common {

class DllLoader:boost::noncopyable
{
public:
    DllLoader(const TCHAR* dll_name)
        :dll_(NULL)
    {
        ::_tcsncpy(dll_name_,dll_name,kMaxPathLen-1);
    }
    ~DllLoader()
    {
        unload();
    }
    const TCHAR*	dll_name()const
    {
        return dll_name_;
    }
    HMODULE		get_handle()
    {
        return dll_;
    }
    bool		loaded()const
    {
        return NULL != dll_;
    }
    bool		load()
    {
        if(!loaded()){
            // will return NULL when fail
            dll_=::LoadLibrary( dll_name_);
        }
        return loaded();
    }
    void		unload()
    {
        if(loaded()){
            ::FreeLibrary(dll_);
            dll_=NULL;
        }
    }
    template	<class FuncType>
    FuncType			load_dll_func(const char* func_name)
    {
        return (FuncType)::GetProcAddress(dll_, func_name);
    }

private:
    enum		{kMaxPathLen=512};	// calced by character,not bytes
    HMODULE		dll_;
    TCHAR		dll_name_[kMaxPathLen];

};
}}
#endif // DLLOADER_HPP
