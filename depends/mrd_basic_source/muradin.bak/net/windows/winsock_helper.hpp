#ifndef winsock_helper_h__
#define winsock_helper_h__

#include <muradin/net/windows/win_socket2_header.h>

namespace muradin{
namespace net{
namespace windows{
namespace socket_helper{

		class winsock_initor
		{
		public:
			winsock_initor(){
				WSADATA	WinSocketsInf;
				WORD	Ver=0x202;
				WSAStartup(Ver,&WinSocketsInf);
			}

			~winsock_initor(){
				WSACleanup();
			}
		};


		class warped_event
		{
		public:
			warped_event(HANDLE* evt){
				m_evt=evt;
				*m_evt=WSACreateEvent ();
			};
			~warped_event(){
				CloseHandle (*m_evt);
				*m_evt=NULL;
			}
		private:
			HANDLE* m_evt;
		};

		//////////////////////////////////////////////////////////////////////////
		static	int set_tcp_nodelay(SOCKET socket,bool nodelay=true)
		{
			int val=nodelay?1:0;
			if( ::setsockopt( socket, IPPROTO_TCP, TCP_NODELAY, (char*)&val, sizeof( val ) ) < 0 ){
				return WSAGetLastError ();
			}
			return 0;
		}
		
		static	int set_send_buff_size(SOCKET socket,int size)
		{
			if( ::setsockopt( socket, SOL_SOCKET, SO_SNDBUF, (char*)&size, sizeof( size ) ) < 0 ){
				return WSAGetLastError ();
			}
			return 0;
		}

		static	int set_recv_buff_size(SOCKET socket,int size)
		{
			if( ::setsockopt( socket, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof( size ) ) < 0 ){
				return WSAGetLastError ();
			}
			return 0;
		}
		static	int set_non_blocking( SOCKET socket,bool non_blocking=true )
		{
			unsigned long ul = non_blocking?1:0;  
			if (SOCKET_ERROR == ::ioctlsocket(socket, FIONBIO, (unsigned long*)&ul) ){
				return WSAGetLastError ();
			}
			return 0;
		}

		static	int set_reuse_addr( SOCKET socket )
		{
			int flags = 1;
			if( ::setsockopt( socket, SOL_SOCKET, SO_REUSEADDR, (char*)&flags, sizeof( flags ) ) < 0 ){
				return WSAGetLastError ();
			}
			return 0;
		}

		static	SOCKET	create_tcp_socket(int& err_code)
		{
			SOCKET fd=WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if(fd==INVALID_SOCKET) err_code=WSAGetLastError();
			else err_code =0;
			return fd;
		}


		/// Retrieves remote address by socket
		/// If no error occurs,reutrn 0 ,otherwise a error code return
		/// @warn NOT thread-safe 
		/// error list:
		///     -# WSANOTINITIALISED
		///     -# WSAENETDOWN
		///     -# WSAEFAULT
		///     -# WSAEINPROGRESS
		///     -# WSAENOTCONN
		///     -# WSAENOTSOCK
		static	int		retrieve_remote_addr(SOCKET socket,char* buff_ip,size_t buff_bytes,unsigned short* port)
		{
			struct sockaddr_in addr={0};
			int len=sizeof(addr);
			if ( SOCKET_ERROR == getpeername(socket,(struct sockaddr *)&addr,&len)){
				return WSAGetLastError();
			}else{
				char* ip_str=::inet_ntoa(addr.sin_addr);
				if(ip_str) {
					strcpy_s(buff_ip,buff_bytes,ip_str);
				}else{
					strcpy_s(buff_ip,buff_bytes,"bad address");
				}
				*port=ntohs(addr.sin_port);
				return 0;
			}
		}

		/// Retrieves local address by socket,This call is especially useful when a connect call has been made without doing a bind first
		/// If no error occurs,reutrn 0 ,otherwise a error code return
		/// @warn NOT thread-safe 
		/// error list:
		///     -# WSANOTINITIALISED
		///     -# WSAENETDOWN
		///     -# WSAEFAULT
		///     -# WSAEINPROGRESS
		///     -# WSAENOTSOCK
		///     -# WSAEINVAL
		static	int		retrieve_local_addr(SOCKET socket,char* buff_ip,size_t buff_bytes,unsigned short* port)
		{
			struct sockaddr_in addr={0};
			int len=sizeof(addr);
			if ( SOCKET_ERROR == getsockname(socket,(struct sockaddr *)&addr,&len)){
				return WSAGetLastError();
			}else{
				char* ip_str=::inet_ntoa(addr.sin_addr);
				if(ip_str) {
					strcpy_s(buff_ip,buff_bytes,ip_str);
				}else{
					strcpy_s(buff_ip,buff_bytes,"bad address");
				}
				*port=ntohs(addr.sin_port);
				return 0;
			}

		}

