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
  // disable copy constructor.
  CMPool(const CMPool&);
  void operator = (const CMPool&);

 protected:
  CPMutex M;
  SLink*  head;
  const unsigned short esize;
  const unsigned char  mult;

  SLink* grow();
 public:
  CMPool(size_t sz,unsigned char m=0x8): head(NULL),esize(sz),mult(m) {}
  ~CMPool() {}
  void* pAlloc();
  void  pFree(void* b);
};

// Placement operator new & delete
extern void* operator new(size_t, void* bf) throw();
extern void operator delete(void* p, CMPool& pool);

#endif // MEM_POOL_H

// $Id: memPool.h 359 2010-03-20 21:11:12Z asus $
