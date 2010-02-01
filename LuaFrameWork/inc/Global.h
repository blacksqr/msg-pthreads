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
#ifndef INC_GLOBAL_H
#define INC_GLOBAL_H

#include <fcntl.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/resource.h>
//#include <signal.h>

// typedef's
typedef char*    pChar;
typedef void*    pVoid;
typedef unsigned char  uChar;
typedef unsigned short uShort;
typedef unsigned int   uInt;
typedef unsigned long  uLong;
typedef long     long  lLong;
typedef const    uChar cuChar;
typedef unsigned long  long ulLong;

extern int errno;
extern const int log_facility;

// max number of working threads
#define MAX_N_WTHREAD    48

// Contetxt State-Automate ================
#define Stat_NoStat      0x00
#define Stat_Idle        0x01
#define Stat_WaitPRS0    0x02   // wait prosody channel
#define Stat_Online      0x03   // Online

// HK-Context Errors-id ===================
#define hk_wTclFail      0x01

// Context Events =========================
#define Evnt_DelCtxt     0x01   // Event to remove context
#define Evnt_PreDelCtxt  0x02   // Event to remove context
#define Evnt_HsKeepCtxt  0x03   // Hause keep problem in some context
#define Evnt_SThAlive    0x04   // signaling thread keepAlive event
#define Evnt_EndSigTh    0x05   // Term signaling thread
#define Evnt_wThrdEnd    0x06   // Term work thread L-Script
#define Evnt_Que12Full   0x07   // Application queue 50% full
// End of API internal events
#define Evnt_TstCntxt    0x09   // event to create test Context
#define Evnt_TstSeqw     0x0A   // event from test sign thread
#define Evnt_SaveData    0x0B   // To store data in Cash
#define Evnt_DbReady     0x0C   // Data stored in DB
#define Evnt_StoreInDb   0x0D   // Store new set of data in DB

#define Evnt_newFsm      0x0E   // Setup new FSM
#define Evnt_fsmEv1      0x0F   // FSM event 1
#define Evnt_fsmEv2      0x10   // FSM event 2
#define Evnt_fsmEv3      0x11   // FSM event 3
#define Evnt_fsmEv4      0x12   // FSM event 4
#define Evnt_fsmEv5      0x13   // FSM event 5
#define Evnt_fsmEv6      0x14   // FSM event 6



#define Evnt_sipCtxt     0x31   // New parser context
#define Evnt_sipMsgRq    0x32   // new SIP Request
#define Evnt_sipMsgRp    0x33   // new SIP Replay

#define Evnt_NewCall     0x34   // icoming call
#define Evnt_Idle        0x35   // call closed
#define Evnt_Connct      0x36   // call online

// TimeOut ID's ===========================
#define TOut_NoTmOut     0x00
#define TOut_HsKeep      0x01   // System Check
#define TOut_sigThAlrm   0x02   // signal-thread Alarm
#define TOut_FlushCash   0x03   // Flush cash context
// End of system time-out's
#define TOut_LstSysTm    0x03

// Normal timer id
#define TOut_FlushDb     0x04
#define TOut_DTMF        0x05
#define TOut_PIN         0x06
#define TOut_DLI         0x07
#define TOut_RemConn     0x08
#define TOut_LocConn     0x09
#define TOut_EndCall     0x0A
#define TOut_FsmSwitch   0x0B
// number of defined TimeOuts
#define TOut_MAX         0x0C

// shift for timeout ID
const uChar TOut_Shift = 0x60;

// LOG file interface
extern time_t globCurentTime;
// return string - Current time
#define tNOW ctime((const time_t*)&globCurentTime)

inline char strlen_zero(const char *s) { return (*s == '\0') ? 'x' : '\0'; }

#define ADM_ONLINE     0x01
#define ADM_EXITAPP    0x02
#define ADM_ACTIVITY   0x04
#define ADM_EXITAPPNOW 0x08
#define ADM_LuaRELOAD  0x10
extern uChar AdmCommand;

// Values to use with <Flag>
#define STOP_EV_THRD   0x01
#define APPL_EXIT      0x02

#endif // INC_GLOBAL_H

// $Id: Global.h 305 2010-01-12 12:06:38Z asus $
