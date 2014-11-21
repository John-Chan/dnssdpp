#ifndef BONJOURFUNC_H
#define BONJOURFUNC_H

#include <ddnspp/bonjourpp/apple_dnsd_incl.h>

/*********************************************************************************************
 *
 *  Service Registration
 *
 *********************************************************************************************/



/* DNSServiceRegister() Parameters:
 *
 * sdRef:           A pointer to an uninitialized DNSServiceRef. If the call succeeds
 *                  then it initializes the DNSServiceRef, returns kDNSServiceErr_NoError,
 *                  and the registration will remain active indefinitely until the client
 *                  terminates it by passing this DNSServiceRef to DNSServiceRefDeallocate().
 *
 * interfaceIndex:  If non-zero, specifies the interface on which to register the service
 *                  (the index for a given interface is determined via the if_nametoindex()
 *                  family of calls.) Most applications will pass 0 to register on all
 *                  available interfaces. See "Constants for specifying an interface index" for more details.
 *
 * flags:           Indicates the renaming behavior on name conflict (most applications
 *                  will pass 0). See flag definitions above for details.
 *
 * name:            If non-NULL, specifies the service name to be registered.
 *                  Most applications will not specify a name, in which case the computer
 *                  name is used (this name is communicated to the client via the callback).
 *                  If a name is specified, it must be 1-63 bytes of UTF-8 text.
 *                  If the name is longer than 63 bytes it will be automatically truncated
 *                  to a legal length, unless the NoAutoRename flag is set,
 *                  in which case kDNSServiceErr_BadParam will be returned.
 *
 * regtype:         The service type followed by the protocol, separated by a dot
 *                  (e.g. "_ftp._tcp"). The service type must be an underscore, followed
 *                  by 1-14 characters, which may be letters, digits, or hyphens.
 *                  The transport protocol must be "_tcp" or "_udp". New service types
 *                  should be registered at <http://www.dns-sd.org/ServiceTypes.html>.
 *
 *                  Additional subtypes of the primary service type (where a service
 *                  type has defined subtypes) follow the primary service type in a
 *                  comma-separated list, with no additional spaces, e.g.
 *                      "_primarytype._tcp,_subtype1,_subtype2,_subtype3"
 *                  Subtypes provide a mechanism for filtered browsing: A client browsing
 *                  for "_primarytype._tcp" will discover all instances of this type;
 *                  a client browsing for "_primarytype._tcp,_subtype2" will discover only
 *                  those instances that were registered with "_subtype2" in their list of
 *                  registered subtypes.
 *
 *                  The subtype mechanism can be illustrated with some examples using the
 *                  dns-sd command-line tool:
 *
 *                  % dns-sd -R Simple _test._tcp "" 1001 &
 *                  % dns-sd -R Better _test._tcp,HasFeatureA "" 1002 &
 *                  % dns-sd -R Best   _test._tcp,HasFeatureA,HasFeatureB "" 1003 &
 *
 *                  Now:
 *                  % dns-sd -B _test._tcp             # will find all three services
 *                  % dns-sd -B _test._tcp,HasFeatureA # finds "Better" and "Best"
 *                  % dns-sd -B _test._tcp,HasFeatureB # finds only "Best"
 *
 * domain:          If non-NULL, specifies the domain on which to advertise the service.
 *                  Most applications will not specify a domain, instead automatically
 *                  registering in the default domain(s).
 *
 * host:            If non-NULL, specifies the SRV target host name. Most applications
 *                  will not specify a host, instead automatically using the machine's
 *                  default host name(s). Note that specifying a non-NULL host does NOT
 *                  create an address record for that host - the application is responsible
 *                  for ensuring that the appropriate address record exists, or creating it
 *                  via DNSServiceRegisterRecord().
 *
 * port:            The port, in network byte order, on which the service accepts connections.
 *                  Pass 0 for a "placeholder" service (i.e. a service that will not be discovered
 *                  by browsing, but will cause a name conflict if another client tries to
 *                  register that same name). Most clients will not use placeholder services.
 *
 * txtLen:          The length of the txtRecord, in bytes. Must be zero if the txtRecord is NULL.
 *
 * txtRecord:       The TXT record rdata. A non-NULL txtRecord MUST be a properly formatted DNS
 *                  TXT record, i.e. <length byte> <data> <length byte> <data> ...
 *                  Passing NULL for the txtRecord is allowed as a synonym for txtLen=1, txtRecord="",
 *                  i.e. it creates a TXT record of length one containing a single empty string.
 *                  RFC 1035 doesn't allow a TXT record to contain *zero* strings, so a single empty
 *                  string is the smallest legal DNS TXT record.
 *                  As with the other parameters, the DNSServiceRegister call copies the txtRecord
 *                  data; e.g. if you allocated the storage for the txtRecord parameter with malloc()
 *                  then you can safely free that memory right after the DNSServiceRegister call returns.
 *
 * callBack:        The function to be called when the registration completes or asynchronously
 *                  fails. The client MAY pass NULL for the callback -  The client will NOT be notified
 *                  of the default values picked on its behalf, and the client will NOT be notified of any
 *                  asynchronous errors (e.g. out of memory errors, etc.) that may prevent the registration
 *                  of the service. The client may NOT pass the NoAutoRename flag if the callback is NULL.
 *                  The client may still deregister the service at any time via DNSServiceRefDeallocate().
 *
 * context:         An application context pointer which is passed to the callback function
 *                  (may be NULL).
 *
 * return value:    Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous
 *                  errors are delivered to the callback), otherwise returns an error code indicating
 *                  the error that occurred (the callback is never invoked and the DNSServiceRef
 *                  is not initialized).
 */
