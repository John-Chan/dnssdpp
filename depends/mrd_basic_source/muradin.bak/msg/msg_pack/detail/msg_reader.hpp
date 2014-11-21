
/**
 * 
 * @file       msg_reader.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-2-8
 *  
 * @brief      ½âÎöÆ÷
 * 
 * @e Copyright (c) <year> <copyright holders>
 */

#ifndef MURADIN_MSGPACK_READER_HPP__
#define MURADIN_MSGPACK_READER_HPP__


#include <muradin/base/bytebuffer.hpp>
#include <muradin/msg/msg_pack/detail/msg_buff.hpp>
#include <muradin/msg/msg_pack/detail/check_value.hpp>
#include <iostream>

namespace muradin{
	namespace msg_pack
	{
		template<typename pack_traits>
		class reader
		{
		public:
			reader()
			{
					reset();
			}
			size_t	count()const
			{
				return good_packs_.size();
			}
			void	reset()
			{
				bad_count_=0;
				good_packs_.clear();
				data_.discard_all();
			}
			bool	pop_as_null()
			{
				if(good_packs_.size() >0 ){
					good_packs_.pop_front();
					return true;
				}else{
					return false;
				}
			}
			bool	pop_next(msg_buff& saver)
			{
				if(good_packs_.size() >0 ){
					saver.swap(good_packs_.front());
					good_packs_.pop_front();
					return true;
				}else{
					return false;
				}
			}
			bool	peek_first(msg_buff& saver)
			{
				if(good_packs_.size() >0 ){
					saver = good_packs_.front();
					return true;
				}else{
					return false;
				}
			}
			bool	peek_last(msg_buff& saver)
			{
				if(good_packs_.size() >0 ){
					saver = good_packs_.back();
					return true;
				}else{
					return false;
				}
			}
			reader&		append_and_prase(const char* ptr,size_t bytes,unsigned int max_queued_count=0)
			{
				data_.append(ptr,bytes);
				prase(max_queued_count);
				return *this;
			}
			size_t		un_prased()const
			{
				return data_.readable_bytes();
			}
			// for debug only!
			std::string		print_data()const
			{
				return data_.as_string();
			}
		private:
			void		check_max_save_pack()
			{
				if(good_packs_.size() +1 >= good_packs_.max_size()){

					std::cout<<__FUNCTION__ << " => reach to container max_size: "<< good_packs_.max_size()<< ",current size :" <<good_packs_.size() <<std::endl;
				}
			}
			void		prase(unsigned int max_queued_count=0)
			{
				std::string what_wrong;
				while((good_packs_.size()<= max_queued_count || max_queued_count==0 ) && data_.readable_bytes()>0){
					check_max_save_pack();
					if(!prase_one(what_wrong)){
						std::cout<<"prase fail : "<< what_wrong <<std::endl;
						break;
					}
				}
			}
			bool		prase_one(std::string& what_wrong)
			{
				if(got_min_pack_size(data_.readable_bytes()) ){
					msg_piece not_sure_good(data_.rd_ptr(),data_.readable_bytes());
					size_t save_or_discard=0;
					if(good_pack<pack_traits>(not_sure_good,save_or_discard,save_or_discard,what_wrong)){
						good_packs_.push_back(msg_buff(data_.rd_ptr(),save_or_discard ));
						data_.discard(save_or_discard);
						return true;
					}else{
						if(save_or_discard>0){
							data_.discard(save_or_discard);
							++bad_count_;
						}
						return false;
					}
				}
				what_wrong="need more data(min_pack_size)";
				return false;
			}
		private:
			muradin::base::bytebuffer	data_;
			MsgBuffList					good_packs_;
			size_t						bad_count_;
		};

	}
}
#endif//MURADIN_MSGPACK_READER_HPP__