		/// bind a address
		/// ip can be NULL
		/// port can be 0
		/// return 0 when  success 
		/// return a error number   
		static	int	bind_addr(SOCKET socket,const char* ip,unsigned short port)
		{
			struct sockaddr_in local_addr;
			memset( &local_addr, 0, sizeof(struct sockaddr_in) );

			if (ip!=NULL){
				local_addr.sin_addr.s_addr = inet_addr( ip );
			}else{
				local_addr.sin_addr.s_addr = ADDR_ANY;
			}
			if (port!=0){
				local_addr.sin_port = htons( port );
			}else{
				local_addr.sin_port = 0;
			}
			local_addr.sin_family = AF_INET;

			if(SOCKET_ERROR== ::bind(socket,(struct sockaddr *)&local_addr, (int)(sizeof(local_addr)))){
				return WSAGetLastError ();
			}
			return 0;
		}


		/// 检查套接字是否可写
		/// - 返回0 表示操作成功,并且可写
		/// - 其他返回结果:
		///     -# WAIT_TIMEOUT
		///     -# WSANOTINITIALISED
		///     -# WSAEFAULT
		///     -# WSAENETDOWN
		///     -# WSAEINVAL
		///     -# WSAEINTR
		///     -# WSAEINPROGRESS
		///     -# WSAENOTSOCK
		static	int select_writeable(SOCKET socket,unsigned int millisecond)
		{
			fd_set  fdwrite;
			int     ret;
			FD_ZERO(&fdwrite);
			timeval timeout_val;
			timeout_val.tv_sec=millisecond/1000;
			timeout_val.tv_usec=1000* (millisecond%1000);

			FD_SET(socket, &fdwrite);
			ret = select(0, NULL, &fdwrite, NULL,  &timeout_val);
			if (ret < 0) {
				return WSAGetLastError();
			}else if(ret == 0){
				return WAIT_TIMEOUT;
			}else{
				// no need to check FD_ISSET(socket, &fdread);
				return 0;
			}
		}


		/// 检查套接字是否可读
		/// - 返回0 表示操作成功,并且可读
		/// - 其他返回结果:
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
		static	int select_readable(SOCKET socket,unsigned int millisecond )
		{
			fd_set  fdread;
			int     ret;
			FD_ZERO(&fdread);
			timeval timeout_val;
			timeout_val.tv_sec=millisecond/1000;
			timeout_val.tv_usec=1000* (millisecond%1000);

			FD_SET(socket, &fdread);
			ret = select(0, &fdread, NULL, NULL, &timeout_val);
			if (ret < 0) {
				return WSAGetLastError();
			}else if(ret == 0){
				return WAIT_TIMEOUT;
			}else {
				// no need to check FD_ISSET(socket, &fdread);
				return 0;
			}
		}
		/// connect 
		/// socket must be a non-block socket
		/// If no error occurs,reutrn 0 ,otherwise a error code return
		static	int	connect(SOCKET socket,const char* ip,unsigned short port,unsigned int millisecond )
		{

			struct sockaddr_in remote_addr;
			memset( &remote_addr, 0, sizeof(remote_addr) );

			remote_addr.sin_family = AF_INET;
			remote_addr.sin_addr.s_addr = inet_addr( ip );
			remote_addr.sin_port = htons( port );

			if (SOCKET_ERROR == ::connect (socket,(struct sockaddr *)&remote_addr, sizeof(remote_addr))){
				int err_no=WSAGetLastError ();
				if (WSAEWOULDBLOCK == err_no){
					return select_writeable(socket,millisecond);
				}else{
					return err_no;
				}
			}else{
				return 0;
			}
		}

	};//socket_helper
}
}
}

#endif // winsock_helper_h__
