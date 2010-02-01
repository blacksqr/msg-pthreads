/**
 * This software package has been extensively modified by members of the
 * Worldforge Project. See the file ChangeLog for details.
 *
 * $Id: skpoll.cpp,v 1.6 2006/10/12 01:10:12 alriddoch Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <skpoll.h>

BSockPoll::BSockPoll() : maxfd_(0) {
  FD_ZERO(&read_);
  FD_ZERO(&write_);
  FD_ZERO(&except_);
}

int BSockPoll::poll(const socket_map& map, unsigned long timeout) {
  FD_ZERO(&read_);
  FD_ZERO(&write_);
  FD_ZERO(&except_);
  maxfd_ = 0;

  for(socket_map::const_iterator I = map.begin(); I != map.end(); ++I) {
    SOCKET_TYPE socket;

    if(!(I->second & MASK) || !I->first || (socket = I->first->getSocket()) == INVALID_SOCKET)
      continue;

    if(I->second & READ)     FD_SET(socket, &read_);
    if(I->second & WRITE)    FD_SET(socket, &write_);
    if(I->second & EXCEPT)   FD_SET(socket, &except_);
    if(socket >= maxfd_)
      maxfd_ = socket + 1;
  }
  struct timeval timeout_val = {timeout/1000, timeout%1000};
  return ::select(maxfd_, &read_, &write_, &except_, &timeout_val);
}

BSockPoll::poll_type BSockPoll::isReady(const BasicSock* soc, poll_type mask) {
  SOCKET_TYPE socket;
  if(!(mask & MASK) || !soc || (socket = soc->getSocket()) == INVALID_SOCKET || socket >= maxfd_)
    return (poll_type) 0;

  unsigned result = 0;
  if((mask & READ) && FD_ISSET(socket, &read_))      result |= READ;
  if((mask & WRITE) && FD_ISSET(socket, &write_))    result |= WRITE;
  if((mask & EXCEPT) && FD_ISSET(socket, &except_))  result |= EXCEPT;

  return (poll_type) result;
}
