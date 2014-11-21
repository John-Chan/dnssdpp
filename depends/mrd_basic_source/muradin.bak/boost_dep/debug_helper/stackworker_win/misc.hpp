#ifndef MURADIN_DBG_HELPER_MISC_HPP__
#define MURADIN_DBG_HELPER_MISC_HPP__

#include <boost/noncopyable.hpp>
#include <boost/assert.hpp>


#include <Windows.h>


namespace	muradin{namespace dbg{
template<class T>
static	void	zero_mem(T* ptr)
{
	::memset(ptr,0x0,sizeof(T));
}


static	int retrieve_sys_error(int& save)
{
	save=::GetLastError();
	::SetLastError(0);
	return save;
}

static int peek_sys_error()
{
	return ::GetLastError();
}

class c_alloctor
{
public:
	static void*	alloc(size_t bytes)
	{
		return ::malloc(bytes);
	}
	static void		dealloc(void* ptr)
	{
		if(NULL != ptr)::free(ptr);
	}
};
template <class T,class alloctor=c_alloctor>
class auto_buff_c:boost::noncopyable
{
public:
	typedef	auto_buff_c<T>	ThisType;
public:
	auto_buff_c(size_t elem_count)
		:elem_count_(elem_count),ptr_(NULL)
	{
		alloc(elem_count_);
	}
	~auto_buff_c()
	{
		dealloc();
	}
	bool	is_null()const
	{
		return NULL == ptr_;
	}
	T*		ptr()
	{
		return ptr_;
	}

	void			memory_set(char	fill_as)
	{
		if(!is_null())
			::memset(ptr_,fill_as,bytes());
	}
	void			zero_memory()
	{
		memory_set(0);
	}
	const T*		ptr()const 
	{
		return ptr_;
	}
	size_t			elem_count()const
	{
		return elem_count_;
	}
	size_t			bytes()const
	{
		return sizeof(T) * elem_count_;
	}
	void	copy_from(const ThisType& another)
	{
		dealloc();
		alloc(another.elem_count());
		if(!is_null()){
			::memcpy(ptr_,another.ptr_, another.bytes());
		}
	}
private:
	void	alloc(size_t elem_count)
	{
		BOOST_ASSERT(is_null());
		if(elem_count >0){
			ptr_=(T*) alloctor::alloc(sizeof(T) * elem_count);
		}
		if(ptr_ != NULL)
			elem_count_=elem_count;
	}
	void	dealloc()
	{
		alloctor::dealloc(ptr_);
		ptr_=NULL;
		elem_count_=0;
	}
private:
	size_t elem_count_;
	T*	ptr_;
};



class	Win32Handle:boost::noncopyable
{
public:
	Win32Handle(HANDLE	init_val=NULL)
		:h_(init_val)
	{
		//
	}
	~Win32Handle()
	{
		close();
	}
	void	reset(HANDLE h)
	{
		h_=h;
	}
	HANDLE	get()
	{
		return h_;
	}
	void	close()
	{
		::CloseHandle(h_);
	}

	bool	is_null_value()const
	{
		return NULL==h_;
	}
	bool	is_invalid_handle_value()const
	{
		return INVALID_HANDLE_VALUE==h_;
	}
	/*static	Win32Handle	null_handle()
	{
		return Win32Handle(NULL);
	}
	static	Win32Handle	invalied_handle()
	{
		return Win32Handle(INVALID_HANDLE_VALUE);
	}*/
private:
	HANDLE	h_;
};

/// The number of buf_bytes includes the byte for the terminating null character
static	bool	unicode_to_ansi(const wchar_t* w_str,char* out_ansi_buff,size_t buf_bytes)
{
	int need_bytes=::WideCharToMultiByte(CP_ACP, 0, w_str, -1, NULL, 0, NULL, NULL);
	if(0== need_bytes || ((int)buf_bytes)< need_bytes)
		return false;

	if(0 == WideCharToMultiByte(CP_ACP, 0, w_str, -1, out_ansi_buff, (int)buf_bytes, NULL, NULL)){
		return false;
	}
	return true;
}

//
}}
#endif