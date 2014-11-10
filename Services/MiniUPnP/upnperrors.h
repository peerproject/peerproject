/* $Id: upnperrors.h,v 1.5 2014/06/10 09:41:48 nanard Exp $ */
/* (c) 2007 Thomas Bernard
 * All rights reserved.
 * MiniUPnP Project.
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * This software is subject to the conditions detailed in the
 * provided LICENCE file. */

#ifndef UPNPERRORS_H_INCLUDED
#define UPNPERRORS_H_INCLUDED

#include "declspec.h"

#ifdef __cplusplus
extern "C" {
#endif

/* strupnperror()
 * Return a string description of the UPnP error code
 * or NULL for undefinded errors */
MINIUPNP_LIBSPEC const char * strupnperror(int err);

#ifdef __cplusplus
}
#endif

#endif
