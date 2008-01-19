// $Id$
#ifndef ITS_SQLITE_H
#define ITS_SQLITE_H

#include <Thread.h>
#include <sqlite3.h>

// - SQLITE_OK          0   // Successful result
// - SQLITE_ERROR       1   // SQL error or missing database
// - SQLITE_INTERNAL    2   // An internal logic error in SQLite
// - SQLITE_PERM        3   // Access permission denied
// - SQLITE_ABORT       4   // Callback routine requested an abort
// - SQLITE_BUSY        5   // The database file is locked
// - SQLITE_LOCKED      6   // A table in the database is locked
// - SQLITE_NOMEM       7   // A malloc() failed
// - SQLITE_READONLY    8   // Attempt to write a readonly database
// - SQLITE_INTERRUPT   9   // Operation terminated by sqlite_interrupt()
// - SQLITE_IOERR      10   // Some kind of disk I/O error occurred
// - SQLITE_CORRUPT    11   // The database disk image is malformed
// - SQLITE_NOTFOUND   12   // (Internal Only) Table or record not found
// - SQLITE_FULL       13   // Insertion failed because database is full
// - SQLITE_CANTOPEN   14   // Unable to open the database file
// - SQLITE_PROTOCOL   15   // Database lock protocol error
// - SQLITE_EMPTY      16   // (Internal Only) Database table is empty
// - SQLITE_SCHEMA     17   // The database schema changed
// - SQLITE_TOOBIG     18   // Too much data for one row of a table
// - SQLITE_CONSTRAINT 19   // Abort due to contraint violation
// - SQLITE_MISMATCH   20   // Data type mismatch
// - SQLITE_MISUSE     21   // Library used incorrectly
// - SQLITE_NOLFS      22   // Uses OS features not supported on host
// - SQLITE_AUTH       23   // Authorization denied
// - SQLITE_ROW        100  // sqlite_step() has another row ready
// - SQLITE_DONE       101  // sqlite_step() has finished executing

// - SQLITE_UTF8     1
// - SQLITE_UTF16    2
// - SQLITE_UTF16BE  3
// - SQLITE_UTF16LE  4
// - SQLITE_ANY      5

typedef void (*fClean)(void*);
typedef void (*sqlFunc)(sqlite3_context*,int,sqlite3_value**);

typedef sqlite3* pSqlite;
typedef sqlite3_stmt* HSqlres;
class CSQL;

extern "C" {
  void _xFunc_(sqlite3_context*,int,sqlite3_value**);
  void _xStep_(sqlite3_context*,int,sqlite3_value**);
  void _xFinal_(sqlite3_context*);
  // SQLite DB interface
  int busyHndl(void* p,int k);
}

// user defined function base class
class CCstFunc {
  friend void _xFunc_(sqlite3_context*,int,sqlite3_value**);
  friend void _xStep_(sqlite3_context*,int,sqlite3_value**);
  friend void _xFinal_(sqlite3_context*);
 protected:
  sqlite3_context* Cnt;
  sqlFunc c_xFunc,c_xStep;
  void (*c_xFinal)(sqlite3_context*);
  virtual void xFunc(int,sqlite3_value**) = 0;
  virtual void xStep(int,sqlite3_value**) = 0;
  virtual void xFinal() = 0;
  // Result
  fClean fncVal,fncTxt,fncBlb;
  void result() { sqlite3_result_null(Cnt); }
  void result(int x) { sqlite3_result_int(Cnt,x); }
  void result(lLong x) { sqlite3_result_int64(Cnt,x); }
  void result(double x) { sqlite3_result_double(Cnt,x); }
  void result(const void* x,int i) { sqlite3_result_blob(Cnt,x,i,fncBlb); }
  void result(const char* x, int i) { sqlite3_result_text(Cnt,x,i,fncTxt); }
  void result(sqlite3_value* x) { sqlite3_result_value(Cnt,x); }
 public:
  CCstFunc();
  virtual ~CCstFunc() {}
  void setClean(fClean v,fClean t,fClean b) { fncVal = v; fncTxt = t; fncBlb = b; }
  short init(pSqlite db,const char* nn,int nArg,int eTRep=SQLITE_UTF8) {
    return sqlite3_create_function(db,nn,nArg,eTRep,(void*)this,c_xFunc,c_xStep,c_xFinal);
  }
  void setCntxt(sqlite3_context* p) { Cnt = p; }
  void setScalar() { c_xStep = NULL; c_xFinal = NULL; }
  void setAgregate() { c_xFunc = NULL; }
};

extern "C" int _xComp_(void* pp,int i1,const void* p1,int i2,const void* p2);

// user defined collating base class
class CCstCollation {
  friend int _xComp_(void* pp,int i1,const void* p1,int i2,const void* p2);
 protected:
  virtual int comp(int i1,const void* p1,int i2,const void* p2) = 0;
 public:
  CCstCollation(pSqlite db,const char* nn,int eTRep=SQLITE_UTF8) {
    sqlite3_create_collation(db,nn,eTRep,(void*)this,_xComp_);
  }
  virtual ~CCstCollation() {}
};

//////////////////////////////////////
// Query interface
//////////////////////////////////////

// base data types
//SQLITE_INTEGER  1
//SQLITE_FLOAT    2
//SQLITE_TEXT     3
//SQLITE_BLOB     4
//SQLITE_NULL     5

