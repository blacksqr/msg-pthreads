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
#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <stdlib.h>
#include <pMutex.h>

typedef struct s_Link {
  struct s_Link* nxt;
} SLink;

// It must be a static object
class CMPool {
  CPMutex M;
  SLink* head;
#ifndef NO_DEBUG
#endif // NO_DEBUG
  const unsigned short esize;
  const unsigned char  mult;
  static SLink* grow(size_t sz,unsigned char m);
  // disable copy constructor.
  CMPool(const CMPool&);
  void operator = (const CMPool&);
 public:
#ifdef PROC_WORD32
  CMPool(size_t sz,unsigned char m=0x2):
    head(NULL),esize((sz%4) ? 4u*(1u+sz/4u) : sz),mult(m) {}
#else
  CMPool(size_t sz,unsigned char m=0x2):
    head(NULL),esize((sz%8) ? 8u*(1u+sz/8u) : sz),mult(m) {}
#endif
  ~CMPool() {}
  void* pAlloc();
  void  pFree(void* b);
};

// Placement operator new
extern void* operator new(size_t, void* bf) throw();
// Placement operator delete
extern void operator delete(void* p, CMPool& pool);

#endif // MEM_POOL_H
