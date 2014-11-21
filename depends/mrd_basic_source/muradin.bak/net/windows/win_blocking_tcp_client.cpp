#include <muradin/net/windows/win_blocking_tcp_client.h>
#include <muradin/net/windows/win_socket2_header.h>
#include <assert.h>
#include <iostream>	//for debug

namespace muradin{
namespace net{
namespace windows{

	
	int sys_error_msg(int error_number,char* buff,size_t buff_bytes) 
	{ 
		return FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error_number,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buff,
			buff_bytes/sizeof (buff[0]), NULL );
	}

	/************************************************************************
	//  describtion ......
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/
 
	blocking_tcp_client::blocking_tcp_client(
		const std::string& remote_ip,
		unsigned short remote_port,
		const std::string& local_ip,
		unsigned short local_port,
		unsigned int read_timeout_ms,
		unsigned int conn_timeout_ms,
		unsigned int write_timeout_ms
		)
		:m_socket(INVALID_SOCKET),
		m_remote_host(remote_ip),
		m_remote_port(remote_port),
		m_local_host(local_ip),
		m_local_port(local_port),
		m_connected(false),
		m_conn_timeout(conn_timeout_ms),
		m_write_timeout(write_timeout_ms),
		m_read_timeout(read_timeout_ms)
	{
		muradin::err_msg err;
		creat_socket_nb(err);
	}
	blocking_tcp_client::blocking_tcp_client(
		unsigned int read_timeout_ms,
		unsigned int conn_timeout_ms,
		unsigned int write_timeout_ms
		):m_socket(INVALID_SOCKET),
		m_remote_host(""),
		m_remote_port(0),
		m_local_host(""),
		m_local_port(0),
		m_connected(false),
		m_conn_timeout(conn_timeout_ms),
		m_write_timeout(write_timeout_ms),
		m_read_timeout(read_timeout_ms)
	{
		muradin::err_msg err;
		creat_socket_nb(err);
	}
	blocking_tcp_client::~blocking_tcp_client()
	{
		tcp_shutdown(k_tcp_close_both);
		close();
	}

	muradin::err_msg&			blocking_tcp_client::warp_error(int error_numbner,muradin::err_msg& out,const std::string& context)
	{
		out.code=error_numbner;
		
		if(context.length()>0){
			char sys_e_msg[200]={0};
			sys_error_msg(error_numbner,sys_e_msg,sizeof(sys_e_msg)-1);
			sprintf_s(out.msg,sizeof(out.msg)-1,"%s,%s",context.c_str(),sys_e_msg);
		}else{
			sys_error_msg(error_numbner,out.msg,sizeof(out.msg)-1);
		}
		return out;
	}
	void			blocking_tcp_client::clear_error(muradin::err_msg& error)
	{
		error.code=0;
		error.msg[0]=0;
	}
	bool			blocking_tcp_client::connect(muradin::err_msg& error)
	{
		clear_error(error);
		if( error.code == 0 ) error.code = socket_helper::bind_addr(m_socket,m_local_host.length()==0?NULL:m_local_host.c_str(),m_local_port);
		if( error.code == 0 ) error.code =socket_helper::connect(m_socket,m_remote_host.c_str(),m_remote_port,m_conn_timeout);
		if(error.code!=0){
			warp_error(error.code,error);
			m_connected=false;
		}else{
			m_connected=true;
		}
		update_local_addr();
		return m_connected;
	}
	void			blocking_tcp_client::update_local_addr()
	{
		char local_ip[128]={0};
		unsigned short local_port;
		if( 0== socket_helper::retrieve_local_addr(m_socket,local_ip,sizeof(local_ip),&local_port) ){
			m_local_host=local_ip;
			m_local_port=local_port;
		}
	}
	void			blocking_tcp_client::close()
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		m_connected=false;
		m_read_buf.discard_all();
	}
	void			blocking_tcp_client::tcp_shutdown(tcp_close_flag flag)
	{
		shutdown(m_socket,flag);
		if(flag== k_tcp_close_both || flag == k_tcp_close_read)
			m_read_buf.discard_all();
	}
	int				blocking_tcp_client::write(const void* data,size_t bytes,muradin::err_msg& error)
	{
		clear_error(error);
		int bytes_transmited=0;
		const char* send_ptr=static_cast<const char*>(data);
		const int			total_bytes=(int)bytes;

		if(!connected()){
			warp_error(WSAENOTCONN,error);
			return bytes_transmited;
		}
		unsigned int start=GetTickCount();
		while(total_bytes > bytes_transmited && total_bytes>0){
			unsigned int time_gone=(unsigned int)(GetTickCount()-start);
			if(m_write_timeout <= time_gone){
				/// time is over
				error.code = WAIT_TIMEOUT;
				break;
			}
			unsigned int timeout_remanent = m_write_timeout - time_gone ;
			
			if(error.code ==0){
				int socket_ret=0;
				socket_ret = nonblcoking_write(send_ptr + bytes_transmited,total_bytes-bytes_transmited,error);
				if(SOCKET_ERROR == socket_ret){
					if(WSAEWOULDBLOCK == error.code){
						if(0 == (error.code =socket_helper::select_writeable(m_socket,timeout_remanent))){
							continue;
						}else{
							error.code = WAIT_TIMEOUT;
							break;
						}
					}else{
						break;
					}
				}else{
					bytes_transmited+=socket_ret;
				}
			}else{
				// abort ,error occcurt or select timeout
				break;
			}
		}

		warp_error(error.code,error);
		return bytes_transmited;
		
	}
	size_t			blocking_tcp_client::retrive_from_read_buf(void* buffer,size_t max_bytes)
	{
		size_t readable=m_read_buf.readable_bytes();
		//readable=max_bytes>=readable ? readable : readable-max_bytes;

		readable=readable>max_bytes ? max_bytes : readable;
		assert(max_bytes >= readable);
		memcpy(buffer,m_read_buf.rd_ptr(),readable);
		m_read_buf.discard(readable);
		return readable;
	}
	size_t			blocking_tcp_client::retrive_str_from_read_buf(std::string& out)
	{
		size_t found_eof=m_read_buf.char_pos('\0');
		if(found_eof != 0)
			out=m_read_buf.as_string();
		m_read_buf.discard(found_eof);
		return found_eof;
	}
	int				blocking_tcp_client::nonblcoking_read(muradin::err_msg& error)
	{
		clear_error(error);

		static const size_t read_bytes_once=64*1024;
		m_read_buf.ensure_write_space(read_bytes_once);

		int socket_ret = ::recv(m_socket,m_read_buf.wt_ptr(),read_bytes_once,0);
		if(socket_ret > 0){
			m_read_buf.has_written((size_t)socket_ret);
		}else if(SOCKET_ERROR == socket_ret){
			warp_error(WSAGetLastError(),error);
			socket_ret=0;
		}else if(0 == socket_ret) {
			// get FIN
			//warp_error(WSAESHUTDOWN,error);
			tcp_shutdown(k_tcp_close_read);
		}
		return socket_ret;
	}
	int				blocking_tcp_client::nonblcoking_write(const void* buffer,size_t bytes,muradin::err_msg& error)
	{
		clear_error(error);
		int socket_ret = ::send(m_socket,(const char*)buffer,(int)bytes,0);
		if(SOCKET_ERROR == socket_ret){
			warp_error(WSAGetLastError(),error);
			return 0;
		}
		return socket_ret;
	}

	int				blocking_tcp_client::read_some(void* buffer,size_t max_bytes,muradin::err_msg& error)
	{
		/// get data from cache
		int has_read=(int)retrive_from_read_buf(buffer,max_bytes);
		if( has_read > 0 /*(int)max_bytes*/ ) return has_read;

