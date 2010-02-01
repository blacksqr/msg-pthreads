/* This software package has been extensively modified by members of the
 * Worldforge Project. See the file ChangeLog for details.
 *
 * $Id: skpoll.h,v 1.5 2006/10/12 01:10:12 alriddoch Exp $
 */
#ifndef SOCKET_POLL_H_
#define SOCKET_POLL_H_

#include <map>
#include <skstream.h>

class BSockPoll {
  BSockPoll(const BSockPoll&);
  BSockPoll& operator=(const BSockPoll&);

private:
  fd_set read_, write_, except_;
  SOCKET_TYPE maxfd_;

public:
  BSockPoll();

  enum poll_type {
    READ   = 1,
    WRITE  = 2,
    EXCEPT = 4,
    MASK   = 7
  };

  typedef std::map<const BasicSock*,poll_type> socket_map;

  int poll(const socket_map& sockets, unsigned long timeout = 0);

  poll_type isReady(const BasicSock* soc, poll_type mask = MASK);
  poll_type isReady(const socket_map::value_type& val) { return isReady(val.first, val.second); }
  poll_type isReady(const socket_map::iterator& I) { return isReady(I->first, I->second); }
  poll_type isReady(const socket_map::const_iterator& I) { return isReady(I->first, I->second); }
  poll_type isReady(const socket_map::reverse_iterator& I) { return isReady(I->first, I->second); }
  poll_type isReady(const socket_map::const_reverse_iterator& I) { return isReady(I->first, I->second); }
};

#endif // SOCKET_POLL_H_
