#ifndef BONJOUR_DNSD_CALLBACK_FUNC_DEF_H__
#define BONJOUR_DNSD_CALLBACK_FUNC_DEF_H__

#include <boost/function.hpp>

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>
#include <ddnspp/bonjourpp/txtrecords.hpp>
#include <ddnspp/bonjourpp/prototype.hpp>
#include <ddnspp/bonjourpp/nat_status.hpp>
#include <boost/shared_ptr.hpp>


namespace air{namespace bonjour {

	class NatMappingService;
	class DomainEumerater;
	class AddressResolver;
	class ServiceResolver;
	class RemoteService;
	class LocalService;
	

	/// Event callback  for air::bonjour::DomainEumerater
	/* 
	 *
	 * Asynchronously enumerate domains available for browsing and registration.
	 *
	 * The enumeration MUST be cancelled via DNSServiceRefDeallocate() when no more domains
	 * are to be found.
	 *
	 * Note that the names returned are (like all of DNS-SD) UTF-8 strings,
	 * and are escaped using standard DNS escaping rules.
	 * (See "Notes on DNS Name Escaping" earlier in this file for more details.)
	 * A graphical browser displaying a hierarchical tree-structured view should cut
	 * the names at the bare dots to yield individual labels, then de-escape each
	 * label according to the escaping rules, and then display the resulting UTF-8 text.
	 *
	 * Parameters:
	 *
	 *
	 * flags:           Possible values are:
	 *                  kDNSServiceFlagsMoreComing
	 *                  kDNSServiceFlagsAdd
	 *                  kDNSServiceFlagsDefault
	 *
	 * interfaceIndex:  Specifies the interface on which the domain exists. (The index for a given
	 *                  interface is determined via the if_nametoindex() family of calls.)
	 *
	 * errorCode:       Will be kDNSServiceErr_NoError (0) on success, otherwise indicates
	 *                  the failure that occurred (other parameters are undefined if errorCode is nonzero).
	 *
	 * replyDomain:     The name of the domain.
	 *
	 * @note The enumeration MUST be cancelled via DomainEumeraterPtr->close() when no more domains  are to be found.
	 *
	 */
	typedef boost::function
		<
		void
		(
		boost::shared_ptr<air::bonjour::DomainEumerater>,	/// _1 the owner service  
		DNSServiceFlags,					/// _2 flags
		boost::uint32_t,					/// _3 interfaceIndex
		air::bonjour::BonjourError,			/// _4 error
		std::string							/// _5 replyDomain
		)
		>
		EnumerationEvtCallback;



	/// Event callback for air::bonjour::LocalService
	/* 
	 * Parameters:
	 *
	 *
	 * flags:           When a name is successfully registered, the callback will be
	 *                  invoked with the kDNSServiceFlagsAdd flag set. When Wide-Area
	 *                  DNS-SD is in use, it is possible for a single service to get
	 *                  more than one success callback (e.g. one in the "local" multicast
	 *                  DNS domain, and another in a wide-area unicast DNS domain).
	 *                  If a successfully-registered name later suffers a name conflict
	 *                  or similar problem and has to be deregistered, the callback will
	 *                  be invoked with the kDNSServiceFlagsAdd flag not set. The callback
	 *                  is *not* invoked in the case where the caller explicitly terminates
	 *                  the service registration by calling DNSServiceRefDeallocate(ref);
	 *
	 * errorCode:       Will be kDNSServiceErr_NoError on success, otherwise will
	 *                  indicate the failure that occurred (including name conflicts,
	 *                  if the kDNSServiceFlagsNoAutoRename flag was used when registering.)
	 *                  Other parameters are undefined if errorCode is nonzero.
	 *
	 * name:            The service name registered (if the application did not specify a name in
	 *                  DNSServiceRegister(), this indicates what name was automatically chosen).
	 *
	 * regtype:         The type of service registered, as it was passed to the callout.
	 *
	 * domain:          The domain on which the service was registered (if the application did not
	 *                  specify a domain in DNSServiceRegister(), this indicates the default domain
	 *                  on which the service was registered).
	 *
	 *
	 */
	typedef boost::function
		<
		void
		(
		boost::shared_ptr<air::bonjour::LocalService>,		/// _1 the owner service 
		DNSServiceFlags,					/// _2 flags
		air::bonjour::BonjourError,			/// _3 errorCode
		std::string,						/// _4 service name
		std::string,						/// _5 service type
		std::string							/// _6 domian
		)
		>		LocalServiceEvtCallback;


