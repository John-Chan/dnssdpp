
/**
 * 
 * @file       log_txt_file_writer.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-1-17
 *  
 * @brief      简单文件操作
 */


#ifndef MURADIN_LOG_TXT_FILE_WRITER_HPP__
#define MURADIN_LOG_TXT_FILE_WRITER_HPP__

#include <stdio.h>
#include <string>
#include <Windows.h>
namespace muradin
{
namespace loging
{
	class file_writer
	{
	public:
		file_writer(const std::string& file_name,const std::string& open_flag)
			:m_fp(NULL)
		{
			m_fp= ::fopen (file_name.c_str (),open_flag.c_str ());
			if (!m_fp){
				std::cout<<"fopen fail " <<std::endl;
				std::cout<<"file : " << file_name<<std::endl;
				std::cout<<"error : " <<  GetLastError()<< std::endl;
			}
		}
		virtual ~file_writer(void)
		{
			if(m_fp) ::fclose(m_fp);
			m_fp=NULL;
		}
		///提交消息
		file_writer&	operator<<(const std::string& msg)
		{
			write(msg.c_str(),msg.length());
			return *this;
		}
		///提交消息
		file_writer&	operator<<(const char* msg)
		{
			write(msg,::strlen(msg));
			return *this;
		}

		int		write(const void* data,int data_len)
		{
			int result = 0;
			if( m_fp == NULL) return result;
			result = static_cast<int> (::fwrite(data,data_len,1,m_fp));
			if (result == 1 ){
				return data_len;
			}else{
				std::cout<<"fwrite fail " <<std::endl;
				std::cout<<"error : " <<  GetLastError()<< std::endl;
				return -1;
			}
		}
		int		read(void* data,int max_read_len)
		{
			int result = 0;
			if( m_fp == NULL) return result;
			result = static_cast<int> (::fread(data,1,max_read_len,m_fp));
			if (result > 0 ){
				return result;
			}else{
				std::cout<<"fread fail " <<std::endl;
				std::cout<<"error : " <<  GetLastError() << std::endl;
				return -1;
			}
		}
		void	flush()
		{
			if( m_fp ) ::fflush(m_fp);
		}
	private:
		FILE*	m_fp;
	};
}
}
#endif // MURADIN_LOG_TXT_FILE_WRITER_HPP__
