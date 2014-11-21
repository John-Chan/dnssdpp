#ifndef MURADIN_NET_BLOCKING_TCP_CLIENT_H__
#define MURADIN_NET_BLOCKING_TCP_CLIENT_H__

#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/smart_ptr.hpp>

namespace muradin{
namespace net
{
//----------------------------------------------------------------------

//
// This class manages socket timeouts by applying the concept of a deadline.
// Each asynchronous operation is given a deadline by which it must complete.
// Deadlines are enforced by an "actor" that persists for the lifetime of the
// client object:
//
//  +----------------+
//  |                |     
//  | check_deadline |<---+
//  |                |    |
//  +----------------+    | async_wait()
//              |         |
//              +---------+
//
// If the actor determines that the deadline has expired, the socket is closed
// and any outstanding operations are consequently cancelled. The socket
// operations themselves use boost::lambda function objects as completion
// handlers. For a given socket operation, the client object runs the
// io_service to block thread execution until the actor completes.
//
class block_tcp_client
{
public:
	block_tcp_client()
		: socket_(io_service_),
		deadline_(io_service_),
		connected_(false)
	{
		// No deadline is required until the first socket operation is started. We
		// set the deadline to positive infinity so that the actor takes no action
		// until a specific deadline is set.
		deadline_.expires_at(boost::posix_time::pos_infin);

		// Start the persistent actor that checks for deadline expiry.
		check_deadline();
	}

	bool	connected()const
	{
		return connected_;
	}
	void try_connect(const std::string& host, const std::string& service,
		boost::posix_time::time_duration timeout,boost::system::error_code& ec)
	{
		// Resolve the host name and service to a list of endpoints.
		boost::asio::ip::tcp::resolver::query query(host, service);
		boost::asio::ip::tcp::resolver::iterator iter = boost::asio::ip::tcp::resolver(io_service_).resolve(query);

		// Set a deadline for the asynchronous operation. As a host name may
		// resolve to multiple endpoints, this function uses the composed operation
		// async_connect. The deadline applies to the entire operation, rather than
		// individual connection attempts.
		deadline_.expires_from_now(timeout);

		// Set up the variable that receives the result of the asynchronous
		// operation. The error code is set to would_block to signal that the
		// operation is incomplete. Asio guarantees that its asynchronous
		// operations will never fail with would_block, so any other value in
		// ec indicates completion.
		ec = boost::asio::error::would_block;

		// Start the asynchronous operation itself. The boost::lambda function
		// object is used as a callback and will update the ec variable when the
		// operation completes. The blocking_udp_client.cpp example shows how you
		// can use boost::bind rather than boost::lambda.
		boost::asio::async_connect(socket_, iter, boost::lambda::var(ec) = boost::lambda::_1);

		// Block until the asynchronous operation has completed.
		do io_service_.run_one(); while (ec == boost::asio::error::would_block);

		// Determine whether a connection was successfully established. The
		// deadline actor may have had a chance to run and close our socket, even
		// though the connect operation notionally succeeded. Therefore we must
		// check whether the socket is still open before deciding if we succeeded
		// or failed.
		if( !socket_.is_open()){
			ec = ec ? ec : boost::asio::error::operation_aborted;
		}else{
			connected_=true;
		}
	}
	
	void connect(const std::string& host, const std::string& service,
		boost::posix_time::time_duration timeout)
	{
		/*boost::asio::ip::tcp::resolver::query query(host, service);
		boost::asio::ip::tcp::resolver::iterator iter = boost::asio::ip::tcp::resolver(io_service_).resolve(query);

		deadline_.expires_from_now(timeout);

		boost::system::error_code ec = boost::asio::error::would_block;

		boost::asio::async_connect(socket_, iter, boost::lambda::var(ec) = boost::lambda::_1);

		do io_service_.run_one(); while (ec == boost::asio::error::would_block);

		if (ec || !socket_.is_open())
			throw boost::system::system_error(
			ec ? ec : boost::asio::error::operation_aborted);
		else
			connected_=true;*/
		boost::system::error_code ec;
		try_connect(host,service,timeout,ec);
		if(ec){
			throw boost::system::system_error(ec);
		}
	}

