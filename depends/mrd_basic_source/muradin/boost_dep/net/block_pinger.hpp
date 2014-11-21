#ifndef MURADIN_NET_ICMP_PING_HPP__
#define MURADIN_NET_ICMP_PING_HPP__

#include <muradin/boost_dep/net/ipv4_header.hpp>
#include <muradin/boost_dep/net/icmp_header.hpp>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <istream>
#include <iostream>
#include <ostream>

namespace muradin {namespace net{

	class blocking_pinger
	{
	public:

		blocking_pinger(const char* destination,size_t timeout_millisecond)
			: io_service_(),
			resolver_(io_service_), 
			socket_(io_service_, boost::asio::ip::icmp::v4()),
			timer_(io_service_), 
			sequence_number_(0), 
			num_replies_(0),
			timeout_millisecond_(timeout_millisecond)
		{
			boost::asio::ip::icmp::resolver::query query(boost::asio::ip::icmp::v4(), destination, "");
			destination_ = *resolver_.resolve(query);

			timer_.expires_at(boost::posix_time::pos_infin);

			// Start the persistent actor that checks for deadline expiry.
			check_deadline();

		}

		void	ping( muradin::net::icmp_header& icmp_hdr, muradin::net::ipv4_header& ipv4_hdr,size_t& time_cost_ms,size_t& recv_data_bytes,boost::system::error_code& ec)
		{

			bool got_good_rsp=false;
			time_sent_ = boost::posix_time::microsec_clock::universal_time();
			send(ec);
			while(!ec ){

				boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
				time_cost_ms=(size_t)((now - time_sent_).total_milliseconds());
				if( time_cost_ms > timeout_millisecond_){
					break;
				}
				do_recv(reply_buffer_,ec,recv_data_bytes);
				if(!ec && handle_receive(recv_data_bytes,icmp_hdr,ipv4_hdr)){
					break;
				}
			}
			
			ec=boost::asio::error::timed_out;
			//ec=boost::system::errc::timed_out;
			
		}
		void print_rsp(const muradin::net::icmp_header& icmp_hdr,const muradin::net::ipv4_header& ipv4_hdr,size_t time_cost_ms,size_t length)
		{
			std::cout << length - ipv4_hdr.header_length()
				<< " bytes from " << ipv4_hdr.source_address()
				<< ": icmp_seq=" << icmp_hdr.sequence_number()
				<< ", ttl=" << ipv4_hdr.time_to_live()
				<< ", time=" << time_cost_ms << " ms"
				<< std::endl;
		}

	private:

		void	do_send(const boost::asio::streambuf& buff,boost::system::error_code& ec,size_t& bytes)
		{
			//
			timer_.expires_from_now( boost::posix_time::millisec(timeout_millisecond_));

			ec = boost::asio::error::would_block;
			socket_.async_send_to(buff.data(), destination_,
				boost::bind(&blocking_pinger::handle_io_completion, _1, _2, &ec, &bytes));
			// Block until the asynchronous operation has completed.
			do io_service_.run_one(); while (ec == boost::asio::error::would_block);
		}

		void	do_recv(boost::asio::streambuf& buff,boost::system::error_code& ec,size_t& bytes)
		{
			//
			timer_.expires_from_now( boost::posix_time::millisec(timeout_millisecond_));

			ec = boost::asio::error::would_block;

			// Wait for a reply. We prepare the buffer to receive up to 64KB.
			socket_.async_receive(buff.prepare(65536),
				boost::bind(&blocking_pinger::handle_io_completion, _1, _2, &ec, &bytes));
			// Block until the asynchronous operation has completed.
			do io_service_.run_one(); while (ec == boost::asio::error::would_block);
		}


		void send(boost::system::error_code& ec)
		{
			std::string body("blocking_pinger");

			// Create an ICMP header for an echo request.
			icmp_header echo_request;
			echo_request.type(icmp_header::echo_request);
			echo_request.code(0);
			echo_request.identifier(get_identifier());
			echo_request.sequence_number(++sequence_number_);
			compute_checksum(echo_request, body.begin(), body.end());

			// Encode the request packet.
			boost::asio::streambuf request_buffer;
			std::ostream os(&request_buffer);
			os << echo_request << body;

			// Send the request.
			size_t bytes_trans=0;
			do_send(request_buffer,ec,bytes_trans);
		}



		bool handle_receive(std::size_t length, muradin::net::icmp_header& icmp_hdr, muradin::net::ipv4_header& ipv4_hdr)
		{
			// The actual number of bytes received is committed to the buffer so that we
			// can extract it using a std::istream object.
			reply_buffer_.commit(length);

			// Decode the reply packet.
			std::istream is(&reply_buffer_);
			is >> ipv4_hdr >> icmp_hdr;

			// We can receive all ICMP packets received by the host, so we need to
			// filter out only the echo replies that match the our identifier and
			// expected sequence number.
			if (is && icmp_hdr.type() == icmp_header::echo_reply
				&& icmp_hdr.identifier() == get_identifier()
				&& icmp_hdr.sequence_number() == sequence_number_)
			{
				return true;
			}
			else
			{
#ifdef _DEBUG
				std::cerr<<"Got wrong ICMP respons"  <<std::endl;
#endif // _DEBUG
				return false;
			}

		}
		void check_deadline()
		{
			// Check whether the deadline has passed. We compare the deadline against
			// the current time since a new asynchronous operation may have moved the
			// deadline before this actor had a chance to run.
			if (timer_.expires_at() <= boost::asio::deadline_timer::traits_type::now())
			{
				// The deadline has passed. The socket is closed so that any outstanding
				// asynchronous operations are cancelled. This allows the blocked
				// connect(), read_line() or write_line() functions to return.
				boost::system::error_code ignored_ec;
				socket_.close(ignored_ec);

				// There is no longer an active deadline. The expiry is set to positive
				// infinity so that the actor takes no action until a new deadline is set.
				timer_.expires_at(boost::posix_time::pos_infin);
			}

			// Put the actor back to sleep.
			timer_.async_wait(boost::bind(&blocking_pinger::check_deadline, this));
		}

		static void handle_io_completion(
			const boost::system::error_code& ec, std::size_t length,
			boost::system::error_code* out_ec, std::size_t* out_length)
		{
			*out_ec = ec;
			*out_length = length;
		}
		static unsigned short get_identifier()
		{
#if defined(BOOST_ASIO_WINDOWS)
			return static_cast<unsigned short>(::GetCurrentProcessId());
#else
			return static_cast<unsigned short>(::getpid());
#endif
		}
	private:
		boost::asio::io_service io_service_;
		boost::asio::ip::icmp::resolver resolver_;
		boost::asio::ip::icmp::endpoint destination_;
		boost::asio::ip::icmp::socket socket_;
		//async_blocker<boost::asio::ip::icmp::socket>	blocker_;
		
		boost::asio::deadline_timer timer_;
		unsigned short sequence_number_;
		boost::posix_time::ptime time_sent_;
		boost::asio::streambuf reply_buffer_;
		std::size_t num_replies_;
		size_t timeout_millisecond_;

	};

}}

#endif//MURADIN_NET_ICMP_PING_HPP__