typedef DNSServiceErrorType (DNSSD_API *FuncDNSServiceRegister)(
        DNSServiceRef                       *sdRef,
        DNSServiceFlags                     flags,
        uint32_t                            interfaceIndex,
        const char                          *name,         /* may be NULL */
        const char                          *regtype,
        const char                          *domain,       /* may be NULL */
        const char                          *host,         /* may be NULL */
        uint16_t                            port,
        uint16_t                            txtLen,
        const void                          *txtRecord,    /* may be NULL */
        DNSServiceRegisterReply             callBack,      /* may be NULL */
        void                                *context       /* may be NULL */
        );



/*********************************************************************************************
 *
 *  Service Discovery
 *
 *********************************************************************************************/


/* DNSServiceBrowse() Parameters:
 *
 * sdRef:           A pointer to an uninitialized DNSServiceRef. If the call succeeds
 *                  then it initializes the DNSServiceRef, returns kDNSServiceErr_NoError,
 *                  and the browse operation will run indefinitely until the client
 *                  terminates it by passing this DNSServiceRef to DNSServiceRefDeallocate().
 *
 * flags:           Currently ignored, reserved for future use.
 *
 * interfaceIndex:  If non-zero, specifies the interface on which to browse for services
 *                  (the index for a given interface is determined via the if_nametoindex()
 *                  family of calls.) Most applications will pass 0 to browse on all available
 *                  interfaces. See "Constants for specifying an interface index" for more details.
 *
 * regtype:         The service type being browsed for followed by the protocol, separated by a
 *                  dot (e.g. "_ftp._tcp"). The transport protocol must be "_tcp" or "_udp".
 *                  A client may optionally specify a single subtype to perform filtered browsing:
 *                  e.g. browsing for "_primarytype._tcp,_subtype" will discover only those
 *                  instances of "_primarytype._tcp" that were registered specifying "_subtype"
 *                  in their list of registered subtypes.
 *
 * domain:          If non-NULL, specifies the domain on which to browse for services.
 *                  Most applications will not specify a domain, instead browsing on the
 *                  default domain(s).
 *
 * callBack:        The function to be called when an instance of the service being browsed for
 *                  is found, or if the call asynchronously fails.
 *
 * context:         An application context pointer which is passed to the callback function
 *                  (may be NULL).
 *
 * return value:    Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous
 *                  errors are delivered to the callback), otherwise returns an error code indicating
 *                  the error that occurred (the callback is not invoked and the DNSServiceRef
 *                  is not initialized).
 */

