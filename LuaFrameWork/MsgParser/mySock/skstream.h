#ifndef STREAM_H_
#define STREAM_H_

#include <stdlib.h>
#include <iostream>
#include <sksocket.h>


class SockBuf: public std::streambuf {
  // Disable copy constructor
  SockBuf(const SockBuf&);
  SockBuf& operator=(const SockBuf&);

private:
  char* _buffer;

protected:
  SOCKET_TYPE  _socket;
  timeval      _timeout;

  bool Timeout;
  // Handle writing data from the buffer to the socket.
  virtual int overflow(int nCh=EOF) = 0;
  // Handle reading data from the socket to the buffer.
  virtual int underflow()           = 0;

  // Set the buffer area this stream buffer uses. Only works if not already set. */
  std::streambuf* setbuf(std::streambuf::char_type* buf, std::streamsize len);
  // Flush the output buffer.
  int sync() { return (overflow() == EOF) ? EOF : 0; }

public:
  // Make a new socket buffer from an existing socket, with optional buffer sizes.
  explicit SockBuf(SOCKET_TYPE sock, unsigned insize=0x10000, unsigned outsize=0x10000);
  // Make a new socket buffer from an existing socket, with an existing buffer.
  SockBuf(SOCKET_TYPE sock, char* buf, int length);

  // Destroy the socket buffer.
  virtual ~SockBuf() {
    if(_buffer) {
      free(_buffer);
      _buffer = NULL;
    }
  }
  void setSocket(SOCKET_TYPE sock) {_socket = sock; }
  SOCKET_TYPE getSocket() const    { return _socket; }

  /* Set up a timeout value after which an error flag is set if the socket
   *  is not ready for a read or write. */
  void setTimeout(unsigned sec, unsigned usec=0) {
    _timeout.tv_sec  = sec;
    _timeout.tv_usec = usec;
  }
  // Return the flag indicating whether a timeout has occured.
  bool timeout() const { return Timeout; }
};


class StreamSockBuf: public SockBuf {
  // Disable copy constructor
  StreamSockBuf(const StreamSockBuf&);
  StreamSockBuf& operator=(const StreamSockBuf&);

protected:
  // Handle writing data from the buffer to the socket.
  int overflow(int nCh=EOF);
  // Handle reading data from the socket to the buffer.
  int underflow();

  virtual int ssend() { return ::send(_socket, pbase(), pptr()-pbase(), 0); }
  virtual int rrecv() { return ::recv(_socket, eback(), egptr()-eback(), 0); }

public:
  // Make a new socket buffer from an existing socket, with optional buffer sizes.
  explicit StreamSockBuf(SOCKET_TYPE sock, unsigned insize=0x10000, unsigned outsize=0x10000):
  SockBuf(sock, insize, outsize)
  {}
  // Make a new socket buffer from an existing socket, with an existing buffer.
  StreamSockBuf(SOCKET_TYPE sock, char* buf, int length):
  SockBuf(sock, buf, length)
  {}
  virtual ~StreamSockBuf() { sync(); }
};


class DGramSockBuf: public SockBuf {
protected:
  // Target address of datagrams sent via this stream
  sockaddr_storage out_peer;
  // Source address of last datagram received by this stream
  sockaddr_storage in_peer;
  // Size of target address
  SOCKLEN out_p_size;
  // Size of source address
  SOCKLEN in_p_size;

  virtual int ssend() {
    return ::sendto(_socket,pbase(), pptr()-pbase(), 0, (sockaddr*)&out_peer, out_p_size);
  }
  virtual int rrecv() {
    in_p_size = sizeof(in_peer);
    return ::recvfrom(_socket, eback(), egptr()-eback(), 0, (sockaddr*)&in_peer, &in_p_size);
  }

public:
  // Make a new socket buffer from an existing socket, with optional buffer sizes. */
  explicit DGramSockBuf(SOCKET_TYPE sock, unsigned insize=0x8000, unsigned outsize=0x8000);
  // Make a new socket buffer from an existing socket, with an existing buffer. */
  DGramSockBuf(SOCKET_TYPE sock, char* buf, int length);

  // Destroy the socket buffer.
  virtual ~DGramSockBuf();

  bool setTarget(const std::string& address, unsigned port, int proto);
  void setOutpeer(const sockaddr_storage & peer) { out_peer = peer; }
  const sockaddr_storage & getOutpeer() const { return out_peer; }
  const sockaddr_storage & getInpeer() const { return in_peer; }
  SOCKLEN getOutpeerSize() const { return out_p_size; }
  SOCKLEN getInpeerSize() const { return in_p_size; }
};

