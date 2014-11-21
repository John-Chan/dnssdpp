#ifndef BONJOURERROR_H
#define BONJOURERROR_H

#include <ddnspp/bonjourpp/apple_dnsd_incl.h>
#include <stdio.h> //sprintf
#include <string>
#include <boost/cstdint.hpp>

namespace air{namespace bonjour {

static  std::string     getErrorString(boost::int32_t ec)
{
	std::string errorString;
	switch(ec){
	case kDNSServiceErr_NoError:
		errorString= std::string("NoError");
		break;
	case kDNSServiceErr_Unknown:
		errorString= std::string("Unknown");
		break;
	case kDNSServiceErr_NoSuchName:
		errorString= std::string("NoSuchName");
		break;
	case kDNSServiceErr_NoMemory:
		errorString= std::string("NoMemory");
		break;
	case kDNSServiceErr_BadParam:
		errorString= std::string("BadParam");
		break;
	case kDNSServiceErr_BadReference:
		errorString= std::string("BadReference");
		break;
	case kDNSServiceErr_BadState:
		errorString= std::string("BadState");
		break;
	case kDNSServiceErr_BadFlags:
		errorString= std::string("BadFlags");
		break;
	case kDNSServiceErr_Unsupported:
		errorString= std::string("Unsupported");
		break;
	case kDNSServiceErr_NotInitialized:
		errorString= std::string("NotInitialized");
		break;
	case kDNSServiceErr_AlreadyRegistered:
		errorString= std::string("AlreadyRegistered");
		break;
	case kDNSServiceErr_NameConflict:
		errorString= std::string("NameConflict");
		break;
	case kDNSServiceErr_Invalid:
		errorString= std::string("Invalid");
		break;
	case kDNSServiceErr_Firewall:
		errorString= std::string("Firewall");
		break;
	case kDNSServiceErr_Incompatible:
		errorString= std::string("Incompatible,client library incompatible with daemon");
		break;
	case kDNSServiceErr_BadInterfaceIndex:
		errorString= std::string("BadInterfaceIndex");
		break;
	case kDNSServiceErr_Refused:
		errorString= std::string("Refused");
		break;
	case kDNSServiceErr_NoSuchRecord:
		errorString= std::string("NoSuchRecord");
		break;
	case kDNSServiceErr_NoAuth:
		errorString= std::string("NoAuth");
		break;
	case kDNSServiceErr_NoSuchKey:
		errorString= std::string("NoSuchKey");
		break;
	case kDNSServiceErr_NATTraversal:
		errorString= std::string("NATTraversal");
		break;
	case kDNSServiceErr_DoubleNAT:
		errorString= std::string("DoubleNAT");
		break;
	case kDNSServiceErr_BadTime:
		errorString= std::string("BadTime,Codes up to here existed in Tiger");
		break;
	case kDNSServiceErr_BadSig:
		errorString= std::string("BadSig");
		break;
	case kDNSServiceErr_BadKey:
		errorString= std::string("BadKey");
		break;
	case kDNSServiceErr_Transient:
		errorString= std::string("Transient");
		break;
	case kDNSServiceErr_ServiceNotRunning:
		errorString= std::string("ServiceNotRunning,Background daemon not running");
		break;
	case kDNSServiceErr_NATPortMappingUnsupported:
		errorString= std::string("NATPortMappingUnsupported,NAT doesn't support NAT-PMP or UPnP");
		break;
	case kDNSServiceErr_NATPortMappingDisabled:
		errorString= std::string("NATPortMappingDisabled,NAT supports NAT-PMP or UPnP but it's disabled by the administrator");
		break;
	case kDNSServiceErr_NoRouter:
		errorString= std::string("NoRouter,No router currently configured (probably no network connectivity)");
		break;
	case kDNSServiceErr_PollingMode:
		errorString= std::string("PollingMode");
		break;
	case kDNSServiceErr_Timeout:
		errorString= std::string("Timeout");
		break;

	default:
		char msg[256];
		::sprintf(msg,"Unknow error code:0x%08x (%d)",ec,ec);
		errorString= std::string(msg);
	}
	return errorString;
}
class BonjourError
{
private:
    boost::int32_t ec;
public:
	BonjourError()
		:ec(kDNSServiceErr_NoError)
	{
	}
	BonjourError(boost::int32_t code)
		:ec(code)
	{
		//
	}

    typedef void (*unspecified_bool_type)();
    static void unspecified_bool_true() {}
	static boost::int32_t    good()
	{
		return kDNSServiceErr_NoError;
	}


	void	reset(boost::int32_t code)
	{
		ec=code;
	}
	void	reset()
	{
		ec=kDNSServiceErr_NoError;
	}

    operator unspecified_bool_type() const   // true if error
    {
        return ec == good() ? 0 : unspecified_bool_true;
    }

    bool operator!() const   // true if no error
    {
        return ec == good();
    }


	boost::int32_t getErrorCode()const
	{
		return ec;
	}
	std::string     getMessage()const
	{
		return getErrorString(  ec);
	}
};

}}
#endif // BONJOURERROR_H