// ohne Result Set
class CSqlStmt {
  friend class CSQL;
 protected:
  HSqlres hSql;
  fClean fncTxt,fncBlb;
 public:
  CSqlStmt(const char *sql, char& flag);
  virtual ~CSqlStmt() { (void)sqlite3_finalize(hSql); }
  uChar exec();
  // reset - allow to rebind params
  uChar reset() { return (uChar)sqlite3_reset(hSql); }
  // for special cleaning procedure
  void setClean(fClean t,fClean b) { fncTxt = t; fncBlb = b; }
  HSqlres get() { return hSql; }
  // binding query parameters
  uChar bind(char x,cuChar k)   { return (uChar)sqlite3_bind_int64(hSql,(int)k,(lLong)x); }
  uChar bind(uChar x,cuChar k)  { return (uChar)sqlite3_bind_int64(hSql,(int)k,(lLong)x); }
  uChar bind(short x,cuChar k)  { return (uChar)sqlite3_bind_int64(hSql,(int)k,(lLong)x); }
  uChar bind(uShort x,cuChar k) { return (uChar)sqlite3_bind_int64(hSql,(int)k,(lLong)x); }
  uChar bind(int x,cuChar k)    { return (uChar)sqlite3_bind_int64(hSql,(int)k,x); }
  uChar bind(uInt x,cuChar k)   { return (uChar)sqlite3_bind_int64(hSql,(int)k,(lLong)x); }
  uChar bind(lLong x,cuChar k)  { return (uChar)sqlite3_bind_int64(hSql,(int)k,x); }
  uChar bind(float x,cuChar k)  { return (uChar)sqlite3_bind_double(hSql,(int)k,(double)x); }
  uChar bind(double x,cuChar k) { return (uChar)sqlite3_bind_double(hSql,(int)k,x); }
  uChar bind(cuChar k)          { return (uChar)sqlite3_bind_null(hSql,(int)k);} // NULL binding
  // string / var-char
  uChar bind(const char* x,short l,cuChar k)
    { return (uChar)sqlite3_bind_text(hSql,(int)k,x,(int)l,fncTxt); }
  // blob - binary data
  uChar bind(const void* x,short l,cuChar k)
    { return (uChar)sqlite3_bind_blob(hSql,(int)k,x,(int)l,fncBlb); }
};

// with Result Set (-SELECT-)
class CSqlSStmt: public CSqlStmt {
 public:
  CSqlSStmt(const char *sql, char& flag): CSqlStmt(sql,flag) {}
  virtual ~CSqlSStmt() {}
  // result interface
  uChar nCols() { return sqlite3_column_count(hSql); }
  uChar nData() { return sqlite3_data_count(hSql); }
  uShort blLen(cuChar k) { return (uShort)sqlite3_column_bytes(hSql,(int)k); }
  char cType(cuChar k) { return sqlite3_column_type(hSql,(int)k); }
  const char* cName(cuChar k) { return sqlite3_column_name(hSql,(int)k); }
  // result collum's
  void clmn(char& pp,cuChar k)   { pp = (char)sqlite3_column_int(hSql,(int)k); }
  void clmn(uChar& pp,cuChar k)  { pp = (uChar)sqlite3_column_int(hSql,(int)k); }
  void clmn(short& pp,cuChar k)  { pp = (short)sqlite3_column_int(hSql,(int)k); }
  void clmn(uShort& pp,cuChar k) { pp = (uShort)sqlite3_column_int(hSql,(int)k); }
  void clmn(int& pp,cuChar k)    { pp = sqlite3_column_int(hSql,(int)k); }
  void clmn(uInt& pp,cuChar k)   { pp = (uInt)sqlite3_column_int64(hSql,(int)k); }
  void clmn(lLong& pp,cuChar k)  { pp = sqlite3_column_int64(hSql,(int)k); }
  void clmn(double& pp,cuChar k) { pp = sqlite3_column_double(hSql,(int)k); }
  void clmn(cuChar** pp,cuChar k)     { *pp = sqlite3_column_text(hSql,(int)k); }
  void clmn(const void** pp,cuChar k) { *pp = sqlite3_column_blob(hSql,(int)k); }
};

// Wrapper for CSqlStmt.reset()
class CSqlStClean {
 private:
  CSqlStmt* St;
 public:
  CSqlStClean(CSqlStmt* s): St(s) {}
  ~CSqlStClean() { St->reset(); }
  CSqlStmt* operator () () { return St; }
};

// Transaction interface
class CSqlTrans {
 protected:
  char flg;
 public:
  CSqlTrans(): flg('\0') {}
  ~CSqlTrans();
  char run();
  void ok() { flg = 'x'; }
};

// Base class for threads using SQLite
class  CDbThread: public CThreadObj {
 protected:
  pSqlite DB;
 public:
  CDbThread(): CThreadObj('x') {}
  ~CDbThread() { sqlite3_close(DB); }
  uChar dbOpen(const char* d);
  lLong rowId() { return sqlite3_last_insert_rowid(DB); }
  int error() { return sqlite3_errcode(DB); }
  pSqlite& sql() { return DB; }
  virtual void* go() = 0;
};

#endif // ITS_SQLITE_H
// $Log$