		/// recv remanent
		int socket_ret=read_some(m_read_timeout,error);
		if(socket_ret >0 ){
			char* read_buf=(char*)buffer + has_read;
			has_read +=(int)retrive_from_read_buf(read_buf,max_bytes-(size_t)has_read);
		}else{
			int i=0;
		}
		return has_read ;
	}
	int				blocking_tcp_client::read_some(unsigned int read_timeout_ms,muradin::err_msg& error)
	{
		clear_error(error);

		if(!connected()){
			warp_error(WSAENOTCONN,error);
			return 0;
		}

		int socket_ret=0;
		socket_ret = nonblcoking_read(error);
		if(socket_ret <= 0 && WSAEWOULDBLOCK ==error.code){
			// read nothing 
			if( 0 == (error.code =socket_helper::select_readable(m_socket,read_timeout_ms))){
				socket_ret = nonblcoking_read(error);
			}else{
				warp_error(WAIT_TIMEOUT,error);
			}
		}

		warp_error(error.code,error);
		return socket_ret;
	}

	int				blocking_tcp_client::read_untill(void* buffer,size_t bytes,muradin::err_msg& error)
	{
		clear_error(error);
		char* read_ptr=static_cast< char*>(buffer);

		unsigned int start=GetTickCount();

		while(m_read_buf.readable_bytes() < bytes ){

			unsigned int time_gone= (GetTickCount()-start);
			if(m_write_timeout <= time_gone){
				/// time  over
				error.code = WAIT_TIMEOUT;
				break;
			}
			unsigned int timeout_remanent = m_write_timeout - time_gone ;
			int read_ret=read_some(timeout_remanent,error);
			if(0 >= read_ret){
				break;
			}
		}

		warp_error(error.code,error);
		return (int)retrive_from_read_buf(buffer,bytes);
	}

	int				blocking_tcp_client::read_string(std::string& out,muradin::err_msg& error)
	{
		clear_error(error);
		unsigned int start=GetTickCount();

		size_t str_len=0;
		while( (str_len=retrive_str_from_read_buf(out)) <=0 ){

			unsigned int time_gone= (GetTickCount()-start);
			if(m_write_timeout <= time_gone){
				/// time  over
				error.code = WAIT_TIMEOUT;
				break;
			}
			if(  read_some(m_read_timeout,error) <= 0){
				break;
			}
		}

		warp_error(error.code,error);
		return (int)str_len;;
	}
	int				blocking_tcp_client::write_string(const std::string& str,muradin::err_msg& error,bool append_eof)
	{
		if(str.empty()) return 0;
		size_t length = str.size();
		if(append_eof) 
			length +=1;
		return write(str.data(),length,error);
	}
	bool			blocking_tcp_client::creat_socket_nb(muradin::err_msg& error)
	{
		clear_error(error);
		if(m_socket == INVALID_SOCKET){
			m_socket = socket_helper::create_tcp_socket(error.code);
			if(m_socket == INVALID_SOCKET){
				warp_error(error.code,error);
				std::cout<< "create socket fail ,error= " << error.code <<",message = " << error.msg <<std::endl;
				return false;
			}else{
				if(0 == error.code) error.code = socket_helper::set_non_blocking(m_socket);
				if(0 == error.code) error.code = socket_helper::set_tcp_nodelay(m_socket);
				if(0 == error.code) error.code = socket_helper::set_recv_buff_size(m_socket,64*1024);

				if( 0!=error.code){
					warp_error(error.code,error);
					return false;
				}
			}
		}
		return true;
	}

}
}
}