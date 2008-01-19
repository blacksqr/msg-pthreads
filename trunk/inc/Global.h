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
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/param.h>
//#include <sys/types.h>
//#include <sys/ioctl.h>
#include <sys/resource.h>

// typedef's
typedef char*    pChar;
typedef void*    pVoid;
typedef unsigned char  uChar;
typedef unsigned short uShort;
typedef unsigned int   uInt;
typedef unsigned long  uLong;
typedef long long      lLong;
typedef const uChar    cuChar;
typedef unsigned long long ulLong;

extern int errno;
extern const int log_facility;

// max number of working threads
#define MAX_N_WTHREAD 32

// Contetxt State-Automate ================
#define Stat_NoStat      0x00
#define Stat_Idle        0x01
#define Stat_WaitPRS0    0x02   // wait prosody channel
#define Stat_Online      0x03   // Online

// HK-Context Errors-id ===================
#define hk_wTclFail      0x01

// Context Events =========================
#define Evnt_wThrdEnd    0x01   // Term work thread TCL-Script
#define Evnt_HsKeepCtxt  0x02   // Hause keep problem in some context
#define Evnt_SThAlive    0x03   // signaling thread keepAlive event
#define Evnt_EndSigTh    0x04   // Term signaling thread
#define Evnt_Que12Full   0x05   // Application queue 50% full
// End of hauseKeep events
#define Evnt_TstCntxt    0x09   // event to create test Context
#define Evnt_TstSeqw     0x0A   // event from test sign thread
#define Evnt_SaveData    0x0B   // To store data in Cash
#define Evnt_DbReady     0x0C   // Data stored in DB
#define Evnt_StoreInDb   0x0D   // Store new set of data in DB
#define Evnt_DelCtxt     0x10   // to remove test context

#define Evnt_sipCtxt     0x11   // New parser context
#define Evnt_sipMsgRq    0x12   // new SIP Request
#define Evnt_sipMsgRp    0x13   // new SIP Replay

#define Evnt_NewCall     0x14   // icoming call
#define Evnt_Idle        0x15   // call closed
#define Evnt_Discnct     0x16   // start disconnecting
#define Evnt_Connct      0x17   // call online

// TimeOut ID's ===========================
#define TOut_NoTmOut     0x00
#define TOut_DelCtxt     0x01   // Del context
#define TOut_HsKeep      0x02   // System Check
#define TOut_sigThAlrm   0x03   // signal-thread Alarm
#define TOut_FlushCash   0x04   // Flush cash context
// End of system time-out's
#define TOut_LstSysTm    0x04

// Normal timer id
#define TOut_DTMF        0x05
#define TOut_PIN         0x06
#define TOut_DLI         0x07
#define TOut_RemConn     0x08
#define TOut_LocConn     0x09
#define TOut_EndCall     0x0A
#define TOut_FlushDb     0x0B
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
#define ADM_TCLRELOAD  0x10
extern uChar AdmCommand;

#endif // INC_GLOBAL_H
