#ifndef BONJOUR_ADDRESS_RESOLVER_HPP__
#define BONJOUR_ADDRESS_RESOLVER_HPP__


#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <ddnspp/bonjourpp/dnsd_callback.hpp>		/// callback func def

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/core.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>
#include <ddnspp/bonjourpp/servicetype.hpp>
#include <ddnspp/logv/logv.hpp>

namespace air
{
namespace bonjour
{

class ServiceFactory;


/// AddressResolver
/// Queries for the IP address of a hostname by using either Multicast or Unicast DNS
class AddressResolver:boost::noncopyable,public boost::enable_shared_from_this<AddressResolver>
{

    friend class ServiceFactory;
private:
    CoreContextPtr		core;
    AddressResolverEvtCallback	evtCallback;
public:
    AddressResolver(boost::asio::io_service& ios,const DNSDApi &dll,const AddressResolverEvtCallback& func)
        :core(new CoreContext(ios,dll)),evtCallback(func)
    {
    }
    ~AddressResolver()
    {
        //LOG_DEBUG<<"AddressResolver dead";
    }

    CoreContextPtr		getCoreContext()
    {
        return core;
	}

	void	close()
	{
		core->close();
	}

    /// return old one
    AddressResolverEvtCallback				setEvtCallback(const AddressResolverEvtCallback& func)
    {
        AddressResolverEvtCallback old=evtCallback;
        evtCallback=func;
        return old;
    }

private:
    static void		DNSSD_API	DNSServiceGetAddrInfoReplyCallback(
        DNSServiceRef                    sdRef,
        DNSServiceFlags                  flags,
        uint32_t                         interfaceIndex,
        DNSServiceErrorType              errorCode,
        const char                       *hostname,
        const struct sockaddr            *address,
        uint32_t                         ttl,
        void                             *context
    )
    {

        AddressResolver* thisService=NULL;
        if(context != NULL) {
            thisService=(AddressResolver*)context;
            thisService->evtHanler(sdRef,flags,interfaceIndex,errorCode,hostname,address,ttl,context);
        }
    }

    //Possible values are kDNSServiceFlagsMoreComing and kDNSServiceFlagsAdd.
    void	evtHanler(
        DNSServiceRef                    sdRef,
        DNSServiceFlags                  flags,
        uint32_t                         interfaceIndex,
        DNSServiceErrorType              errorCode,
        const char                       *hostname,
        const struct sockaddr            *address,
        uint32_t                         ttl,
        void                             *context)
    {

        air::bonjour::BonjourError err(errorCode);
        /*
        bool moreData=(flags & kDNSServiceFlagsMoreComing);
        if (moreData) return;
        */

        boost::asio::ip::address warped_address;
        if(!err && NULL != address) {
            if (AF_INET == address->sa_family ) {
                boost::uint32_t be32=air::common::as_be32(reinterpret_cast<const struct sockaddr_in*>(address)->sin_addr.s_addr);
                warped_address = boost::asio::ip::address_v4(be32);
            } else if (AF_INET6 == address->sa_family) {
                boost::asio::ip::address_v6::bytes_type addr_bytes;
                for (int i=0; i< 16; i++) {
                    addr_bytes[i] = reinterpret_cast<const struct sockaddr_in6*>(address)->sin6_addr.s6_addr[i];
                }
                warped_address=boost::asio::ip::address_v6 (
                                   addr_bytes,
                                   reinterpret_cast<const struct sockaddr_in6*>(address)->sin6_scope_id);

            } else {
                //bug here!
            }
        }
        if(err) {
            LOG_ERR<<err.getErrorCode()<<"," <<err.getMessage();
        } else {
            if (flags & kDNSServiceFlagsAdd) {
                LOG_TRACE<<"Addres added";
            } else {
                LOG_TRACE<<"Addres removed";
            }
            LOG_TRACE<<"flags:"<< flags;
            LOG_TRACE<<"interfaceIndex:"<< interfaceIndex;
            LOG_TRACE<<"hostname:"<< hostname;
            LOG_TRACE<<"address:"<<warped_address;
            LOG_TRACE<<"ttl:"<<ttl;
        }
        if(evtCallback) {
			air::bonjour::AddressResolveData	data;
			data.addr			=	warped_address;
			data.error			=	err;
			data.flags			=	flags;
			data.hostName		=	hostname;
			data.interfaceIndex	=	interfaceIndex;
			data.ttl			=	ttl;
			data.owner			=	shared_from_this();

			evtCallback(data);

        }
    }
};
typedef	boost::shared_ptr<AddressResolver>	AddressResolverPtr;


}
}
#endif // BONJOUR_ADDRESS_RESOLVER_HPP__