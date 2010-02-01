/**
 * This software package has been extensively modified by members of the
 * Worldforge Project. See the file ChangeLog for details.
 *
 * $Id: skserver_unix.h,v 1.7 2006/10/12 01:10:12 alriddoch Exp $
 *
 */
#ifndef THREADS_SERVER_UNIX_H_
#define THREADS_SERVER_UNIX_H_

#include <skserver.h> // FreeSockets are needed

#include <string>

/////////////////////////////////////////////////////////////////////////////
// class UnixSockSrv
/////////////////////////////////////////////////////////////////////////////
class UnixSockSrv: public BSockSrv {
public:
  UnixSockSrv() {}

  explicit UnixSockSrv(const std::string & service) { 
    open(service); 
  }

  // Destructor
  virtual ~UnixSockSrv();

  SOCKET_TYPE accept();

  void open(const std::string & service);
};

#endif // THREADS_SERVER_UNIX_H_