typedef DNSServiceErrorType (DNSSD_API *FuncDNSServiceBrowse)
(
 DNSServiceRef                       *sdRef,
 DNSServiceFlags                     flags,
 uint32_t                            interfaceIndex,
 const char                          *regtype,
 const char                          *domain,    /* may be NULL */
 DNSServiceBrowseReply               callBack,
 void                                *context    /* may be NULL */
 );



/* DNSServiceResolve() Parameters
 *
 * sdRef:           A pointer to an uninitialized DNSServiceRef. If the call succeeds
 *                  then it initializes the DNSServiceRef, returns kDNSServiceErr_NoError,
 *                  and the resolve operation will run indefinitely until the client
 *                  terminates it by passing this DNSServiceRef to DNSServiceRefDeallocate().
 *
 * flags:           Specifying kDNSServiceFlagsForceMulticast will cause query to be
 *                  performed with a link-local mDNS query, even if the name is an
 *                  apparently non-local name (i.e. a name not ending in ".local.")
 *
 * interfaceIndex:  The interface on which to resolve the service. If this resolve call is
 *                  as a result of a currently active DNSServiceBrowse() operation, then the
 *                  interfaceIndex should be the index reported in the DNSServiceBrowseReply
 *                  callback. If this resolve call is using information previously saved
 *                  (e.g. in a preference file) for later use, then use interfaceIndex 0, because
 *                  the desired service may now be reachable via a different physical interface.
 *                  See "Constants for specifying an interface index" for more details.
 *
 * name:            The name of the service instance to be resolved, as reported to the
 *                  DNSServiceBrowseReply() callback.
 *
 * regtype:         The type of the service instance to be resolved, as reported to the
 *                  DNSServiceBrowseReply() callback.
 *
 * domain:          The domain of the service instance to be resolved, as reported to the
 *                  DNSServiceBrowseReply() callback.
 *
 * callBack:        The function to be called when a result is found, or if the call
 *                  asynchronously fails.
 *
 * context:         An application context pointer which is passed to the callback function
 *                  (may be NULL).
 *
 * return value:    Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous
 *                  errors are delivered to the callback), otherwise returns an error code indicating
 *                  the error that occurred (the callback is never invoked and the DNSServiceRef
 *                  is not initialized).
 */

typedef DNSServiceErrorType (DNSSD_API *FuncDNSServiceResolve)
(
 DNSServiceRef                       *sdRef,
 DNSServiceFlags                     flags,
 uint32_t                            interfaceIndex,
 const char                          *name,
 const char                          *regtype,
 const char                          *domain,
 DNSServiceResolveReply              callBack,
 void                                *context  /* may be NULL */
 );


/*********************************************************************************************
 *
 * Unix Domain Socket access, DNSServiceRef deallocation, and data processing functions
 *
 *********************************************************************************************/

/* DNSServiceRefSockFD()
 *
 * Access underlying Unix domain socket for an initialized DNSServiceRef.
 * The DNS Service Discovery implementation uses this socket to communicate between the client and
 * the mDNSResponder daemon. The application MUST NOT directly read from or write to this socket.
 * Access to the socket is provided so that it can be used as a kqueue event source, a CFRunLoop
 * event source, in a select() loop, etc. When the underlying event management subsystem (kqueue/
 * select/CFRunLoop etc.) indicates to the client that data is available for reading on the
 * socket, the client should call DNSServiceProcessResult(), which will extract the daemon's
 * reply from the socket, and pass it to the appropriate application callback. By using a run
 * loop or select(), results from the daemon can be processed asynchronously. Alternatively,
 * a client can choose to fork a thread and have it loop calling "DNSServiceProcessResult(ref);"
 * If DNSServiceProcessResult() is called when no data is available for reading on the socket, it
 * will block until data does become available, and then process the data and return to the caller.
 * When data arrives on the socket, the client is responsible for calling DNSServiceProcessResult(ref)
 * in a timely fashion -- if the client allows a large backlog of data to build up the daemon
 * may terminate the connection.
 *
 * sdRef:           A DNSServiceRef initialized by any of the DNSService calls.
 *
 * return value:    The DNSServiceRef's underlying socket descriptor, or -1 on
 *                  error.
 */
