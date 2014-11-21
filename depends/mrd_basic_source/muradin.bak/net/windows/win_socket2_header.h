
#ifndef win_socket2_header_h__
#define win_socket2_header_h__

#include <Winsock2.h>
#include <mswsock.h>
#include <Ws2tcpip.h>
#include <MSTcpIP.h>
#ifndef DO_NOT_LINK_WINSOCK_LIB_

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"mswsock.lib")

#endif


#endif // win_socket2_header_h__
