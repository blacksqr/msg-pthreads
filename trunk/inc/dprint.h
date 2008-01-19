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
