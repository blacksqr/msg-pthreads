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
#ifdef REG_EXP_H
#define REG_EXP_H

#include </usr/include/regex.h>

// FegExpression wrapper
class CRegExp {
 protected:
  regex_t reg;
 public:
  CRegExp(const char* pat=NULL) {
    if(pat)
      (void)init(pat);
  }
  ~CRegExp() {
    regfree(&reg);
  }
  int init(const char* pat) const {
    return regcomp((regex_t*)&reg,pat,REG_EXTENDED);
  }
  int exec(const char* str,regmatch_t* pMatch=NULL) {
    return regexec(&reg,str,(pMatch ? 0x1 : 0x0),pMatch,0x0);
  }
};

#endif // REG_EXP_H
