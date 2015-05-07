// testNat.cpp : 定义控制台应用程序的入口点。
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



int	run_test()
{
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

	/*
	air::bonjour::ServiceTypeList all_type_tcp;
	BonjourServiceBrowerPtrList all_srv;
	listAllService(all_type_tcp);

	air::bonjour::ServiceTypeList::const_iterator it=all_type_tcp.begin();
	air::bonjour::ServiceTypeList::const_iterator over=all_type_tcp.end();

	while (it != over){
		air::bonjour::RemoteServicePtr srv=fac.createServiceBrower(*it,"local",boost::bind(&onFoundService,boost::ref(fac),_1,_2,_3,_4,_5,_6),err);
		if(NULL == srv){
			std::cout<< "createServiceBrower fail:"<< err.getMessage() <<std::endl;
		}else{
			all_srv.push_back(srv);
		}
		++it;
	}
	*/

	//air::bonjour::ProtoType proto=air::bonjour::ProtoType::ProtoTypeALL();
	//air::bonjour::ProtoType proto=air::bonjour::ProtoType::ProtoTcpOnly();
	air::bonjour::ProtoType proto=air::bonjour::ProtoType::ProtoUdpOnly();
	unsigned short listen_port=65432;
	unsigned int ttl_second=0;
	/// 
	air::bonjour::NatMappingServicePtr srv=fac.createNatMappingService(
		0,
		proto,
		listen_port,
		0,
		ttl_second, 
		NULL,
		err);
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
}
int _tmain(int argc, _TCHAR* argv[])
{
	run_test();
	return 0;
}

