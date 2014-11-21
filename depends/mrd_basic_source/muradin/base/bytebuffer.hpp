#ifndef MURADIN_BASE_BYTEBUFFER_HPP__
#define MURADIN_BASE_BYTEBUFFER_HPP__
#include <assert.h>

#include <vector>
#include <string>

#include <algorithm>


namespace muradin
{
namespace base{
	
/** 
 * @class    bytebuffer
 *
 * @brief    A buffer like netty.ByteBuf.
 *
 * datastructure:
 *
 *     +---------------+-------------------+------------------+------------------+
 *     | reserve bytes | discardable bytes |  readable bytes  |  writable bytes  |
 *     |               |                   |     (CONTENT)    |                  |
 *     +---------------+-------------------+------------------+------------------+
 *     |               |                   |                  |                  |
 *      0         reserveIndex  <=      readerIndex   <=   writerIndex    <=    capacity
 *
 *  
 *
 * See netty/buffer/ByteBuf.java for more information. 
 *
 */
class bytebuffer
{
public:
	
	bytebuffer( size_t size=2048)
		:m_contianer(size+m_resver_index),
		m_rd_index(m_resver_index),
		m_wt_index(m_resver_index)
	{
	}

	bytebuffer(const void* data, size_t bytes)
		:m_contianer(bytes+m_resver_index),
		m_rd_index(m_resver_index),
		m_wt_index(m_resver_index)
	{
		append(data,bytes);
	}

	/// 是否为空
	bool	empty()const{return m_rd_index == m_wt_index;};

	/// 最大容量(含保留区域)
	size_t	storage()const{return m_contianer.size();};

	/// 最大容量(不含保留区域)
	size_t	space()const{return m_contianer.size()- m_resver_index;};

	/// 保留区域的大小
	static size_t	resver_size() {return m_resver_index;};



	/// 放入bytebuffer
	/// 返回可读数据量
	size_t		append(const bytebuffer& another)
	{
		return append(another.rd_ptr(),another.readable_bytes());
	}

	/// 放入任意数据
	/// 返回可读数据量
	size_t		append(const void* data_ptr,size_t size)
	{
		//assert(writeable_bytes()>= size );
		ensure_write_space(size);
		const char* ptr=static_cast<const char*>(data_ptr);
		std::copy(ptr, ptr+size, wt_ptr());
		has_written(size);
		return readable_bytes();
	}

	/// 放入字符串
	/// 返回可读数据量
	size_t		append_str(const char* sz_str)
	{
		return append(sz_str,strlen(sz_str));
	}

	/// 手动增加可读数据量大小
	/// 通过 wt_ptr() 手动进行memcopy 后需要调用本函数
	/// 返回可读数据量
	size_t		has_written(size_t bytes)
	{
		assert(storage()>= m_wt_index+bytes);
		m_wt_index+=bytes;
		return readable_bytes();
	}

	/// 确保可写数据空间,可能会引起capacity 增大
	void	ensure_write_space(size_t bytes)
	{
		make_space(bytes);
	}

	/// 丢弃一些未读取的数据
	/// 返回剩余的可读数据量
	size_t	discard(size_t bytes)
	{
		assert(readable_bytes() >= bytes);
		if(readable_bytes() <= bytes ) {
			discard_all();
		}else{
			m_rd_index+=bytes;
		}
		return readable_bytes();
	}

	/// 回退写指针索引
	/// 返回剩余的可读数据量
	size_t	rollback_write(size_t bytes)
	{
		assert(readable_bytes() >= bytes);
		if(readable_bytes() <= bytes ) {
			discard_all();
		}else{
			m_wt_index-= bytes;
		}
		return readable_bytes();
	}

	/// 丢弃所有未读取的数据
	void	discard_all()
	{
		m_rd_index=m_resver_index;
		m_wt_index=m_resver_index;
	}

	/// 设置大小,可能会引起内存的重新分配,丢失数据
	void	resize(size_t new_capacity)
	{
		m_contianer.resize(new_capacity+m_resver_index);
		//m_capacity=m_contianer.capacity();
	}

	/// 可读的数据量
	size_t	readable_bytes()const
	{
		assert(m_rd_index <= m_wt_index);
		return m_wt_index - m_rd_index;
	}

	/// 可写的数据量
	size_t	writeable_bytes()const
	{
		assert(m_wt_index <= m_contianer.capacity());
		return m_contianer.size() - m_wt_index;
	}

	/// 可丢弃(已经被读取)的数据量
	size_t	discardable_bytes()const
	{
		assert(m_rd_index >= m_resver_index);
		return m_rd_index - m_resver_index;
	}

	/// 临时读指针
	const char* rd_ptr()const
	{
		return begin()+m_rd_index;
	}

	/// 临时写指针
	const char* wt_ptr()const
	{
		return begin()+m_wt_index;
	}

	/// 临时写指针
	char* wt_ptr()
	{
		return begin()+m_wt_index;
	}


	/// 缩减容量,resver_bytes 表示希望保留的剩余空间(字节数)
	/// 执行后,容量为m_resver_index + readable_bytes() + resver_bytes
	void	shrink(size_t resver_bytes)
	{
		size_t readable = readable_bytes();
		container_type buf(m_resver_index +readable+resver_bytes);
		std::copy(rd_ptr(), rd_ptr()+readable, buf.begin()+ m_resver_index);
		m_contianer.swap(buf);
		m_rd_index = m_resver_index;
		m_wt_index = m_rd_index + readable;
		//m_capacity = buf.capacity();
	}