// Supported Protocols
namespace FreeSockets {
  enum IP_Protocol {
    proto_IP   = IPPROTO_IP,
    proto_ICMP = IPPROTO_ICMP,
    proto_IGMP = IPPROTO_IGMP, 
    proto_TCP  = IPPROTO_TCP,
    proto_PUP  = IPPROTO_PUP,
    proto_UDP  = IPPROTO_UDP,
    proto_IDP  = IPPROTO_IDP,
    proto_RAW  = IPPROTO_RAW
  };

  // Well known ports
  enum IP_Service {
    echo        =       7, //
    daytime     =      13, //
    ftp         =      21, //
    ssh         =      22, //
    telnet      =      23, //
    smtp        =      25, //       mail
    time        =      37, //       timserver
    name        =      42, //       nameserver
    nameserver  =      53, //       domain        # name-domain server
    finger      =      79, //
    http        =      80, //
    pop         =     109, //       postoffice
    pop2        =     109, //                     # Post Office
    pop3        =     110, //       postoffice
    nntp        =     119  //       usenet        # Network News Transfer
  };
};


class BSockStream: public BasicSock,
  public std::iostream
{
protected:
  SockBuf& _sockbuf;
  int m_protocol;

public:
  // Make a socket stream.
  BSockStream(SockBuf & buffer, int proto = FreeSockets::proto_IP);
  // Destructor
  virtual ~BSockStream();

  bool fail();
  bool operator ! () { return fail(); }
  bool timeout() const { return _sockbuf.timeout(); }

  virtual SOCKET_TYPE getSocket() const;

  // Needs to be virtual to handle in-progress connect()'s for
  // tcp sockets
  virtual void close();

  void shutdown();

  void setSocket(SOCKET_TYPE sock) { _sockbuf.setSocket(sock); }
  void setTimeout(unsigned sec, unsigned usec=0) { _sockbuf.setTimeout(sec,usec); }
  int getProtocol() const { return m_protocol; }
};


struct addrinfo;

class TcpSockStream: public BSockStream {
private:
  TcpSockStream(const TcpSockStream&);

  TcpSockStream& operator=(const TcpSockStream& socket);

  SOCKET_TYPE _connecting_socket;
  struct addrinfo * _connecting_address;
  struct addrinfo * _connecting_addrlist;

  StreamSockBuf & stream_sockbuf;

public:
  TcpSockStream();
  TcpSockStream(SOCKET_TYPE socket);
  TcpSockStream(const std::string& address, int service, bool nonblock = false);
  TcpSockStream(const std::string& address, int service, unsigned int milliseconds);
  virtual ~TcpSockStream();

  void open(const std::string& address, int service, bool nonblock = false);
  void open(const std::string& address, int service, unsigned int milliseconds);

  virtual void close();
  virtual SOCKET_TYPE getSocket() const;

  const std::string getRemoteHost(bool lookup = false) const;
  const std::string getRemoteService(bool lookup = false) const;
  bool isReady(unsigned int milliseconds = 0);
};


class DGramSockStream: public BSockStream {
  // Disable copy constructor
  DGramSockStream(const DGramSockStream&);
  DGramSockStream& operator=(const DGramSockStream& socket);

protected:
  DGramSockBuf & dgram_sockbuf;
  int bindToIpService(int service, int type, int protocol);

public:
  DGramSockStream();
  virtual ~DGramSockStream() {}

  bool setTarget(const std::string& address, unsigned port) { return dgram_sockbuf.setTarget(address, port, m_protocol); }
  void setOutpeer(const sockaddr_storage& peer) { return dgram_sockbuf.setOutpeer(peer); }
  const sockaddr_storage & getOutpeer() const { return dgram_sockbuf.getOutpeer(); }
  const sockaddr_storage & getInpeer() const { return dgram_sockbuf.getInpeer(); }

  SOCKLEN getOutpeerSize() const { return dgram_sockbuf.getOutpeerSize(); }
  SOCKLEN getInpeerSize() const { return dgram_sockbuf.getInpeerSize(); }
};


class UdpSockStream: public DGramSockStream {
private:
  UdpSockStream(const UdpSockStream&);
  UdpSockStream& operator=(const UdpSockStream& socket);

public:
  UdpSockStream();
  virtual ~UdpSockStream();

  int open(int service);
};


class RawSockStream: public DGramSockStream {
  // Disable copy constructor
  RawSockStream(const RawSockStream&);
  RawSockStream& operator=(const RawSockStream& socket);

public:
  RawSockStream(FreeSockets::IP_Protocol proto=FreeSockets::proto_RAW);
  virtual ~RawSockStream();

  void setProtocol(FreeSockets::IP_Protocol proto);

  bool setTarget(const std::string& address, unsigned port) { 
    return dgram_sockbuf.setTarget(address, port, m_protocol); 
  }
  bool setBroadcast(bool opt=false);
};

#endif // STREAM_H_

