#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <skstream.h>

#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>

#include <cstdio>
#include <cassert>

#define getSysError() (errno)

#ifndef HAVE_CLOSESOCKET
static inline int closesocket(SOCKET_TYPE sock) { return ::close(sock); }
#endif // HAVE_CLOSESOCKET

#ifndef HAVE_GAI_STRERROR
const char * skstream_gai_strerror(int errcode) {
  switch (errcode) {
#ifdef EAI_BADFLAGS
    case EAI_BADFLAGS:
      return "Invalid value for `ai_flags' field.";
#endif
#ifdef EAI_NONAME
    case EAI_NONAME:
      return "NAME or SERVICE is unknown.";
#endif
#ifdef EAI_AGAIN
    case EAI_AGAIN:
      return "Temporary failure in name resolution.";
#endif
#ifdef EAI_FAIL
    case EAI_FAIL:
      return "Non-recoverable failure in name res.";
#endif
#ifdef EAI_NODATA
    case EAI_NODATA:
      return "No address associated with NAME.";
#endif
#ifdef EAI_FAMILY
    case EAI_FAMILY:
      return "`ai_family' not supported.";
#endif
#ifdef EAI_SOCKTYPE
    case EAI_SOCKTYPE:
      return "`ai_socktype' not supported.";
#endif
#ifdef EAI_SERVICE
    case EAI_SERVICE:
      return "SERVICE not supported for `ai_socktype'.";
#endif
#ifdef EAI_ADDRFAMILY
    case EAI_ADDRFAMILY:
      return "Address family for NAME not supported.";
#endif
#ifdef EAI_MEMORY
    case EAI_MEMORY:
      return "Memory allocation failure.";
#endif
#ifdef EAI_SYSTEM
    case EAI_SYSTEM:
      return "System error returned in `errno'.";
#endif
#ifdef EAI_OVERFLOW
    case EAI_OVERFLOW:
      return "Argument buffer overflow.";
#endif
    default:
      return "Unknown error.";
  }
}

static const char * gai_strerror(int errcode) { return skstream_gai_strerror(errcode); }
#endif // HAVE_GAI_STRERROR


#ifndef HAVE_IN_ADDR_T
typedef unsigned long in_addr_t;
#endif // HAVE_IN_ADDR_T

SockBuf::SockBuf(SOCKET_TYPE sock, unsigned insize, unsigned outsize)
  : std::streambuf(), _socket(sock), Timeout(false)
{
  _buffer = NULL;
  const int bufsize = insize+outsize;
  char* buffer = (char*)::malloc(bufsize);
  ::memset(buffer,0,bufsize);
  // Setup the buffer
  setbuf(buffer, bufsize);
  _timeout.tv_sec  = _timeout.tv_usec = 0;
}

// Constructor
SockBuf::SockBuf(SOCKET_TYPE sock, char* buf, int length)
  : std::streambuf(), _socket(sock), Timeout(false)
{
  _buffer = NULL;
  setbuf(buf, length);
  _timeout.tv_sec  = _timeout.tv_usec = 0;
}

std::streambuf* SockBuf::setbuf(std::streambuf::char_type* buf, std::streamsize len) {
  if(buf && (len > 0)) {
    _buffer = buf;
    setp(_buffer,          _buffer+(len>>1));
    setg(_buffer+(len>>1), _buffer+len, _buffer+len);
  }
  return this;
}


