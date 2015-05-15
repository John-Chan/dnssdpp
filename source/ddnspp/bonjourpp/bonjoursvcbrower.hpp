#ifndef BONJOURSERVICE_BROWER_H
#define BONJOURSERVICE_BROWER_H

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <ddnspp/bonjourpp/dnsd_callback.hpp>		/// callback func def

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/core.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>

#include <ddnspp/logv/logv.hpp>

namespace air
{
namespace bonjour
{

class ServiceFactory;


/// RemoteService
/// A service which is found from network(e.g. via ServiceFactory.createServiceBrower)
class RemoteService:boost::noncopyable,public boost::enable_shared_from_this<RemoteService>
{
    friend class ServiceFactory;
private:
    CoreContextPtr		core;
    RemoteServiceEvtCallback	evtCallback;

public:
    RemoteService(boost::asio::io_service& ios,const DNSDApi &dll,const RemoteServiceEvtCallback& func)
        :core(new CoreContext(ios,dll)),evtCallback(func)
    {
    }
    ~RemoteService()
    {
    }

    CoreContextPtr		getCoreContext()
    {
        return core;
    }

    /// return old one
    RemoteServiceEvtCallback				setEvtCallback(const RemoteServiceEvtCallback& func)
    {
        RemoteServiceEvtCallback old=evtCallback;
        evtCallback=func;
        return old;
    }

private:
    static void		DNSSD_API	DNSServiceBrowseReplyCallback(
        DNSServiceRef                       sdRef,
        DNSServiceFlags                     flags,
        uint32_t                            interfaceIndex,
        DNSServiceErrorType                 errorCode,
        const char                          *serviceName,
        const char                          *regtype,
        const char                          *replyDomain,
        void                                *context
    )
    {

        RemoteService* thisService=NULL;
        if(context != NULL) {
            thisService=(RemoteService*)context;
            thisService->evtHanler(sdRef,flags,interfaceIndex,errorCode,serviceName,regtype,replyDomain,context);
        }

    }

    /// Possible values are kDNSServiceFlagsMoreComing and kDNSServiceFlagsAdd.
    void	evtHanler(
        DNSServiceRef                       sdRef,
        DNSServiceFlags                     flags,
        uint32_t                            interfaceIndex,
        DNSServiceErrorType                 errorCode,
        const char                          *serviceName,
        const char                          *regtype,
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
                LOG_TRACE<<"DNSServiceBrowse found service";
            } else {
                LOG_TRACE<<serviceName<<" removed";
			}
			LOG_TRACE<<"flags:"<< flags;
			LOG_TRACE<<"interfaceIndex:"<< interfaceIndex;
			LOG_TRACE<<"serviceName:"<< serviceName;
			LOG_TRACE<<"regtype:"<<regtype;
			LOG_TRACE<<"replyDomain:"<<replyDomain;
        }

        if(evtCallback) {
			evtCallback(
				shared_from_this(),
                flags,
                interfaceIndex,
                err,
                serviceName,
                regtype,
                replyDomain
            );
        }
    }

};
typedef	boost::shared_ptr<RemoteService>	RemoteServicePtr;


}
}
#endif // BONJOURSERVICE_BROWER_H
