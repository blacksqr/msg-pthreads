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
#ifndef dprint_h
#define dprint_h
#include <syslog.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */
  int str2facility(const char *s);
  void dprint (const char* format, ...);
#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

extern char dbgLevel;
extern const int  log_facility;

#define DAEMON_MODE    0x40
#define LOG_IN_STDERR  0x80
extern unsigned char GlAppsFlag;

#define L_ALERT -3
#define L_CRIT  -2
#define L_ERR   -1
#define L_WARN   1
#define L_NOTICE 2
#define L_INFO   3
#define L_DBG    4

#define D_OUT(lev, fmt, args...) { \
    if(dbgLevel >= (lev)) { \
        if(GlAppsFlag & LOG_IN_STDERR) { \
          dprint (fmt, ##args); \
        } else { \
          switch(lev) { \
            case L_CRIT:   syslog(LOG_CRIT    | log_facility, fmt, ##args); break; \
            case L_ALERT:  syslog(LOG_ALERT   | log_facility, fmt, ##args); break; \
            case L_ERR:    syslog(LOG_ERR     | log_facility, fmt, ##args); break; \
            case L_WARN:   syslog(LOG_WARNING | log_facility, fmt, ##args); break; \
            case L_NOTICE: syslog(LOG_NOTICE  | log_facility, fmt, ##args); break; \
            case L_INFO:   syslog(LOG_INFO    | log_facility, fmt, ##args); break; \
            case L_DBG:    syslog(LOG_DEBUG   | log_facility, fmt, ##args); break; \
    }  }  }  }

#ifdef NO_LOG
#define LOG(lev, fmt, args...)
#else // NO_LOG
#define LOG(lev, fmt, args...) D_OUT(lev, fmt, ##args)
#endif // NO_LOG

#ifdef NO_DEBUG
#define DBG(fmt, args...)
#else
#define DBG(fmt, args...) D_OUT(L_DBG, fmt, ##args)
#endif // NO_DEBUG

#endif // dprint_h
