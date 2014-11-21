#ifndef MURADIN_BASE_BUFF_HOLDER_HPP_
#define MURADIN_BASE_BUFF_HOLDER_HPP_

#include <utility>

namespace muradin
{
	namespace base{
		
		template<class T>
		class BuffHolder
		{
		public:
			typedef T			ObjType;
		public:
			/// constructor
			BuffHolder( ObjType* ptr,size_t count_of_elem)
				:ptr_(ptr),count_(count_of_elem)
			{
				//
			}
			/// copy constructor
			BuffHolder(const BuffHolder& rh)
			{
				ptr_=rh.ptr_;
				count_=rh.count_;
			}

			/// assignment constructor
			BuffHolder& operator=(const BuffHolder& rh)
			{
				if(this != &rh){
					BuffHolder tmp(rh);
					this->swap(tmp);
				}
				return *this;
			}
			~BuffHolder()
			{
				//nothing to do
			}

			void	swap(BuffHolder& rh)
			{
				std::swap(ptr_,rh.ptr_);
				std::swap(count_,rh.count_);
			}
			bool operator==(const BuffHolder& rh)const
			{
				return ptr_==rh.ptr_ && count_==rh.count_;
			}
			bool operator!=(const BuffHolder& rh)const
			{
				return !(*this == rh);
			}
			bool	not_null()const
			{
				return ptr_!=NULL;
			}
			size_t count()const
			{
				return count_;
			}
			size_t bytes()const
			{
				return sizeof(ObjType)*count_;
			}
			ObjType*	ptr()const
			{
				return ptr_;
			}
		private:
			ObjType* ptr_;
			size_t count_;
		};

		typedef	BuffHolder<const char>		ConstBuff;
		typedef	BuffHolder<char>			NonConstBuff;
	}
}
#endif//MURADIN_BASE_BUFF_HOLDER_HPP_