#ifndef SERVICETYPE_HPP
#define SERVICETYPE_HPP

/*
 * service type:
 *                  The service type followed by the protocol, separated by a dot
 *                  (e.g. "_ftp._tcp"). The service type must be an underscore, followed
 *                  by 1-14 characters, which may be letters, digits, or hyphens.
 *                  The transport protocol must be "_tcp" or "_udp". New service types
 *                  should be registered at <http://www.dns-sd.org/ServiceTypes.html>.
 *
 *                  Additional subtypes of the primary service type (where a service
 *                  type has defined subtypes) follow the primary service type in a
 *                  comma-separated list, with no additional spaces, e.g.
 *                      "_primarytype._tcp,_subtype1,_subtype2,_subtype3"
 *                  Subtypes provide a mechanism for filtered browsing: A client browsing
 *                  for "_primarytype._tcp" will discover all instances of this type;
 *                  a client browsing for "_primarytype._tcp,_subtype2" will discover only
 *                  those instances that were registered with "_subtype2" in their list of
 *                  registered subtypes.
 *
 *                  The subtype mechanism can be illustrated with some examples using the
 *                  dns-sd command-line tool:
 *
 *                  % dns-sd -R Simple _test._tcp "" 1001 &
 *                  % dns-sd -R Better _test._tcp,HasFeatureA "" 1002 &
 *                  % dns-sd -R Best   _test._tcp,HasFeatureA,HasFeatureB "" 1003 &
 *
 *                  Now:
 *                  % dns-sd -B _test._tcp             # will find all three services
 *                  % dns-sd -B _test._tcp,HasFeatureA # finds "Better" and "Best"
 *                  % dns-sd -B _test._tcp,HasFeatureB # finds only "Best"
 *
 *@see RegisterService
 *@link :  <http://www.dns-sd.org/ServiceTypes.html>.
 *
 */

#include <string>
#include <list>
#include <map>
#include <exception>
#include <ddnspp/bonjourpp/apple_dnsd_incl.h>

namespace air{namespace bonjour {

enum	ADDRESS_TYPE{
	IP_V4=kDNSServiceProtocol_IPv4,
	IP_V6=kDNSServiceProtocol_IPv6,
	IP_V4_V6=kDNSServiceProtocol_IPv4|kDNSServiceProtocol_IPv6
};
enum SERVICE_PROTOCOL{
	SP_TCP=kDNSServiceProtocol_TCP,
	SP_UDP=kDNSServiceProtocol_UDP,
	SP_TCP_UDP=kDNSServiceProtocol_TCP|kDNSServiceProtocol_UDP
};

static  const char* protocol_name(SERVICE_PROTOCOL protocol)
{
    if(SP_TCP == protocol){
        return "_tcp";
    }else if(SP_UDP == protocol){
        return "_udp";
    }else{
        throw std::runtime_error("SERVICE_PROTOCOL must be SP_TCP or SP_UDP");
    }
}
static  bool prase_protocol_type(const std::string& str,SERVICE_PROTOCOL& type)
{
	if(str == "_tcp") {
		type=SP_TCP;
		return true;
	}else if(str == "_udp"){
		type=SP_UDP;
		return true;
	}else{
		return false;
	}
}
class ServiceType
{
public:

	static const char*  PROTO_SEPATALER()
	{
		static const char* val=".";
		return val;
	}

	static const char* SUBTYPE_SEPATALER()
	{
		static const char* val=",";
		return val;
	}
private:
    SERVICE_PROTOCOL    protocol;
    std::list<std::string>      sub_type;
    std::string                 primary_type;
public:
	ServiceType()
		:protocol(SP_TCP),primary_type("")
	{
	}
    ServiceType(SERVICE_PROTOCOL protocol,const std::string& primary)
        :protocol(protocol),primary_type(primary)
    {
    }
	SERVICE_PROTOCOL& protocolType()
	{
		return protocol;
	}

	SERVICE_PROTOCOL protocolType()const
	{
		return protocol;
	}
    std::string&    primaryType()
    {
        return primary_type;
    }
    const std::string&    primaryType()const
    {
        return primary_type;
    }

    const std::list<std::string>&   subType()const
    {
        return sub_type;
    }
    std::list<std::string>&   subType()
    {
        return sub_type;
    }

    std::string     toString()const
    {
        std::string prased=primary_type+PROTO_SEPATALER()+protocol_name(protocol) ;
        std::list<std::string>::const_iterator it=sub_type.begin();
        std::list<std::string>::const_iterator over=sub_type.begin();
        while(it != over){
            if(it->length() > 0 ){
                prased=prased+SUBTYPE_SEPATALER()+ (*it);
            }
            ++it;
        }

        return prased;
    }


};

typedef std::list<ServiceType>	ServiceTypeList;

}}

#endif // SERVICETYPE_HPP
