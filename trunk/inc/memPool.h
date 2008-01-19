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
  CMPool(size_t sz,unsigned char m=0x2): head(NULL),esize(sz),mult(m) {}
  ~CMPool() {}
  void* pAlloc();
  void  pFree(void* b);
};
// Placement operator new
extern void* operator new(size_t, void* bf) throw();
// Placement operator delete
extern void operator delete(void* p, CMPool& pool);

#endif // MEM_POOL_H
