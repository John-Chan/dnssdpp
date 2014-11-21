﻿#ifndef BONJOUR_DOMIAN_ENUMERATER_H__
#define BONJOUR_DOMIAN_ENUMERATER_H__

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/core.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>

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
		std::string				/// _4 replyDomain
		)
		>		
		EnumerationEvtCallback;

	/// DomainEumerater
	/// A service for Domain Enumeration
	class DomainEumerater:boost::noncopyable,public boost::enable_shared_from_this<DomainEumerater>
	{
		friend class ServiceFactory;

	private:
		CoreContextPtr		core;
		EnumerationEvtCallback	evtCallback;	

	public:
		DomainEumerater(boost::asio::io_service& ios,const DNSDApi &dll,const EnumerationEvtCallback& func);
		~DomainEumerater();

		CoreContextPtr		getCoreContext();

		/// return old one
		EnumerationEvtCallback	setEvtCallback(const EnumerationEvtCallback& func);

	private:
		static void		DNSSD_API	DNSServiceDomainEnumReplyCallback(
			DNSServiceRef                       sdRef,
			DNSServiceFlags                     flags,
			uint32_t                            interfaceIndex,
			DNSServiceErrorType                 errorCode,
			const char                          *replyDomain,
			void                                *context
			);

		void	evtHanler(
			DNSServiceRef                       sdRef,
			DNSServiceFlags                     flags,
			uint32_t                            interfaceIndex,
			DNSServiceErrorType                 errorCode,
			const char                          *replyDomain,
			void                                *context);
	};
	typedef	boost::shared_ptr<DomainEumerater>	DomainEumeraterPtr;
	
	/************************************************************************
	//  Impl
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	DomainEumerater::DomainEumerater(boost::asio::io_service& ios,const DNSDApi &dll,const EnumerationEvtCallback& func)
		:core(new CoreContext(ios,dll)),evtCallback(func)
	{
	}
	DomainEumerater::~DomainEumerater()
	{
	}
	CoreContextPtr		DomainEumerater::getCoreContext()
	{
		return core;
	}

	EnumerationEvtCallback	DomainEumerater::setEvtCallback(const EnumerationEvtCallback& func)
	{
		EnumerationEvtCallback old=evtCallback;
		evtCallback=func;
		return old;
	}
	void		DNSSD_API	DomainEumerater::DNSServiceDomainEnumReplyCallback(
		DNSServiceRef                       sdRef,
		DNSServiceFlags                     flags,
		uint32_t                            interfaceIndex,
		DNSServiceErrorType                 errorCode,
		const char                          *replyDomain,
		void                                *context
		)
	{

		DomainEumerater* thisService=NULL;
		if(context != NULL){
			thisService=(DomainEumerater*)context;
			thisService->evtHanler(sdRef,flags,interfaceIndex,errorCode,replyDomain,context);
		}

	}

	// flags: Possible values are:
	// kDNSServiceFlagsMoreComing
	// kDNSServiceFlagsAdd
	// kDNSServiceFlagsDefault
	void	DomainEumerater::evtHanler(
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

		if(err){
			LOG_ERROR<<err.getErrorCode()<<"," <<err.getMessage();
		}else{

			if (flags & kDNSServiceFlagsAdd){
				LOG_INFO<<"Domain Enumeration found ";
				LOG_INFO<<"flags:"<< flags;
				LOG_INFO<<"interfaceIndex:"<< interfaceIndex;
				LOG_INFO<<"replyDomain:"<< replyDomain;
			}
		}

		if(evtCallback){
			evtCallback(
				flags,
				interfaceIndex,
				err,
				replyDomain
				);
		}
	}

}}
#endif // BONJOUR_DOMIAN_ENUMERATER_H__