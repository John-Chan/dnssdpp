#ifndef BONJOUR_UTILITY_FUNCS_H
#define BONJOUR_UTILITY_FUNCS_H

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>

#include <boost/cstdint.hpp>

#include <string>

#include <muradin/string_helper/str_int.hpp>

namespace air{namespace bonjour {

	class BonjourUtility
	{
	private:
		const DNSDApi& dnsDll;
	public:
		BonjourUtility(const DNSDApi& dll)
			:dnsDll(dll)
		{
			//
		}
		virtual ~BonjourUtility()
		{
			//
		}

		static void		extractVersionNumber(boost::uint32_t u32_ver,boost::uint32_t& major_ver,boost::uint32_t& minor_ver)
		{
			major_ver=u32_ver/10000;
			minor_ver=(u32_ver/100)%100;
		}
	
		/// get the Bonjour SDK version ( used by your project )
		/// return kDNSServiceErr_Invalid Bonjour SDK header file not exists
		BonjourError	getSdkVersion(boost::uint32_t& u32_ver)const{
#ifndef _DNS_SD_H
			return BonjourError(kDNSServiceErr_Invalid);
#else
			u32_ver=_DNS_SD_H;
			return BonjourError(kDNSServiceErr_NoError);
#endif
		}

		/// get the Bonjour SDK version ( used by your project )
		/// return def_val on  fail to get the version
		boost::uint32_t	getSdkVersionDef(boost::uint32_t def_val)const
		{
			boost::uint32_t u32_ver;
			BonjourError be=getSdkVersion(u32_ver);
			if(be.isGood()){
				return u32_ver;
			}else{
				return def_val;
			}
		}
		BonjourError	getDaemonVersion(boost::uint32_t& u32_ver)const
		{
			if(NULL == dnsDll.getFunctiontable().funcDNSServiceGetProperty){
				return BonjourError(kDNSServiceErr_Invalid);
			}
			boost::uint32_t igrone_u32=0;
			/// maybe bug in DNSServiceGetProperty,u32_ver not update even the api return no_error.
			DNSServiceErrorType	ec=dnsDll.getFunctiontable().funcDNSServiceGetProperty(
				kDNSServiceProperty_DaemonVersion,&u32_ver,&igrone_u32
				);
			u32_ver=air::common::as_be32(u32_ver);
			return BonjourError(ec);
		}
		/// get the Bonjour SDK version ( used by your project )
		/// return def_val on  fail to get the version
		boost::uint32_t	getDaemonVersionDef(boost::uint32_t def_val)const
		{
			boost::uint32_t u32_ver;
			BonjourError be=getDaemonVersion(u32_ver);
			if(be.isGood()){
				return u32_ver;
			}else{
				return def_val;
			}
		}
		BonjourError	getDaemonVersion(boost::uint32_t& major_ver,boost::uint32_t& minor_ver)const{

			boost::uint32_t u32_ver;
			BonjourError be=getDaemonVersion(u32_ver);
			if(!be){
				extractVersionNumber(u32_ver,major_ver,minor_ver);
			}
			return be;
		}
		BonjourError	getDaemonVersion(std::string& ver_str)const
		{
			boost::uint32_t u32_ver;
			BonjourError be=getDaemonVersion(u32_ver);
			if(!be){
				ver_str=muradin::string_helper::int_to_str<boost::uint32_t>(u32_ver);
			}
			return be;
		}

		BonjourError	getDaemonVersion(std::string& major_ver,std::string& minor_ver)const
		{
			boost::uint32_t major_part;
			boost::uint32_t minor_part;
			BonjourError be=getDaemonVersion(major_part,minor_part);
			if(!be){
				major_ver=muradin::string_helper::int_to_str<boost::uint32_t>(major_part);
				minor_ver=muradin::string_helper::int_to_str<boost::uint32_t>(minor_part);
			}
			return be;
		}
		/// get the version number of Bonjour Daemon service 
		/// return def_val on  fail to get the version
		std::string	getDaemonVersionDef(const std::string& def_val)const
		{
			std::string ver_str;
			BonjourError be=getDaemonVersion(ver_str);
			if(be.isGood()){
				return ver_str;
			}else{
				return def_val;
			}
		}

		/* ConstructFullDomainName()
		 *
		 * Concatenate a three-part domain name (as returned by the above callbacks) into a
		 * properly-escaped full domain name. Note that callbacks in the above functions ALREADY ESCAPE
		 * strings where necessary.
		 *
		 * Parameters:
		 *
		 * fullName:        The resulting full domain name is to be written.
		 *                  The buffer must be kDNSServiceMaxDomainName (1009) bytes in length to
		 *                  accommodate the longest legal domain name without buffer overrun.
		 *
		 * serviceName:     The service name - any dots or backslashes must NOT be escaped.
		 *                  May be EMPTY (to construct a PTR record name, e.g.
		 *                  "_ftp._tcp.apple.com.").
		 *
		 * serviceType:     The service type followed by the protocol, separated by a dot
		 *                  (e.g. "_ftp._tcp").
		 *
		 * domainName:      The domain name, e.g. "apple.com.". Literal dots or backslashes,
		 *                  if any, must be escaped, e.g. "1st\. Floor.apple.com."
		 *
		 * return value:    Returns kDNSServiceErr_NoError (0) on success, kDNSServiceErr_BadParam or kDNSServiceErr_Invalid on error.
		 *
		 */
		BonjourError	constructFullDomainName(
			const std::string& serviceName,
			const std::string& serviceType,
			const std::string& domainName,
			std::string& fullName)const
		{
			if(NULL == dnsDll.getFunctiontable().funcDNSServiceConstructFullName){
				return BonjourError(kDNSServiceErr_Invalid);
			}
			const char* svc_or_null=serviceName.empty()?(NULL):(serviceName.c_str());
			// The buffer must be kDNSServiceMaxDomainName (1009) bytes in length to
			// accommodate the longest legal domain name without buffer overrun.
			char buff[kDNSServiceMaxDomainName+10]={0};
			DNSServiceErrorType	ec=dnsDll.getFunctiontable().funcDNSServiceConstructFullName(
				buff,svc_or_null,serviceType.c_str(),domainName.c_str()
				);
			BonjourError warpedEc(ec);
			if(warpedEc.isGood()){
				fullName=buff;
			}
			return warpedEc;
		}
	};
}}
#endif//BONJOUR_UTILITY_FUNCS_H