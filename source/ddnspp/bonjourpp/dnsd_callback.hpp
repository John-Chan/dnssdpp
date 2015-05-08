#ifndef BONJOUR_DNSD_CALLBACK_FUNC_DEF_H__
#define BONJOUR_DNSD_CALLBACK_FUNC_DEF_H__

#include <boost/function.hpp>

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>
#include <ddnspp/bonjourpp/txtrecords.hpp>
#include <ddnspp/bonjourpp/prototype.hpp>
#include <ddnspp/bonjourpp/nat_status.hpp>


namespace air{namespace bonjour {

	/// Event callback for air::bonjour::LocalService
	typedef boost::function
		<
		void
		(
		DNSServiceFlags,					/// _1 flags
		air::bonjour::BonjourError,			/// _2 error
		std::string,						/// _3 service name
		std::string,						/// _4 service type
		std::string							/// _5 domian
		)
		>		LocalServiceEvtCallback;


	/// Event callback  for air::bonjour::RemoteService
	typedef boost::function
		<
		void
		(
		DNSServiceFlags,					/// _1 flags
		boost::uint32_t,					/// _2 interfaceIndex
		air::bonjour::BonjourError,			/// _3 error
		std::string,						/// _4 service name (for subsequent use in the ServiceResolver )
		std::string,						/// _5 service type
		std::string							/// _6 domian
		)
		>
		RemoteServiceEvtCallback;


	/// Event callback  for air::bonjour::ServiceResolver
	typedef boost::function
		<
		void
		(
		DNSServiceFlags,					/// _1 flags
		boost::uint32_t,					/// _2 interfaceIndex
		air::bonjour::BonjourError,			/// _3 error 
		std::string,						/// _4 service name(fullname, format:<servicename>.<protocol>.<domain>) 
		std::string,						/// _5 host name ,This name can  be passed to functions like gethostbyname() to identify the host's IP address.
		boost::uint16_t,					/// _6 port
		TxtRecordDecoderPtr					/// _7 TxtRecordDecoder (include a copy of dns record)
		)
		>		ServiceResolverEvtCallback;

	/// Event callback  for air::bonjour::AddressResolver
	/// @note   After the TTL expires, the client should consider the result no longer valid
	typedef boost::function
		<
		void
		(
		DNSServiceFlags,			/// _1 flags
		boost::uint32_t,			/// _2 interfaceIndex
		air::bonjour::BonjourError,	/// _3 error
		std::string,				/// _4 hostname the hostname that you ask for rsolve address
		boost::asio::ip::address,	/// _5 address	rsolved address
		boost::uint32_t				/// _6 TTL indicates how long the client may legitimately hold onto this result(address), in seconds.
		)
		>		AddressResolverEvtCallback;


	/// Event callback  for air::bonjour::DomainEumerater
	typedef boost::function
		<
		void
		(
		DNSServiceFlags,					/// _1 flags
		boost::uint32_t,					/// _2 interfaceIndex
		air::bonjour::BonjourError,			/// _3 error
		std::string							/// _4 replyDomain
		)
		>
		EnumerationEvtCallback;


	/// Event callback  for air::bonjour::NatMappingService
	typedef boost::function
		<
		void
		(
		DNSServiceFlags,			/// _1 flags,Currently unused, reserved for future use
		boost::uint32_t,			/// _2 interfaceIndex
		air::bonjour::BonjourError,	/// _3 error
		boost::uint16_t,			/// _6 internal port
		boost::asio::ip::address,	/// _4 external address
		boost::uint16_t,			/// _5 external port,may be different than the requested port
		air::bonjour::ProtoType,	/// _6 protocol used for nat mapping
		boost::uint32_t,			/// _7 TTL , in seconds.indicates The lifetime of the NAT port mapping created on the gateway.
		air::bonjour::NatStatus		/// _8 NatStatus ,used to check nat status if error==true
		)
		>		NatMapingEvtCallback;



}//namespace
}//namespace
#endif//BONJOUR_DNSD_CALLBACK_FUNC_DEF_H__