/**
 * This software package has been extensively modified by members of the
 * Worldforge Project. See the file ChangeLog for details.
 *
 * $Id: skserver.cpp,v 1.23 2006/10/12 01:10:12 alriddoch Exp $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <skserver.h>

#include <sys/types.h>
#include <netdb.h>
#include <errno.h>

#include <string.h>
#include <iostream>
#include <cstdio>

static inline int getSysError() { return errno; }

#ifndef HAVE_CLOSESOCKET
static inline int closesocket(SOCKET_TYPE sock) { return ::close(sock); }
#endif // HAVE_CLOSESOCKET

#ifdef HAVE_GETADDRINFO
#ifndef HAVE_GAI_STRERROR
const char * skstream_gai_strerror(int errcode);
const char * gai_strerror(int errcode) { return skstream_gai_strerror(errcode); }
#endif // HAVE_GAI_STRERROR
#endif // HAVE_GETADDRINFO

/////////////////////////////////////////////////////////////////////////////
// class BSockSrv implementation
/////////////////////////////////////////////////////////////////////////////

BSockSrv::~BSockSrv() { BSockSrv::close(); }

SOCKET_TYPE BSockSrv::getSocket() const { return _socket; }

// close server's underlying socket
//   The shutdown is a little rude... -  RGJ
void BSockSrv::close() {
  if(is_open()) {
    if(::shutdown(_socket, SHUT_RDWR) == SOCKET_ERROR) {
      setError();
      //not necessarily a returning offense because there could be a socket
      //open that has never connected to anything and hence, does not need
      //to be shutdown.
    }

    if(::closesocket(_socket) == SOCKET_ERROR) {
      setError();
      return;
    }
    _socket = INVALID_SOCKET;
  }
}

void BSockSrv::shutdown() {
  if(is_open()) {
    if(::shutdown(_socket, SHUT_RDWR) == SOCKET_ERROR) {
      setError();
    }
  }
}

bool BSockSrv::can_accept() {
  if(_socket == INVALID_SOCKET) return false;

  fd_set sock_fds;
  struct timeval tv = {0,0};

  FD_ZERO(&sock_fds);
  FD_SET(_socket, &sock_fds);

  int ret = ::select((_socket + 1), &sock_fds, NULL, NULL, &tv);

  if( ret > 0) {
      return true;
  } else if (ret < 0)
      setError();
  return false;
}

/////////////////////////////////////////////////////////////////////////////
// class IpSockSrv implementation
/////////////////////////////////////////////////////////////////////////////

int IpSockSrv::bindToIpService(int service, int type, int protocol) {
#ifdef HAVE_GETADDRINFO
  struct addrinfo req, *ans;
  char serviceName[32];

  ::sprintf(serviceName, "%d", service);

  req.ai_flags = AI_PASSIVE;
  req.ai_family = PF_UNSPEC;
  req.ai_socktype = type;
  req.ai_protocol = 0;
  req.ai_addrlen = 0;
  req.ai_addr = 0;
  req.ai_canonname = 0;
  req.ai_next = 0;

  int ret;
  if ((ret = ::getaddrinfo(0, serviceName, &req, &ans)) != 0) {
    std::cout << "skstream: " << gai_strerror(ret)
              << std::endl << std::flush;
    setError();
    return -1;
  }

  int success = -1;

  for(struct addrinfo * i = ans; success == -1 && i != 0; i = i->ai_next) {
    _socket = ::socket(i->ai_family, i->ai_socktype, i->ai_protocol);
    if (_socket == INVALID_SOCKET) {
      setError();
      continue;
    }

    sockaddr_storage iaddr;
    ::memcpy(&iaddr, i->ai_addr, i->ai_addrlen);
    SOCKLEN iaddrlen = i->ai_addrlen;

    if (::bind(_socket, (sockaddr*)&iaddr, iaddrlen) == SOCKET_ERROR) {
      setError();
      close();
    } else {
      success = 0;
    }
  }

  ::freeaddrinfo(ans);

  return 0;
#else // HAVE_GETADDRINFO
  // create socket
  _socket = ::socket(AF_INET, type, protocol);
  if(_socket == INVALID_SOCKET) {
    setError();
    return false;
  }
  // Bind Socket
  sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = INADDR_ANY; // we want to connect to ANY client!
  sa.sin_port = htons((unsigned short)service); // define service port
  if(::bind(_socket, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
    setError();
    close();
    return false;
  }
  return true;
#endif // HAVE_GETADDRINFO
}

IpSockSrv::~IpSockSrv() {}

/////////////////////////////////////////////////////////////////////////////
// class TcpSockSrv implementation
/////////////////////////////////////////////////////////////////////////////

TcpSockSrv::~TcpSockSrv() {}

// handles tcp connections request
SOCKET_TYPE TcpSockSrv::accept() {
  if(_socket==INVALID_SOCKET) return INVALID_SOCKET;
  SOCKET_TYPE commsock = ::accept(_socket, NULL, NULL);
  if(commsock == INVALID_SOCKET) {
    setError();
    close();
    return INVALID_SOCKET;
  }
  return commsock;
}

// start tcp server and put it in listen state
bool TcpSockSrv::open(int service) {
  if (is_open())
    close();
  if (bindToIpService(service, SOCK_STREAM, IPPROTO_TCP) != 0)
    return false;

  // Listen
  if(::listen(_socket, 5) == SOCKET_ERROR) {
    // max backlog
    setError();
    close();
    return false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////
// class UdpSockSrv implementation
/////////////////////////////////////////////////////////////////////////////

UdpSockSrv::~UdpSockSrv() {}

// create a UDP socket binded to a given port
bool UdpSockSrv::open(int service) {
  if (is_open())
    close();
  return !bindToIpService(service, SOCK_DGRAM, IPPROTO_UDP);
}

#ifdef SKSTREAM_UNIX_SOCKETS

#include <skserver_unix.h>
#include <sys/un.h>

/////////////////////////////////////////////////////////////////////////////
// class UnixSockSrv implementation
/////////////////////////////////////////////////////////////////////////////

UnixSockSrv::~UnixSockSrv() {}

// handles unix connections request
SOCKET_TYPE UnixSockSrv::accept() {
  if(_socket==INVALID_SOCKET) return INVALID_SOCKET;
  SOCKET_TYPE commsock = ::accept(_socket, NULL, NULL);
  if(commsock == INVALID_SOCKET) {
    setError();
    close();
    return INVALID_SOCKET;
  }
  return commsock;
}

// start unix server and put it in listen state
void UnixSockSrv::open(const std::string & service) {
  if(is_open()) close();

  if (service.size() > 107)
    return;

  // create socket
  _socket = ::socket(AF_UNIX, SOCK_STREAM, 0);
  if(_socket == INVALID_SOCKET) {
    setError();
    return;
  }

  // Bind Socket
  sockaddr_un sa;
  sa.sun_family = AF_UNIX;
  strncpy(sa.sun_path, service.c_str(), 108);
  if(::bind(_socket, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
    setError();
    close();
    return;
  }

  // Listen
  if(::listen(_socket, 5) == SOCKET_ERROR) { // max backlog
    setError();
    close();
    return;
  }
}

#endif // SKSTREAM_UNIX_SOCKETS
