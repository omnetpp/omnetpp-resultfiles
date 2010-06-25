/*
 * Copyright (c) 2010, Andras Varga and Opensim Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Opensim Ltd. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Andras Varga or Opensim Ltd. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PLATDEP_SOCKETS_H
#define __PLATDEP_SOCKETS_H

//
// With some care, it's possible to write platform-independent socket code
// using the macros below.
//

#ifdef _WIN32
//
// Winsock version
//

// include <winsock.h> or <winsock2.h> (mutually exclusive) if neither has been included yet
#if !defined(_WINSOCKAPI_) && !defined(_WINSOCK2API_)
# ifndef WANT_WINSOCK2
#  include <winsock.h>
# else
#  include <winsock2.h>
# endif
#endif
#undef min
#undef max

// Winsock prefixes error codes with "WS"
#define SOCKETERR(x)  WS#x
inline int sock_errno()  {return WSAGetLastError();}

// Shutdown mode constants are named differently (and only in winsock2.h
// which we don't want to pull in)
#define SHUT_RD   0x00  // as SD_RECEIVE in winsock2.h
#define SHUT_WR   0x01  // as SD_SEND in winsock2.h
#define SHUT_RDWR 0x02  // as SD_BOTH in winsock2.h

typedef int socklen_t;

inline int initsocketlibonce() {
    static bool inited = false;  //FIXME "static" and "inline" conflict!
    if (inited) return 0;
    inited = true;
    WSAData wsaData;
#ifdef _WINSOCKAPI_
    return WSAStartup(MAKEWORD(1,1), &wsaData);
#else
    return WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
}


#else
//
// Unix version
//
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCKET int
inline int initsocketlibonce() {return 0;}
inline void closesocket(int) {}
inline int sock_errno()  {return errno;}
#define SOCKETERR(x)  x
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1

#endif

#endif
