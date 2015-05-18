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

void	onNatDone(air::bonjour::NatMapingData	data)
{
	//
}


int	run_test()
{
	std::cout<< "init longing"<<std::endl;

	boost::asio::io_service ios;
	std::cout<< "loading dnssd.dll"<<std::endl;
	air::bonjour::DNSDApi dll("dnssd.dll");
	if(!dll.load()){
		std::cout<< "dll.load fail"<<std::endl;
		return -1;
	}
	air::bonjour::BonjourError err;
	air::bonjour::ServiceFactory fac(ios,dll);


	air::bonjour::ProtoType proto=air::bonjour::ProtoType::ProtoTypeALL();
	//air::bonjour::ProtoType proto=air::bonjour::ProtoType::ProtoTcpOnly();
	//air::bonjour::ProtoType proto=air::bonjour::ProtoType::ProtoUdpOnly();

	unsigned short internalPort=65432;
	unsigned short externalPort=0;
	unsigned int ttl_second=10;
	/// 
	{

		LOG_INFO<<"createNatMappingService..." ;
		LOG_INFO<<"para - proto:"<< proto.toStringLong();
		LOG_INFO<<"para - internalPort:"<<internalPort;
		LOG_INFO<<"para - externalPort:"<<externalPort;
		LOG_INFO<<"para - ttl:"<<ttl_second;
	}

	/*
	air::bonjour::NatMappingServicePtr srv=fac.createNatMappingService(
		0,
		proto,
		internalPort,
		externalPort,
		ttl_second, 
		NULL,
		err);
		*/


	air::bonjour::NatMappingServicePtr srv=fac.createNatMappingService(
		0,
		proto,
		internalPort,
		externalPort,
		ttl_second, 
		boost::bind(&onNatDone,_1),
		err);
	//onNatDone
	if(NULL == srv){
		std::cout<< "createNatMappingService fail:"<< err.getMessage() <<std::endl;
	}

	boost::thread_group threads;
	size_t	thread_size=1;
	for (size_t i=0;i<thread_size;++i){
		threads.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run,boost::ref(ios)) ));
	}

	threads.join_all();
	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	muradin::loger_v2::setGlobalLogLvl(muradin::loger_v2::LOG_LVL_TRACE);
	run_test();
	return 0;
}