	/// 与另一个对象进行交换
	void	swap(bytebuffer& another)
	{
		m_contianer.swap(another.m_contianer);
		std::swap(m_wt_index, another.m_wt_index);
		std::swap(m_rd_index, another.m_rd_index);
		//std::swap(m_byteorder, another.m_byteorder);
	}


	/// 将可读区域当成string取出
	std::string	retrieve_string(){std::string ret=as_string();discard_all();return ret;} ;

	/// 在可读区域中查找c第一次出现的索引.
	/// 如果存在,返回非空指针
	/// 如果不存在,返回NULL
	const char*		find_char(char c)const
	{
		const char* ptr = std::find(rd_ptr(),wt_ptr(),c);
		return   (ptr == wt_ptr()) ? NULL:ptr;
	}

	size_t			char_pos(char c)const
	{
		const char* ptr = std::find(rd_ptr(),wt_ptr(),c);
		if(ptr == wt_ptr()) return 0;
		return  ptr - rd_ptr() +1;
	}

	/// 在区域[search_start,search_end)中搜索 序列[start,end)第一次出现的位置
	/// 如果search_start为NULL,等同于rd_ptr()
	/// 如果search_end为NULL,等同于wt_ptr()
	/// 如果存在,返回非空指针
	/// 如果不存在,返回NULL
	/// 条件:
	/// - end > start
	/// - search_end > search_start
	/// - search_start >= rd_ptr()
	/// - search_end <= wt_ptr()
	const char*		search(const char* start,const char* end,const char* search_start=NULL,const char* search_end=NULL)
	{
		if(search_start==NULL) search_start=rd_ptr();
		if(search_end==NULL)search_end=wt_ptr();

		assert(rd_ptr() >= search_start );
		assert(wt_ptr() >= search_end );
		assert(end > start );

		if(search_end <= search_start )
			return NULL;
		const char* ptr = std::search(search_start,search_end , start, end);
		return   (ptr == search_end) ? NULL:ptr;
	}

	/// 搜索字符串
	/// 等同于 search(str,str+strlen(str),search_start,search_end);
	const char*		search_str(const char* str,const char* search_start=NULL,const char* search_end=NULL)
	{
		return search(str,str+strlen(str),search_start,search_end);
	}

	/// 将可读区域当成string返回
	/// 等同于as_string(0,readable_bytes());
	std::string	as_string() const
	{
		return as_string(0,readable_bytes());
	}

	/// 将可读区域[start_pos,start_pos+len]当成string返回
	std::string	as_string(size_t start_pos,size_t len) const
	{
		assert(readable_bytes() >= len + start_pos );
		return std::string(rd_ptr()+start_pos,len);
	}

	template <class T>
	T	peek_integer()const{
		return *((T*)(rd_ptr()) );
	};

	template <class T>
	T	retrieve_integer()const{
		size_t length=sizeof (T);
		assert(readable_bytes() >= length);
		T val= peek_integer<T>();
		discard(length);
		return val;
	};

	template <class T>
	size_t	append_integer(T int_val)const{
		return append(&int_val,sizeof(int_val));
	};

protected:

	char*	begin(){return &*m_contianer.begin();};
	const char*	begin()const{return &*m_contianer.begin();};

	/// 获取bytes 字节数的的可用空间
	/// 如果当前可利用的空间大于等于bytes,则挪动数据,而不重新申请内存
	/// 如果当前可利用的空间小于bytes,则重新申请内存
	void	make_space(size_t bytes)
	{
		if(discardable_bytes() + writeable_bytes() < bytes){
			m_contianer.resize(bytes + m_wt_index);
			//m_capacity=m_contianer.capacity();
			/// move readable data ?
		}else{
			size_t readable = readable_bytes();
			std::copy(begin()+m_rd_index,
				begin()+m_wt_index,
				begin()+m_resver_index);
			m_rd_index = m_resver_index;
			m_wt_index = m_rd_index + readable;
			assert(readable == readable_bytes());
		}
	}

private:
	typedef	std::vector<char>	container_type;
	static const size_t		m_resver_index=8;
	container_type	m_contianer;
	size_t		m_rd_index;
	size_t		m_wt_index;
};

/// 用序列[start,end)将bytebuffer分割成多个对象
/// 假设src 现在的可读内容为
/// ['1',2'','@','#','a','b','c','@','#','@','#','a','a']
/// 本函数执行后将得到3个分片,其内容分别是:
/// 
/// ['1',2'','@','#']  ['a','b','c','@','#']  ['@','#'];
/// src 剩下的可读内容为
/// ['a','a']
static std::vector<bytebuffer>		bytebuffer_make_pieces(bytebuffer& src,const char* start,const char* end)
{
	std::vector<bytebuffer>	pieces;
	size_t	range=end-start;

	if(range > 0 ){
		const	char* found=NULL;
		while( NULL != (found=src.search(start,end) ) ){
			size_t	length=found-src.rd_ptr() + range;
			pieces.push_back(bytebuffer(src.rd_ptr(),length));
			src.discard(length);
		}
	}
	return pieces;
}

}
}
#endif // MURADIN_BASE_BYTEBUFFER_HPP__
