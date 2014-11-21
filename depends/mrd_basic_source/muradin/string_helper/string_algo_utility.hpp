#ifndef MURADIN_BASE_STRING_ALGO_UTILITY_HPP_
#define MURADIN_BASE_STRING_ALGO_UTILITY_HPP_


namespace muradin
{
	namespace string_helper{

		namespace utility{

			template<typename ContainerType>
			static void increment_count(const typename ContainerType::key_type& key,ContainerType& container)
			{
				typedef typename ContainerType::iterator	Itr;
				typedef typename ContainerType::value_type	ValType;
				Itr exits=container.find(key);
				if (exits!=container.end()){
					++exits->second;
				}else{
					container.insert(ValType(key,1) );
				}
			}
			

		}
	}
}
#endif//MURADIN_BASE_STRING_ALGO_UTILITY_HPP_