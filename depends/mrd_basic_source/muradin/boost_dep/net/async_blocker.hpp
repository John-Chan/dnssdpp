#ifndef MURADIN_NET_ASYNC_BLOCKER_HPP__
#define MURADIN_NET_ASYNC_BLOCKER_HPP__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
//#include <cstdlib>
//#include <iostream>
//#include <string>
//#include <boost/lambda/bind.hpp>
//#include <boost/lambda/lambda.hpp>

namespace muradin {namespace net{

	template<typename AsyncStream>
	class async_blocker
	{
	public:
		async_blocker(AsyncStream &s,boost::asio::io_service& io_service)
			:io_service_(io_service),deadline_(io_service),async_stream_(s)
		{
			// No deadline is required until the first socket operation is started. We
			// set the deadline to positive infinity so that the actor takes no action
			// until a specific deadline is set.
			deadline_.expires_at(boost::posix_time::pos_infin);

			// Start the persistent actor that checks for deadline expiry.
			check_deadline();
		}
		boost::asio::io_service& get_ioservice()
		{
			return io_service_;
		}
		template<typename ConstBufferSequence>
			void  write_certain_bytes(
			const ConstBufferSequence & buffers,
			size_t	certain_bytes,
			boost::posix_time::time_duration timeout,
			boost::system::error_code& ec)
		{
			// Set a deadline for the asynchronous operation.
			deadline_.expires_from_now(timeout);

			// Set up the variables that receive the result of the asynchronous
			// operation. The error code is set to would_block to signal that the
			// operation is incomplete. Asio guarantees that its asynchronous
			// operations will never fail with would_block, so any other value in
			// ec indicates completion.
			ec = boost::asio::error::would_block;
			std::size_t length = 0;

			// Start the asynchronous operation itself. The handle_receive function
			// used as a callback will update the ec and length variables.
			//socket_.async_receive(boost::asio::buffer(buffer),
			//	boost::bind(&client::handle_receive, _1, _2, &ec, &length));

			//boost::asio::async_write(s, boost::asio::buffer(data, size), handler);
			boost::asio::async_write(async_stream_,
				buffers,
				boost::bind(&async_blocker::handle_io_completion, _1, _2, &ec, &length));

			// Block until the asynchronous operation has completed.
			do io_service_.run_one(); while (ec == boost::asio::error::would_block);
		}
		template<typename MutableBufferSequence>
			void read_certain_bytes(
			const MutableBufferSequence & buffers,
			size_t	certain_bytes,
			boost::posix_time::time_duration timeout,
			boost::system::error_code& ec)
		{
			// Set a deadline for the asynchronous operation.
			deadline_.expires_from_now(timeout);

			// Set up the variables that receive the result of the asynchronous
			// operation. The error code is set to would_block to signal that the
			// operation is incomplete. Asio guarantees that its asynchronous
			// operations will never fail with would_block, so any other value in
			// ec indicates completion.
			ec = boost::asio::error::would_block;
			std::size_t length = 0;

			// Start the asynchronous operation itself. The handle_receive function
			// used as a callback will update the ec and length variables.
			//socket_.async_receive(boost::asio::buffer(buffer),
			//	boost::bind(&client::handle_receive, _1, _2, &ec, &length));

			//boost::asio::async_read(s, boost::asio::buffer(buff,bytes), boost::lambda::var(ec) = boost::lambda::_1);
			boost::asio::async_read(async_stream_,
				buffers,
				boost::asio::transfer_at_least(certain_bytes),
				boost::bind(&async_blocker::handle_io_completion, _1, _2, &ec, &length));

			// Block until the asynchronous operation has completed.
			do io_service_.run_one(); while (ec == boost::asio::error::would_block);
		}
		

	private:
		static void handle_io_completion(
			const boost::system::error_code& ec, std::size_t length,
			boost::system::error_code* out_ec, std::size_t* out_length)
		{
			*out_ec = ec;
			*out_length = length;
		}
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
				async_stream_.close(ignored_ec);

				// There is no longer an active deadline. The expiry is set to positive
				// infinity so that the actor takes no action until a new deadline is set.
				deadline_.expires_at(boost::posix_time::pos_infin);
			}

			// Put the actor back to sleep.
			deadline_.async_wait(boost::lambda::bind(&async_blocker::check_deadline, this));
		}
	private:
		boost::asio::io_service& io_service_;
		boost::asio::deadline_timer deadline_;
		AsyncStream&	async_stream_;
	};
}}
#endif//MURADIN_NET_ASYNC_BLOCKER_HPP__