	void		try_read_data(void* buff,size_t bytes,boost::posix_time::time_duration timeout,boost::system::error_code& ec)
	{
		// Set a deadline for the asynchronous operation. Since this function uses
		// a composed operation (async_read_until), the deadline applies to the
		// entire operation, rather than individual reads from the socket.
		deadline_.expires_from_now(timeout);

		// Set up the variable that receives the result of the asynchronous
		// operation. The error code is set to would_block to signal that the
		// operation is incomplete. Asio guarantees that its asynchronous
		// operations will never fail with would_block, so any other value in
		// ec indicates completion.
		ec = boost::asio::error::would_block;

		// Start the asynchronous operation itself. The boost::lambda function
		// object is used as a callback and will update the ec variable when the
		// operation completes. The blocking_udp_client.cpp example shows how you
		// can use boost::bind rather than boost::lambda.
		boost::asio::async_read(socket_, boost::asio::buffer(buff,bytes), boost::lambda::var(ec) = boost::lambda::_1);


		// Block until the asynchronous operation has completed.
		do io_service_.run_one(); while (ec == boost::asio::error::would_block);

	}
	
	void		read_data(void* buff,size_t bytes,boost::posix_time::time_duration timeout)
	{
		/*deadline_.expires_from_now(timeout);

		boost::system::error_code ec = boost::asio::error::would_block;

		boost::asio::async_read(socket_, boost::asio::buffer(buff,bytes), boost::lambda::var(ec) = boost::lambda::_1);

		do io_service_.run_one(); while (ec == boost::asio::error::would_block);

		if (ec)
			throw boost::system::system_error(ec);*/
		boost::system::error_code ec;
		try_read_data(buff,bytes,timeout,ec);
		if(ec)
			throw boost::system::system_error(ec);
	}
	bool try_read_line(std::string& recv_str,boost::posix_time::time_duration timeout,boost::system::error_code& ec)
	{
		// Set a deadline for the asynchronous operation. Since this function uses
		// a composed operation (async_read_until), the deadline applies to the
		// entire operation, rather than individual reads from the socket.
		deadline_.expires_from_now(timeout);

		// Set up the variable that receives the result of the asynchronous
		// operation. The error code is set to would_block to signal that the
		// operation is incomplete. Asio guarantees that its asynchronous
		// operations will never fail with would_block, so any other value in
		// ec indicates completion.
		ec = boost::asio::error::would_block;

		// Start the asynchronous operation itself. The boost::lambda function
		// object is used as a callback and will update the ec variable when the
		// operation completes. The blocking_udp_client.cpp example shows how you
		// can use boost::bind rather than boost::lambda.
		boost::asio::async_read_until(socket_, input_buffer_, '\n', boost::lambda::var(ec) = boost::lambda::_1);

		// Block until the asynchronous operation has completed.
		do io_service_.run_one(); while (ec == boost::asio::error::would_block);

		if (!ec){
			std::istream is(&input_buffer_);
			std::getline(is, recv_str);
			return true;
		}
	}
	std::string read_line(boost::posix_time::time_duration timeout)
	{
		/*deadline_.expires_from_now(timeout);

		boost::system::error_code ec = boost::asio::error::would_block;

		boost::asio::async_read_until(socket_, input_buffer_, '\n', boost::lambda::var(ec) = boost::lambda::_1);

		do io_service_.run_one(); while (ec == boost::asio::error::would_block);

		if (ec)
			throw boost::system::system_error(ec);

		std::string line;
		std::istream is(&input_buffer_);
		std::getline(is, line);
		return line;*/
		std::string line;
		boost::system::error_code ec;
		try_read_line(line,timeout,ec);
		if(ec)
			throw boost::system::system_error(ec);
		return line;
	}

	void try_write_data(const void* data,size_t bytes,
		boost::posix_time::time_duration timeout,boost::system::error_code& ec)
	{
		// Set a deadline for the asynchronous operation. Since this function uses
		// a composed operation (async_write), the deadline applies to the entire
		// operation, rather than individual writes to the socket.
		deadline_.expires_from_now(timeout);

		// Set up the variable that receives the result of the asynchronous
		// operation. The error code is set to would_block to signal that the
		// operation is incomplete. Asio guarantees that its asynchronous
		// operations will never fail with would_block, so any other value in
		// ec indicates completion.
		ec = boost::asio::error::would_block;

		// Start the asynchronous operation itself. The boost::lambda function
		// object is used as a callback and will update the ec variable when the
		// operation completes. The blocking_udp_client.cpp example shows how you
		// can use boost::bind rather than boost::lambda.
		boost::asio::async_write(socket_, boost::asio::buffer(data,bytes), boost::lambda::var(ec) = boost::lambda::_1);

		// Block until the asynchronous operation has completed.
		do io_service_.run_one(); while (ec == boost::asio::error::would_block);

	}

