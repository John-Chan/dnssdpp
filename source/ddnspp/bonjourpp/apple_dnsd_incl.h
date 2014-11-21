#ifndef APPLE_DNSD_INCL_H__
#define APPLE_DNSD_INCL_H__
#ifdef _DNS_SD_H
#error "dont include [dns_sd.h] directly"
#endif
#define  WIN32_LEAN_AND_MEAN
#include <dns_sd.h>
#endif // APPLE_DNSD_INCL_H__