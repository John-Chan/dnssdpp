#ifndef DNS_SD_INCL_H__
#define DNS_SD_INCL_H__

/// utility
#include <ddnspp/bonjourpp/servicetypepraser.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>
#include <ddnspp/bonjourpp/servicetype.hpp>
#include <ddnspp/bonjourpp/txtrecords.hpp>
#include <ddnspp/bonjourpp/prototype.hpp>
#include <ddnspp/bonjourpp/nat_status.hpp>
#include <ddnspp/common/utf8string.hpp>

///user no need to use the follow headers
#include <ddnspp/bonjourpp/bonjourservice.hpp>		/// to publish a service
#include <ddnspp/bonjourpp/bonjoursvcbrower.hpp>	/// to find a service
#include <ddnspp/bonjourpp/serviceresolver.hpp>		/// get service information
#include <ddnspp/bonjourpp/addressresolver.hpp>		/// get address information
#include <ddnspp/bonjourpp/domianenumerater.hpp>	/// get domian information
#include <ddnspp/bonjourpp/natmaping.hpp>			/// NAT mapping

/// the follow 2 header is useful for user
#include <ddnspp/bonjourpp/servicefactory.hpp>		/// used to create a service to do something
#include <ddnspp/bonjourpp/dnsd_callback.hpp>		/// callback func definition,used to receive event  



#endif // DNS_SD_INCL_H__