typedef int (DNSSD_API *FuncDNSServiceRefSockFD)(DNSServiceRef sdRef);



/* DNSServiceProcessResult()
 *
 * Read a reply from the daemon, calling the appropriate application callback. This call will
 * block until the daemon's response is received. Use DNSServiceRefSockFD() in
 * conjunction with a run loop or select() to determine the presence of a response from the
 * server before calling this function to process the reply without blocking. Call this function
 * at any point if it is acceptable to block until the daemon's response arrives. Note that the
 * client is responsible for ensuring that DNSServiceProcessResult() is called whenever there is
 * a reply from the daemon - the daemon may terminate its connection with a client that does not
 * process the daemon's responses.
 *
 * sdRef:           A DNSServiceRef initialized by any of the DNSService calls
 *                  that take a callback parameter.
 *
 * return value:    Returns kDNSServiceErr_NoError on success, otherwise returns
 *                  an error code indicating the specific failure that occurred.
 */

typedef DNSServiceErrorType (DNSSD_API *FuncDNSServiceProcessResult)(DNSServiceRef sdRef);




/* DNSServiceRefDeallocate()
 *
 * Terminate a connection with the daemon and free memory associated with the DNSServiceRef.
 * Any services or records registered with this DNSServiceRef will be deregistered. Any
 * Browse, Resolve, or Query operations called with this reference will be terminated.
 *
 * Note: If the reference's underlying socket is used in a run loop or select() call, it should
 * be removed BEFORE DNSServiceRefDeallocate() is called, as this function closes the reference's
 * socket.
 *
 * Note: If the reference was initialized with DNSServiceCreateConnection(), any DNSRecordRefs
 * created via this reference will be invalidated by this call - the resource records are
 * deregistered, and their DNSRecordRefs may not be used in subsequent functions. Similarly,
 * if the reference was initialized with DNSServiceRegister, and an extra resource record was
 * added to the service via DNSServiceAddRecord(), the DNSRecordRef created by the Add() call
 * is invalidated when this function is called - the DNSRecordRef may not be used in subsequent
 * functions.
 *
 * Note: This call is to be used only with the DNSServiceRef defined by this API. It is
 * not compatible with dns_service_discovery_ref objects defined in the legacy Mach-based
 * DNSServiceDiscovery.h API.
 *
 * sdRef:           A DNSServiceRef initialized by any of the DNSService calls.
 *
 */

typedef void (DNSSD_API *FuncDNSServiceRefDeallocate)(DNSServiceRef sdRef);


/*********************************************************************************************
 *
 *   TXT Record Construction Functions
 *
 *********************************************************************************************/

/*
 * A typical calling sequence for TXT record construction is something like:
 *
 * Client allocates storage for TXTRecord data (e.g. declare buffer on the stack)
 * TXTRecordCreate();
 * TXTRecordSetValue();
 * TXTRecordSetValue();
 * TXTRecordSetValue();
 * ...
 * DNSServiceRegister( ... TXTRecordGetLength(), TXTRecordGetBytesPtr() ... );
 * TXTRecordDeallocate();
 * Explicitly deallocate storage for TXTRecord data (if not allocated on the stack)
 */


/* TXTRecordRef
 *
 * Opaque internal data type.
 * Note: Represents a DNS-SD TXT record.
 */

//typedef union _TXTRecordRef_t { char PrivateData[16]; char *ForceNaturalAlignment; } TXTRecordRef;


