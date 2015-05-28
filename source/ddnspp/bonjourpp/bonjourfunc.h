#ifndef BONJOURFUNC_H
#define BONJOURFUNC_H

#include <ddnspp/bonjourpp/apple_dnsd_incl.h>


/*********************************************************************************************
 *
 * Version checking
 *
 *********************************************************************************************/

/* DNSServiceGetProperty() Parameters:
 *
 * property:        The requested property.
 *                  Currently the only property defined is kDNSServiceProperty_DaemonVersion.
 *
 * result:          Place to store result.
 *                  For retrieving DaemonVersion, this should be the address of a uint32_t.
 *
 * size:            Pointer to uint32_t containing size of the result location.
 *                  For retrieving DaemonVersion, this should be sizeof(uint32_t).
 *                  On return the uint32_t is updated to the size of the data returned.
 *                  For DaemonVersion, the returned size is always sizeof(uint32_t), but
 *                  future properties could be defined which return variable-sized results.
 *
 * return value:    Returns kDNSServiceErr_NoError on success, or kDNSServiceErr_ServiceNotRunning
 *                  if the daemon (or "system service" on Windows) is not running.
 */

typedef DNSServiceErrorType (DNSSD_API *FuncDNSServiceGetProperty)(
	const char *property,  /* Requested property (i.e. kDNSServiceProperty_DaemonVersion) */
	void       *result,    /* Pointer to place to store result */
	uint32_t   *size       /* size of result location */
	);

/*********************************************************************************************
 *
 *  General Utility Functions
 *
 *********************************************************************************************/

/* DNSServiceConstructFullName()
 *
 * Concatenate a three-part domain name (as returned by the above callbacks) into a
 * properly-escaped full domain name. Note that callbacks in the above functions ALREADY ESCAPE
 * strings where necessary.
 *
 * Parameters:
 *
 * fullName:        A pointer to a buffer that where the resulting full domain name is to be written.
 *                  The buffer must be kDNSServiceMaxDomainName (1009) bytes in length to
 *                  accommodate the longest legal domain name without buffer overrun.
 *
 * service:         The service name - any dots or backslashes must NOT be escaped.
 *                  May be NULL (to construct a PTR record name, e.g.
 *                  "_ftp._tcp.apple.com.").
 *
 * regtype:         The service type followed by the protocol, separated by a dot
 *                  (e.g. "_ftp._tcp").
 *
 * domain:          The domain name, e.g. "apple.com.". Literal dots or backslashes,
 *                  if any, must be escaped, e.g. "1st\. Floor.apple.com."
 *
 * return value:    Returns kDNSServiceErr_NoError (0) on success, kDNSServiceErr_BadParam on error.
 *
 */
typedef DNSServiceErrorType (DNSSD_API *FuncDNSServiceConstructFullName)(
	char                            *fullName,
	const char                      *service,      /* may be NULL */
	const char                      *regtype,
	const char                      *domain
	);



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


/*********************************************************************************************
 *
 *  NAT Port Mapping
 *
 *********************************************************************************************/

/* DNSServiceNATPortMappingCreate
 *
 * Request a port mapping in the NAT gateway, which maps a port on the local machine
 * to an external port on the NAT.
 *
 * The port mapping will be renewed indefinitely until the client process exits, or
 * explicitly terminates the port mapping request by calling DNSServiceRefDeallocate().
 * The client callback will be invoked, informing the client of the NAT gateway's
 * external IP address and the external port that has been allocated for this client.
 * The client should then record this external IP address and port using whatever
 * directory service mechanism it is using to enable peers to connect to it.
 * (Clients advertising services using Wide-Area DNS-SD DO NOT need to use this API
 * -- when a client calls DNSServiceRegister() NAT mappings are automatically created
 * and the external IP address and port for the service are recorded in the global DNS.
 * Only clients using some directory mechanism other than Wide-Area DNS-SD need to use
 * this API to explicitly map their own ports.)
 *
 * It's possible that the client callback could be called multiple times, for example
 * if the NAT gateway's IP address changes, or if a configuration change results in a
 * different external port being mapped for this client. Over the lifetime of any long-lived
 * port mapping, the client should be prepared to handle these notifications of changes
 * in the environment, and should update its recorded address and/or port as appropriate.
 *
 * NOTE: There are two unusual aspects of how the DNSServiceNATPortMappingCreate API works,
 * which were intentionally designed to help simplify client code:
 *
 *  1. It's not an error to request a NAT mapping when the machine is not behind a NAT gateway.
 *     In other NAT mapping APIs, if you request a NAT mapping and the machine is not behind a NAT
 *     gateway, then the API returns an error code -- it can't get you a NAT mapping if there's no
 *     NAT gateway. The DNSServiceNATPortMappingCreate API takes a different view. Working out
 *     whether or not you need a NAT mapping can be tricky and non-obvious, particularly on
 *     a machine with multiple active network interfaces. Rather than make every client recreate
 *     this logic for deciding whether a NAT mapping is required, the PortMapping API does that
 *     work for you. If the client calls the PortMapping API when the machine already has a
 *     routable public IP address, then instead of complaining about it and giving an error,
 *     the PortMapping API just invokes your callback, giving the machine's public address
 *     and your own port number. This means you don't need to write code to work out whether
 *     your client needs to call the PortMapping API -- just call it anyway, and if it wasn't
 *     necessary, no harm is done:
 *
 *     - If the machine already has a routable public IP address, then your callback
 *       will just be invoked giving your own address and port.
 *     - If a NAT mapping is required and obtained, then your callback will be invoked
 *       giving you the external address and port.
 *     - If a NAT mapping is required but not obtained from the local NAT gateway,
 *       or the machine has no network connectivity, then your callback will be
 *       invoked giving zero address and port.
 *
 *  2. In other NAT mapping APIs, if a laptop computer is put to sleep and woken up on a new
 *     network, it's the client's job to notice this, and work out whether a NAT mapping
 *     is required on the new network, and make a new NAT mapping request if necessary.
 *     The DNSServiceNATPortMappingCreate API does this for you, automatically.
 *     The client just needs to make one call to the PortMapping API, and its callback will
 *     be invoked any time the mapping state changes. This property complements point (1) above.
 *     If the client didn't make a NAT mapping request just because it determined that one was
 *     not required at that particular moment in time, the client would then have to monitor
 *     for network state changes to determine if a NAT port mapping later became necessary.
 *     By unconditionally making a NAT mapping request, even when a NAT mapping not to be
 *     necessary, the PortMapping API will then begin monitoring network state changes on behalf of
 *     the client, and if a NAT mapping later becomes necessary, it will automatically create a NAT
 *     mapping and inform the client with a new callback giving the new address and port information.
 *
 * DNSServiceNATPortMappingReply() parameters:
 *
 * sdRef:           The DNSServiceRef initialized by DNSServiceNATPortMappingCreate().
 *
 * flags:           Currently unused, reserved for future use.
 *
 * interfaceIndex:  The interface through which the NAT gateway is reached.
 *
 * errorCode:       Will be kDNSServiceErr_NoError on success.
 *                  Will be kDNSServiceErr_DoubleNAT when the NAT gateway is itself behind one or
 *                  more layers of NAT, in which case the other parameters have the defined values.
 *                  For other failures, will indicate the failure that occurred, and the other
 *                  parameters are undefined.
 *
 * externalAddress: Four byte IPv4 address in network byte order.
 *
 * protocol:        Will be kDNSServiceProtocol_UDP or kDNSServiceProtocol_TCP or both.
 *
 * internalPort:    The port on the local machine that was mapped.
 *
 * externalPort:    The actual external port in the NAT gateway that was mapped.
 *                  This is likely to be different than the requested external port.
 *
 * ttl:             The lifetime of the NAT port mapping created on the gateway.
 *                  This controls how quickly stale mappings will be garbage-collected
 *                  if the client machine crashes, suffers a power failure, is disconnected
 *                  from the network, or suffers some other unfortunate demise which
 *                  causes it to vanish without explicitly removing its NAT port mapping.
 *                  It's possible that the ttl value will differ from the requested ttl value.
 *
 * context:         The context pointer that was passed to the callout.
 *
 */

