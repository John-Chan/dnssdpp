#ifndef blocking_tcp_client_h__
#define blocking_tcp_client_h__

#include <muradin/base/error_type.h>
#include <muradin/net/windows/winsock_helper.hpp>
#include <muradin/base/bytebuffer.hpp>

#include <string>

namespace muradin{
namespace net{
namespace windows{


	class blocking_tcp_client
	{
	public:

		/// TCP 半开连接关闭标志
		enum tcp_close_flag{
			k_tcp_close_read	=0x00,	/* 0x00 关闭TCP的读端 */
			k_tcp_close_writ	=0x01,	/* 0x01 关闭TCP的写端 */
			k_tcp_close_both	=0x02	/* 0x02 关闭读和写    */
		};
		blocking_tcp_client(
			const std::string& remote_ip,
			unsigned short remote_port,
			const std::string& local_ip="",
			unsigned short local_port=0,
			unsigned int read_timeout_ms=k_def_read_timeout_ms,
			unsigned int conn_timeout_ms=k_def_conntimeout_ms,
			unsigned int write_timeout_ms=k_def_conntimeout_ms
			);

		blocking_tcp_client(
			unsigned int read_timeout_ms=k_def_read_timeout_ms,
			unsigned int conn_timeout_ms=k_def_conntimeout_ms,
			unsigned int write_timeout_ms=k_def_conntimeout_ms
			);
		virtual ~blocking_tcp_client();

		static void			clear_error(muradin::err_msg& error);


		void		set_remote_host(const std::string& remote_ip){ m_remote_host=remote_ip;};
		void		set_remote_port(unsigned short remote_port){ m_remote_port=remote_port;};
		std::string		remote_host()const{return m_remote_host;};
		unsigned short	remote_port()const{return m_remote_port;};
		std::string		local_host()const{return m_local_host;};
		unsigned short	local_port()const{return m_local_port;};
		bool			connected()const{return m_connected;};

		unsigned int			conn_timeout()const{return m_conn_timeout;};
		void					conn_timeout(unsigned int millisecond){m_conn_timeout=millisecond;};

		unsigned int			read_timeout()const{return m_read_timeout;};
		void					read_timeout(unsigned int millisecond){m_read_timeout=millisecond;};

		unsigned int			write_timeout()const{return m_write_timeout;};
		void					write_timeout(unsigned int millisecond){m_write_timeout=millisecond;};


		/// connect to host
		/// if connect successful,return true,otherwise false return.check error to get error infomation
		/// error list:
		///     -# WSANOTINITIALISED
		///     -# WSAENETDOWN
		///     -# WSAEADDRINUSE
		///     -# WSAEINTR
		///     -# WSAEINPROGRESS
		///     -# WSAEALREADY
		///     -# WSAEADDRNOTAVAIL
		///     -# WSAEAFNOSUPPORT
		///     -# WSAECONNREFUSED
		///     -# WSAEFAULT
		///     -# WSAEINVAL
		///     -# WSAEISCONN
		///     -# WSAENETUNREACH
		///     -# WSAEHOSTUNREACH
		///     -# WSAENOBUFS
		///     -# WSAENOTSOCK
		///     -# WSAETIMEDOUT
		///     -# WSAEWOULDBLOCK
		///     -# WSAEACCES
		///     -# WAIT_TIMEOUT
		bool			connect(muradin::err_msg& error);

		void			tcp_shutdown(tcp_close_flag flag=k_tcp_close_writ);

		void			close();

		/// write data,block untill  write all datas.
		/// return bytes has been wrote
		/// return <=0 when write nothing,check error to get error infomation
		/// error list:
		///     -# WSANOTINITIALISED
		///     -# WSAENETDOWN
		///     -# WSAEACCES
		///     -# WSAEINTR
		///     -# WSAEINPROGRESS
		///     -# WSAEFAULT
		///     -# WSAENETRESET
		///     -# WSAENOBUFS
		///     -# WSAENOTCONN
		///     -# WSAENOTSOCK
		///     -# WSAEOPNOTSUPP
		///     -# WSAESHUTDOWN
		///     -# WSAEWOULDBLOCK
		///     -# WSAEMSGSIZE
		///     -# WSAEHOSTUNREACH
		///     -# WSAEINVAL
		///     -# WSAECONNABORTED
		///     -# WSAECONNRESET
		///     -# WSAETIMEDOUT
		///     -# WAIT_TIMEOUT
		int				write(const void* data,size_t bytes,muradin::err_msg& error);


