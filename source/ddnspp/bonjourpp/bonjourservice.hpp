#ifndef BONJOURSERVICE_H
#define BONJOURSERVICE_H

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/core.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>
#include <ddnspp/bonjourpp/servicetype.hpp>
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
    BonjourError,			/// _2 error
    std::string,			/// _3 service name
    std::string,			/// _4 service type
    std::string				/// _5 domian
)
>		LocalServiceEvtCallback;
/// LocalService
/// A service which is published by yourself(e.g. via ServiceFactory.registerService)
class LocalService:boost::noncopyable,public boost::enable_shared_from_this<LocalService>
{
    friend class ServiceFactory;
private:
    CoreContextPtr		core;
    LocalServiceEvtCallback	evtCallback;
public:
    LocalService(boost::asio::io_service& ios,const DNSDApi &dll,const LocalServiceEvtCallback& func)
        :core(new CoreContext(ios,dll)),evtCallback(func)
    {
    }
    ~LocalService()
    {
        //LOG_DEBUG<<"LocalService dead";;
    }
    CoreContextPtr		getCoreContext()
    {
        return core;
    }

    /// return old one
    LocalServiceEvtCallback				setEvtCallback(const LocalServiceEvtCallback& func)
    {
        LocalServiceEvtCallback old=evtCallback;
        evtCallback=func;
        return old;
    }

private:
    static	void        DNSSD_API DNSServiceRegisterReplyCallback(
        DNSServiceRef                       sdRef,
        DNSServiceFlags                     flags,
        DNSServiceErrorType                 errorCode,
        const char                          *name,
        const char                          *regtype,
        const char                          *domain,
        void                                *context)
    {
        LocalService* thisService=NULL;
        if(context != NULL) {
            thisService=(LocalService*)context;
            thisService->evtHanler(sdRef,flags,errorCode,name,regtype,domain,context);
        }
    }

    /************************************************************************
    flags:
      When a name is successfully registered, the callback will be
      invoked with the kDNSServiceFlagsAdd flag set. When Wide-Area
      DNS-SD is in use, it is possible for a single service to get
      more than one success callback (e.g. one in the "local" multicast
      DNS domain, and another in a wide-area unicast DNS domain).
      If a successfully-registered name later suffers a name conflict
      or similar problem and has to be deregistered, the callback will
      be invoked with the kDNSServiceFlagsAdd flag not set. The callback
      is *not* invoked in the case where the caller explicitly terminates
      the service registration by calling DNSServiceRefDeallocate(ref);
    ************************************************************************/
    void	evtHanler(
        DNSServiceRef                       sdRef,
        DNSServiceFlags                     flags,
        DNSServiceErrorType                 errorCode,
        const char                          *name,
        const char                          *regtype,
        const char                          *domain,
        void                                *context)
    {
        air::bonjour::BonjourError err(errorCode);
        /*bool moreData=(flags & kDNSServiceFlagsMoreComing);
        if (moreData) return;*/

        if(err) {
            if(kDNSServiceErr_NameConflict == err.getErrorCode()) {
                LOG_ERROR<<err.getErrorCode()<<"," <<err.getMessage()<<"(" <<name << "in use,choose another"<<")";
            } else {
                LOG_ERROR<<err.getErrorCode()<<"," <<err.getMessage();
            }
        } else {

            if (flags & kDNSServiceFlagsAdd) {
                LOG_INFO<<"DNSServiceRegister successful";
                LOG_INFO<<"name:"<<name;
                LOG_INFO<<"type:"<<regtype;
                LOG_INFO<<"domain:"<<domain;
            } else {
                LOG_INFO<<name<<" removed";
            }

        }

        if(evtCallback) {
            evtCallback(
                flags,
                err,
                name,
                regtype,
                domain
            );
        }
    }

};

typedef	boost::shared_ptr<LocalService>	LocalServicePtr;


}
}
#endif // BONJOURSERVICE_H
