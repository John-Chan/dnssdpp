#ifndef BONJOUR_DOMIAN_ENUMERATER_H__
#define BONJOUR_DOMIAN_ENUMERATER_H__

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/core.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>
#include <ddnspp/bonjourpp/dnsd_callback.hpp>		/// callback func def

#include <ddnspp/logv/logv.hpp>

namespace air
{
namespace bonjour
{

class ServiceFactory;


/// DomainEumerater
/// A service for Domain Enumeration
class DomainEumerater:boost::noncopyable,public boost::enable_shared_from_this<DomainEumerater>
{
    friend class ServiceFactory;

private:
    CoreContextPtr		core;
    EnumerationEvtCallback	evtCallback;

public:
    DomainEumerater(boost::asio::io_service& ios,const DNSDApi &dll,const EnumerationEvtCallback& func)
        :core(new CoreContext(ios,dll)),evtCallback(func)
    {
    }
    ~DomainEumerater()
    {
    }

    CoreContextPtr		getCoreContext()
    {
        return core;
    }

    /// return old one
    EnumerationEvtCallback	setEvtCallback(const EnumerationEvtCallback& func)
    {
        EnumerationEvtCallback old=evtCallback;
        evtCallback=func;
        return old;
    }

private:
    static void		DNSSD_API	DNSServiceDomainEnumReplyCallback(
        DNSServiceRef                       sdRef,
        DNSServiceFlags                     flags,
        uint32_t                            interfaceIndex,
        DNSServiceErrorType                 errorCode,
        const char                          *replyDomain,
        void                                *context
    )
    {

        DomainEumerater* thisService=NULL;
        if(context != NULL) {
            thisService=(DomainEumerater*)context;
            thisService->evtHanler(sdRef,flags,interfaceIndex,errorCode,replyDomain,context);
        }

    }

    // flags: Possible values are:
    // kDNSServiceFlagsMoreComing
    // kDNSServiceFlagsAdd
    // kDNSServiceFlagsDefault
    void	evtHanler(
        DNSServiceRef                       sdRef,
        DNSServiceFlags                     flags,
        uint32_t                            interfaceIndex,
        DNSServiceErrorType                 errorCode,
        const char                          *replyDomain,
        void                                *context)
    {
        air::bonjour::BonjourError err(errorCode);
        /*bool moreData=(flags & kDNSServiceFlagsMoreComing);
        if (moreData) return;*/

        if(err) {
            LOG_ERR<<err.getErrorCode()<<"," <<err.getMessage();
        } else {

            if (flags & kDNSServiceFlagsAdd) {
                LOG_TRACE<<"Domain Enumeration found ";
                LOG_TRACE<<"flags:"<< flags;
                LOG_TRACE<<"interfaceIndex:"<< interfaceIndex;
                LOG_TRACE<<"replyDomain:"<< replyDomain;
            }
        }

        if(evtCallback) {
			evtCallback(
				shared_from_this(),
                flags,
                interfaceIndex,
                err,
                replyDomain
            );
        }
    }
};
typedef	boost::shared_ptr<DomainEumerater>	DomainEumeraterPtr;



}
}
#endif // BONJOUR_DOMIAN_ENUMERATER_H__