/* DNSServiceNATPortMappingCreate() Parameters:
 *
 * sdRef:           A pointer to an uninitialized DNSServiceRef. If the call succeeds then it
 *                  initializes the DNSServiceRef, returns kDNSServiceErr_NoError, and the nat
 *                  port mapping will last indefinitely until the client terminates the port
 *                  mapping request by passing this DNSServiceRef to DNSServiceRefDeallocate().
 *
 * flags:           Currently ignored, reserved for future use.
 *
 * interfaceIndex:  The interface on which to create port mappings in a NAT gateway. Passing 0 causes
 *                  the port mapping request to be sent on the primary interface.
 *
 * protocol:        To request a port mapping, pass in kDNSServiceProtocol_UDP, or kDNSServiceProtocol_TCP,
 *                  or (kDNSServiceProtocol_UDP | kDNSServiceProtocol_TCP) to map both.
 *                  The local listening port number must also be specified in the internalPort parameter.
 *                  To just discover the NAT gateway's external IP address, pass zero for protocol,
 *                  internalPort, externalPort and ttl.
 *
 * internalPort:    The port number in network byte order on the local machine which is listening for packets.
 *
 * externalPort:    The requested external port in network byte order in the NAT gateway that you would
 *                  like to map to the internal port. Pass 0 if you don't care which external port is chosen for you.
 *
 * ttl:             The requested renewal period of the NAT port mapping, in seconds.
 *                  If the client machine crashes, suffers a power failure, is disconnected from
 *                  the network, or suffers some other unfortunate demise which causes it to vanish
 *                  unexpectedly without explicitly removing its NAT port mappings, then the NAT gateway
 *                  will garbage-collect old stale NAT port mappings when their lifetime expires.
 *                  Requesting a short TTL causes such orphaned mappings to be garbage-collected
 *                  more promptly, but consumes system resources and network bandwidth with
 *                  frequent renewal packets to keep the mapping from expiring.
 *                  Requesting a long TTL is more efficient on the network, but in the event of the
 *                  client vanishing, stale NAT port mappings will not be garbage-collected as quickly.
 *                  Most clients should pass 0 to use a system-wide default value.
 *
 * callBack:        The function to be called when the port mapping request succeeds or fails asynchronously.
 *
 * context:         An application context pointer which is passed to the callback function
 *                  (may be NULL).
 *
 * return value:    Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous
 *                  errors are delivered to the callback), otherwise returns an error code indicating
 *                  the error that occurred.
 *
 *                  If you don't actually want a port mapped, and are just calling the API
 *                  because you want to find out the NAT's external IP address (e.g. for UI
 *                  display) then pass zero for protocol, internalPort, externalPort and ttl.
 */
typedef DNSServiceErrorType (DNSSD_API *FuncDNSServiceNATPortMappingCreate)
(
 DNSServiceRef                    *sdRef,
 DNSServiceFlags                  flags,
 uint32_t                         interfaceIndex,
 DNSServiceProtocol               protocol,          /* TCP and/or UDP          */
 uint16_t                         internalPort,      /* network byte order      */
 uint16_t                         externalPort,      /* network byte order      */
 uint32_t                         ttl,               /* time to live in seconds */
 DNSServiceNATPortMappingReply    callBack,
 void                             *context           /* may be NULL             */
 );


#endif // BONJOURFUNC_H
