#ifndef BONJOUR_NAT_MAP_STATUS_HPP
#define BONJOUR_NAT_MAP_STATUS_HPP

#include <string>

namespace air
{
	namespace bonjour
	{
		/*
		*     - If the machine already has a routable public IP address, then your callback
		*       will just be invoked giving your own address and port.
		*     - If a NAT mapping is required and obtained, then your callback will be invoked
		*       giving you the external address and port.
		*     - If a NAT mapping is required but not obtained from the local NAT gateway,
		*       or the machine has no network connectivity, then your callback will be
		*       invoked giving zero address and port.
		*/
		class NatStatus
		{
		public:
			enum	NAT_STAT_CODE{
				NAT_MAPED,				//need NAT,and done
				NAT_ALREADY_ROUTABLE,	//no need NAT,already has a routable public IP address
				NAT_NOT_OBTAINED		//need NAT,but not obtained from the local NAT gateway 
			};
		private:
			NAT_STAT_CODE	sc;
		public:
			NatStatus()
				:sc(NAT_NOT_OBTAINED)
			{

			}
			explicit NatStatus(NatStatus::NAT_STAT_CODE stat)
				:sc(stat)
			{

			}
			NatStatus::NAT_STAT_CODE	getStatus()const{
				return sc;
			}

			/// call this when nat call done
			void	reset(unsigned int externalAddress,unsigned short externalPort,unsigned short internalPort)
			{
				if(externalAddress == 0 && externalPort == 0){
					sc=NAT_NOT_OBTAINED;
					return;
				}
				if(externalAddress != 0 && externalPort != 0){
					sc=NAT_MAPED;
					return;
				}
				sc=NAT_ALREADY_ROUTABLE;

			}
			bool	isMaped()const{
				return sc==NAT_MAPED;
			}
			bool	isRouteable()const{
				return sc==NAT_MAPED || sc==NAT_ALREADY_ROUTABLE;
			}
			std::string	toString()const
			{
				if(sc==NAT_MAPED){
					return "NAT_MAPED";
				}else if(sc==NAT_ALREADY_ROUTABLE){
					return "NAT_ALREADY_ROUTABLE";
				}else if(sc==NAT_NOT_OBTAINED){
					return "NAT_NOT_OBTAINED";
				}else{
					return "Unknow NAT status";
				}
			}
			std::string	getDescribtion()const
			{
				if(sc==NAT_MAPED){
					return "NAT mapping is required and obtained";
				}else if(sc==NAT_ALREADY_ROUTABLE){
					return "already has a routable public IP address";
				}else if(sc==NAT_NOT_OBTAINED){
					return "NAT mapping is required but not obtained from the local NAT gateway";
				}else{
					return "Unknow NAT status";
				}
			}
		};
		
	}
}
#endif//BONJOUR_NAT_MAP_STATUS_HPP