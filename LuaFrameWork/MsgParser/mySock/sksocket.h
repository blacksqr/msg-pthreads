#ifndef SOCKET_H_
#define SOCKET_H_

#include <skstreamconfig.h>

#ifndef SOCKET_ERROR
static const int SOCKET_ERROR = -1;
#endif

#ifndef INVALID_SOCKET
 #define INVALID_SOCKET   (SOCKET_TYPE)~0
#endif // INVALID_SOCKET

// All systems should define this, but it is here just in case
#ifndef INADDR_NONE
 #warning System headers do not define INADDR_NONE
 #define INADDR_NONE   0xFFFFFFFF
#endif // INADDR_NONE

class BasicSock {
  // Disable copy constructor
  BasicSock(const BasicSock&);
  BasicSock& operator=(const BasicSock&);

protected:
  mutable int LastError;
  void setError() const { LastError = errno; }

  BasicSock()           { startup(); }
public:
  virtual ~BasicSock() {}

  virtual SOCKET_TYPE getSocket() const = 0;

  int  getLastError() const { return LastError; }
  bool is_open() const      { return (getSocket() != INVALID_SOCKET); }
  // System dependant initialization
  static bool startup() { return true; }
};

#endif // SOCKET_H_