// overflow() - handles output to a connected socket.
int StreamSockBuf::overflow(int nCh) {
  if(_socket==INVALID_SOCKET)
    return EOF; // Invalid socket // traits::eof()
  if(pptr()-pbase() <= 0)
    return 0; // nothing to send

  // if a timeout was specified, wait for it.
  {
    timeval _tv = _timeout;
    if((_tv.tv_sec+_tv.tv_usec) > 0) {
      int sr;
      fd_set socks;
      FD_ZERO(&socks); // zero fd_set
      FD_SET(_socket,&socks); // add buffer socket to fd_set
      sr = ::select(_socket+1,NULL,&socks,NULL,&_tv);
      if(!FD_ISSET(_socket,&socks)) {
	Timeout = true;
	return EOF; // a timeout error should be set here! - RGJ
      } else if(sr < 0)
	return EOF; // error on select() // traits::eof()
    }
    Timeout = false;
  }
  // send pending data or return EOF on error
  int size = ssend();

  if(size <= 0)
    return EOF; // Socket Could not send // traits::eof()
                // OR remote site has closed this connection

  if(nCh != EOF) { // traits::eof()
    // size >= 1 at this point
    size--;
    *(pbase()+size)=nCh;
  }
  // move remaining pbase()+size .. pptr()-1 => pbase() .. pptr()-size-1
  for(char* p=pbase()+size; p<pptr(); p++)
    *(p-size) = *p;

  const int newlen = (pptr() - pbase()) - size;

  setp(pbase(),epptr());
  pbump(newlen);
  return 0;
}

// underflow() - handles input from a connected socket.
int StreamSockBuf::underflow() {
  if(_socket == INVALID_SOCKET)
    return EOF; // Invalid socket! // traits::eof()

  if((gptr()) && (egptr()-gptr() > 0))
    return (int)(unsigned char)(*gptr());

  // if a timeout was specified, wait for it.
  if((_timeout.tv_sec+_timeout.tv_usec) > 0) {
    int sr;
    timeval _tv = _timeout;
    fd_set socks;
    FD_ZERO(&socks); // zero fd_set
    FD_SET(_socket,&socks); // add buffer socket to fd_set
    sr = ::select(_socket+1,&socks,NULL,NULL,&_tv);
    if((sr == 0) || !FD_ISSET(_socket,&socks)){
      Timeout = true;
      return EOF; // a timeout error should be set here! - RGJ
    } else if(sr < 0) return EOF;  // error on select()
  }
  Timeout = false;
  // receive data or return EOF on error
  int size = rrecv();

  if(size <= 0)
    return EOF; // remote site has closed connection or (TCP) Receive error

  // move receivd data from eback() .. eback()+size to egptr()-size .. egptr()
  const int delta = egptr()-(eback()+size);
  for(char *p=eback()+size-1; p >= eback(); p--)
    *(p+delta) = *p;
  setg(eback(), egptr()-size, egptr());
  return (int)(unsigned char)(*gptr());
}


DGramSockBuf::DGramSockBuf(SOCKET_TYPE sock, unsigned insize, unsigned outsize)
  : SockBuf(sock, insize, outsize),
    out_p_size(sizeof(out_peer)), in_p_size(sizeof(in_peer))
{}

DGramSockBuf::DGramSockBuf(SOCKET_TYPE sock, char* buf, int length)
  : SockBuf(sock, buf, length),
    out_p_size(sizeof(out_peer)), in_p_size(sizeof(in_peer))
{}

DGramSockBuf::~DGramSockBuf() { sync(); }


// setTarget() - set the target socket address
bool DGramSockBuf::setTarget(const std::string& address, unsigned port, int proto) {
  if (_socket != INVALID_SOCKET) {
    ::closesocket(_socket);
    _socket = INVALID_SOCKET;
  }

  char portName[32];
  struct addrinfo req, *ans;

  ::sprintf(portName, "%d", port);

  req.ai_flags = 0;
  req.ai_family = PF_UNSPEC;
  req.ai_socktype = SOCK_DGRAM;
  req.ai_protocol = proto;
  req.ai_addrlen = 0;
  req.ai_addr = 0;
  req.ai_canonname = 0;
  req.ai_next = 0;

  int ret = ::getaddrinfo(address.c_str(), portName, &req, &ans);
  if (ret)
    return false;

  bool success = false;
  for(struct addrinfo * i = ans; success == false && i != 0; i = i->ai_next) {

    _socket = ::socket(i->ai_family, i->ai_socktype, i->ai_protocol);
    if (_socket != INVALID_SOCKET) {
      ::memcpy(&out_peer, i->ai_addr, i->ai_addrlen);
      out_p_size = i->ai_addrlen;
      success = true;
    }
  }
  ::freeaddrinfo(ans);
  return success;
}


