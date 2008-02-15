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
#ifndef BDB_CNTXT_H
#define BDB_CNTXT_H

// ==================================
// N -readers; ONE -writer concept
// ==================================

#include <db.h>
#include <Context.h>

// Berkeley DB - context type
#define C_CTXT_BDB 0x07

class CBDbTrns {
  DB_TXN* pTxnp;
  char flg;
  // disable copy constructor.
  CBDbTrns(const CBDbTrns&);
  void operator = (const CBDbTrns&);
 public:
  CBDbTrns(DB_ENV* p);
  CBDbTrns(DB *p);
  ~CBDbTrns();
  void ok() { flg = 'x'; }
  DB_TXN* operator () () { return pTxnp; }
};

// ==============================================

// Cursor
//  DB_TXN_SNAPSHOT DB_MULTIVERSION
class CBDbCrsr {
  friend class CBDbTbl;
 protected:
  DBC* pCrsr;
  CBDbCrsr(DBC* p): pCrsr(p) {}
  // disable copy constructor.
  CBDbCrsr(const CBDbCrsr&);
  void operator = (const CBDbCrsr&);
 public:
  ~CBDbCrsr() { pCrsr->close(pCrsr); }
  int count(uInt flg);
  int del(uInt flg) { return pCrsr->del(pCrsr, flg); }
  //get()   int DBcursor->get(DBC *DBcursor, DBT *key, DBT *data, u_int32_t flags);
  //put()   int DBcursor->put(DBC *DBcursor, DBT *key, DBT *data, u_int32_t flags); ???
};

// ==============================================
// Berkeley DB (contain one o more tables)

inline void dbtCln(DBT* x, char f = '\0') {
  x->data = NULL; x->size = 0u;
  x->flags = f ? DB_DBT_USERMEM : DB_DBT_MALLOC;
}

inline void dbtSet(DBT &x, void* d, size_t s) {
  x.data = d;
  x.size = s;
  x.flags = 0;
}

class CBDbTbl {
  friend class CBDbCntx;
 protected:
  DB *pDb;
  CBDbTrns* T;
  DBT data;
  CBDbTbl(DB_ENV* pe, const char* dbFile, uInt pg = 0u);
  // disable copy constructor.
  CBDbTbl(const CBDbTbl&);
  void operator = (const CBDbTbl&);
 public:
  ~CBDbTbl() { (void)pDb->close(pDb, 0); }
  int get(void* k, size_t s);
  int put(void* k, size_t s);
  int del(void* k, size_t s);
  CBDbCrsr* Cursor(uInt Flg=DB_TXN_SNAPSHOT);
  const DBT* rec() { return &data; }
};

// ==============================================

// Berkeley DB - context
extern uInt iBDbCtxt;
extern "C" DB_ENV* glb_pDbEnv;

class CBDbCntx: public CContext {
 protected:
  DB_ENV* pDbEnv;
  // disable copy constructor.
  CBDbCntx(const CBDbCntx&);
  void operator = (const CBDbCntx&);
  public:
  CBDbCntx(uInt i, DB_ENV* p): CContext(i),pDbEnv(p) { iBDbCtxt = i; }
  CBDbCntx(uInt i, const char* homeDir, int chSize = 512);
  ~CBDbCntx();
  CBDbTbl* dbOpen(const char* dbFile, uInt pg = 0u) { return new CBDbTbl(pDbEnv, dbFile, pg); }
  // Write DB checkpoint
  int checkpoint(uInt kbyte, uInt min=0u, uInt flg=0u) {
    return pDbEnv->txn_checkpoint(pDbEnv, kbyte, min, 0u);
  }
  int Flags(uInt Flg, char OnOff = 'x') { return pDbEnv->set_flags(pDbEnv, Flg, (int)OnOff); }
  const DB_ENV* operator () () { return pDbEnv; }
};

#endif // BDB_CNTXT_H
