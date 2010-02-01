/**
 * This software package has been extensively modified by members of the
 * Worldforge Project. See the file ChangeLog for details.
 *
 * $Id: skserver.h,v 1.16 2006/10/12 01:10:12 alriddoch Exp $
 */
#ifndef THREADS_SERVER_H_
#define THREADS_SERVER_H_

#include <sksocket.h> // FreeSockets are needed

/////////////////////////////////////////////////////////////////////////////
// class BSockSrv
/////////////////////////////////////////////////////////////////////////////
class BSockSrv: public BasicSock {
protected:
  SOCKET_TYPE _socket;

private:
  BSockSrv(const BSockSrv&);
  BSockSrv& operator=(const BSockSrv&);

protected:
  explicit BSockSrv(SOCKET_TYPE _sock = INVALID_SOCKET): _socket(_sock) { startup(); }

public:
  // Destructor
  virtual ~BSockSrv();
  virtual SOCKET_TYPE getSocket() const;

  void close();
  void shutdown();

  /// See if accept() can be called without blocking on it.
  bool can_accept();
};

/////////////////////////////////////////////////////////////////////////////
// class IpSockSrv
/////////////////////////////////////////////////////////////////////////////
class IpSockSrv: public BSockSrv {
protected:
  int bindToIpService(int service, int type, int protocol);
  explicit IpSockSrv(SOCKET_TYPE _sock = INVALID_SOCKET): BSockSrv(_sock) {}

public:
  virtual ~IpSockSrv();
};

/////////////////////////////////////////////////////////////////////////////
// class TcpSockSrv
/////////////////////////////////////////////////////////////////////////////
class TcpSockSrv: public IpSockSrv {
public:
  TcpSockSrv() {}
  explicit TcpSockSrv(int service) { open(service); }
  // Destructor
  virtual ~TcpSockSrv();

  SOCKET_TYPE accept();
  bool open(int service);
};

/////////////////////////////////////////////////////////////////////////////
// class UdpSockSrv
/////////////////////////////////////////////////////////////////////////////
class UdpSockSrv: public IpSockSrv {
public:
  explicit UdpSockSrv(int service) { open(service); }
  // Destructor
  virtual ~UdpSockSrv();
  bool open(int service);
};

#endif // THREADS_SERVER_H_
