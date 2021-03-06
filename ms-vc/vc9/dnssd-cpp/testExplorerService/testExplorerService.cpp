// testExplorerService.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <ddnspp/dnssd.h>
#include <ddnspp/initlog.hpp>
#include <ddnspp/logv/logv.hpp>
#include <iostream>
#include <boost/thread.hpp>
#include <list>


typedef std::list<air::bonjour::RemoteServicePtr>	BonjourServiceBrowerPtrList;

void	listAllService( air::bonjour::ServiceTypeList& list)
{

	//"_exec._tcp",
	//air::bonjour::ServiceType st(air::bonjour::SP_TCP, "_exec");
	//list.push_back(st);
	air::bonjour::ServiceTypePraser::allWellkonwServiceTypeList(list);
}
void	onResolveAddress
(
 air::bonjour::ServiceFactory& fac,
 air::bonjour::AddressResolveData	data
 )
{
	if(data.error){
		std::cout<< "onResolveAddress fail:"<< data.error.getMessage() <<std::endl;
	}else{
		std::cout<< "onResolveAddress success:"<<  data.hostName << " <= "<<data.addr.to_string() <<std::endl;
	}
	//
}
//
void	onResolveService(air::bonjour::ServiceFactory& fac,
						 air::bonjour::ServiceResolveData data)
{
	air::bonjour::AddressResolverPtr service;

	/// must close it or you got xxx alive 2 minutes error in windows event manager
	fac.removeService(data.owner);
	data.owner->close();
	data.owner.reset();

	air::bonjour::BonjourError err;
	if(!data.error){
		//service=fac.createAddressResolver(air::bonjour::IP_V4,host,NULL,err);
		service=fac.createAddressResolver(
			air::bonjour::IP_V4,
			data.hostName,
			boost::bind(&onResolveAddress, boost::ref(fac), _1 ),
			err);
	}
	if(err){
		std::cout<< "createAddressResolver fail:"<< err.getMessage() <<std::endl;
	}
}

void	onFoundService(air::bonjour::ServiceFactory& fac,
					   air::bonjour::RemoteServiceData	data)
{
	//air::bonjour::ServiceResolverPtr rolser=fac.createServiceResolver(name,type,domain,err);
	air::bonjour::ServiceResolverPtr rolser;

	air::bonjour::BonjourError err;
	if(!data.error){
		//rolser=fac.createServiceResolver(name,type,domain,boost::bind(&onResolveService,boost::ref(fac),_1,_2,_3,_4,_5,_6,_7,_8),err);
		rolser=fac.createServiceResolver(data.serviceName,data.serviceType,data.domainName,boost::bind(&onResolveService,boost::ref(fac),_1),err);
	}
	if(err){
		std::cout<< "createServiceResolver fail:"<< err.getMessage() <<std::endl;
	}
}

void	close_all(air::bonjour::ServiceFactory& fac,BonjourServiceBrowerPtrList& all_srv)
{
	while(all_srv.size() > 0){
		boost::this_thread::sleep(boost::posix_time::milliseconds(3000));
		air::bonjour::RemoteServicePtr ptr=all_srv.front();
		all_srv.pop_front();
		ptr->getCoreContext()->close();
		fac.removeService(ptr);
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	//muradin::loger_v2::setGlobalLogLvl(muradin::loger_v2::LOG_LVL::LOG_LVL_TRACE);
	//muradin::loger_v2::setGlobalLogLvl(muradin::loger_v2::LOG_LVL_TRACE);
	std::cout<< "init longing"<<std::endl;
	//air::loging::logingInit(false,"service-explorer",argv[0]);

	boost::asio::io_service ios;
	std::cout<< "loading dnssd.dll"<<std::endl;
	air::bonjour::DNSDApi dll("dnssd.dll");
	if(!dll.load()){
		std::cout<< "dll.load fail"<<std::endl;
		return -1;
	}
	air::bonjour::BonjourError err;
	air::bonjour::ServiceFactory fac(ios,dll);

	air::bonjour::ServiceTypeList all_type_tcp;
	BonjourServiceBrowerPtrList all_srv;
	listAllService(all_type_tcp);

	air::bonjour::ServiceTypeList::const_iterator it=all_type_tcp.begin();
	air::bonjour::ServiceTypeList::const_iterator over=all_type_tcp.end();

	while (it != over){
		//air::bonjour::RemoteServicePtr srv=fac.createServiceBrower(*it,"local",err);
		air::bonjour::RemoteServicePtr srv=fac.createServiceBrower(*it,"local",boost::bind(&onFoundService,boost::ref(fac),_1),err);
		if(NULL == srv){
			std::cout<< "createServiceBrower fail:"<< err.getMessage() <<std::endl;
		}else{
			//srv->setEvtCallback(boost::bind(&onFoundService,boost::ref(fac),_1,_2,_3,_4,_5,_6));
			all_srv.push_back(srv);
		}
		++it;
	}

	/// 
	air::bonjour::DomainEumeraterPtr srv=fac.createDomainEumerater(0,NULL,err);
	if(NULL == srv){
		std::cout<< "createServiceBrower fail:"<< err.getMessage() <<std::endl;
	}

	boost::thread_group threads;
	size_t	thread_size=1;
	for (size_t i=0;i<thread_size;++i){
		threads.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run,boost::ref(ios)) ));
	}
	//threads.add_thread(new boost::thread(boost::bind(&close_all,boost::ref(fac),boost::ref(all_srv)) ));

	//close_all(fac,all_srv);

	threads.join_all();

	return 0;
}


