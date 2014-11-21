#ifndef AIRSERVER_C_BUFFER_HPP__
#define AIRSERVER_C_BUFFER_HPP__

#include <memory.h>
#include <boost/assert.hpp>

namespace air{namespace common {
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
	const unsigned char* begin()const{
		return (unsigned char*)ptr();
	}
	const unsigned char* end()const{
		return (begin() + bytes());
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

}}
#endif // AIRSERVER_C_BUFFER_HPP__