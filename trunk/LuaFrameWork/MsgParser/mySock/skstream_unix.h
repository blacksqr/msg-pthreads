/**
 * This software package has been extensively modified by members of the
 * Worldforge Project. See the file ChangeLog for details.
 *
 * $Id: skstream_unix.h,v 1.9 2006/10/12 01:10:12 alriddoch Exp $
 *
 */
#ifndef SKSTREAM_UNIX_H_
#define SKSTREAM_UNIX_H_

#include <skstream.h>

/////////////////////////////////////////////////////////////////////////////
// class UnixSockStream
/////////////////////////////////////////////////////////////////////////////
class UnixSockStream: public BSockStream {
private:
  explicit UnixSockStream(const UnixSockStream&);
  explicit UnixSockStream(SOCKET_TYPE socket);

  UnixSockStream& operator=(const UnixSockStream& socket);

  SOCKET_TYPE _connecting_socket;
  StreamSockBuf & stream_sockbuf;

public:
  UnixSockStream();
  explicit UnixSockStream(const std::string& address, bool nonblock = false);
  UnixSockStream(const std::string& address, unsigned int milliseconds);
  virtual ~UnixSockStream();

  void open(const std::string& address, bool nonblock = false);
  void open(const std::string& address, unsigned int milliseconds) {
    open(address, true);
    if(!isReady(milliseconds)) {
      close();
      fail();
    }
  }

  virtual void close();
  virtual SOCKET_TYPE getSocket() const;

  bool isReady(unsigned int milliseconds = 0);
};

#endif // SKSTREAM_UNIX_H_
