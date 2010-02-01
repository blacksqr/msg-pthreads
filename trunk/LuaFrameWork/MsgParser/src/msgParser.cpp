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

#include <sys/types.h>
#include <netinet/in.h>

#include <msgParser.h>

namespace MsgModel {

#ifdef DIFF_HARDWARE

uInt8 htonll(uInt8 ll) {
  uInt2  x = 1;
  if(*(uInt1*)&x == 1) {
    /* little endian */
    uInt4* p = &ll;
    *p = htonl(*p);
    p[1] = htonl(p[1]);
    return ll;
  } else
    return ll;
}

uInt1  n2h(uInt1 v)  { return v; }
Float4 n2h(Float4 v) { return v; }
Float8 n2h(Float8 v) { return v; }

uInt2 n2h(uInt2 v)   { return ntohs(v); }
uInt4 n2h(uInt4 v)   { return ntohl(v); }
uInt8 n2h(uInt8 v)   { return htonll(v); }

/////////////////////////////////////////

uInt1  h2n(uInt1 v)  { return v; }
Float4 h2n(Float4 v) { return v; }
Float8 h2n(Float8 v) { return v; }

uInt2 h2n(uInt2 v)   { return htons(v); }
uInt4 h2n(uInt4 v)   { return htonl(v); }
uInt8 h2n(uInt8 v)   { return htonll(v); }

#endif // DIFF_HARDWARE

#ifdef USE_DUMP

CRegExp::CRegExp(const char* expr,uInt1 x):
  pmatch(NULL),str(NULL),ss(x)
{
  int rr = regcomp(&re, expr, REG_EXTENDED);
  //int rr = regcomp(&re, expr, 0);
  if(rr) {
    char ebf[0x100];
    regerror(rr, &re, &ebf[0], sizeof(ebf)); 
    printf("Error: regcomp <%s>\n",ebf);
  } else {
    //printf("*** RegComp <%s>\n",expr);
    pmatch = new regmatch_t[ss];
  }
}

CRegExp::~CRegExp() {
  delete [] pmatch;
  regfree(&re);
}

int CRegExp::exe(char* s) {
  int rr = regexec(&re, s, (size_t)ss, pmatch,0);
  str = s;
  //if(rr) printf("**** NOT matched <<<%s>>>\n",s);
  return rr;
}

char* CRegExp::operator () (uInt1 x, uInt2* o) {
  if(o) *o = pmatch[x].rm_eo;
  return (pmatch[x].rm_so != -1) ? str + pmatch[x].rm_so : NULL;
}

char* CRegExp::end(uInt1 x) {
  return str + pmatch[x].rm_eo;
}

void a2i(uInt1& v, char* p)  { v = (uInt1)atoi(p); }
void a2i(uInt2& v, char* p)  { v = (uInt2)atoi(p); }
void a2i(uInt4& v, char* p)  {  v = (uInt4)atoll(p); }
void a2i(uInt8& v, char* p)  {  v = (uInt8)atoll(p); }

void a2i(sInt1& v, char* p)  {  v = (sInt1)atoi(p); }
void a2i(sInt2& v, char* p)  {  v = (sInt2)atoi(p); }
void a2i(sInt4& v, char* p)  {  v = (sInt4)atoi(p); }
void a2i(sInt8& v, char* p)  {  v = atoll(p); }

void a2f(Float4& v, char* p) { v = atof(p); }
void a2f(Float8& v, char* p) { v = atof(p); }

char* setDefOffset_(uInt1 of) {
  static char tmp_[0x20];
  (char*)memset(tmp_,' ',of); tmp_[of] = '\0';
  return tmp_;
}
#endif // USE_DUMP

} // namespace MsgModel

// $Id: msgParser.cpp 342 2010-01-25 17:28:36Z asus $