/* TXTRecordCreate()
 *
 * Creates a new empty TXTRecordRef referencing the specified storage.
 *
 * If the buffer parameter is NULL, or the specified storage size is not
 * large enough to hold a key subsequently added using TXTRecordSetValue(),
 * then additional memory will be added as needed using malloc().
 *
 * On some platforms, when memory is low, malloc() may fail. In this
 * case, TXTRecordSetValue() will return kDNSServiceErr_NoMemory, and this
 * error condition will need to be handled as appropriate by the caller.
 *
 * You can avoid the need to handle this error condition if you ensure
 * that the storage you initially provide is large enough to hold all
 * the key/value pairs that are to be added to the record.
 * The caller can precompute the exact length required for all of the
 * key/value pairs to be added, or simply provide a fixed-sized buffer
 * known in advance to be large enough.
 * A no-value (key-only) key requires  (1 + key length) bytes.
 * A key with empty value requires     (1 + key length + 1) bytes.
 * A key with non-empty value requires (1 + key length + 1 + value length).
 * For most applications, DNS-SD TXT records are generally
 * less than 100 bytes, so in most cases a simple fixed-sized
 * 256-byte buffer will be more than sufficient.
 * Recommended size limits for DNS-SD TXT Records are discussed in
 * <http://files.dns-sd.org/draft-cheshire-dnsext-dns-sd.txt>
 *
 * Note: When passing parameters to and from these TXT record APIs,
 * the key name does not include the '=' character. The '=' character
 * is the separator between the key and value in the on-the-wire
 * packet format; it is not part of either the key or the value.
 *
 * txtRecord:       A pointer to an uninitialized TXTRecordRef.
 *
 * bufferLen:       The size of the storage provided in the "buffer" parameter.
 *
 * buffer:          Optional caller-supplied storage used to hold the TXTRecord data.
 *                  This storage must remain valid for as long as
 *                  the TXTRecordRef.
 */

typedef void (DNSSD_API *FuncTXTRecordCreate)(
	TXTRecordRef     *txtRecord,
	uint16_t         bufferLen,
	void             *buffer
	);



/* TXTRecordDeallocate()
 *
 * Releases any resources allocated in the course of preparing a TXT Record
 * using TXTRecordCreate()/TXTRecordSetValue()/TXTRecordRemoveValue().
 * Ownership of the buffer provided in TXTRecordCreate() returns to the client.
 *
 * txtRecord:           A TXTRecordRef initialized by calling TXTRecordCreate().
 *
 */
typedef void (DNSSD_API *FuncTXTRecordDeallocate)(TXTRecordRef     *txtRecord);



/* TXTRecordSetValue()
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
 * txtRecord:       A TXTRecordRef initialized by calling TXTRecordCreate().
 *
 * key:             A null-terminated string which only contains printable ASCII
 *                  values (0x20-0x7E), excluding '=' (0x3D). Keys should be
 *                  9 characters or fewer (not counting the terminating null).
 *
 * valueSize:       The size of the value.
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

typedef DNSServiceErrorType (DNSSD_API *FuncTXTRecordSetValue)(
	TXTRecordRef     *txtRecord,
	const char       *key,
	uint8_t          valueSize,        /* may be zero */
	const void       *value            /* may be NULL */
	);



/* TXTRecordRemoveValue()
 *
 * Removes a key from a TXTRecordRef. The "key" must be an
 * ASCII string which exists in the TXTRecordRef.
 *
 * txtRecord:       A TXTRecordRef initialized by calling TXTRecordCreate().
 *
 * key:             A key name which exists in the TXTRecordRef.
 *
 * return value:    Returns kDNSServiceErr_NoError on success.
 *                  Returns kDNSServiceErr_NoSuchKey if the "key" does not
 *                  exist in the TXTRecordRef.
 */
typedef DNSServiceErrorType (DNSSD_API *FuncTXTRecordRemoveValue)(
	TXTRecordRef     *txtRecord,
	const char       *key
	);



