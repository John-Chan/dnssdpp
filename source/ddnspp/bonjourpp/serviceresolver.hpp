#ifndef BONJOUR_SERVICE_RESOLVER_H__
#define BONJOUR_SERVICE_RESOLVER_H__

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/core.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>
#include <ddnspp/bonjourpp/txtrecords.hpp>
#include <ddnspp/common/endian.hpp>
#include <logv/logv.hpp>

namespace air{namespace bonjour {

	class ServiceFactory;


	/// Event callback
	typedef boost::function
		<
		void
		(
		DNSServiceFlags,		/// _1 flags
		boost::uint32_t,		/// _2 interfaceIndex
		BonjourError,			/// _3 error 
		std::string,			/// _4 service name(fullname, format:<servicename>.<protocol>.<domain>) 
		std::string,			/// _5 host 
		boost::uint16_t,		/// _6 port
		TxtRecordDecoderPtr		/// _7 TxtRecordDecoder (include a copy of dns record)
		)
		>		ServiceResolverEvtCallback;

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
			LOG_INFO<<"DNSServiceResolve successful";
			LOG_INFO<<"flags:"<< flags;
			LOG_INFO<<"interfaceIndex:"<< interfaceIndex;
			LOG_INFO<<"fullname:"<< fullname;
			LOG_INFO<<"hosttarget:"<<hosttarget;
			LOG_INFO<<"port:"<<air::common::as_be16(port);
			LOG_INFO<<"txtLen:"<<txtLen;

			ConstRecordList records;
			TxtRecordDecoder dc(core->getApiProvider(),txtRecord,txtLen);
			err = dc.getAll(records);
			if(err){
				LOG_ERR<<err.getErrorCode()<<"," <<err.getMessage();
			}else{
				ConstRecordList::const_iterator it=records.begin();
				ConstRecordList::const_iterator over=records.end();
				while(it != over){
					LOG_INFO<< it->first<<"\t"<< it->second.allAsString() ;
					++it;
				}
			}
		}
		if(evtCallback){
			evtCallback(
				flags,
				interfaceIndex,
				err,
				fullname,
				hosttarget,
				air::common::as_be16(port),
				TxtRecordDecoderPtr(new TxtRecordDecoder(core->getApiProvider(),txtRecord,txtLen))
				);
		}

	}

	};
	typedef	boost::shared_ptr<ServiceResolver>	ServiceResolverPtr;


}}
#endif // BONJOUR_SERVICE_RESOLVER_H__