	/// Event callback  for air::bonjour::RemoteService
	/* Parameters:
	 *
	 *
	 * flags:           Possible values are kDNSServiceFlagsMoreComing and kDNSServiceFlagsAdd.
	 *                  See flag definitions for details.
	 *
	 * interfaceIndex:  The interface on which the service is advertised. This index should
	 *                  be passed to DNSServiceResolve() when resolving the service.
	 *
	 * errorCode:       Will be kDNSServiceErr_NoError (0) on success, otherwise will
	 *                  indicate the failure that occurred. Other parameters are undefined if
	 *                  the errorCode is nonzero.
	 *
	 * serviceName:     The discovered service name. This name should be displayed to the user,
	 *                  and stored for subsequent use in the DNSServiceResolve() call.
	 *
	 * serviceType:     The service type, which is usually (but not always) the same as was passed
	 *                  to DNSServiceBrowse(). One case where the discovered service type may
	 *                  not be the same as the requested service type is when using subtypes:
	 *                  The client may want to browse for only those ftp servers that allow
	 *                  anonymous connections. The client will pass the string "_ftp._tcp,_anon"
	 *                  to DNSServiceBrowse(), but the type of the service that's discovered
	 *                  is simply "_ftp._tcp". The regtype for each discovered service instance
	 *                  should be stored along with the name, so that it can be passed to
	 *                  DNSServiceResolve() when the service is later resolved.
	 *
	 * domain:          The domain of the discovered service instance. This may or may not be the
	 *                  same as the domain that was passed to DNSServiceBrowse(). The domain for each
	 *                  discovered service instance should be stored along with the name, so that
	 *                  it can be passed to DNSServiceResolve() when the service is later resolved.
	 *
	 * @note When the desired results have been returned, the client MUST terminate the resolve by calling ServiceResolverPtr->close() 
	 *
	 */
	typedef boost::function
		<
		void
		(
		boost::shared_ptr<air::bonjour::RemoteService>,		/// _1 the owner service 
		DNSServiceFlags,					/// _2 flags
		boost::uint32_t,					/// _3 interfaceIndex
		air::bonjour::BonjourError,			/// _4 error
		std::string,						/// _5 service name (for subsequent use in the ServiceResolver )
		std::string,						/// _6 service type
		std::string							/// _7 domian
		)
		>
		RemoteServiceEvtCallback;


	/// Event callback  for air::bonjour::ServiceResolver
	/* 
	 *
	 * Resolve a service name discovered via DNSServiceBrowse() to a target host name, port number, and
	 * txt record.
	 *
	 * Note: Applications should NOT use DNSServiceResolve() solely for txt record monitoring - use
	 * DNSServiceQueryRecord() instead, as it is more efficient for this task.
	 *
	 * Note: When the desired results have been returned, the client MUST terminate the resolve by calling
	 * DNSServiceRefDeallocate().
	 *
	 * Note: DNSServiceResolve() behaves correctly for typical services that have a single SRV record
	 * and a single TXT record. To resolve non-standard services with multiple SRV or TXT records,
	 * DNSServiceQueryRecord() should be used.
	 *
	 * Parameters:
	 *
	 *
	 * flags:           Possible values: kDNSServiceFlagsMoreComing
	 *
	 * interfaceIndex:  The interface on which the service was resolved.
	 *
	 * errorCode:       Will be kDNSServiceErr_NoError (0) on success, otherwise will
	 *                  indicate the failure that occurred. Other parameters are undefined if
	 *                  the errorCode is nonzero.
	 *
	 * fullname:        The full service domain name, in the form <servicename>.<protocol>.<domain>.
	 *                  (This name is escaped following standard DNS rules, making it suitable for
	 *                  passing to standard system DNS APIs such as res_query(), or to the
	 *                  special-purpose functions included in this API that take fullname parameters.
	 *                  See "Notes on DNS Name Escaping" earlier in this file for more details.)
	 *
	 * hosttarget:      The target hostname of the machine providing the service. This name can
	 *                  be passed to functions like gethostbyname() to identify the host's IP address.
	 *
	 * port:            The port, in network byte order, on which connections are accepted for this service.
	 *
	 * txtRecord:       The service's primary txt record, in standard txt record format.
	 *
	 * NOTE: In earlier versions of this header file, the txtRecord parameter was declared "const char *"
	 * This is incorrect, since it contains length bytes which are values in the range 0 to 255, not -128 to +127.
	 * Depending on your compiler settings, this change may cause signed/unsigned mismatch warnings.
	 * These should be fixed by updating your own callback function definition to match the corrected
	 * function signature using "const unsigned char *txtRecord". Making this change may also fix inadvertent
	 * bugs in your callback function, where it could have incorrectly interpreted a length byte with value 250
	 * as being -6 instead, with various bad consequences ranging from incorrect operation to software crashes.
	 * If you need to maintain portable code that will compile cleanly with both the old and new versions of
	 * this header file, you should update your callback function definition to use the correct unsigned value,
	 * and then in the place where you pass your callback function to DNSServiceResolve(), use a cast to eliminate
	 * the compiler warning, e.g.:
	 *   DNSServiceResolve(sd, flags, index, name, regtype, domain, (DNSServiceResolveReply)MyCallback, context);
	 * This will ensure that your code compiles cleanly without warnings (and more importantly, works correctly)
	 * with both the old header and with the new corrected version.
	 *
	 */
	typedef boost::function
		<
		void
		(
		boost::shared_ptr<air::bonjour::ServiceResolver>,	/// _1 the owner service 
		DNSServiceFlags,					/// _2 flags
		boost::uint32_t,					/// _3 interfaceIndex
		air::bonjour::BonjourError,			/// _4 error 
		std::string,						/// _5 service name(fullname, format:<servicename>.<protocol>.<domain>) 
		std::string,						/// _6 host name ,This name can  be passed to functions like gethostbyname() to identify the host's IP address.
		boost::uint16_t,					/// _7 port
		TxtRecordDecoderPtr					/// _8 TxtRecordDecoder (include a copy of dns record)
		)
		>		ServiceResolverEvtCallback;

