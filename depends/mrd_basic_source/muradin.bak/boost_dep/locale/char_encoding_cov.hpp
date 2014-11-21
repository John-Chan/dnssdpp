#ifndef MURADIN_LOCALE_CHAR_ENCODEING_COV_HPP__
#define MURADIN_LOCALE_CHAR_ENCODEING_COV_HPP__

#include <boost/locale.hpp>
#include <string>
#include <iostream>

namespace muradin{namespace locale{
	
	static bool	try_ansi_to_utf8(const std::string& encoding_name,const std::string& ansi_encod,std::string& utf8_encod)
	{
		try{
			utf8_encod=boost::locale::conv::to_utf<char>( ansi_encod, encoding_name ); 
			return true;
		}catch (const std::exception& e){
			std::cout<<__FUNCTION__ << " "<< e.what()<<std::endl;
			return false;
		}catch (...){
			std::cout<<__FUNCTION__<< " unknow exception"<<std::endl;
			return false;
		}
	}

	static bool	try_utf8_to_ansi(const std::string& encoding_name,const std::string& utf8_encod,std::string& ansi_encod)
	{
		try{
			ansi_encod=boost::locale::conv::from_utf<char>( utf8_encod, encoding_name ); 
			return true;
		}catch (const std::exception& e){
			std::cout<<__FUNCTION__ << " "<< e.what()<<std::endl;
			return false;
		}catch (...){
			std::cout<<__FUNCTION__<< " unknow exception"<<std::endl;
			return false;
		}
	}

	static bool  try_conv(
		char const *begin,
		char const *end,
		std::string&	dest, 
		std::string const &to_encoding,
		std::string const &from_encoding,
		boost::locale::conv::method_type how=boost::locale::conv::stop)
	{

		try{
			dest=boost::locale::conv::between( begin, end,to_encoding,from_encoding,how ); 
			return true;
		}catch (const std::exception& e){
			std::cout<<__FUNCTION__ << " "<< e.what()<<std::endl;
			return false;
		}catch (...){
			std::cout<<__FUNCTION__<< " unknow exception"<<std::endl;
			return false;
		}
	}

	static bool  try_conv(
		const std::string& src, 
		std::string&	dest, 
		std::string const &to_encoding,
		std::string const &from_encoding,
		boost::locale::conv::method_type how=boost::locale::conv::stop)
	{

		try{
			dest=boost::locale::conv::between( src,to_encoding,from_encoding,how ); 
			return true;
		}catch (const std::exception& e){
			std::cout<<__FUNCTION__ << " "<< e.what()<<std::endl;
			return false;
		}catch (...){
			std::cout<<__FUNCTION__<< " unknow exception"<<std::endl;
			return false;
		}
	}
}}

#endif//MURADIN_LOCALE_CHAR_ENCODEING_COV_HPP__