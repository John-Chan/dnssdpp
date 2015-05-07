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

/// worker
#include <ddnspp/bonjourpp/bonjourservice.hpp>		/// to publish a service
#include <ddnspp/bonjourpp/bonjoursvcbrower.hpp>	/// to find a service
#include <ddnspp/bonjourpp/serviceresolver.hpp>		/// get service information
#include <ddnspp/bonjourpp/addressresolver.hpp>		/// get address information
#include <ddnspp/bonjourpp/domianenumerater.hpp>	/// get domian information
#include <ddnspp/bonjourpp/natmaping.hpp>			/// NAT mapping

/// factory for workers
#include <ddnspp/bonjourpp/servicefactory.hpp>



#endif // DNS_SD_INCL_H__