BSockStream::BSockStream(SockBuf & buffer, int proto)
  : std::iostream(&buffer), _sockbuf(buffer),
    m_protocol(proto)
{
  startup();
  init(&_sockbuf); // initialize underlying streambuf
}

BSockStream::~BSockStream() {
  if(is_open()) {
    ::shutdown(_sockbuf.getSocket(), SHUT_RDWR);
    ::closesocket(_sockbuf.getSocket());
  }
  delete &_sockbuf;
}

SOCKET_TYPE BSockStream::getSocket() const {
  return _sockbuf.getSocket();
}

void BSockStream::shutdown() {
  if(is_open()) {
    if(::shutdown(_sockbuf.getSocket(), SHUT_RDWR) == SOCKET_ERROR) {
      setError();
    }
  }
}

// closes a socket connection
void BSockStream::close() {
  if(is_open()) {
    if(::shutdown(_sockbuf.getSocket(), SHUT_RDWR) == SOCKET_ERROR) {
      setError();
      //not necessarily a returning offense because there could be a socket
      //open that has never connected to anything and hence, does not need
      //to be shutdown.
    }

    if(::closesocket(_sockbuf.getSocket()) == SOCKET_ERROR) {
      setError();
      return;
    }
    _sockbuf.setSocket(INVALID_SOCKET);
  }
}

// Check for failure condition
bool BSockStream::fail() {
  if(timeout()) {
    clear();
    return false;
  }
  if(std::iostream::fail()) {
    setError();
    return true;
  }
  return false;
}


TcpSockStream::TcpSockStream() :
  BSockStream(*new StreamSockBuf(INVALID_SOCKET)),
  _connecting_socket(INVALID_SOCKET),
  _connecting_address(0),
  _connecting_addrlist(0),
  stream_sockbuf((StreamSockBuf&)_sockbuf) {
  m_protocol = FreeSockets::proto_TCP;
}

TcpSockStream::TcpSockStream(SOCKET_TYPE socket) :
  BSockStream(*new StreamSockBuf(socket)),
  _connecting_socket(INVALID_SOCKET),
  _connecting_address(0),
  _connecting_addrlist(0),
  stream_sockbuf((StreamSockBuf&)_sockbuf) {
  m_protocol = FreeSockets::proto_TCP;
}

TcpSockStream::TcpSockStream(const std::string& address, int service,
			     bool nonblock) :
  BSockStream(*new StreamSockBuf(INVALID_SOCKET)),
  _connecting_socket(INVALID_SOCKET),
  _connecting_address(0),
  _connecting_addrlist(0),
  stream_sockbuf((StreamSockBuf&)_sockbuf) {
  m_protocol = FreeSockets::proto_TCP;
  open(address, service, nonblock);
}

TcpSockStream::TcpSockStream(const std::string& address, int service,
			     unsigned int milliseconds) :
  BSockStream(*new StreamSockBuf(INVALID_SOCKET)),
  _connecting_socket(INVALID_SOCKET),
  _connecting_address(0),
  _connecting_addrlist(0),
  stream_sockbuf((StreamSockBuf&)_sockbuf) {
  m_protocol = FreeSockets::proto_TCP;
  open(address, service, milliseconds);
}

TcpSockStream::~TcpSockStream() { 
  if(_connecting_socket != INVALID_SOCKET) {
    ::shutdown(_connecting_socket, SHUT_RDWR);
    ::closesocket(_connecting_socket);
    ::freeaddrinfo(_connecting_addrlist);
  }
}

