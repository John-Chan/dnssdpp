/************************************************************************
//  dynamic load dnsd API(dnssd.dll)
//  --------------------------------------------------------------------
//  the dnssd.dll is static linked to runtime library,that shit!
************************************************************************/

#ifndef DNSDAPI_H
#define DNSDAPI_H


#include <string>
#include <ddnspp/common/dlloader.hpp>
#include <ddnspp/bonjourpp/bonjourfunc.h>
#include <memory.h>
namespace air
{
namespace bonjour
{

/// function pointers

typedef struct BonjourExports_ {
    /// Service Registration
    FuncDNSServiceRegister          funcDNSServiceRegister;

    /// Unix Domain Socket access, DNSServiceRef deallocation, and data processing functions
    FuncDNSServiceRefSockFD         funcDNSServiceRefSockFD;
    FuncDNSServiceProcessResult     funcDNSServiceProcessResult;
    FuncDNSServiceRefDeallocate     funcDNSServiceRefDeallocate;

    /// TXT Record Construction Functions
    FuncTXTRecordGetBytesPtr		funcTXTRecordGetBytesPtr;
    FuncTXTRecordGetLength			funcTXTRecordGetLength;
    FuncTXTRecordRemoveValue		funcTXTRecordRemoveValue;
    FuncTXTRecordSetValue			funcTXTRecordSetValue;
    FuncTXTRecordCreate				funcTXTRecordCreate;
    FuncTXTRecordDeallocate			funcTXTRecordDeallocate;

    ///TXT Record Parsing Functions
    FuncTXTRecordContainsKey		funcTXTRecordContainsKey;
    FuncTXTRecordGetValuePtr		funcTXTRecordGetValuePtr;
    FuncTXTRecordGetCount			funcTXTRecordGetCount;
    FuncTXTRecordGetItemAtIndex		funcTXTRecordGetItemAtIndex;

    /// Service Discovery
    FuncDNSServiceBrowse			funcDNSServiceBrowse;
    FuncDNSServiceResolve			funcDNSServiceResolve;

    /// address reslove
    FuncDNSServiceGetAddrInfo		funcDNSServiceGetAddrInfo;

    /// Domain Enumeration
    FuncDNSServiceEnumerateDomains	funcDNSServiceEnumerateDomains;

	/// NAT
	FuncDNSServiceNATPortMappingCreate	funcDNSServiceNATPortMappingCreate;
} BonjourExports;

class DNSDApi
{
private:
    air::common::DllLoader       dll;
    BonjourExports  exported;
public:


    static bool    DNSDApi::haveAll(const BonjourExports& test)
    {
        return (	haveServiceRegistrationFuncs(test)		&&
                    haveSocketAccessFuncs(test)				&&
                    haveRecordConstructionFuncs(test)		&&
                    haveServiceDiscoveryFuncs(test)			&&
                    haveRecordParsingFuncs(test)			&&
                    haveAddressResolveFuncs(test)			&&
                    haveDomainEnumerationFuncs(test)		&&
					haveNatMappingFuncs(test)
               );

    }

    static bool    DNSDApi::haveSocketAccessFuncs(const BonjourExports& test)
    {
        return (test.funcDNSServiceRefSockFD != NULL &&
                test.funcDNSServiceProcessResult != NULL &&
                test.funcDNSServiceRefDeallocate != NULL );
    }
    static bool    DNSDApi::haveServiceRegistrationFuncs(const BonjourExports& test)
    {
        return (test.funcDNSServiceRegister != NULL);
    }
    static bool    DNSDApi::haveRecordConstructionFuncs(const BonjourExports& test)
    {
        return (test.funcTXTRecordDeallocate != NULL &&
                test.funcTXTRecordCreate != NULL &&
                test.funcTXTRecordSetValue != NULL &&
                test.funcTXTRecordRemoveValue != NULL &&
                test.funcTXTRecordGetLength != NULL &&
                test.funcTXTRecordGetBytesPtr != NULL );
    }
    static bool    DNSDApi::haveServiceDiscoveryFuncs(const BonjourExports& test)
    {
        return (test.funcDNSServiceBrowse != NULL && test.funcDNSServiceResolve != NULL);
    }
    static bool    DNSDApi::haveRecordParsingFuncs(const BonjourExports& test)
    {
        return (test.funcTXTRecordContainsKey != NULL &&
                test.funcTXTRecordGetValuePtr != NULL &&
                test.funcTXTRecordGetCount != NULL &&
                test.funcTXTRecordGetItemAtIndex != NULL );
    }
    static bool    DNSDApi::haveAddressResolveFuncs(const BonjourExports& test)
    {
        return (test.funcDNSServiceGetAddrInfo != NULL );
    }
    static bool    DNSDApi::haveDomainEnumerationFuncs(const BonjourExports& test)
    {
        return (test.funcDNSServiceEnumerateDomains != NULL );
	}
	static bool    DNSDApi::haveNatMappingFuncs(const BonjourExports& test)
	{
		return (test.funcDNSServiceNATPortMappingCreate != NULL );
	}
    DNSDApi(const std::string& bonjourDllPath)
        :dll(bonjourDllPath.c_str())
    {
        clean();
    }
    ~DNSDApi()
    {
        unload();
    }


