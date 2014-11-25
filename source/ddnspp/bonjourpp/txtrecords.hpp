#ifndef TXT_RECORDS_H__
#define TXT_RECORDS_H__

#include <ddnspp/bonjourpp/dnsdapi.hpp>
#include <ddnspp/bonjourpp/bonjourerror.hpp>
#include <ddnspp/common/cbuffer.hpp>
#include <ddnspp/common/kvmap.hpp>

#include <string>
#include <list>
#include <iostream>

#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>
#include <boost/shared_ptr.hpp>

namespace air
{
namespace bonjour
{

class TxtRecordEncoder:boost::noncopyable
{
private:
    const DNSDApi &dnsDll;
    air::common::auto_buff_c<char>	buffer;
    TXTRecordRef	recordHolder;
public:

    /// not counting the terminating null
    static	uint16_t	MaxCharacterOfKeyName()
    {
        return 9;
    }
    TxtRecordEncoder(const DNSDApi &dll,boost::uint16_t	maxBytes)
        :dnsDll(dll),buffer(maxBytes)
    {
        TXTRecordRef	txt_record= {0};
        dnsDll.getFunctiontable().funcTXTRecordCreate(&recordHolder,(boost::uint16_t)buffer.bytes(),buffer.ptr());
    }
    ~TxtRecordEncoder()
    {
        dnsDll.getFunctiontable().funcTXTRecordDeallocate(&recordHolder);
    }

    /* putOrReplace()
     *
     * Adds a key (optionally with value) to a TXTRecordRef. If the "key" already
     * exists in the TXTRecordRef, then the current value will be replaced with
     * the new value.
     * Keys may exist in four states with respect to a given TXT record:
     *  - Absent (key does not appear at all)
     *  - Present with no value ("key" appears alone)
     *  - Present with empty value ("key=" appears in TXT record)
     *  - Present with non-empty value ("key=value" appears in TXT record)
     * For more details refer to "Data Syntax for DNS-SD TXT Records" in
     * <http://files.dns-sd.org/draft-cheshire-dnsext-dns-sd.txt>
     *
     *
     * key:             A null-terminated string which only contains printable ASCII
     *                  values (0x20-0x7E), excluding '=' (0x3D). Keys should be
     *                  9 characters or fewer (not counting the terminating null).
     *
     * valueBytes:      The size of the value.
     *
     * value:           Any binary value. For values that represent
     *                  textual data, UTF-8 is STRONGLY recommended.
     *                  For values that represent textual data, valueSize
     *                  should NOT include the terminating null (if any)
     *                  at the end of the string.
     *                  If NULL, then "key" will be added with no value.
     *                  If non-NULL but valueSize is zero, then "key=" will be
     *                  added with empty value.
     *
     * return value:    Returns kDNSServiceErr_NoError on success.
     *                  Returns kDNSServiceErr_Invalid if the "key" string contains
     *                  illegal characters.
     *                  Returns kDNSServiceErr_NoMemory if adding this key would
     *                  exceed the available storage.
     */
    BonjourError	putOrReplace(
        const std::string&       key,
        boost::uint8_t   valueBytes,       /* may be zero */
        const void       *value            /* may be NULL */
    )
    {
        DNSServiceErrorType er=0;
        if(key.length() > MaxCharacterOfKeyName()) {
            std::string noMore9=key.substr(0,MaxCharacterOfKeyName());
            er=dnsDll.getFunctiontable().funcTXTRecordSetValue(&recordHolder,noMore9.c_str(),valueBytes,value);
        } else {
            er= dnsDll.getFunctiontable().funcTXTRecordSetValue(&recordHolder,key.c_str(),valueBytes,value);
        }
        return BonjourError(er);
    }


    BonjourError	putOrReplace(const air::common::StringMap& strMap)
    {
        BonjourError err;

        air::common::StringMap::const_iterator it=strMap.begin();
        air::common::StringMap::const_iterator over=strMap.end();
        while (it != over && !err) {
            err= putOrReplace(it->first,it->second.length(),it->second.c_str());
            if(err) {
                break;
            }
            ++it;
        }
        return err;
    }
    void			remove(const std::string&       key)
    {
        dnsDll.getFunctiontable().funcTXTRecordRemoveValue(&recordHolder,key.c_str());
    }