		int				write_string(const std::string& str,muradin::err_msg& error,bool append_eof=true);

		/// 读数据到缓冲区
		/// 函数阻塞当前线程,在以下情况函数返回:
		/// - 接收到数据(包括收到FIN)
		/// - 发生网络错误
		/// - 超时时限到达
		/// 返回读到的数据量
		/// 可能的错误代码 :
		///     -# WSANOTINITIALISED
		///     -# WSAENETDOWN
		///     -# WSAENOTCONN
		///     -# WSAEINTR
		///     -# WSAEINPROGRESS
		///     -# WSAENOTSOCK
		///     -# WSAENETRESET
		///     -# WSAEOPNOTSUPP
		///     -# WSAESHUTDOWN
		///     -# WSAEWOULDBLOCK
		///     -# WSAEINVAL
		///     -# WSAECONNABORTED
		///     -# WSAECONNRESET
		///     -# WAIT_TIMEOUT
		int				read_some(void* buffer,size_t max_bytes,muradin::err_msg& error);

		/// 读数据到内部缓冲区
		/// 函数阻塞当前线程,在以下情况函数返回:
		/// - 接收到指定数据的数据
		/// - 发生网络错误
		/// - 超时时限到达
		int				read_untill(void* buffer,size_t bytes,muradin::err_msg& error);

		/// read as string
		/// return length of string(not include '\\0')
		int				read_string(std::string& out,muradin::err_msg& error);
	protected:

		/// 读数据到内部缓冲区
		/// 函数阻塞当前线程,在以下情况函数返回:
		/// - 接收到数据(包括收到FIN)
		/// - 发生网络错误
		/// - 超时时限到达
		/// 返回读到的数据量
		/// 可能的错误代码 :
		///     -# WSANOTINITIALISED
		///     -# WSAENETDOWN
		///     -# WSAENOTCONN
		///     -# WSAEINTR
		///     -# WSAEINPROGRESS
		///     -# WSAENOTSOCK
		///     -# WSAENETRESET
		///     -# WSAEOPNOTSUPP
		///     -# WSAESHUTDOWN
		///     -# WSAEWOULDBLOCK
		///     -# WSAEINVAL
		///     -# WSAECONNABORTED
		///     -# WSAECONNRESET
		///     -# WAIT_TIMEOUT
		int				read_some(unsigned int read_timeout_ms,muradin::err_msg& error);

		bool			creat_socket_nb(muradin::err_msg& error);
		void			update_local_addr();
		int				nonblcoking_read(muradin::err_msg& error);
		int				nonblcoking_write(const void* buffer,size_t bytes,muradin::err_msg& error);

		size_t			retrive_from_read_buf(void* buffer,size_t max_bytes);
		size_t			retrive_str_from_read_buf(std::string& out);
		static muradin::err_msg&			warp_error(int error_numbner,muradin::err_msg& out,const std::string& context="");
	private:
		enum{k_def_conntimeout_ms=3000,k_def_write_timeout_ms=1000,k_def_read_timeout_ms=3000};
		socket_helper::winsock_initor			m_winsock_initor;
		SOCKET	m_socket;
		std::string		m_remote_host;
		unsigned short	m_remote_port;
		std::string		m_local_host;
		unsigned short	m_local_port;
		bool					m_connected;
		unsigned int			m_conn_timeout;
		unsigned int			m_write_timeout;
		unsigned int			m_read_timeout;
		muradin::base::bytebuffer	m_read_buf;

	};
}
}


}
#endif // blocking_tcp_client_h__