/* TXTRecordGetLength()
 *
 * Allows you to determine the length of the raw bytes within a TXTRecordRef.
 *
 * txtRecord:       A TXTRecordRef initialized by calling TXTRecordCreate().
 *
 * return value:    Returns the size of the raw bytes inside a TXTRecordRef
 *                  which you can pass directly to DNSServiceRegister() or
 *                  to DNSServiceUpdateRecord().
 *                  Returns 0 if the TXTRecordRef is empty.
 */
typedef uint16_t (DNSSD_API *FuncTXTRecordGetLength)(
	const TXTRecordRef *txtRecord
	);



/* TXTRecordGetBytesPtr()
 *
 * Allows you to retrieve a pointer to the raw bytes within a TXTRecordRef.
 *
 * txtRecord:       A TXTRecordRef initialized by calling TXTRecordCreate().
 *
 * return value:    Returns a pointer to the raw bytes inside the TXTRecordRef
 *                  which you can pass directly to DNSServiceRegister() or
 *                  to DNSServiceUpdateRecord().
 */
typedef void* (DNSSD_API *FuncTXTRecordGetBytesPtr)(
	const TXTRecordRef *txtRecord
	);

/*********************************************************************************************
 *
 *   TXT Record Parsing Functions
 *
 *********************************************************************************************/

/*
 * A typical calling sequence for TXT record parsing is something like:
 *
 * Receive TXT record data in DNSServiceResolve() callback
 * if (TXTRecordContainsKey(txtLen, txtRecord, "key")) then do something
 * val1ptr = TXTRecordGetValuePtr(txtLen, txtRecord, "key1", &len1);
 * val2ptr = TXTRecordGetValuePtr(txtLen, txtRecord, "key2", &len2);
 * ...
 * memcpy(myval1, val1ptr, len1);
 * memcpy(myval2, val2ptr, len2);
 * ...
 * return;
 *
 * If you wish to retain the values after return from the DNSServiceResolve()
 * callback, then you need to copy the data to your own storage using memcpy()
 * or similar, as shown in the example above.
 *
 * If for some reason you need to parse a TXT record you built yourself
 * using the TXT record construction functions above, then you can do
 * that using TXTRecordGetLength and TXTRecordGetBytesPtr calls:
 * TXTRecordGetValue(TXTRecordGetLength(x), TXTRecordGetBytesPtr(x), key, &len);
 *
 * Most applications only fetch keys they know about from a TXT record and
 * ignore the rest.
 * However, some debugging tools wish to fetch and display all keys.
 * To do that, use the TXTRecordGetCount() and TXTRecordGetItemAtIndex() calls.
 */

/* TXTRecordContainsKey()
 *
 * Allows you to determine if a given TXT Record contains a specified key.
 *
 * txtLen:          The size of the received TXT Record.
 *
 * txtRecord:       Pointer to the received TXT Record bytes.
 *
 * key:             A null-terminated ASCII string containing the key name.
 *
 * return value:    Returns 1 if the TXT Record contains the specified key.
 *                  Otherwise, it returns 0.
 */
typedef int (DNSSD_API *FuncTXTRecordContainsKey)(
									 uint16_t         txtLen,
									 const void       *txtRecord,
									 const char       *key);

/* TXTRecordGetValuePtr()
 *
 * Allows you to retrieve the value for a given key from a TXT Record.
 *
 * txtLen:          The size of the received TXT Record
 *
 * txtRecord:       Pointer to the received TXT Record bytes.
 *
 * key:             A null-terminated ASCII string containing the key name.
 *
 * valueLen:        On output, will be set to the size of the "value" data.
 *
 * return value:    Returns NULL if the key does not exist in this TXT record,
 *                  or exists with no value (to differentiate between
 *                  these two cases use TXTRecordContainsKey()).
 *                  Returns pointer to location within TXT Record bytes
 *                  if the key exists with empty or non-empty value.
 *                  For empty value, valueLen will be zero.
 *                  For non-empty value, valueLen will be length of value data.
 */