void TcpSockStream::open(const std::string & address,
			 int service, bool nonblock)
{
  if (is_open() || _connecting_socket != INVALID_SOCKET) {
    close();
  }

  if (_connecting_addrlist != 0) {
    ::freeaddrinfo(_connecting_addrlist);
    _connecting_addrlist = 0;
  }

  struct addrinfo req, *ans;
  char serviceName[32];

  ::sprintf(serviceName, "%d", service);

  req.ai_flags = 0;
  req.ai_family = PF_UNSPEC;
  req.ai_socktype = SOCK_STREAM;
  req.ai_protocol = m_protocol;
  req.ai_addrlen = 0;
  req.ai_addr = 0;
  req.ai_canonname = 0;
  req.ai_next = 0;

  int ret;
  if ((ret = ::getaddrinfo(address.c_str(), serviceName, &req, &ans)) != 0) {
    fail();
    return;
  }

  bool success = false;
  SOCKET_TYPE _socket = INVALID_SOCKET;

  for(struct addrinfo * i = ans; success == false && i != 0; i = i->ai_next) {

    _socket = ::socket(i->ai_family, i->ai_socktype, i->ai_protocol);
    if(_socket == INVALID_SOCKET) {
      continue;
    }

    if(nonblock) {
      int err_val = ::fcntl(_socket, F_SETFL, O_NONBLOCK);
      if(err_val == -1) {
        setError();
        ::closesocket(_socket);
        continue;
      }
    }

    sockaddr_storage iaddr;
    ::memcpy(&iaddr, i->ai_addr, i->ai_addrlen);
    SOCKLEN iaddrlen = i->ai_addrlen;

    if(::connect(_socket, (sockaddr *)&iaddr, iaddrlen) < 0) {
      if(nonblock && getSysError() == SOCKET_BLOCK_ERROR) {
        _connecting_socket = _socket;
        _connecting_address = i;
        _connecting_addrlist = ans;
        return;
      }
      setError();
      ::closesocket(_socket);
    } else {
      success = true;
    }

  }

  ::freeaddrinfo(ans);

  if (!success) {
    fail();
    return;
  }

  // set the socket blocking again for io
  if(nonblock) {
    int err_val = ::fcntl(_socket, F_SETFL, 0);
    if(err_val == -1) {
      setError();
      ::closesocket(_socket);
      _socket = INVALID_SOCKET;
      fail();
      return;
    }
  }

  // set socket for underlying SockBuf
  _sockbuf.setSocket(_socket);
}

void TcpSockStream::open(const std::string & address, int service,
			 unsigned int milliseconds)
{
  open(address, service, true);
  if(!isReady(milliseconds)) {
    close();
    fail();
  }
}

void TcpSockStream::close()
{
  if(_connecting_socket != INVALID_SOCKET) {
    ::closesocket(_connecting_socket);
    _connecting_socket = INVALID_SOCKET;
  }

  BSockStream::close();
}

SOCKET_TYPE TcpSockStream::getSocket() const
{
  return (_connecting_socket == INVALID_SOCKET)
    ? BSockStream::getSocket() : _connecting_socket;
}

const std::string TcpSockStream::getRemoteHost(bool lookup) const
{
  sockaddr_storage peer;
  SOCKLEN peer_size;
#ifdef HAVE_GETADDRINFO
  char hbuf[NI_MAXHOST];
  const int flags = lookup ? 0 : NI_NUMERICHOST;
#endif

  if (::getpeername(getSocket(), (sockaddr*)&peer, &peer_size) != 0) {
    return "[unconnected]";
  }

#ifdef HAVE_GETADDRINFO

  if (::getnameinfo((const sockaddr*)&peer, peer_size,
                    hbuf, sizeof(hbuf), 0, 0, flags) == 0) {
    return std::string(hbuf);
  }
  return "[unknown]";
#else // HAVE_GETADDRINFO
  return std::string(::inet_ntoa(((const sockaddr_in&)peer).sin_addr));
#endif // HAVE_GETADDRINFO
}

