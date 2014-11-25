#ifndef BONJOURSERVICE_BROWER_H
#define BONJOURSERVICE_BROWER_H

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/core.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>

#include <logv/logv.hpp>

namespace air
{
namespace bonjour
{

class ServiceFactory;

/// Event callback
typedef boost::function
<
void
(
    DNSServiceFlags,		/// _1 flags
    boost::uint32_t,		/// _2 interfaceIndex
    BonjourError,			/// _3 error
    std::string,			/// _4 service name (for subsequent use in the ServiceResolver )
    std::string,			/// _5 service type
    std::string				/// _6 domian
)
>
RemoteServiceEvtCallback;

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
            LOG_ERROR<<err.getErrorCode()<<"," <<err.getMessage();
        } else {

            if (flags & kDNSServiceFlagsAdd) {
                LOG_INFO<<"DNSServiceBrowse found service";
                LOG_INFO<<"flags:"<< flags;
                LOG_INFO<<"interfaceIndex:"<< interfaceIndex;
                LOG_INFO<<"serviceName:"<< serviceName;
                LOG_INFO<<"regtype:"<<regtype;
                LOG_INFO<<"replyDomain:"<<replyDomain;
            } else {
                LOG_INFO<<serviceName<<" removed";
            }
        }

        if(evtCallback) {
            evtCallback(
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
