
/**
 * 
 * @file       msg.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-2-8
 *  
 * @brief      上层消息对象,方便应用层处理
 * @remark     <some issue>  
 * @note    
 * @e<somthing important>
 * 
 * @e Copyright (c) <year> <copyright holders>
 */
#ifndef MURADIN_MSGPACK_PACK_HPP__
#define MURADIN_MSGPACK_PACK_HPP__

#include <muradin/msg/msg_pack/detail/predef.hpp>
#include <muradin/base/bytebuffer.hpp>

namespace muradin{
	namespace msg_pack
	{
		class kv_table
		{
		public:

			typedef				StrStrMap::iterator			kv_table_iterator;
			typedef				StrStrMap::const_iterator	const_kv_table_iterator;

			StrStrMap&			container(){return kv_set_;}
			const StrStrMap&	container()const{return kv_set_;}
			void				put(const std::string& key,const std::string& val)
			{
				kv_set_.insert(StrStrMap::value_type(key,val));
			}
			void				put_or_replace(const std::string& key,const std::string& val)
			{
				StrStrMap::iterator it = find(key);
				if (it == kv_set_.end()){
					put(key,val);
				}else{
					it->second=val;
				}
			}
			bool				exists(const std::string& key)const
			{
				return find(key)!=kv_set_.end();
			}
			StrStrMap::iterator	find(const std::string& key)
			{
				return kv_set_.find(key);
			}
			StrStrMap::const_iterator	find(const std::string& key)const
			{
				return kv_set_.find(key);
			}
			const std::string&			get_val(const std::string& key)const
			{
				return find(key)->second;
			}
			std::string&				get_val(const std::string& key)
			{
				return find(key)->second;
			}

			void	swap(kv_table& another)
			{
				kv_set_.swap(another.kv_set_);
			}

		private:
			StrStrMap	kv_set_;
		};

		template <typename OStream>
		static void print_kv_tab(const kv_table& tb,OStream& os)
		{
			const StrStrMap& kv=tb.container();
			kv_table::const_kv_table_iterator it=kv.begin();
			while(it != kv.end()){
				os<< it->first <<" : "<<it->second<<"\r\n";
				++it;
			}
		}

		/*
		class StartLineReq
		{
		public:
			static const char*	start_line(){
				return kReqLine.c_str();
			}
		};
		class StartLineRsp
		{
		public:
			static const char*	start_line(){
				return kRspLine.c_str();
			}
		};
		*/

		//template<typename StartLineType>
		//class pack_basic
		//{
		//public:
		//	pack_basic()
		//		:start_line_(StartLineType::start_line(),kStartLineLen)
		//	{
		//			//
		//	}
		//	const std::string&		start_line()const{return start_line_;}
		//	//std::string&			start_line(){return start_line_;}

		//	kv_table&			headers(){return headers_;}
		//	const kv_table&		headers()const{return headers_;}

		//	muradin::base::bytebuffer&				body(){return body_;}
		//	const muradin::base::bytebuffer&		body()const{return body_;}
		//private:
		//	std::string	start_line_;
		//	kv_table	headers_;
		//	muradin::base::bytebuffer	body_;
		//};

		/*template<typename pack_traits>
		class StartLineReq
		{
		public:
			typedef typename	pack_traits		PackTraits;

			static const char*	start_line(){
				return PackTraits::GetReqLine.c_str();
			}
		};

		template<typename pack_traits>
		class StartLineRsp
		{
		public:
			typedef typename	pack_traits		PackTraits;

			static const char*	start_line(){
				return PackTraits::GetRspLine.c_str();
			}
		};*/

		//template<typename StartLineTypeName>
		//class pack_basic
		//{
		//public:
		//	typedef typename	StartLineTypeName		StartLineType;
		//	pack_basic()
		//		:start_line_(StartLineType::start_line(),kStartLineLen)
		//	{
		//			//
		//	}
		//	const std::string&		start_line()const{return start_line_;}

		//	kv_table&			headers(){return headers_;}
		//	const kv_table&		headers()const{return headers_;}

		//	muradin::base::bytebuffer&				body(){return body_;}
		//	const muradin::base::bytebuffer&		body()const{return body_;}
		//private:
		//	std::string	start_line_;
		//	kv_table	headers_;
		//	muradin::base::bytebuffer	body_;
		//};

		class msg_base
		{
		public:
			kv_table&			headers(){return headers_;}
			const kv_table&		headers()const{return headers_;}

			muradin::base::bytebuffer&				body(){return body_;}
			const muradin::base::bytebuffer&		body()const{return body_;}
			void				clear_header(){headers_.container().clear();};
			void				clear_body(){body_.discard_all();};
			void				clear(){clear_header();clear_body();}
		private:
			kv_table	headers_;
			muradin::base::bytebuffer	body_;
		};


		template<typename pack_traits>
		class req_base : public msg_base
		{
		public:
			typedef typename	pack_traits		PackTraits;
			req_base()
				:start_line_(PackTraits::GetReqLine().c_str(),kStartLineLen)
			{
				//
			}
			const std::string&		start_line()const{return start_line_;}
		private:
			std::string	start_line_;
		};

		template<typename pack_traits>
		class rsp_base : public msg_base
		{
		public:
			typedef typename	pack_traits		PackTraits;
			rsp_base()
				:start_line_(PackTraits::GetRspLine().c_str(),kStartLineLen)
			{
				//
			}
			const std::string&		start_line()const{return start_line_;}
		private:
			std::string	start_line_;
		};
		
	}
}
#endif//MURADIN_MSGPACK_PACK_HPP__