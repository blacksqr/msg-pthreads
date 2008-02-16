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

SLink* CMPool::grow(size_t esize,unsigned char mult) {
  const int overhead   = 14;
  const int chunk_size = (1024 * mult) - overhead;
  const int nelem      = chunk_size/esize - 1;
  // Alloc chunk memory
  SLink* pp;
  SLink* p = pp = (SLink*)malloc(chunk_size);
  LOG(L_INFO,"CMPool::grow %u nEl=%d Sz=%d\n",esize,nelem,chunk_size);
  // Setup chunk memory
  for(int k=0; k<nelem; ++k)
    pp = (*pp).nxt = (SLink*)((char*)pp + esize);
  (*pp).nxt = NULL; // last chunk
  return p;
}

void* CMPool::pAlloc() {
  CSglLock sl(M);
  if(!head)
    head = grow(esize,mult);
  SLink* p = head;
  //DBG(">CMPool::pAlloc %u>\t===    p=0x%X head=0x%X<\n",esize,(int)head,(int)(*head).nxt);
  head = (*head).nxt;
  return (void*)p;
}

void CMPool::pFree(void* b) {
  CSglLock sl(M);
  //DBG(">CMPool::pFree %u>\t===    b=0x%X head=0x%X<===\n",esize,(int)b,(int)head);
  SLink* p = (SLink*)b;
  p->nxt = head;
  head = p;
  //DBG(">CMPool::pFree %u>\t=== head=0x%X  nxt=0x%X<<<<\n",esize,(int)head,(int)(*head).nxt);
}

void* operator new(size_t, void* bf) throw() { return bf; }
void operator delete(void* p, CMPool& pool)  { pool.pFree(p); }