const std::string TcpSockStream::getRemoteService(bool lookup) const
{
  char sbuf[NI_MAXSERV];
  sockaddr_storage peer;
  SOCKLEN peer_size;
#ifdef HAVE_GETADDRINFO
  const int flags = lookup ? 0 : NI_NUMERICSERV;
#endif

  if (::getpeername(getSocket(), (sockaddr*)&peer, &peer_size) != 0) {
    return "[unconnected]";
  }

#ifdef HAVE_GETADDRINFO
  if (::getnameinfo((const sockaddr*)&peer, peer_size,
                    0, 0, sbuf, sizeof(sbuf), flags) == 0) {
    return std::string(sbuf);
  }
  return "[unknown]";
#else // HAVE_GETADDRINFO

  unsigned short port = ntohs(((const sockaddr_in&)peer).sin_port);
  ::sprintf(sbuf, "%d", port);
  return std::string(sbuf);
#endif // HAVE_GETADDRINFO
}

bool TcpSockStream::isReady(unsigned int milliseconds) {
  if(_connecting_socket == INVALID_SOCKET)
    return true;

  fd_set fds;
  struct timeval wait_time = {milliseconds/1000, (milliseconds%1000) * 1000};

  FD_ZERO(&fds);
  FD_SET(_connecting_socket, &fds);

  if (::select(_connecting_socket + 1, 0, &fds, 0, &wait_time) != 1
      || !FD_ISSET(_connecting_socket, &fds)) {
    return false;
  }

  // It's done connecting, check for error

  // We're no longer connecting, put the socket in a tmp variable
  SOCKET_TYPE _socket = _connecting_socket;
  _connecting_socket = INVALID_SOCKET;

  int errnum;
  SOCKLEN errsize = sizeof(errnum);
  ::getsockopt(_socket, SOL_SOCKET, SO_ERROR, &errnum, &errsize);

  // Check for failure, and if it has occured, we need to
  // revisit the address list we got from getaddrinfo.
  assert(_connecting_addrlist != 0);
  assert(_connecting_address != 0);

  if (errnum != 0) {
    ::closesocket(_socket);

    bool success = false;

    struct addrinfo * i = _connecting_address->ai_next;
    for (; success == false && i != 0; i = i->ai_next) {
      _socket = ::socket(i->ai_family, i->ai_socktype, i->ai_protocol);
      if(_socket == INVALID_SOCKET) {
        continue;
      }
      int err_val = ::fcntl(_socket, F_SETFL, O_NONBLOCK);
      if(err_val == -1) {
        setError();
        ::closesocket(_socket);
        continue;
      }

      sockaddr_storage iaddr;
      ::memcpy(&iaddr, i->ai_addr, i->ai_addrlen);
      SOCKLEN iaddrlen = i->ai_addrlen;

      if(::connect(_socket, (sockaddr *)&iaddr, iaddrlen) < 0) {
        if(getSysError() == SOCKET_BLOCK_ERROR) {
          _connecting_socket = _socket;
          _connecting_address = i;
          return false;
        }
        setError();
        ::closesocket(_socket);
      } else {
        success = true;
      }
    }

  }

  ::freeaddrinfo(_connecting_addrlist);
  _connecting_addrlist = 0;
  _connecting_address = 0;

  // set the socket blocking again for io
  int err_val = ::fcntl(_socket, F_SETFL, 0);
  if(err_val == -1) {
    setError();
    ::closesocket(_socket);
    _socket = INVALID_SOCKET;
    fail();
    return true;
  }

  // set socket for underlying SockBuf
  _sockbuf.setSocket(_socket);
  return true;
}


