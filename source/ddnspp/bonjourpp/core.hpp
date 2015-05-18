#ifndef BONJOUR_CORE_H__
#define BONJOUR_CORE_H__

#include <ddnspp/logv/logv.hpp>

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
//#include <boost/thread/latch.hpp>

namespace air
{
namespace bonjour
{

class CoreContext:public boost::enable_shared_from_this<CoreContext>,boost::noncopyable
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
		if(!stoped){
			if(warpedSocket){
				// must clear assigned native socket handle
				// call  DNSServiceRefSockFD after a DNSServiceRefDeallocate call may got same one(same socket number)
				warpedSocket.reset();
			}
			if(serviceRef!=NULL) {
				LOG_DEBUG<<"DNSServiceRefDeallocate"<<",FD="<<socketFD<<",serviceRef="<<serviceRef;
				dnsDll.getFunctiontable().funcDNSServiceRefDeallocate(serviceRef);
				serviceRef=NULL;
			}
		}
		stoped=true;
    }
private:

    void	run()
    {

        socketFD = (serviceRef )? (dnsDll.getFunctiontable().funcDNSServiceRefSockFD(serviceRef))  : (-1);
        if(socketFD == -1) {
            LOG_ERR<<"cant got socket FD form DNSServiceRef.";
            return;
        }
        warpedSocket.reset(new boost::asio::ip::tcp::socket(strand.get_io_service()));
        boost::system::error_code ec;
        if(warpedSocket->assign(boost::asio::ip::tcp::v4(),socketFD,ec)) {
            LOG_ERR<<"assign socket FD to asio::socket fail."
				<<"FD="<<socketFD
				<<",serviceRef="<<serviceRef
                <<",err=  "<< ec.message();
        } else {
			{
				LOG_DEBUG<<"assign socket FD to asio::socket successfully."
					<<"FD="<<socketFD
					<<",serviceRef="<<serviceRef
					<<",start event loop";
			}

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
        warpedSocket->async_read_some(
			boost::asio::null_buffers(),
            boost::bind(&CoreContext::handle_read, shared_from_this(), boost::asio::placeholders::error)
			);
    }
    void	handle_read(boost::system::error_code ec)
    {
        reading=false;
        if(ec) {
            if(stoped) {
				LOG_DEBUG<< "evtloop stoped"<<",FD="<<socketFD<<",serviceRef="<<serviceRef;
            } else {
                LOG_ERR<<"  "<< ec.message()<<",FD="<<socketFD<<",serviceRef="<<serviceRef;
            }
            return;
        }
        if(process_read_data() && !stoped) {
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
			if (!stoped){
				DNSServiceErrorType err = kDNSServiceErr_NoError;
				err = dnsDll.getFunctiontable().funcDNSServiceProcessResult(serviceRef);
				air::bonjour::BonjourError warpedErr(err);
				if(warpedErr) {
					LOG_ERR<<"DNSServiceProcessResult failed ,error = "<< warpedErr.getMessage();
				} else {
					LOG_DEBUG<< "DNSServiceProcessResult successfully";
					keep_evt_loop=true;
				}
			}else{
				LOG_DEBUG<< "evtloop stoped";
			}
			//////
			/*DNSServiceErrorType err = kDNSServiceErr_NoError;
			err = dnsDll.getFunctiontable().funcDNSServiceProcessResult(serviceRef);
			air::bonjour::BonjourError warpedErr(err);
			if(warpedErr) {
				LOG_ERR<<"DNSServiceProcessResult failed ,error = "<< warpedErr.getMessage();
			} else {
				LOG_DEBUG<< "DNSServiceProcessResult successfully";
				if(!stoped) {
					keep_evt_loop=true;
				} else {
					LOG_DEBUG<< "evtloop stoped";
				}
			}*/
        } else {
            LOG_ERR<<"Event Processer has been dead somewhere";
        }
        return keep_evt_loop;
    }

};
typedef	boost::shared_ptr<CoreContext>	CoreContextPtr;

}
}
#endif // BONJOUR_CORE_H__