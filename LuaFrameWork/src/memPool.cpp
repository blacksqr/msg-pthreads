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
#include <dprint.h>
#include <memPool.h>

SLink* CMPool::grow() {
  const int chunk_size = 0x800 * mult;
  const int nelem      = (chunk_size / esize) - 1;
  DBG(">CMPool::grow %u> nEl=%d Sz=%d\n",esize,nelem,chunk_size);
  // Alloc chunk memory
  SLink *pp, *p = pp = (SLink*)malloc(chunk_size);
  // format chunk memory
  for(int k=0; k<nelem; ++k)
    pp = (*pp).nxt = (SLink*)((char*)pp + esize);
  (*pp).nxt = NULL; // last chunk
  return p;
}

void* CMPool::pAlloc() {
  CSglLock sl(M);
  SLink* p = head ? head : grow();
  //DBG(">CMPool::pAlloc %u>\tp=0x%X head=0x%X<\n",esize,(int)head,(int)(*head).nxt);
  head = p->nxt;
  return (void*)p;
}

void CMPool::pFree(void* b) {
  CSglLock sl(M);
  //DBG(">CMPool::pFree %u>\tb=0x%X head=0x%X<===\n",esize,(int)b,(int)head);
  SLink* p = (SLink*)b;
  p->nxt = head;
  head = p;
}

void* operator new(size_t, void* bf) throw() { return bf; }
void operator delete(void* p, CMPool& pool)  { pool.pFree(p); }

// $Id: memPool.cpp 360 2010-03-27 13:25:05Z asus $