	/// Event callback  for air::bonjour::AddressResolver
	/* 
	 *
	 * Queries for the IP address of a hostname by using either Multicast or Unicast DNS.
	 *
	 * parameters:
	 *
	 *
	 * flags:           Possible values are kDNSServiceFlagsMoreComing and
	 *                  kDNSServiceFlagsAdd.
	 *
	 * interfaceIndex:  The interface to which the answers pertain.
	 *
	 * errorCode:       Will be kDNSServiceErr_NoError on success, otherwise will
	 *                  indicate the failure that occurred.  Other parameters are
	 *                  undefined if errorCode is nonzero.
	 *
	 * hostname:        The fully qualified domain name of the host to be queried for.
	 *
	 * address:         IPv4 or IPv6 address.
	 *
	 * ttl:             If the client wishes to cache the result for performance reasons,
	 *                  the TTL indicates how long the client may legitimately hold onto
	 *                  this result, in seconds. After the TTL expires, the client should
	 *                  consider the result no longer valid, and if it requires this data
	 *                  again, it should be re-fetched with a new query. Of course, this
	 *                  only applies to clients that cancel the asynchronous operation when
	 *                  they get a result. Clients that leave the asynchronous operation
	 *                  running can safely assume that the data remains valid until they
	 *                  get another callback telling them otherwise.
	 *
	 *
	 */
	typedef boost::function
		<
		void
		(
		boost::shared_ptr<air::bonjour::AddressResolver>,	/// _1 the owner service 
		DNSServiceFlags,			/// _2 flags
		boost::uint32_t,			/// _3 interfaceIndex
		air::bonjour::BonjourError,	/// _4 error
		std::string,				/// _5 hostname the hostname that you ask for rsolve address
		boost::asio::ip::address,	/// _6 address	rsolved address
		boost::uint32_t				/// _7 TTL indicates how long the client may legitimately hold onto this result(address), in seconds.
		)
		>		AddressResolverEvtCallback;

	
	/// Event callback  for air::bonjour::NatMappingService
	/* 
	 *
	 * Request a port mapping in the NAT gateway, which maps a port on the local machine
	 * to an external port on the NAT.
	 *
	 * The port mapping will be renewed indefinitely until the client process exits, or
	 * explicitly terminates the port mapping request by calling DNSServiceRefDeallocate().
	 * The client callback will be invoked, informing the client of the NAT gateway's
	 * external IP address and the external port that has been allocated for this client.
	 * The client should then record this external IP address and port using whatever
	 * directory service mechanism it is using to enable peers to connect to it.
	 * (Clients advertising services using Wide-Area DNS-SD DO NOT need to use this API
	 * -- when a client calls DNSServiceRegister() NAT mappings are automatically created
	 * and the external IP address and port for the service are recorded in the global DNS.
	 * Only clients using some directory mechanism other than Wide-Area DNS-SD need to use
	 * this API to explicitly map their own ports.)
	 *
	 * It's possible that the client callback could be called multiple times, for example
	 * if the NAT gateway's IP address changes, or if a configuration change results in a
	 * different external port being mapped for this client. Over the lifetime of any long-lived
	 * port mapping, the client should be prepared to handle these notifications of changes
	 * in the environment, and should update its recorded address and/or port as appropriate.
	 *
	 * NOTE: There are two unusual aspects of how the DNSServiceNATPortMappingCreate API works,
	 * which were intentionally designed to help simplify client code:
	 *
	 *  1. It's not an error to request a NAT mapping when the machine is not behind a NAT gateway.
	 *     In other NAT mapping APIs, if you request a NAT mapping and the machine is not behind a NAT
	 *     gateway, then the API returns an error code -- it can't get you a NAT mapping if there's no
	 *     NAT gateway. The DNSServiceNATPortMappingCreate API takes a different view. Working out
	 *     whether or not you need a NAT mapping can be tricky and non-obvious, particularly on
	 *     a machine with multiple active network interfaces. Rather than make every client recreate
	 *     this logic for deciding whether a NAT mapping is required, the PortMapping API does that
	 *     work for you. If the client calls the PortMapping API when the machine already has a
	 *     routable public IP address, then instead of complaining about it and giving an error,
	 *     the PortMapping API just invokes your callback, giving the machine's public address
	 *     and your own port number. This means you don't need to write code to work out whether
	 *     your client needs to call the PortMapping API -- just call it anyway, and if it wasn't
	 *     necessary, no harm is done:
	 *
	 *     - If the machine already has a routable public IP address, then your callback
	 *       will just be invoked giving your own address and port.
	 *     - If a NAT mapping is required and obtained, then your callback will be invoked
	 *       giving you the external address and port.
	 *     - If a NAT mapping is required but not obtained from the local NAT gateway,
	 *       or the machine has no network connectivity, then your callback will be
	 *       invoked giving zero address and port.
	 *
	 *  2. In other NAT mapping APIs, if a laptop computer is put to sleep and woken up on a new
	 *     network, it's the client's job to notice this, and work out whether a NAT mapping
	 *     is required on the new network, and make a new NAT mapping request if necessary.
	 *     The DNSServiceNATPortMappingCreate API does this for you, automatically.
	 *     The client just needs to make one call to the PortMapping API, and its callback will
	 *     be invoked any time the mapping state changes. This property complements point (1) above.
	 *     If the client didn't make a NAT mapping request just because it determined that one was
	 *     not required at that particular moment in time, the client would then have to monitor
	 *     for network state changes to determine if a NAT port mapping later became necessary.
	 *     By unconditionally making a NAT mapping request, even when a NAT mapping not to be
	 *     necessary, the PortMapping API will then begin monitoring network state changes on behalf of
	 *     the client, and if a NAT mapping later becomes necessary, it will automatically create a NAT
	 *     mapping and inform the client with a new callback giving the new address and port information.
	 *
	 * parameters:
	 *
	 *
	 * flags:           Currently unused, reserved for future use.
	 *
	 * interfaceIndex:  The interface through which the NAT gateway is reached.
	 *
	 * errorCode:       Will be kDNSServiceErr_NoError on success.
	 *                  Will be kDNSServiceErr_DoubleNAT when the NAT gateway is itself behind one or
	 *                  more layers of NAT, in which case the other parameters have the defined values.
	 *                  For other failures, will indicate the failure that occurred, and the other
	 *                  parameters are undefined.
	 *
	 * externalAddress: Four byte IPv4 address in network byte order.
	 *
	 * protocol:        Will be kDNSServiceProtocol_UDP or kDNSServiceProtocol_TCP or both.
	 *
	 * internalPort:    The port on the local machine that was mapped.
	 *
	 * externalPort:    The actual external port in the NAT gateway that was mapped.
	 *                  This is likely to be different than the requested external port.
	 *
	 * ttl:             The lifetime of the NAT port mapping created on the gateway.
	 *                  This controls how quickly stale mappings will be garbage-collected
	 *                  if the client machine crashes, suffers a power failure, is disconnected
	 *                  from the network, or suffers some other unfortunate demise which
	 *                  causes it to vanish without explicitly removing its NAT port mapping.
	 *                  It's possible that the ttl value will differ from the requested ttl value.
	 *
	 *
	 */
	typedef boost::function
		<
		void
		(
		boost::shared_ptr<air::bonjour::NatMappingService>,	/// _1 the owner service 
		DNSServiceFlags,			/// _2 flags,Currently unused, reserved for future use
		boost::uint32_t,			/// _3 interfaceIndex
		air::bonjour::BonjourError,	/// _4 error
		boost::uint16_t,			/// _5 internal port
		boost::asio::ip::address,	/// _6 external address
		boost::uint16_t,			/// _7 external port,may be different than the requested port
		air::bonjour::ProtoType,	/// _8 protocol used for nat mapping
		boost::uint32_t,			/// _9 TTL , in seconds.indicates The lifetime of the NAT port mapping created on the gateway.
		air::bonjour::NatStatus		/// _10 NatStatus ,used to check nat status if error==true
		)
		>		NatMapingEvtCallback;



}//namespace
}//namespace
#endif//BONJOUR_DNSD_CALLBACK_FUNC_DEF_H__