    const char*		getRecordPtr()const
    {
        return (const char*)dnsDll.getFunctiontable().funcTXTRecordGetBytesPtr(&recordHolder);
    }
    std::string		getRecordAsString()const
    {
        return std::string(getRecordPtr(),getRecordLenth());
    }
    boost::uint16_t	getRecordLenth()const
    {
        return dnsDll.getFunctiontable().funcTXTRecordGetLength(&recordHolder);
    }



};


class ConstBuffer
{
private:
    const void* ptr;
    boost::uint16_t length;
public:
    ConstBuffer(const void* data,boost::uint16_t len)
        :ptr(data),length(len)
    {
        //
    }
    boost::uint16_t	getLength()const
    {
        return length;
    }
    const void*		getPtr()const
    {
        return ptr;
    }
    const unsigned char*		getUcharPtr()const
    {
        return (const unsigned char*)ptr;
    }
    bool			empty()const
    {
        return (length == 0 || ptr == NULL);
    }
    // offset start with 0
    std::string		asString(boost::uint16_t offset,boost::uint16_t len)const
    {
        BOOST_ASSERT( len + offset <= length);
        return std::string((const char*)ptr + offset,len);
    }
    std::string		allAsString()const
    {
        return asString(0,length);
    }
};

typedef	std::pair<std::string,ConstBuffer>		ConstRecord;
typedef	std::list<ConstRecord>		ConstRecordList;
typedef	std::list<ConstBuffer>		ConstBufferList;

class TxtRecordDecoder:boost::noncopyable
{
public:
    enum	VALUE_FILED_TYPE {
        /// keys with no value
        VALUE_NULL,
        /// keys with empty value
        VALUE_EMPTY,
        /// keys with non-empty value
        VALUE_NORMAL
    };
private:
    const DNSDApi &dnsDll;
    air::common::auto_buff_c<char>	buffer;
public:
    static TxtRecordDecoder::VALUE_FILED_TYPE	getValueType(const ConstBuffer& val)
    {
        if(val.getPtr() != NULL && val.getLength() != 0 ) {
            return VALUE_NORMAL;
        } else if(val.getPtr() == NULL && val.getLength() == 0) {
            return VALUE_NULL;
        } else if(val.getPtr() != NULL && val.getLength() == 0) {
            return VALUE_EMPTY;
        } else {
            // val.getPtr() == NULL && val.getLength() != 0
            // it bug!
            return VALUE_NULL;
        }
    }

    TxtRecordDecoder(const DNSDApi &dll,const void* data,boost::uint16_t len)
        :dnsDll(dll),buffer(len)
    {
        if(!buffer.is_null()) {
            ::memcpy(buffer.ptr(),data,len);
        }
    }
    boost::uint16_t		count()const
    {
        return dnsDll.getFunctiontable().funcTXTRecordGetCount( (boost::uint16_t)buffer.elem_count(),buffer.ptr());
    }

    ConstBuffer			getValue(const std::string& key)const
    {
        const unsigned char* found=NULL;
        boost::uint8_t	bytes=0;

        /*  return value:    Returns NULL if the key does not exist in this TXT record,
        *                  or exists with no value (to differentiate between
        *                  these two cases use TXTRecordContainsKey()).
        *                  Returns pointer to location within TXT Record bytes
        *                  if the key exists with empty or non-empty value.
        *                  For empty value, valueLen will be zero.
        *                  For non-empty value, valueLen will be length of value data.
        */
        found=(const unsigned char*)dnsDll.getFunctiontable().funcTXTRecordGetValuePtr(
                  (boost::uint16_t)buffer.elem_count(),
                  buffer.ptr(),
                  key.c_str(),
                  &bytes);
        if(found != NULL ) {
            BOOST_ASSERT( (found >= buffer.begin()) && (found <buffer.end() ) );
            return ConstBuffer(found,bytes);
        } else {
            return ConstBuffer(NULL,0);
        }
    }
    bool				exists(const std::string& key)const
    {
        return (0 != dnsDll.getFunctiontable().funcTXTRecordContainsKey((boost::uint16_t)buffer.elem_count(),buffer.ptr(),key.c_str()) );
    }

    /// get all record
    /// key is stored as a memory copy
    /// key is stored as a memory pointer within internal buff(TxtRecordDecoder.buffer)
    BonjourError		getAll(ConstRecordList& list)const
    {
        static const boost::uint16_t	key_bytes=512;
        DNSServiceErrorType ec=0;
        boost::uint16_t count_all=count();
        // from zero to Count()-1.
        for (boost::uint16_t i=0; i<count_all; ++i) {
            char key_buff[key_bytes]= {0};
            const  void* val_ptr=NULL;
            boost::uint8_t	val_bytes=0;
            ec= dnsDll.getFunctiontable().funcTXTRecordGetItemAtIndex(
                    (boost::uint16_t)buffer.elem_count(),
                    buffer.ptr(),
                    i,
                    key_bytes,key_buff,
                    &val_bytes,&val_ptr);
            if(ec != 0) {
                break;
            } else {
                list.push_back(ConstRecord(std::string(key_buff),ConstBuffer(val_ptr,val_bytes)));
            }
        }
        return BonjourError(ec);
    }

    void				praseThenPrint(std::ostream& os)
    {
        ConstRecordList all;
        getAll(all);
        print(all,os);
    }
    void				print(const ConstRecordList& list,std::ostream& os)
    {
        ConstRecordList::const_iterator it=list.begin();
        ConstRecordList::const_iterator over=list.end();
        while(it != over) {
            os<< it->first<<std::endl;
            TxtRecordDecoder::VALUE_FILED_TYPE ft=	TxtRecordDecoder::getValueType(it->second);
            std::string valueStr;
            if(VALUE_NORMAL == ft) {
                valueStr="NULL";
            } else if(VALUE_EMPTY == ft) {
                valueStr="EMPTY";
            } else {
                valueStr=it->second.asString(0, it->second.getLength()-1);
            }
            os<< it->first<<std::endl;
            os<< valueStr<<std::endl;
            ++it;
        }
    }
};

typedef boost::shared_ptr<TxtRecordDecoder>		TxtRecordDecoderPtr;


}
}
#endif // TXT_RECORDS_H__
