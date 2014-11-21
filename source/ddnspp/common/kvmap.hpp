#ifndef KVMAP_H
#define KVMAP_H
#include <map>
#include <string>

namespace air{namespace common {

typedef std::map<std::string,std::string>      StringMap;

static std::string  getString(const StringMap& map,const std::string& sepataler)
{
    std::string val;
    StringMap::const_iterator it=map.begin();
    StringMap::const_iterator over=map.end();
    while(it != over){
        val+= it->first + std::string("=")+it->second;
        val+=sepataler;
        ++it;
    }
    return val;
}

}}
#endif // KVMAP_H
