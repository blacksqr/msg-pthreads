/* The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 * The Initial Developer of the Original Code is Alex Goldenstein.
 * All Rights Reserved.
 * Contributor(s): Alex Goldenstein.<goldale.de@googlemail.com>
 */
#include <time.h>
#include <pMutex.h>
#include <getTime.h>
#include <dprint.h>

typedef unsigned int  uInt;
typedef unsigned long uLong;

int CPCond::twait(uLong ms) {
  struct timeval x;
  struct timespec to;
  tOutAbs(ms,&x);
  to.tv_nsec = 1000 * x.tv_usec; // nano-sec
  to.tv_sec = x.tv_sec;
  return pthread_cond_timedwait(&Cond, &Mut, &to);
}

// $Id: pMutex.cpp 299 2010-01-09 22:19:52Z asus $
