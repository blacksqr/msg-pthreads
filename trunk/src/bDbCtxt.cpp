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
  (void)p->txn_begin(p, NULL, &txnp, 0);
}

CBDbTrns::CBDbTrns(DB *p) {
  flg = '\0';
  DB_ENV* pe = p->get_env(p);
  (void)pe->txn_begin(pe, NULL, &txnp, 0);
}

CBDbTrns::~CBDbTrns() {
  if((flg == 'x') && txnp) {
    (void)txnp->commit(txnp, 0);
    return;
  }
  (void)txnp->abort(txnp);
}

// ==============================================

CBDbTbl::CBDbTbl(DB_ENV* pe, const char* dbFile, uInt pg) {
  (void)db_create(&pDb, pe, 0);
  if(pg) (void)pDb->set_pagesize(pDb, pg);
  {
    CBDbTrns T(pe);
    uInt flg = DB_CREATE | DB_THREAD;
    int rr = pDb->open(pDb, T(), dbFile, NULL, DB_BTREE, flg, 0664);
    if(!rr) T.ok();
  }
}

int CBDbTbl::get(void* k, size_t s) {
  DBT key;
  /* DBT's must use local memory or malloc'd memory if
   * the DB handle is accessed in a threaded fashion. */
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  key.data = k;
  key.size = s;
  data.flags = DB_DBT_MALLOC;
  return pDb->get(pDb, NULL, &key, &data, 0);
}

int CBDbTbl::put(void* k, size_t s) {
  DBT key;
  /* DBT's must use local memory or malloc'd memory if
   * the DB handle is accessed in a threaded fashion. */
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  key.data = k;
  key.size = s;
  data.flags = DB_DBT_USERMEM;
  {
    CBDbTrns T(pDb);
    int rr = pDb->put(pDb, T(), &key, &data, 0);
    if(!rr) T.ok();
    return rr;
  }
}

int CBDbTbl::del(void* k, size_t s) {
  DBT key;
  /* DBT's must use local memory or malloc'd memory if
   * the DB handle is accessed in a threaded fashion. */
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  key.data = k;
  key.size = s;
  {
    CBDbTrns T(pDb);
    int rr = pDb->put(pDb, T(), &key, &data, 0);
    if(!rr) T.ok();
    return rr;
  }
}

// ==============================================

CBDbCntx::CBDbCntx(uInt i, const char* homeDir, int chSize): CContext(i) {
  uInt flg = DB_CREATE | DB_INIT_CDB | DB_INIT_LOG |
    DB_INIT_MPOOL | DB_INIT_TXN | DB_THREAD;
  iBDbCtxt = i;
  (void)db_env_create(&pDbEnv, 0);
  pDbEnv->set_errfile(pDbEnv, stderr);
  pDbEnv->set_errpfx(pDbEnv, "its");
  (void)pDbEnv->set_cachesize(pDbEnv, 0, 1024*chSize, 0);
  (void)pDbEnv->set_lg_max(pDbEnv, 200000);
  if(pDbEnv->open(pDbEnv, homeDir, flg, 0))
    (void)pDbEnv->close(pDbEnv, 0);
}

CBDbCntx::~CBDbCntx() {
  iBDbCtxt = 0u;
  (void)pDbEnv->close(pDbEnv, 0u);
  DBG("CBDbCntx::~CBDbCntx cId=%u\n",cId);
}
