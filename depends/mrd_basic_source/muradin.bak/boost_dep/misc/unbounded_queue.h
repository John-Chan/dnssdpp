#ifndef MURADIN_BASE_UNBOUNDED_QUEUE_H__
#define MURADIN_BASE_UNBOUNDED_QUEUE_H__

#include <boost/circular_buffer.hpp>
#include <boost/function.hpp>

#include <vector>

#include <muradin/config/integer_define.h>

namespace muradin{
namespace base
{
	// �ɶ�����
	// first : �ɶ����ݵ�����
	// second: �����ݵ���ʼλ��
	// @note : ���÷�������Ҫ����ʽ���� [second,end) ��[begin,second) ������
	typedef std::pair<uint64_t,uint64_t>	readable_counter;

	static uint64_t cb_fix_outside_index(uint64_t outside_read_index,uint64_t queue_read_index,uint64_t queue_size)
	{
		if(queue_size==0 ) return 0;
		if(outside_read_index > queue_read_index) return queue_read_index;
		uint64_t distance=(queue_read_index-outside_read_index);
		if(distance > queue_size){
			return queue_read_index-queue_size;
		}else{
			return outside_read_index;
		}
	}
	static readable_counter	cb_readable(uint64_t outside_read_index,uint64_t queue_read_index,uint64_t queue_size) 
	{
		if(outside_read_index >= queue_read_index || queue_size==0 || queue_read_index==0) return readable_counter(0,queue_read_index);
		outside_read_index=cb_fix_outside_index(outside_read_index, queue_read_index, queue_size);
		return readable_counter(queue_read_index-outside_read_index,outside_read_index%queue_size);
		
	}

	/*template <class T>
	static uint64_t	cb_copy_data(const boost::circular_buffer<T>& container,uint64_t read_index,uint64_t count_get,std::vector<T>& saver)
	{
		uint64_t got=0;
		size_t container_size=container.size();
		if(container_size<= 0 || count_get<= 0) return got;

		size_t back_copy_index=container_size;
		if (read_index > container_size){
			back_copy_index=read_index%container_size;
			for (;back_copy_index< container_size;++back_copy_index){
				saver.push_back( container[back_copy_index]);
				if(++got >= count_get) break;
			}
		}

		if(got < count_get){
			for (uint64_t i=read_index;i<back_copy_index;++i){
				saver.push_back( container[(size_t)i]);
				if(++got >= count_get) break;
			}
		}
		return got;
	}*/

	template <class T>
	static uint64_t	cb_copy_data(const boost::circular_buffer<T>& container,uint64_t start,uint64_t end,std::vector<T>& saver)
	{
		uint64_t pos=start;
		while (pos!=end ){
			saver.push_back( container[(size_t)pos]);
			++pos;
		}
		return end-start;
	}

	template <class T>
	class unbounded_queue
	{
	public:
		typedef boost::circular_buffer<T>	buff_container_type;
		typedef std::vector<T>				value_vec;
		typedef boost::function< void(const T&) >	overwrite_callback;
	public:

		unbounded_queue(size_t capacity)
			:m_read_index(0),
			m_push_count(0),
			m_try_get_count(0),
			m_get_success_count(0),
			m_container(capacity<=0? 32:capacity),
			m_overwrite_cb(NULL)
		{
		}
		virtual ~unbounded_queue(void)
		{
		}

		/// ����Ԫ�صĸ�д�ص�����
		/// ��д���µ�Ԫ�ظ����˾�Ԫ��ʱ,�ص�����������
		void	set_overwrite_cb(const overwrite_callback& func)
		{
			m_overwrite_cb=func;
		}
		const overwrite_callback& get_overwrite_cb()const
		{
			return m_overwrite_cb;
		}

		void 	push_swap( T& item)
		{
			T new_val(item);
			if(m_container.reserve() == 0 ){
				m_container.front().swap(item);
				if(m_overwrite_cb)
					m_overwrite_cb(item);
			}

			m_container.push_back(new_val);
			m_push_count++;
			m_read_index++;
		}

		/// ����һ����Ԫ��
		void	push_back(const T& item)
		{
			if(m_container.reserve() == 0 && m_overwrite_cb){
				m_overwrite_cb(m_container.front());
			}
			m_container.push_back(item);
			m_push_count++;
			m_read_index++;
		}

		// ��������,����ȡ������������
		uint64_t	copy_data(uint64_t read_index,uint64_t count_get,value_vec& saver) const
		{
			uint64_t got=0;
			uint64_t container_size=m_container.size();

			m_try_get_count+=count_get;

			readable_counter readable=cb_readable(read_index,m_read_index,container_size);
			count_get=count_get>readable.first?readable.first:count_get;
			if(count_get> 0){
				uint64_t back_pos=readable.second + count_get;
				if(back_pos > container_size ) back_pos=container_size;
				got+=cb_copy_data(m_container,readable.second,back_pos,saver);
				if(got<count_get){
					got+=cb_copy_data(m_container,0,count_get-got,saver);
				}
				m_get_success_count+=got;
			}

			return got;
		}

		// ȡ���ڲ���������
		uint64_t	read_index() const
		{
			return m_read_index;
		}

		uint64_t	fix_read_index(uint64_t read_index) const
		{
			return cb_fix_outside_index(read_index, m_read_index, m_container.size());
		}

