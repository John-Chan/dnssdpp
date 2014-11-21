#ifndef MURADIN_STR_INT_HPP__
#define MURADIN_STR_INT_HPP__
 
#include <cstdlib>
#include <string>
#include <sstream>

#include <stdio.h>

namespace muradin
{
	namespace string_helper{
		template <class IntType>
		static bool	str_to_int(const std::string& src,int radix,IntType& number)
		{
			const char* start_ptr =src.c_str();
			char* end_ptr=NULL;
			number =static_cast<IntType> (std::strtol(start_ptr, &end_ptr, radix));

			if (end_ptr == start_ptr){
				// Not a valid number at all
				return false;
			}else if (*end_ptr != '\0'){
				// String begins with a valid number, but also contains something else after the number
				return false;
			}else{
				// String is a number
				return true;
			}

		}

		template <class IntType>
		static bool	str_to_uint(const std::string& src,int radix,IntType& number)
		{
			const char* start_ptr =src.c_str();
			char* end_ptr=NULL;
			number =static_cast<IntType> (std::strtoul(start_ptr, &end_ptr, radix));

			if (end_ptr == start_ptr){
				// Not a valid number at all
				return false;
			}else if (*end_ptr != '\0'){
				// String begins with a valid number, but also contains something else after the number
				return false;
			}else{
				// String is a number
				return true;
			}

		}

		template <class IntType>
		static std::string	int_to_str(IntType n)
		{
			std::stringstream  ss;
			ss << n;
			return ss.str();
		}
		template <>
		static std::string	int_to_str(int n)
		{
			char buff[128]={0};
			sprintf(buff,"%d",n);
			return std::string(buff);
		}
		template <>
		static std::string	int_to_str(unsigned int n)
		{
			char buff[128]={0};
			sprintf(buff,"%u",n);
			return std::string(buff);
		}/*
		template <>
		static std::string	int_to_str(long long n)
		{
			char buff[128]={0};
			sprintf(buff,"%ll",n);
			return std::string(buff);
		}
		template <>
		static std::string	int_to_str(unsigned long long n)
		{
			char buff[128]={0};
			sprintf(buff,"%llu",n);
			return std::string(buff);
		}*/
	}
}
#endif//MURADIN_STR_INT_HPP__