DGramSockStream::DGramSockStream() :
  BSockStream(*new DGramSockBuf(INVALID_SOCKET)),
  dgram_sockbuf((DGramSockBuf&)_sockbuf)
{}

int DGramSockStream::bindToIpService(int service, int type, int protocol) {
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
    SOCKET_TYPE socket = ::socket(i->ai_family, i->ai_socktype, i->ai_protocol);
    if (socket == INVALID_SOCKET) {
      setError();
      continue;
    }
    dgram_sockbuf.setSocket(socket);

    sockaddr_storage iaddr;
    ::memcpy(&iaddr, i->ai_addr, i->ai_addrlen);
    SOCKLEN iaddrlen = i->ai_addrlen;

    if (::bind(socket, (sockaddr*)&iaddr, iaddrlen) == SOCKET_ERROR) {
      setError();
      close();
    } else {
      success = 0;
    }
  }

  ::freeaddrinfo(ans);

  return success;
#else // HAVE_GETADDRINFO
  // create socket
  SOCKET_TYPE socket = ::socket(AF_INET, type, protocol);
  if(socket == INVALID_SOCKET) {
    setError();
    return -1;
  }
  dgram_sockbuf.setSocket(socket);

  // Bind Socket
  sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = INADDR_ANY; // we want to connect to ANY client!
  sa.sin_port = htons((unsigned short)service); // define service port
  if(::bind(socket, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
    setError();
    close();
    return -1;
  }
  return 0;
#endif // HAVE_GETADDRINFO
}

/////////////////////////////////////////////////////////////////////////////
// class UdpSockStream implementation
/////////////////////////////////////////////////////////////////////////////

UdpSockStream::UdpSockStream() {
  m_protocol = FreeSockets::proto_UDP; 
}

UdpSockStream::~UdpSockStream() {
  // Don't close the main socket, that is done in the BSockStream
  // destructor
}

int UdpSockStream::open(int service) {
  if (is_open()) {
    close();
  }
  if (bindToIpService(service, SOCK_DGRAM, IPPROTO_UDP) != 0) {
    return -1;
  }
  return 0;
}

#ifdef SKSTREAM_UNIX_SOCKETS
#include <skstream_unix.h>
#include <sys/un.h>


UnixSockStream::UnixSockStream() :
  BSockStream(*new StreamSockBuf(INVALID_SOCKET)),
  _connecting_socket(INVALID_SOCKET),
  stream_sockbuf((StreamSockBuf&)_sockbuf) {}

UnixSockStream::UnixSockStream(const std::string& address, bool nonblock) :
  BSockStream(*new StreamSockBuf(INVALID_SOCKET)),
  _connecting_socket(INVALID_SOCKET),
  stream_sockbuf((StreamSockBuf&)_sockbuf)
{
  open(address, nonblock);
}

UnixSockStream::UnixSockStream(const std::string & address, unsigned int milliseconds) :
  BSockStream(stream_sockbuf),
  _connecting_socket(INVALID_SOCKET),
  stream_sockbuf((StreamSockBuf&)_sockbuf)
{
  open(address, milliseconds);
}

UnixSockStream::~UnixSockStream() { 
  // Don't close the main socket, that is done in the BSockStream
  // destructor
  if(_connecting_socket != INVALID_SOCKET) {
    ::shutdown(_connecting_socket, SHUT_RDWR);
    ::closesocket(_connecting_socket);
  }
}