typedef const void* (DNSSD_API *FuncTXTRecordGetValuePtr)(
	uint16_t         txtLen,
	const void       *txtRecord,
	const char       *key,
	uint8_t          *valueLen );



/* TXTRecordGetCount()
 *
 * Returns the number of keys stored in the TXT Record. The count
 * can be used with TXTRecordGetItemAtIndex() to iterate through the keys.
 *
 * txtLen:          The size of the received TXT Record.
 *
 * txtRecord:       Pointer to the received TXT Record bytes.
 *
 * return value:    Returns the total number of keys in the TXT Record.
 *
 */

typedef uint16_t (DNSSD_API *FuncTXTRecordGetCount)(
	uint16_t         txtLen,
	const void       *txtRecord);



/* TXTRecordGetItemAtIndex()
 *
 * Allows you to retrieve a key name and value pointer, given an index into
 * a TXT Record. Legal index values range from zero to TXTRecordGetCount()-1.
 * It's also possible to iterate through keys in a TXT record by simply
 * calling TXTRecordGetItemAtIndex() repeatedly, beginning with index zero
 * and increasing until TXTRecordGetItemAtIndex() returns kDNSServiceErr_Invalid.
 *
 * On return:
 * For keys with no value, *value is set to NULL and *valueLen is zero.
 * For keys with empty value, *value is non-NULL and *valueLen is zero.
 * For keys with non-empty value, *value is non-NULL and *valueLen is non-zero.
 *
 * txtLen:          The size of the received TXT Record.
 *
 * txtRecord:       Pointer to the received TXT Record bytes.
 *
 * itemIndex:       An index into the TXT Record.
 *
 * keyBufLen:       The size of the string buffer being supplied.
 *
 * key:             A string buffer used to store the key name.
 *                  On return, the buffer contains a null-terminated C string
 *                  giving the key name. DNS-SD TXT keys are usually
 *                  9 characters or fewer. To hold the maximum possible
 *                  key name, the buffer should be 256 bytes long.
 *
 * valueLen:        On output, will be set to the size of the "value" data.
 *
 * value:           On output, *value is set to point to location within TXT
 *                  Record bytes that holds the value data.
 *
 * return value:    Returns kDNSServiceErr_NoError on success.
 *                  Returns kDNSServiceErr_NoMemory if keyBufLen is too short.
 *                  Returns kDNSServiceErr_Invalid if index is greater than
 *                  TXTRecordGetCount()-1.
 */

typedef DNSServiceErrorType (DNSSD_API *FuncTXTRecordGetItemAtIndex)(
	uint16_t         txtLen,
	const void       *txtRecord,
	uint16_t         itemIndex,
	uint16_t         keyBufLen,
	char             *key,
	uint8_t          *valueLen,
	const void       **value);


/*********************************************************************************************
 *
 *  Unified lookup of both IPv4 and IPv6 addresses for a fully qualified hostname
 *
 *********************************************************************************************/


