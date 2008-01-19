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
#include <stdio.h>
#include <stdarg.h>
#include <strings.h>
#include <dprint.h>

const int  log_facility  = LOG_DAEMON;

static char* str_fac[] = {
  "LOG_AUTH",  "LOG_CRON",   "LOG_DAEMON",
  "LOG_KERN",  "LOG_LOCAL0", "LOG_LOCAL1",
  "LOG_LPR",   "LOG_MAIL",   "LOG_NEWS",
  "LOG_USER",  "LOG_UUCP",   "LOG_AUTHPRIV",
  "LOG_FTP",   "LOG_SYSLOG",
  0
};

static int int_fac[] = {
  LOG_AUTH,  LOG_CRON,   LOG_DAEMON,
  LOG_KERN,  LOG_LOCAL0, LOG_LOCAL1,
  LOG_LPR,   LOG_MAIL,   LOG_NEWS,
  LOG_USER,  LOG_UUCP,   LOG_AUTHPRIV,
  LOG_FTP,   LOG_SYSLOG
};

void dprint(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  vfprintf(stderr,format,ap);
  fflush(stderr);
  va_end(ap);
}

int str2facility(const char* s) {
  int i = 0;
  for(i=0; str_fac[i] ; i++) {
    if (!strcasecmp(s,str_fac[i]))
      return int_fac[i];
  }
  return -1;
}
