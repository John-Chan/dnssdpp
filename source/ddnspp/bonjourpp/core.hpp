#ifndef BONJOUR_CORE_H__
#define BONJOUR_CORE_H__


#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>

#include <boost/noncopyable.hpp>
//#include <boost/thread/latch.hpp>
#include <logv/logv.hpp>

namespace air
{
namespace bonjour
{

class CoreContext:boost::noncopyable,public boost::enable_shared_from_this<CoreContext>
{
private:
    typedef		boost::weak_ptr<void>		AnyWeakPtr;
    //typedef		boost::shared_ptr<void>		AnySharedPtr;
    typedef	boost::shared_ptr<boost::asio::ip::tcp::socket>	TcpSocketPtr;
    typedef	boost::shared_ptr<CoreContext>	CtxPtr;

    boost::asio::io_service::strand strand;
    AnyWeakPtr			obPtr;
    const bool			useStrand;

    const DNSDApi&	dnsDll;
    TcpSocketPtr	warpedSocket;
    DNSServiceRef   serviceRef;
    bool			stoped;
    int				evtTimeoutSecond;
    int				socketFD;
    bool			reading;
public:
    CoreContext(boost::asio::io_service& ios,const DNSDApi& dll,bool	strandExecHandler=true)
        :strand(ios), useStrand(strandExecHandler),dnsDll(dll),serviceRef(NULL),stoped(false),evtTimeoutSecond(2),socketFD(-1),reading(false)
    {
        //
    }
    ~CoreContext()
    {
        close();
        //LOG_DEBUG<< "CoreContext going to dead";
    }
    const DNSDApi&		getApiProvider()const
    {
        return dnsDll;
    }
    DNSServiceRef&		getDNSServiceRef()
    {
        return serviceRef;
    }
    boost::asio::io_service& 	getIOService()
    {
        return strand.get_io_service();
    }
    /// start async event loop
    /// CoreContext hold evtProcesser's weak_ptr to avoid run event call back after  evtProcesser dead
    void	startEventLoop(const boost::weak_ptr<void>&	evtProcesser)
    {
        if(socketFD == -1) {
            obPtr=evtProcesser;
            stoped=false;
            run();
        }
    }

    void	close()
    {
        stoped=true;
        if(serviceRef!=NULL) {
            dnsDll.getFunctiontable().funcDNSServiceRefDeallocate(serviceRef);
        }
        serviceRef=NULL;
    }
private:
    void	run()
    {

        socketFD = serviceRef    ?  dnsDll.getFunctiontable().funcDNSServiceRefSockFD(serviceRef) : -1;
        if(socketFD == -1) {
            LOG_ERROR<<"cant got socket FD form DNSServiceRef.";
            return;
        }
        warpedSocket.reset(new boost::asio::ip::tcp::socket(strand.get_io_service()));
        boost::system::error_code ec;
        if(warpedSocket->assign(boost::asio::ip::tcp::v4(),socketFD,ec)) {
            LOG_ERROR<<"assign socket FD to asio::socket fail."
                     <<"FD="<<socketFD
                     <<",err=  "<< ec.message();
        } else {
            LOG_DEBUG<<"assign socket FD to asio::socket successfully."
                     <<"FD="<<socketFD
                     <<",start event loop";

            if(useStrand) {
                strand.post(boost::bind(&CoreContext::post_read,shared_from_this()));
            } else {
                post_read();
            }
        }


    }
    void	post_read()
    {
        if(reading) {
            LOG_FATAL<< "caller made a post_read call when socket is in reading ";
            return;
        }
        reading=true;
        warpedSocket->async_read_some(boost::asio::null_buffers(),
                                      boost::bind(&CoreContext::handle_read,
                                              shared_from_this(),
                                              boost::asio::placeholders::error));
    }
    void	handle_read(boost::system::error_code ec)
    {
        reading=false;
        if(ec) {
            if(stoped) {
                LOG_DEBUG<< "stop evtloop";
            } else {
                LOG_ERROR<<"  "<< ec.message();
            }
            return;
        }
        if(process_read_data()) {
            if(useStrand) {
                strand.post(boost::bind(&CoreContext::post_read,shared_from_this()));
            } else {
                post_read();
            }
        } else {
            close();
        }
    }

    /// return false means  we got some error ,should stop event loop
    bool	process_read_data()
    {
        bool keep_evt_loop=false;

        /// when  ServiceFactory create a service,it use  service object's RAW pointer(the context prarmeter).
        /// when DNSServiceProcessResult is  called without err,ddns will run the callback function and
        /// pass back the context parameter,if that service was dead before calling callback function,  context
        /// is a dangling pointer.
        if(!obPtr.expired()) {
            DNSServiceErrorType err = kDNSServiceErr_NoError;
            err = dnsDll.getFunctiontable().funcDNSServiceProcessResult(serviceRef);
            air::bonjour::BonjourError warpedErr(err);
            if(warpedErr) {
                LOG_ERROR<<"DNSServiceProcessResult failed ,error = "<< warpedErr.getMessage();
            } else {
                LOG_DEBUG<< "DNSServiceProcessResult successfully";
                if(!stoped) {
                    keep_evt_loop=true;
                } else {
                    LOG_DEBUG<< "stop evtloop";
                }
            }
        } else {
            LOG_ERROR<<"Event Processer has been dead somewhere";
        }
        return keep_evt_loop;
    }

};
typedef	boost::shared_ptr<CoreContext>	CoreContextPtr;

}
}
#endif // BONJOUR_CORE_H__