		uint64_t	readable_count(uint64_t read_index) const
		{
			if(m_container.size() <= 0) return 0;
			uint64_t got=0;
			if (m_read_index>read_index){
				got=m_read_index - read_index;
			}
			if(got>m_container.size()) return m_container.size();
			
			return got;

		}
		uint64_t	readable_count() const
		{
			if(m_container.size() <= 0) return 0;
			return m_read_index%m_container.size() ;
		}
		bool		full()
		{
			return m_container.full();
		}
		bool		empty()
		{
			return m_container.empty();
		}
		void	clear()
		{
			m_read_index=0;
			m_container.clear();
		}

		std::size_t size()
		{
			return m_container.size();
		}

		const buff_container_type& get_container()const
		{
			return m_container;
		}
		buff_container_type& get_container()
		{
			return m_container;
		}
	private:
		size_t				m_read_index;			
		uint64_t			m_push_count;					/* for debug */
		mutable uint64_t			m_try_get_count;		/* for debug */
		mutable uint64_t			m_get_success_count;	/* for debug */
		buff_container_type	m_container;
		overwrite_callback	m_overwrite_cb;
	};


	
	/** 
	 * @class    unbouned_ptr_queue
	 *
	 * @brief    �ޱ߽��ָ������(���λ�����)
	 *
	 * @note     
	 *     - ������������ָ�뷽ʽ��������
	 *     - �������Կ������ݷ�ʽȡ����
	 */

	template<class T>
	class unbouned_ptr_queue
	{
	public:
		typedef	boost::shared_ptr<T>				ptr_type;
		typedef std::vector<T>						value_vec;
		typedef boost::circular_buffer<ptr_type>	buff_container_type;
		typedef boost::function< void(const T&) >	overwrite_callback;

	public:
		unbouned_ptr_queue(size_t capacity)
			:m_read_index(0),
			m_push_count(0),
			m_try_get_count(0),
			m_get_success_count(0),
			m_container(capacity<=0? 32:capacity),
			m_overwrite_cb(NULL)
		{
			//
		}
		virtual ~unbouned_ptr_queue()
		{
			//
		}

		/// ����Ԫ�صĸ�д�ص�����
		/// ��д���µ�Ԫ�ظ����˾�Ԫ��ʱ,�ص�����������
		void	set_overwrite_cb(const overwrite_callback& func)
		{
			m_overwrite_cb=func;
		}
		const overwrite_callback& get_overwrite_cb()const
		{
			return m_overwrite_cb;
		}

		void	clear()
		{
			m_container.clear();
			m_read_index=0;
		}
		size_t	size()
		{
			return m_container.size();
		}



		// ��������,����ȡ������������
		uint64_t	copy_data(uint64_t read_index,uint64_t count_get,value_vec& saver)const
		{
			uint64_t got=0;
			uint64_t container_size=m_container.size();

			m_try_get_count+=count_get;

			readable_counter readable=cb_readable(read_index,m_read_index,container_size);
			count_get=count_get>readable.first?readable.first:count_get;
			if(count_get> 0){
				std::vector<ptr_type> val_list;
				uint64_t back_pos=readable.second + count_get;
				if(back_pos > container_size ) back_pos=container_size;
				got+=cb_copy_data(m_container,readable.second,back_pos,val_list);
				if(got<count_get){
					got+=cb_copy_data(m_container,0,count_get-got,val_list);
				}
				for (size_t i=0;i != val_list.size();++i){
					saver.push_back( *val_list[i]);
				}
				m_get_success_count+=got;
			}

			return got;
		}
		
		
		// ȡ���ڲ���������
		uint64_t	read_index() const
		{
			return m_read_index;
		}


		uint64_t	fix_read_index(uint64_t read_index) const
		{
			return cb_fix_outside_index(read_index, m_read_index, m_container.size());
		}

		/// �����read_index��˵,�ж���Ԫ�ؿɶ�
		uint64_t	readable_count(uint64_t read_index) const
		{
			if(m_container.size() <= 0) return 0;
			//return m_read_index>read_index?(m_read_index - read_index)%m_container.size() : 0;
			uint64_t got=0;
			if (m_read_index>read_index){
				got=m_read_index - read_index;
			}
			if(got>m_container.size()) return m_container.size();

			return got;
		}
		/// �ܹ��ж���Ԫ�ؿɶ�
		uint64_t	readable_count() const
		{
			return m_container.size() ;
		}

		bool		full()const
		{
			return m_container.full();
		}
		bool		empty()const
		{
			return m_container.empty();
		}
		/// ����һ����Ԫ��,�����ر����ǵľ�Ԫ��
		void 	push_swap( ptr_type& item)
		{
			ptr_type new_val=item;

			if(m_container.reserve() == 0 ){
				m_container.front().swap(item);
				if(m_overwrite_cb)
					m_overwrite_cb(*item);
			}else{
				item.reset();
			}
			m_container.push_back(new_val);
			m_push_count++;
			m_read_index++;
		}
		void 	push_back( ptr_type& item)
		{
			if(m_container.reserve() == 0 && m_overwrite_cb){
				m_overwrite_cb(*m_container.front());
			}
			m_container.push_back(item);
			m_push_count++;
			m_read_index++;
		}
		const buff_container_type& get_container()const
		{
			return m_container;
		}
		buff_container_type& get_container()
		{
			return m_container;
		}
	protected:
	private:
		size_t				m_read_index;
		uint64_t			m_push_count;					/* for debug */			
		mutable uint64_t			m_try_get_count;		/* for debug */
		mutable uint64_t			m_get_success_count;	/* for debug */
		buff_container_type	m_container;
		overwrite_callback	m_overwrite_cb;
	};

}
}

#endif	
