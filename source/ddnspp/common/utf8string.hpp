#ifndef UTF8STRING_H
#define UTF8STRING_H

#include <string>

namespace air{namespace common {
class UTF8String
{
private:
    std::string     str;

public:
    UTF8String();
    std::string   getString()const;
};

UTF8String::UTF8String()
{
}

std::string   UTF8String::getString()const
{
	return "";
}

}}
#endif // UTF8STRING_H
