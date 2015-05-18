#ifndef BONJOUR_SERVICE_RESOLVER_H__
#define BONJOUR_SERVICE_RESOLVER_H__

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <ddnspp/bonjourpp/dnsd_callback.hpp>		/// callback func def

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/core.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>
#include <ddnspp/bonjourpp/txtrecords.hpp>
#include <ddnspp/common/endian.hpp>
#include <ddnspp/logv/logv.hpp>
#include <ddnspp/bonjourpp/dnsd_callback.hpp>


namespace air{namespace bonjour {

	class ServiceFactory;

	/// ServiceResolver
	/// Resolve a service name discovered  (e.g. via ServiceFactory.createServiceBrower) to 
	/// get target host name, port number, and txt record
	class ServiceResolver:boost::noncopyable,public boost::enable_shared_from_this<ServiceResolver>
	{
		friend class ServiceFactory;
	private:
		CoreContextPtr		core;
		ServiceResolverEvtCallback	evtCallback;
	public:
		ServiceResolver(boost::asio::io_service& ios,const DNSDApi &dll,const ServiceResolverEvtCallback& func)
			:core(new CoreContext(ios,dll)),evtCallback(func)
		{
		}
		~ServiceResolver()
		{
			//LOG_DEBUG<<"ServiceResolver dead";
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
		ServiceResolverEvtCallback				setEvtCallback(const ServiceResolverEvtCallback& func)
		{
			ServiceResolverEvtCallback old=evtCallback;
			evtCallback=func;
			return old;
		}

	private:
		static void		DNSSD_API	DNSServiceResolveReplyCallback(
			DNSServiceRef                       sdRef,
			DNSServiceFlags                     flags,
			uint32_t                            interfaceIndex,
			DNSServiceErrorType                 errorCode,
			const char                          *fullname,
			const char                          *hosttarget,
			uint16_t                            port,
			uint16_t                            txtLen,
			const unsigned char                 *txtRecord,
			void                                *context
			)
		{

			ServiceResolver* thisService=NULL;
			if(context != NULL){
				thisService=(ServiceResolver*)context;
				thisService->evtHanler(sdRef,flags,interfaceIndex,errorCode,fullname,hosttarget,port,txtLen,txtRecord,context);
			}
		}
		/// Possible values: kDNSServiceFlagsMoreComing
		void	evtHanler(
			DNSServiceRef                       sdRef,
			DNSServiceFlags                     flags,
			uint32_t                            interfaceIndex,
			DNSServiceErrorType                 errorCode,
			const char                          *fullname,
			const char                          *hosttarget,
			uint16_t                            port,
			uint16_t                            txtLen,
			const unsigned char                 *txtRecord,
			void                                *context)
	{

		air::bonjour::BonjourError err(errorCode);
		/*bool moreData=(flags & kDNSServiceFlagsMoreComing);
		if (moreData) return;*/

		if(err){
			LOG_ERR<<err.getErrorCode()<<"," <<err.getMessage();
		}else{
			LOG_TRACE<<"DNSServiceResolve successful";
			LOG_TRACE<<"flags:"<< flags;
			LOG_TRACE<<"interfaceIndex:"<< interfaceIndex;
			LOG_TRACE<<"fullname:"<< fullname;
			LOG_TRACE<<"hosttarget:"<<hosttarget;
			LOG_TRACE<<"port:"<<air::common::as_be16(port);
			LOG_TRACE<<"txtLen:"<<txtLen;

			ConstRecordList records;
			TxtRecordDecoder dc(core->getApiProvider(),txtRecord,txtLen);
			err = dc.getAll(records);
			if(err){
				LOG_ERR<<err.getErrorCode()<<"," <<err.getMessage();
			}else{
				ConstRecordList::const_iterator it=records.begin();
				ConstRecordList::const_iterator over=records.end();
				while(it != over){
					LOG_TRACE<< it->first<<"\t"<< it->second.allAsString() ;
					++it;
				}
			}
		}
		if(evtCallback){
			air::bonjour::ServiceResolveData data;
			data.error	=err;
			data.flags	=flags;
			data.fullServiceName	=fullname;
			data.hostName			=hosttarget;
			data.interfaceIndex		=interfaceIndex;
			data.owner				=shared_from_this();
			data.port				=air::common::as_be16(port);
			data.recordDecoder		=TxtRecordDecoderPtr(new TxtRecordDecoder(core->getApiProvider(),txtRecord,txtLen));

			evtCallback(data);

		}

	}

	};
	typedef	boost::shared_ptr<ServiceResolver>	ServiceResolverPtr;


}}
#endif // BONJOUR_SERVICE_RESOLVER_H__