#ifndef BONJOUR_SERVICETYPE_PRASER_HPP__
#define BONJOUR_SERVICETYPE_PRASER_HPP__
#include <ddnspp/bonjourpp/knownservice.hpp>
#include <ddnspp/bonjourpp/servicetype.hpp>

#include <muradin/string_helper/string_piece.hpp>
#include <muradin/string_helper/algo_single_char.hpp>


namespace air{namespace bonjour {


	class ServiceTypePraser
	{
	public:

		/// for 'xxx.xxx' format only
		static bool	prasePrimaryFromString(const std::string& str,ServiceType& st)
		{
			using namespace muradin::string_helper;
			bool got=false;
			string_piece dot(ServiceType::PROTO_SEPATALER());
			string_piece primary(str);
			if(primary.count()>= 6){// atleast '_x.xxx'
				StringPieceVec dot_strs=split_by_any_char(primary,dot,2,false);
				if(dot_strs.size() >= 2){ 
					SERVICE_PROTOCOL proto;
					if(prase_protocol_type(dot_strs[1].std_string(),proto)){
						st.protocolType()=proto;
						st.primaryType()=dot_strs[0].std_string();
						got=true;
					}
				}
			}
			return got;
		}
		/// format requirement: <TYPE.PROTO>[,XXX,XXX,....]
		/// E.G. _http.tcp
		/// E.G. _http.tcp,test1,test2
		static bool	praseFromString(const std::string& str,ServiceType& st)
		{
			using namespace muradin::string_helper;
			bool got=false;
			string_piece all(str);
			string_piece comma(ServiceType::SUBTYPE_SEPATALER());
			StringPieceVec comma_strs=split_by_any_char(all,comma,0,false);
			if(comma_strs.size()>0){

				string_piece  firstOne=comma_strs.front();
				if(prasePrimaryFromString(firstOne.std_string(),st)){
					StringPieceVec::const_iterator it=comma_strs.begin();
					StringPieceVec::const_iterator over=comma_strs.begin();
					++it; 
					while (it != over){
						st.subType().push_back(it->std_string());
					}
					got=true;
				}
				//if(firstOne.count()>= 6){// atleast '_x.xxx'
				//	StringPieceVec dot_strs=split_by_any_char(firstOne,dot,0,false);
				//	if(dot_strs.size() >= 2){ 
				//		SERVICE_PROTOCOL proto;
				//		if(prase_protocol_type(dot_strs[1].std_string(),proto)){
				//			st.protocolType()=proto;
				//			st.primaryType()=dot_strs[0].std_string();
				//			
				//			got=true;
				//		}
				//	}
				//}

			}else{
				got=prasePrimaryFromString(str,st);
			}

			return got;
		}


		/// got std::list<ServiceType> 
		static	size_t	allWellkonwServiceTypeList(std::list<ServiceType>& lst)
		{
			size_t got=0;
			for (unsigned int i=0;i< WELL_KNOWN_SRVICE_COUNT;++i){
				ServiceType st;
				if( praseFromString(WELL_KNOWN_SERVICE_TYPES[i],st)){
					lst.push_back(st);
					++got;
				}
			}
			return got;
		}

		/// std::map<std::string,ServiceType>
		/// map.key is ServiceType.primaryType()
		static	size_t	allWellkonwServiceTypeMap(std::map<std::string,ServiceType>& map)
		{
			std::list<ServiceType> lst;
			allWellkonwServiceTypeList(lst);

			std::list<ServiceType>::const_iterator it=lst.begin();
			std::list<ServiceType>::const_iterator over=lst.end();
			size_t got=0;
			while(it != over){
				if(map.end() !=  map.find(it->primaryType())){
					map.insert(std::map<std::string,ServiceType>::value_type(it->primaryType(),*it) );
					++got;
				}
				++it;
			}
			return got;
		}
	};
	
}}
#endif // BONJOUR_SERVICETYPE_PRASER_HPP__