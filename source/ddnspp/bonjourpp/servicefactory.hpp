﻿#ifndef BONJOUR_SERVICE_FACTORY_H
#define BONJOUR_SERVICE_FACTORY_H

//#include <dns_sd.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>
#include <ddnspp/bonjourpp/bonjourservice.hpp>
#include <ddnspp/bonjourpp/bonjoursvcbrower.hpp>
#include <ddnspp/bonjourpp/serviceresolver.hpp>
#include <ddnspp/bonjourpp/addressresolver.hpp>
#include <ddnspp/bonjourpp/domianenumerater.hpp>
#include <ddnspp/bonjourpp/servicetype.hpp>
#include <ddnspp/bonjourpp/txtrecords.hpp>
#include <ddnspp/common/endian.hpp>
#include <logv/logv.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

#include <list>

#define		ENABLE_SERVICEFACTORY_AUTO_HOLD_SERVICES		1

//#define		TEST_NO_DANGLING_PTR(SHARED_PTR) do{ (SHARED_PTR).reset(); }while(0);
#define		TEST_NO_DANGLING_PTR(SHARED_PTR) do{ ; }while(0);
namespace air{namespace bonjour {

class ServiceFactory:boost::noncopyable
{
private:
	typedef boost::mutex					mutex_type;
	typedef	boost::unique_lock<mutex_type>	lock_type;

	typedef		boost::shared_ptr<void>		AnyPtr;
	typedef		std::list<AnyPtr>			AnyPtrList;
#if ENABLE_SERVICEFACTORY_AUTO_HOLD_SERVICES

	AnyPtrList	createdServices; 
	mutex_type	mutex;

#endif
    const DNSDApi& dnsDll;
	boost::asio::io_service& ioService;

private:
	bool		hold(const ServiceFactory::AnyPtr& whatever);
	bool		remove(const ServiceFactory::AnyPtr& whatever);
	ServiceFactory::AnyPtrList::iterator	find(const ServiceFactory::AnyPtr& whatever);

public:
    ServiceFactory(boost::asio::io_service& ios,const DNSDApi& dll);
	~ServiceFactory();
	bool	removeService(const boost::shared_ptr<void>& ptr);

	 /**
     * @brief registerService
     * @param name  empty str will do
     * @param typ
     * @param domain    empty str will do
     * @param host      empty str will do
     * @param port
	 * @param txtRecord empty  will do
	 * @param func can be NULL
     * @return NULL when failed
     */
	LocalServicePtr registerService(
		const std::string& name,
		const ServiceType& type,
		const std::string& domain,
		const std::string& host,
		const std::string& port,
		const TxtRecordEncoder& txtRecord,
		const LocalServiceEvtCallback& func,
		BonjourError& err);

    /**
     * @brief createServiceBrower to discovery other service
     * @param type  a ServiceType you looking for
	 * @param domain    empty str will do
	 * @param func can be NULL
	 * @return NULL when failed
     */
	RemoteServicePtr	createServiceBrower(
		const ServiceType& type,
		const std::string& domain,
		const RemoteServiceEvtCallback& func,
		BonjourError& err);

	/**
     * @brief createServiceResolver to get  service's dns record 
     * @param name  The name of the service instance to be resolved
     * @param type  a ServiceType you looking for
	 * @param domain    The domain of the service instance to be resolved
	 * @param func can be NULL
	 * @return NULL when failed
     */
	ServiceResolverPtr	createServiceResolver(
		const std::string& name,
		const ServiceType& type,
		const std::string& domain,
		const ServiceResolverEvtCallback& func,
		BonjourError& err
		);
	ServiceResolverPtr	createServiceResolver(
		const std::string& name,
		const std::string& type,
		const std::string& domain,
		const ServiceResolverEvtCallback& func,
		BonjourError& err
		);

