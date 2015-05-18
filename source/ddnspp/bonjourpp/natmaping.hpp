#ifndef BONJOUR_NAT_MAPING_SERVICE_H
#define BONJOUR_NAT_MAPING_SERVICE_H

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/core.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>
#include <ddnspp/bonjourpp/servicetype.hpp>
#include <ddnspp/bonjourpp/prototype.hpp>
#include <ddnspp/bonjourpp/nat_status.hpp>
#include <ddnspp/bonjourpp/dnsd_callback.hpp>		/// callback func def


#include <ddnspp/logv/logv.hpp>


namespace air
{
namespace bonjour
{

class ServiceFactory;

/// NatService
/// works for nat mapping
class NatMappingService:boost::noncopyable,public boost::enable_shared_from_this<NatMappingService>
{

    friend class ServiceFactory;
private:
    CoreContextPtr		core;
    NatMapingEvtCallback	evtCallback;
public:
    NatMappingService(boost::asio::io_service& ios,const DNSDApi &dll,const NatMapingEvtCallback& func)
        :core(new CoreContext(ios,dll)),evtCallback(func)
    {
    }
    ~NatMappingService()
    {
        //LOG_DEBUG<<"NatMappingService dead";
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
    NatMapingEvtCallback				setEvtCallback(const NatMapingEvtCallback& func)
    {
        NatMapingEvtCallback old=evtCallback;
        evtCallback=func;
        return old;
    }

private:
    static void		DNSSD_API	DNSServiceNATPortMappingReplyCallback(
		DNSServiceRef                    sdRef,
		DNSServiceFlags                  flags,
		uint32_t                         interfaceIndex,
		DNSServiceErrorType              errorCode,
		uint32_t                         externalAddress,   /* four byte IPv4 address in network byte order */
		DNSServiceProtocol               protocol,
		uint16_t                         internalPort,
		uint16_t                         externalPort,      /* may be different than the requested port     */
		uint32_t                         ttl,               /* may be different than the requested ttl      */
		void                             *context
    )
    {

        NatMappingService* thisService=NULL;
        if(context != NULL) {
            thisService=(NatMappingService*)context;
            thisService->evtHanler(sdRef,flags,interfaceIndex,errorCode,externalAddress,protocol,internalPort,externalPort,ttl,context);
        }
    }

    //Possible values are kDNSServiceErr_NoError and kDNSServiceErr_DoubleNAT.
	void	evtHanler(
		DNSServiceRef                    sdRef,
		DNSServiceFlags                  flags,
		uint32_t                         interfaceIndex,
		DNSServiceErrorType              errorCode,
		uint32_t                         externalAddress,   /* four byte IPv4 address in network byte order */
		DNSServiceProtocol               protocol,
		uint16_t                         internalPort,
		uint16_t                         externalPort,      /* may be different than the requested port     */
		uint32_t                         ttl,               /* may be different than the requested ttl      */
		void                             *context
		)
    {


        air::bonjour::BonjourError err(errorCode);
        /*
		flags:Currently unused, reserved for future use
        */

		NatStatus natStatus;
		ProtoType	protoType(protocol);
		boost::asio::ip::address warped_external_address;
		if(!err){
			natStatus.reset(externalAddress,externalPort,internalPort);
			boost::uint32_t be32=air::common::as_be32(externalAddress);
			warped_external_address = boost::asio::ip::address_v4(be32);

			internalPort=air::common::as_be16(internalPort);
			externalPort=air::common::as_be16(externalPort);


		}
        if(err) {
            LOG_ERR<<err.getErrorCode()<<"," <<err.getMessage();
        } else {
            LOG_TRACE<<"flags:"<< flags;
			LOG_TRACE<<"interfaceIndex:"<< interfaceIndex;
			LOG_TRACE<<"internalPort:"<<internalPort;
			//LOG_INFO<<"protoType:"<< protoType.toString();
			LOG_TRACE<<"protoType:"<< protoType.toStringLong();
			LOG_TRACE<<"externalAddress:"<<warped_external_address.to_string();
			LOG_TRACE<<"externalPort:"<<externalPort;
            LOG_TRACE<<"ttl:"<<ttl;
			LOG_TRACE<<"natStatus:"<<natStatus.toString();
			//
        }

        if(evtCallback) {
			air::bonjour::NatMapingData	data;
			data.error			=err;
			data.externalAddr	= warped_external_address;
			data.externalPort	= externalPort;
			data.flags			= flags;
			data.interfaceIndex	= interfaceIndex;
			data.internalPort	= internalPort;
			data.natStatus		= natStatus;
			data.owner			= shared_from_this();
			data.protoType		= protoType;
			data.ttl			= ttl;

            evtCallback(data);

        }
    }
};
typedef	boost::shared_ptr<NatMappingService>	NatMappingServicePtr;

}//namespace
}//namespace
#endif//BONJOUR_NAT_MAPING_SERVICE_H