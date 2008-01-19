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