    /************************************************************************
    //  load function
    //  --------------------------------------------------------------------
    //  those functions not loaded:
    //
    //  DNSServiceAddRecord
    //  DNSServiceConstructFullName
    //  DNSServiceCreateConnection
    //  DNSServiceGetProperty
    //  DNSServiceQueryRecord
    //  DNSServiceReconfirmRecord
    //  DNSServiceRegisterRecord
    //  DNSServiceRemoveRecord
    //  DNSServiceUpdateRecord
    //
    //
    //
    //
    ************************************************************************/
    bool    load()
    {
        if(dll.load()) {
            exported.funcDNSServiceRegister=dll.load_dll_func<FuncDNSServiceRegister>("DNSServiceRegister") ;
            exported.funcDNSServiceRefSockFD=dll.load_dll_func<FuncDNSServiceRefSockFD>("DNSServiceRefSockFD") ;
            exported.funcDNSServiceProcessResult=dll.load_dll_func<FuncDNSServiceProcessResult>("DNSServiceProcessResult") ;
            exported.funcDNSServiceRefDeallocate=dll.load_dll_func<FuncDNSServiceRefDeallocate>("DNSServiceRefDeallocate") ;
            ///
            exported.funcTXTRecordGetBytesPtr=dll.load_dll_func<FuncTXTRecordGetBytesPtr>("TXTRecordGetBytesPtr") ;
            exported.funcTXTRecordGetLength=dll.load_dll_func<FuncTXTRecordGetLength>("TXTRecordGetLength") ;
            exported.funcTXTRecordRemoveValue=dll.load_dll_func<FuncTXTRecordRemoveValue>("TXTRecordRemoveValue") ;
            exported.funcTXTRecordSetValue=dll.load_dll_func<FuncTXTRecordSetValue>("TXTRecordSetValue") ;
            exported.funcTXTRecordCreate=dll.load_dll_func<FuncTXTRecordCreate>("TXTRecordCreate") ;
            exported.funcTXTRecordDeallocate=dll.load_dll_func<FuncTXTRecordDeallocate>("TXTRecordDeallocate") ;
            ///
            exported.funcDNSServiceBrowse=dll.load_dll_func<FuncDNSServiceBrowse>("DNSServiceBrowse") ;
            exported.funcDNSServiceResolve=dll.load_dll_func<FuncDNSServiceResolve>("DNSServiceResolve") ;
            ///
            exported.funcTXTRecordContainsKey=dll.load_dll_func<FuncTXTRecordContainsKey>("TXTRecordContainsKey") ;
            exported.funcTXTRecordGetValuePtr=dll.load_dll_func<FuncTXTRecordGetValuePtr>("TXTRecordGetValuePtr") ;
            exported.funcTXTRecordGetCount=dll.load_dll_func<FuncTXTRecordGetCount>("TXTRecordGetCount") ;
            exported.funcTXTRecordGetItemAtIndex=dll.load_dll_func<FuncTXTRecordGetItemAtIndex>("TXTRecordGetItemAtIndex") ;
            ///
            exported.funcDNSServiceGetAddrInfo=dll.load_dll_func<FuncDNSServiceGetAddrInfo>("DNSServiceGetAddrInfo") ;
            ///
            exported.funcDNSServiceEnumerateDomains=dll.load_dll_func<FuncDNSServiceEnumerateDomains>("DNSServiceEnumerateDomains") ;
			///
			exported.funcDNSServiceNATPortMappingCreate=dll.load_dll_func<FuncDNSServiceNATPortMappingCreate>("DNSServiceNATPortMappingCreate") ;

        }
        return haveAll(exported);
    }
    void    unload()
    {
        if(dll.loaded()) {
            dll.unload();
        }
        clean();
    }
    bool    fullLoaded()const
    {
        return haveAll(exported);
    }

    const BonjourExports&  getFunctiontable()const
    {
        return exported;
    }

private:
    void    clean()
    {
        ::memset(&exported,0x0,sizeof (exported));
    }

};


}
}

#endif // DNSDAPI_H