	/**
     * @brief createAddressResolver to resolve  address
     * @param protocol  
	 * @param hostname  
	 * @param func can be NULL
	 * @return NULL when failed
     */
	AddressResolverPtr	createAddressResolver(
		ADDRESS_TYPE					protocol,
		const std::string&              hostname,
		const AddressResolverEvtCallback& func,
		BonjourError& err
		);
	/**
     * @brief Domain Enumeration
     * @param interfaceIndex  can be 0(recommended),If non-zero, specifies the interface on which to look for domains
	 * @param func can be NULL
	 * @return NULL when failed
     */
	DomainEumeraterPtr	createDomainEumerater(
		boost::uint32_t     interfaceIndex,
		const EnumerationEvtCallback& func,
		BonjourError& err
		);

};


/************************************************************************
//  Impl
//  --------------------------------------------------------------------
//                                                                  
************************************************************************/
ServiceFactory::ServiceFactory(boost::asio::io_service& ios,const DNSDApi &dll)
:ioService(ios),dnsDll(dll)
{
}

ServiceFactory::~ServiceFactory()
{
#if ENABLE_SERVICEFACTORY_AUTO_HOLD_SERVICES
	createdServices.clear();
#endif
}
LocalServicePtr ServiceFactory::registerService(
	const std::string& name,
	const ServiceType& type,
	const std::string& domain,
	const std::string& host,
	const std::string& port,
	const TxtRecordEncoder& txtRecord,
	const LocalServiceEvtCallback& func,
	BonjourError& err)
{
	/************************************************************************
	* flags:           Indicates the renaming behavior on name conflict (most applications
	*                  will pass 0).                                                    
	************************************************************************/
	LocalServicePtr service(new LocalService(ioService,dnsDll,func));
	DNSServiceFlags flags=0;
	boost::uint32_t interfaceIndex=kDNSServiceInterfaceIndexAny;
	std::string     serviceType=type.toString();
	boost::uint16_t port_be= air::common::as_be16(boost::lexical_cast<boost::uint16_t>(port.c_str())) ;
	DNSServiceErrorType err_code= dnsDll.getFunctiontable().funcDNSServiceRegister(
		&service->getCoreContext()->getDNSServiceRef(),
		flags,
		interfaceIndex,
		name.c_str(),
		serviceType.c_str(),
		(domain.length()==0)?NULL: domain.c_str(),
		(host.length()==0)?NULL:host.c_str(),
		port_be,
		txtRecord.getRecordLenth(),
		txtRecord.getRecordPtr(),
		LocalService::DNSServiceRegisterReplyCallback,
		service.get()
		);
	err.reset(err_code);
	if(!err){
		hold(service);
		service->getCoreContext()->startEventLoop(service);
		TEST_NO_DANGLING_PTR(service);
	}else{
		service.reset();
	}
	return service;
}
RemoteServicePtr	ServiceFactory::createServiceBrower(
	const ServiceType& type,
	const std::string& domain,
	const RemoteServiceEvtCallback& func,
	BonjourError& err)
{
	/// flags:           Currently ignored, reserved for future use.

	RemoteServicePtr service(new RemoteService(ioService,dnsDll,func));
	DNSServiceFlags flags=0;
	boost::uint32_t interfaceIndex=kDNSServiceInterfaceIndexAny;
	std::string     serviceType=type.toString();
	DNSServiceErrorType err_code= dnsDll.getFunctiontable().funcDNSServiceBrowse(
		&service->getCoreContext()->getDNSServiceRef(),
		flags,
		interfaceIndex,
		serviceType.c_str(),
		(domain.length()==0)?NULL: domain.c_str(),
		RemoteService::DNSServiceBrowseReplyCallback,
		service.get());
	err.reset(err_code);
	if(!err){
		hold(service);
		service->getCoreContext()->startEventLoop(service);
		TEST_NO_DANGLING_PTR(service);
	}else{
		service.reset();
	}
	return service;
}

ServiceResolverPtr	ServiceFactory::createServiceResolver(
	const std::string& name,
	const std::string& type,
	const std::string& domain,
	const ServiceResolverEvtCallback& func,
	BonjourError& err
	)
{
	/************************************************************************
	* flags:           Specifying kDNSServiceFlagsForceMulticast will cause query to be
	*                  performed with a link-local mDNS query, even if the name is an
	*                  apparently non-local name (i.e. a name not ending in ".local.")                                                          
	************************************************************************/
	ServiceResolverPtr service(new ServiceResolver(ioService,dnsDll,func));
	DNSServiceFlags flags=0;
	boost::uint32_t interfaceIndex=kDNSServiceInterfaceIndexAny;
	DNSServiceErrorType err_code= dnsDll.getFunctiontable().funcDNSServiceResolve(
		&service->getCoreContext()->getDNSServiceRef(),
		flags,
		interfaceIndex,
		name.c_str(),
		type.c_str(),
		(domain.length()==0)?NULL: domain.c_str(),
		ServiceResolver::DNSServiceResolveReplyCallback,
		service.get());
	err.reset(err_code);
	if(!err){
		hold(service);
		service->getCoreContext()->startEventLoop(service);
		TEST_NO_DANGLING_PTR(service);
	}else{
		service.reset();
	}
	return service;
}
ServiceResolverPtr	ServiceFactory::createServiceResolver(
	const std::string& name,
	const ServiceType& type,
	const std::string& domain,
	const ServiceResolverEvtCallback& func,
	BonjourError& err
	)
{
	std::string     serviceType=type.toString();
	ServiceResolverPtr ptr=createServiceResolver(name,serviceType,domain,func,err);
	return ptr;
}
AddressResolverPtr	ServiceFactory::createAddressResolver(
	ADDRESS_TYPE					protocol,
	const std::string&              hostname,
	const AddressResolverEvtCallback& func,
	BonjourError& err
	)
{
	/************************************************************************
	* flags:           kDNSServiceFlagsForceMulticast or kDNSServiceFlagsLongLivedQuery.
	*                  Pass kDNSServiceFlagsLongLivedQuery to create a "long-lived" unicast
	*                  query in a non-local domain. Without setting this flag, unicast queries
	*                  will be one-shot - that is, only answers available at the time of the call
	*                  will be returned. By setting this flag, answers (including Add and Remove
	*                  events) that become available after the initial call is made will generate
	*                  callbacks. This flag has no effect on link-local multicast queries.                                                                  
	************************************************************************/
	AddressResolverPtr service(new AddressResolver(ioService,dnsDll,func));
	//DNSServiceFlags flags=kDNSServiceFlagsReturnIntermediates;
	DNSServiceFlags flags=kDNSServiceFlagsLongLivedQuery;
	boost::uint32_t interfaceIndex=kDNSServiceInterfaceIndexAny;
	DNSServiceErrorType err_code= dnsDll.getFunctiontable().funcDNSServiceGetAddrInfo(
		&service->getCoreContext()->getDNSServiceRef(),
		flags,
		interfaceIndex,
		protocol,
		hostname.c_str(),
		AddressResolver::DNSServiceGetAddrInfoReplyCallback,
		service.get());
	err.reset(err_code);
	if(!err){
		hold(service);
		service->getCoreContext()->startEventLoop(service);
		TEST_NO_DANGLING_PTR(service);
	}else{
		service.reset();
	}
	return service;
}
DomainEumeraterPtr	ServiceFactory::createDomainEumerater(
	boost::uint32_t     interfaceIndex,
	const EnumerationEvtCallback& func,
	BonjourError& err
	)
{
	//flags:           Possible values are:
	//                 kDNSServiceFlagsBrowseDomains to enumerate domains recommended for browsing.
	//                 kDNSServiceFlagsRegistrationDomains to enumerate domains recommended for registration.

	DomainEumeraterPtr service(new DomainEumerater(ioService,dnsDll,func));
	DNSServiceFlags flags=kDNSServiceFlagsBrowseDomains;
	//boost::uint32_t interfaceIndex=kDNSServiceInterfaceIndexAny; 
	DNSServiceErrorType err_code= dnsDll.getFunctiontable().funcDNSServiceEnumerateDomains (
		&service->getCoreContext()->getDNSServiceRef(),
		flags,
		interfaceIndex,
		DomainEumerater::DNSServiceDomainEnumReplyCallback,
		service.get());
	err.reset(err_code);
	if(!err){
		hold(service);
		service->getCoreContext()->startEventLoop(service);
		TEST_NO_DANGLING_PTR(service);
	}else{
		service.reset();
	}
	return service;
}
ServiceFactory::AnyPtrList::iterator	ServiceFactory::find(const ServiceFactory::AnyPtr& whatever)
{
#if ENABLE_SERVICEFACTORY_AUTO_HOLD_SERVICES
	ServiceFactory::AnyPtrList::iterator it=createdServices.begin();
	ServiceFactory::AnyPtrList::iterator over=createdServices.end();
	while(it != over){
		if(whatever.get() == (*it).get())
			break;
		++it;
	}
	return it;
#else
	return ServiceFactory::AnyPtrList::iterator();
#endif
}
bool		ServiceFactory::hold(const ServiceFactory::AnyPtr& whatever)
{
#if ENABLE_SERVICEFACTORY_AUTO_HOLD_SERVICES
	lock_type lock(mutex);
	ServiceFactory::AnyPtrList::iterator found=find(whatever);
	if(found == createdServices.end()){
		createdServices.push_back(whatever);
		return true;
	}else{
		return false;
	}
#else
	return false;

#endif
}
bool		ServiceFactory::remove(const ServiceFactory::AnyPtr& whatever)
{
#if ENABLE_SERVICEFACTORY_AUTO_HOLD_SERVICES
	lock_type lock(mutex);
	ServiceFactory::AnyPtrList::iterator found=find(whatever);
	if(found == createdServices.end()){
		return false;
	}else{
		createdServices.erase(found);
		return true;
	}
#else
	return false;
#endif
}
bool	ServiceFactory::removeService(const boost::shared_ptr<void>& ptr)
{
#if ENABLE_SERVICEFACTORY_AUTO_HOLD_SERVICES
	return remove(ptr);
#else
	return false;
#endif
}

}}
#endif // BONJOUR_SERVICE_FACTORY_H