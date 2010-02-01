/**
 * This software package has been extensively modified by members of the
 * Worldforge Project. See the file ChangeLog for details.
 *
 * $Id: skstreamconfig.h.in,v 1.5 2005/09/15 10:38:56 alriddoch Exp $
 *
 */
#ifndef SOCKET_CONFIG_H_
#define SOCKET_CONFIG_H_

#include <unistd.h>
#include <sys/time.h>

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef socklen_t SOCKLEN;
typedef int SOCKET_TYPE;

#define SOCKET_BLOCK_ERROR EINPROGRESS

#endif // SOCKET_CONFIG_H_
