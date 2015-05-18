// testRegService.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <ddnspp/dnssd.h>
#include <ddnspp/initlog.hpp>
#include <ddnspp/logv/logv.hpp>
#include <iostream>
#include <boost/thread.hpp>
// "vv=1 pw=0 srcvers=150.33 deviceid=00:0E:C6:F5:6B:46 features=0x100029ff model=AppleTV3,1 "
// "txtvers=1 vv=1 cn=0,1,2,3 ek=1 am=AppleTV3,1 et=0,1,3 sv=false tp=UDP sm=false ss=16 sr=44100 vs=150.33 ch=2 vn=65537 da=true md=0,1,2 sf=0x4 pw=false "

air::bonjour::LocalServicePtr	makeService(boost::asio::io_service& ios,const air::bonjour::DNSDApi& dll,std::string srv_type ,std::string name,std::string port,air::common::StringMap records)
{
	//  -R "My Test" _http._tcp . 80 path=/path-to-page.html
	air::bonjour::ServiceFactory fac(ios,dll);
	air::bonjour::ServiceType st(air::bonjour::SP_TCP,srv_type);
	air::bonjour::TxtRecordEncoder txtEncoder(dll,5,true);
	const char* old_ptr=txtEncoder.getRecordPtr();
	air::bonjour::BonjourError err;
	err = txtEncoder.putOrReplace(records);
	if(err){
		LOG_ERR<<"txtEncoder.putOrReplace "<< err.getMessage()<<std::endl;
	}
	size_t len=txtEncoder.getRecordLenth();
	const char* new_ptr=txtEncoder.getRecordPtr();
	//air::bonjour::LocalServicePtr ptr=fac.registerService(name,st,"local","sunnysce.local",port,txtEncoder,NULL,err);
	air::bonjour::LocalServicePtr ptr=fac.registerService(name,st,"","",port,txtEncoder,NULL,err);
	if(err){
		LOG_ERR<<"RegisterService "<< err.getMessage()<<std::endl;
	}
	return ptr;
}

void	list_all_srvtype()
{
	std::list<air::bonjour::ServiceType> all;
	air::bonjour::ServiceTypePraser::allWellkonwServiceTypeList(all);
}
int _tmain(int argc, _TCHAR* argv[])
{
	std::cout<< "init longing"<<std::endl;
	//air::loging::logingInit(true,"service-reg",argv[0]);

	boost::asio::io_service ios;
	std::cout<< "loading dnssd.dll"<<std::endl;
	air::bonjour::DNSDApi dll("dnssd.dll");
	if(!dll.load()){
		std::cout<< "dll.load fail"<<std::endl;
		return -1;
	}
	// "vv=1 pw=0 srcvers=150.33 deviceid=00:0E:C6:F5:6B:46 features=0x100029ff model=AppleTV3,1 "
	air::common::StringMap airPlayRecords;
	airPlayRecords.insert(air::common::StringMap::value_type("vv","1"));
	airPlayRecords.insert(air::common::StringMap::value_type("pw","0"));
	airPlayRecords.insert(air::common::StringMap::value_type("srcvers","150.33"));
	//38:59:F9:85:AD:23
	//00:0E:C6:F5:6B:46
	airPlayRecords.insert(air::common::StringMap::value_type("deviceid","00:0E:C6:F5:6B:46"));
	airPlayRecords.insert(air::common::StringMap::value_type("features","0x100029ff"));
	airPlayRecords.insert(air::common::StringMap::value_type("model","AppleTV3,1"));

	// "txtvers=1 vv=1 cn=0,1,2,3 ek=1 am=AppleTV3,1 et=0,1,3 sv=false tp=UDP sm=false ss=16 sr=44100 vs=150.33 ch=2 vn=65537 da=true md=0,1,2 sf=0x4 pw=false "
	air::common::StringMap raopRecords;
	raopRecords.insert(air::common::StringMap::value_type("txtvers","1"));
	raopRecords.insert(air::common::StringMap::value_type("vv","1"));
	raopRecords.insert(air::common::StringMap::value_type("cn","0,1,2,3"));
	raopRecords.insert(air::common::StringMap::value_type("ek","1"));
	raopRecords.insert(air::common::StringMap::value_type("am","AppleTV3,1"));
	raopRecords.insert(air::common::StringMap::value_type("et","0,1,3"));
	raopRecords.insert(air::common::StringMap::value_type("sv","false"));
	raopRecords.insert(air::common::StringMap::value_type("tp","UDP"));
	raopRecords.insert(air::common::StringMap::value_type("sm","false"));
	raopRecords.insert(air::common::StringMap::value_type("ss","16"));
	raopRecords.insert(air::common::StringMap::value_type("sr","44100"));
	raopRecords.insert(air::common::StringMap::value_type("vs","150.33"));
	raopRecords.insert(air::common::StringMap::value_type("ch","2"));
	raopRecords.insert(air::common::StringMap::value_type("vn","65537"));
	raopRecords.insert(air::common::StringMap::value_type("da","true"));
	raopRecords.insert(air::common::StringMap::value_type("md","0,1,2"));
	raopRecords.insert(air::common::StringMap::value_type("sf","0x4"));
	raopRecords.insert(air::common::StringMap::value_type("pw","false"));

	boost::thread_group threads;
	std::string airplaySrvName="airplay-cj";
	std::string raopSrvName="000EC6F56B46@cj";
	//std::string airplayPort="7000";
	//std::string raopPort="49152";
	std::string airplayPort="34000";
	std::string raopPort="34000";

	air::bonjour::LocalServicePtr airplaySvc=makeService(ios,dll,"_airplay",airplaySrvName,airplayPort,airPlayRecords);
	air::bonjour::LocalServicePtr raopSvc=makeService(ios,dll,"_raop",raopSrvName,raopPort,raopRecords);


	threads.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run,boost::ref(ios)) ));
	threads.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run,boost::ref(ios)) ));
	threads.join_all();
	
	return 0;
}
/*
POST /fp-setup RTSP/1.0
X-Apple-ET: 32
CSeq: 0
	  X-Apple-Device-ID: 0xf4f15a2307da
	  DACP-ID: 247974BAEE94AB89
	  Active-Remote: 3511553067
	  Content-Type: application/octet-stream
	  Content-Length: 16
	  User-Agent: AirPlay/211.2

	  FPLY
*/
