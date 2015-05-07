#ifndef BONJOUR_TCP_PROTO_TYPE_HPP
#define BONJOUR_TCP_PROTO_TYPE_HPP

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <muradin/base/bitmask.hpp>

#include <string>

namespace air
{
	namespace bonjour
	{
		
		class ProtoType
		{
		private:
			muradin::base::bitmasku32 masku32;
		public:
			static	ProtoType	ProtoTypeALL()
			{
				return ProtoType(kDNSServiceProtocol_UDP|kDNSServiceProtocol_TCP);
			}
			static	ProtoType	ProtoTcpOnly()
			{
				return ProtoType(kDNSServiceProtocol_TCP);
			}
			static	ProtoType	ProtoUdpOnly()
			{
				return ProtoType(kDNSServiceProtocol_UDP);
			}
			ProtoType(unsigned int val){
				masku32.reset(val);
			}
			ProtoType(){
				masku32.reset(0);
			}
			~ProtoType(){
				//
			}
			unsigned int	getVal()const
			{
				return masku32.getVal();
			}
			bool	isEmpty()const
			{
				return (!haveUdp()) && (!haveTcp());
			}
			bool	haveUdp()const{
				return masku32.contian(kDNSServiceProtocol_UDP);
			}
			bool	haveTcp()const{
				return masku32.contian(kDNSServiceProtocol_TCP);
			}
			bool	haveBoth()const{
				return haveUdp()&&haveTcp();
			}
			void	setUdp(){
				masku32.add_flag(kDNSServiceProtocol_UDP);
			}
			void	setTcp(){
				masku32.add_flag(kDNSServiceProtocol_TCP);
			}
			void	setBoth(){
				setUdp();
				setTcp();
			}
			void	clear(){
				masku32.reset(0);
			}
			std::string	toString()const{
				if(haveBoth()){
					return "TCP|UDP" ;
				}else if(haveUdp()){
					return "UDP" ;
				}else if(haveUdp()){
					return "TCP" ;
				}else{
					return "Empty" ;
				}
			}
		};
	}
}
#endif//BONJOUR_TCP_PROTO_TYPE_HPP