	void write_data(const void* data,size_t bytes,
		boost::posix_time::time_duration timeout)
	{

		/*deadline_.expires_from_now(timeout);

		boost::system::error_code ec = boost::asio::error::would_block;

		boost::asio::async_write(socket_, boost::asio::buffer(data,bytes), boost::lambda::var(ec) = boost::lambda::_1);

		do io_service_.run_one(); while (ec == boost::asio::error::would_block);

		if (ec)
			throw boost::system::system_error(ec);*/

		boost::system::error_code ec;
		try_write_data(data,bytes,timeout,ec);
		if(ec)
			throw boost::system::system_error(ec);
	}

	void write_line(const std::string& line,
		boost::posix_time::time_duration timeout)
	{
		std::string data = line + "\n";
		write_data(data.c_str(),data.length(),timeout);
	}
	void try_connect(const std::string& host, const std::string& service,
		unsigned int  timeout_millisecond,boost::system::error_code& ec)
	{
		try_connect(host,service,boost::posix_time::millisec(timeout_millisecond),ec);
	}
	void		try_read_data(void* buff,size_t bytes,unsigned int  timeout_millisecond,boost::system::error_code& ec)
	{
		try_read_data(buff,bytes,boost::posix_time::millisec(timeout_millisecond),ec);
	}
	bool try_read_line(std::string& recv_str,unsigned int  timeout_millisecond,boost::system::error_code& ec)
	{
		try_read_line(recv_str,boost::posix_time::millisec(timeout_millisecond),ec);
	}
	void try_write_data(const void* data,size_t bytes,
		unsigned int  timeout_millisecond,boost::system::error_code& ec)
	{
		try_write_data(data,bytes,boost::posix_time::millisec(timeout_millisecond),ec);
	}

	void connect(const std::string& host, const std::string& service,unsigned int timeout_millisecond)
	{
		return connect(host,service,boost::posix_time::millisec(timeout_millisecond));
	}
	void read_data(void* buff,size_t bytes,unsigned int timeout_millisecond)
	{
		return read_data(buff,bytes,boost::posix_time::millisec(timeout_millisecond));
	}
	std::string read_line(unsigned int timeout_millisecond)
	{
		return read_line(boost::posix_time::millisec(timeout_millisecond));
	}
	void write_data(const void* data,size_t bytes,unsigned int timeout_millisecond)
	{
		return write_data(data,bytes,boost::posix_time::millisec(timeout_millisecond));
	}
	void write_line(const std::string& line,unsigned int timeout_millisecond)
	{
		return write_line(line,boost::posix_time::millisec(timeout_millisecond));
	}
private:
	void check_deadline()
	{
		// Check whether the deadline has passed. We compare the deadline against
		// the current time since a new asynchronous operation may have moved the
		// deadline before this actor had a chance to run.
		if (deadline_.expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			// The deadline has passed. The socket is closed so that any outstanding
			// asynchronous operations are cancelled. This allows the blocked
			// connect(), read_line() or write_line() functions to return.
			boost::system::error_code ignored_ec;
			socket_.close(ignored_ec);

			// There is no longer an active deadline. The expiry is set to positive
			// infinity so that the actor takes no action until a new deadline is set.
			deadline_.expires_at(boost::posix_time::pos_infin);
		}

		// Put the actor back to sleep.
		deadline_.async_wait(boost::lambda::bind(&block_tcp_client::check_deadline, this));
	}

	boost::asio::io_service io_service_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::deadline_timer deadline_;
	boost::asio::streambuf input_buffer_;
	bool					connected_;
};



typedef boost::shared_ptr<block_tcp_client>		ConnectionPtr;
}
}
#endif//MURADIN_NET_BLOCKING_TCP_CLIENT_H__