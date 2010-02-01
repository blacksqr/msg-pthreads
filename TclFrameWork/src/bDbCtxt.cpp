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

// ==================================
// N -readers; ONE -writer concept
// ==================================

#include <bDbCtxt.h>

// Berkeley DB - context
uInt iBDbCtxt;

CBDbTrns::CBDbTrns(DB_ENV* p) {
  flg = '\0';
  (void)p->txn_begin(p, NULL, &pTxnp, 0);
}

CBDbTrns::CBDbTrns(DB *p) {
  flg = '\0';
  DB_ENV* pe = p->get_env(p);
  (void)pe->txn_begin(pe, NULL, &pTxnp, 0);
}

CBDbTrns::~CBDbTrns() {
  if((flg == 'x') && pTxnp) {
    (void)pTxnp->commit(pTxnp, 0);
    return;
  }
  (void)pTxnp->abort(pTxnp);
}

// ==============================================

int CBDbCrsr::count(uInt flg) {
  db_recno_t countp = 0;
  if(!pCrsr->count(pCrsr, &countp, flg))
    return (int)countp;
  return -1;
}

// ==============================================

CBDbTbl::CBDbTbl(DB_ENV* pe, const char* dbFile, uInt pg) {
  (void)db_create(&pDb, pe, 0);
  if(pg) (void)(pDb->set_pagesize(pDb, pg));
  {
    CBDbTrns T(pe);
    uInt flg = DB_CREATE | DB_THREAD | DB_MULTIVERSION;
    int rr = pDb->open(pDb, T(), dbFile, NULL, DB_BTREE, flg, 0664);
    if(!rr) T.ok();
  }
}

int CBDbTbl::get(void* k, size_t s) {
  DBT key;
  /* DBT's must use local memory or malloc'd memory if
   * the DB handle is accessed in a threaded fashion. */
  dbtCln(&data);
  dbtSet(key,k,s);
  return pDb->get(pDb, NULL, &key, &data, 0);
}

int CBDbTbl::put(void* k, size_t s) {
  DBT key;
  /* DBT's must use local memory or malloc'd memory if
   * the DB handle is accessed in a threaded fashion. */
  dbtCln(&data);
  dbtSet(key,k,s);
  return pDb->put(pDb, T ? (*T)() : NULL, &key, &data, 0);
}

int CBDbTbl::del(void* k, size_t s) {
  DBT key;
  /* DBT's must use local memory or malloc'd memory if
   * the DB handle is accessed in a threaded fashion. */
  dbtSet(key,k,s);
  return pDb->del(pDb, T ? (*T)() : NULL, &key, 0);
}

CBDbCrsr* CBDbTbl::Cursor(uInt Flg) {
  DBC* pCrsr = NULL;
  if(!pDb->cursor(pDb, T ? (*T)() : NULL, &pCrsr, Flg) && pCrsr)
    return new CBDbCrsr(pCrsr);
  return NULL;
}

// ==============================================

const uInt bDbFlag = DB_CREATE | DB_INIT_CDB | DB_INIT_LOG |
DB_INIT_MPOOL | DB_INIT_TXN | DB_THREAD | DB_REGISTER;

CBDbCntx::CBDbCntx(uInt i, const char* homeDir, int chSize): CContext(i) {
  iBDbCtxt = i;
  (void)db_env_create(&pDbEnv, 0);
  pDbEnv->set_errfile(pDbEnv, stderr);
  pDbEnv->set_errpfx(pDbEnv, "its");
  (void)pDbEnv->set_cachesize(pDbEnv, 0, 1024*chSize, 0);
  (void)pDbEnv->set_lg_max(pDbEnv, 200000);
  if(!pDbEnv->open(pDbEnv, homeDir, bDbFlag, 0)) {
    glb_pDbEnv = pDbEnv;
    return;
  }
  (void)pDbEnv->close(pDbEnv, 0);
}

CBDbCntx::~CBDbCntx() {
  iBDbCtxt = 0u;
  (void)pDbEnv->close(pDbEnv, 0u);
  DBG("CBDbCntx::~CBDbCntx cId=%u\n",cId);
}
