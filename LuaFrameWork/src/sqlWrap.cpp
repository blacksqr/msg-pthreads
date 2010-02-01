#include <string.h>
#include <stdlib.h>
#include <sqlWrap.h>

extern "C" {
  void _xFunc_(sqlite3_context* pc,int i,sqlite3_value** ppv) {
    CCstFunc* cntx = (CCstFunc*)sqlite3_user_data(pc);
    cntx->setCntxt(pc);
    cntx->xFunc(i,ppv);
  }
  void _xStep_(sqlite3_context* pc,int i,sqlite3_value** ppv) {
    CCstFunc* cntx = (CCstFunc*)sqlite3_user_data(pc);
    cntx->setCntxt(pc);
    cntx->xStep(i,ppv);
  }
  void _xFinal_(sqlite3_context* pc) {
    CCstFunc* cntx = (CCstFunc*)sqlite3_user_data(pc);
    cntx->setCntxt(pc);
    cntx->xFinal();
  }
  int busyHndl(void*,int) {
    return 0x1;
  }
  int _xComp_(void* pp,int i1,const void* p1,int i2,const void* p2) {
    CCstCollation* cl = (CCstCollation*)pp;
    return cl->comp(i1,p1,i2,p2);
  }
}

CCstFunc::CCstFunc() {
  fncVal = fncTxt = fncBlb = SQLITE_TRANSIENT;
  c_xFunc = _xFunc_;
  c_xStep = _xStep_;
  c_xFinal = _xFinal_;
}

//////////////////////////////////////
// Query interface
//////////////////////////////////////

CSqlStmt::CSqlStmt(const char *ss, char& flag):
  hSql(NULL),fncTxt(free),fncBlb(free)
{
  flag = 'x';
  fncTxt = fncBlb = SQLITE_TRANSIENT;
  pSqlite pdb = ((CDbThread*)CThreadObj::getSelf())->sql();
  if(pdb && (SQLITE_OK == sqlite3_prepare(pdb,ss,strlen(ss),&hSql,NULL)))
    flag = '\0';
}

uChar CSqlStmt::exec() {
  uChar rv = 'x';
  for(uChar k='\0'; k<3; ++k) {
    rv = (uChar)sqlite3_step(hSql);
    if(rv == SQLITE_BUSY) {
      yield();
    } else
      break;
  }
  return rv;
}

//////////////////////////////////////
// db Transaction
//////////////////////////////////////

CSqlTrans::~CSqlTrans() {
  const char* sql = flg ? "COMMIT" : "ROLLBACK";
  pSqlite pdb = ((CDbThread*)CThreadObj::getSelf())->sql();
  (void)sqlite3_exec(pdb,sql,NULL,NULL,NULL);
}

char CSqlTrans::run() {
  pSqlite pdb = ((CDbThread*)CThreadObj::getSelf())->sql();
  return (sqlite3_exec(pdb,"BEGIN",NULL,NULL,NULL) == SQLITE_OK) ? '\0' : 'x';
}

//////////////////////////////////////
// db thread
//////////////////////////////////////

uChar CDbThread::dbOpen(const char* d) {
  uChar rr = (uChar)sqlite3_open(d,&DB);
  if(rr == SQLITE_OK) {
    rr = (uChar)sqlite3_busy_handler(DB,busyHndl,(void*)this);
    if(rr == SQLITE_OK)
      rr = (uChar)sqlite3_busy_timeout(DB,777);
  }
  return rr;
}

// $Id: sqlWrap.cpp 299 2010-01-09 22:19:52Z asus $