void UnixSockStream::open(const std::string& address, bool nonblock) {
  if (address.size() >  107)
    return;
  if(is_open() || _connecting_socket != INVALID_SOCKET)
    close();
  // Create socket
  SOCKET_TYPE _socket = ::socket(AF_UNIX, SOCK_STREAM, m_protocol);
  if(_socket == INVALID_SOCKET) {
    fail();
    return;
  }
  if(nonblock) {
    int err_val = ::fcntl(_socket, F_SETFL, O_NONBLOCK);
    if(err_val == -1) {
      setError();
      ::closesocket(_socket);
      _socket = INVALID_SOCKET;
      fail();
      return;
    }
  }
  // Fill host information
  sockaddr_un sa;
  sa.sun_family = AF_UNIX;
  strncpy(sa.sun_path, address.c_str(), 108);

  if(::connect(_socket,(sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
    if(nonblock && getSysError() == SOCKET_BLOCK_ERROR) {
      _connecting_socket = _socket;
      return;
    }
    setError();
    fail();
    ::closesocket(_socket);
    return;
  }

  // set the socket blocking again for io
  if(nonblock) {
    int err_val = ::fcntl(_socket, F_SETFL, 0);
    if(err_val == -1) {
      setError();
      ::closesocket(_socket);
      _socket = INVALID_SOCKET;
      fail();
      return;
    }
  }
  // set socket for underlying SockBuf
  _sockbuf.setSocket(_socket);
}

void UnixSockStream::close() {
  if(_connecting_socket != INVALID_SOCKET) {
    ::closesocket(_connecting_socket);
    _connecting_socket = INVALID_SOCKET;
  }

  BSockStream::close();
}

SOCKET_TYPE UnixSockStream::getSocket() const {
  return (_connecting_socket == INVALID_SOCKET)
    ? BSockStream::getSocket() : _connecting_socket;
}

bool UnixSockStream::isReady(unsigned int milliseconds) {
  if(_connecting_socket == INVALID_SOCKET)
    return true;

  fd_set fds;
  struct timeval wait_time = {milliseconds / 1000, (milliseconds % 1000) * 1000};

  FD_ZERO(&fds);
  FD_SET(_connecting_socket, &fds);

  if ((1 != ::select(_connecting_socket + 1, 0, &fds, 0, &wait_time)) || !FD_ISSET(_connecting_socket, &fds))
    return false;

  // It's done connecting, check for error

  // We're no longer connecting, put the socket in a tmp variable
  SOCKET_TYPE _socket = _connecting_socket;
  _connecting_socket = INVALID_SOCKET;

  int errnum;
  SOCKLEN errsize = sizeof(errnum);
  getsockopt(_socket, SOL_SOCKET, SO_ERROR, &errnum, &errsize);
  if(errnum != 0) {
    // Can't use setError(), since errno doesn't have the error
    LastError = errnum;
    fail();
    ::closesocket(_socket);
    return true;
  }

  // set the socket blocking again for io
  int err_val = ::fcntl(_socket, F_SETFL, 0);
  if(err_val == -1) {
    setError();
    ::closesocket(_socket);
    _socket = INVALID_SOCKET;
    fail();
    return true;
  }
  // set socket for underlying SockBuf
  _sockbuf.setSocket(_socket);
  return true;
}
#endif // SKSTREAM_UNIX_SOCKETS


#ifdef SOCK_RAW

RawSockStream::RawSockStream(FreeSockets::IP_Protocol proto) {
  m_protocol = proto;
  SOCKET_TYPE _socket = ::socket(AF_INET, SOCK_RAW, m_protocol);
  _sockbuf.setSocket(_socket);
}

void RawSockStream::setProtocol(FreeSockets::IP_Protocol proto) {
  if(is_open()) close();
  m_protocol = proto;
  SOCKET_TYPE _socket = ::socket(AF_INET, SOCK_RAW, m_protocol);
  _sockbuf.setSocket(_socket);
}

RawSockStream::~RawSockStream() {
  // Don't close the main socket, that is done in the BSockStream
  // destructor
}

bool RawSockStream::setBroadcast(bool opt) {
  int ok = opt?1:0;
  ok = ::setsockopt(_sockbuf.getSocket(),SOL_SOCKET,SO_BROADCAST,(char*)&ok,sizeof(ok));
  bool ret = (ok != SOCKET_ERROR);
  if(!ret) setError();
  return ret;
}

#endif // SOCK_RAW