/* DNSServiceGetAddrInfo() Parameters:
 *
 * sdRef:           A pointer to an uninitialized DNSServiceRef. If the call succeeds then it
 *                  initializes the DNSServiceRef, returns kDNSServiceErr_NoError, and the query
 *                  begins and will last indefinitely until the client terminates the query
 *                  by passing this DNSServiceRef to DNSServiceRefDeallocate().
 *
 * flags:           kDNSServiceFlagsForceMulticast or kDNSServiceFlagsLongLivedQuery.
 *                  Pass kDNSServiceFlagsLongLivedQuery to create a "long-lived" unicast
 *                  query in a non-local domain. Without setting this flag, unicast queries
 *                  will be one-shot - that is, only answers available at the time of the call
 *                  will be returned. By setting this flag, answers (including Add and Remove
 *                  events) that become available after the initial call is made will generate
 *                  callbacks. This flag has no effect on link-local multicast queries.
 *
 * interfaceIndex:  The interface on which to issue the query.  Passing 0 causes the query to be
 *                  sent on all active interfaces via Multicast or the primary interface via Unicast.
 *
 * protocol:        Pass in kDNSServiceProtocol_IPv4 to look up IPv4 addresses, or kDNSServiceProtocol_IPv6
 *                  to look up IPv6 addresses, or both to look up both kinds. If neither flag is
 *                  set, the system will apply an intelligent heuristic, which is (currently)
 *                  that it will attempt to look up both, except:
 *
 *                   * If "hostname" is a wide-area unicast DNS hostname (i.e. not a ".local." name)
 *                     but this host has no routable IPv6 address, then the call will not try to
 *                     look up IPv6 addresses for "hostname", since any addresses it found would be
 *                     unlikely to be of any use anyway. Similarly, if this host has no routable
 *                     IPv4 address, the call will not try to look up IPv4 addresses for "hostname".
 *
 *                   * If "hostname" is a link-local multicast DNS hostname (i.e. a ".local." name)
 *                     but this host has no IPv6 address of any kind, then it will not try to look
 *                     up IPv6 addresses for "hostname". Similarly, if this host has no IPv4 address
 *                     of any kind, the call will not try to look up IPv4 addresses for "hostname".
 *
 * hostname:        The fully qualified domain name of the host to be queried for.
 *
 * callBack:        The function to be called when the query succeeds or fails asynchronously.
 *
 * context:         An application context pointer which is passed to the callback function
 *                  (may be NULL).
 *
 * return value:    Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous
 *                  errors are delivered to the callback), otherwise returns an error code indicating
 *                  the error that occurred.
 */
typedef DNSServiceErrorType (DNSSD_API *FuncDNSServiceGetAddrInfo)
	(
	DNSServiceRef                    *sdRef,
	DNSServiceFlags                  flags,
	uint32_t                         interfaceIndex,
	DNSServiceProtocol               protocol,
	const char                       *hostname,
	DNSServiceGetAddrInfoReply       callBack,
	void                             *context          /* may be NULL */
	);

/*********************************************************************************************
 *
 * Domain Enumeration
 *
 *********************************************************************************************/


/* DNSServiceEnumerateDomains() Parameters:
 *
 * sdRef:           A pointer to an uninitialized DNSServiceRef. If the call succeeds
 *                  then it initializes the DNSServiceRef, returns kDNSServiceErr_NoError,
 *                  and the enumeration operation will run indefinitely until the client
 *                  terminates it by passing this DNSServiceRef to DNSServiceRefDeallocate().
 *
 * flags:           Possible values are:
 *                  kDNSServiceFlagsBrowseDomains to enumerate domains recommended for browsing.
 *                  kDNSServiceFlagsRegistrationDomains to enumerate domains recommended
 *                  for registration.
 *
 * interfaceIndex:  If non-zero, specifies the interface on which to look for domains.
 *                  (the index for a given interface is determined via the if_nametoindex()
 *                  family of calls.) Most applications will pass 0 to enumerate domains on
 *                  all interfaces. See "Constants for specifying an interface index" for more details.
 *
 * callBack:        The function to be called when a domain is found or the call asynchronously
 *                  fails.
 *
 * context:         An application context pointer which is passed to the callback function
 *                  (may be NULL).
 *
 * return value:    Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous
 *                  errors are delivered to the callback), otherwise returns an error code indicating
 *                  the error that occurred (the callback is not invoked and the DNSServiceRef
 *                  is not initialized).
 */
typedef DNSServiceErrorType (DNSSD_API *FuncDNSServiceEnumerateDomains)
(
 DNSServiceRef                       *sdRef,
 DNSServiceFlags                     flags,
 uint32_t                            interfaceIndex,
 DNSServiceDomainEnumReply           callBack,
 void                                *context  /* may be NULL */
 );




#endif // BONJOURFUNC_H
