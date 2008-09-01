// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include <aygshell.h>
#include <tlhelp32.h>
#include <shellapi.h>

// Local Header Files
#include "..\share\shguim.h"
#include "..\share\uihelper.h"

// TODO: reference additional headers your program requires here
#define countof(a) (sizeof(a)/sizeof(a[0]))


#define NOTIF_SOFTKEY_FLAGS_DISMISS		0x0001
#define NOTIF_SOFTKEY_FLAGS_HIDE		0x0004


typedef struct _SOFTKEYCMD {
	WPARAM wpCmd;
	DWORD grfFlags;
} SOFTKEYCMD;

typedef struct _SOFTKEYMENU {
	HMENU hMenu;
	SOFTKEYCMD *prgskc;
	UINT cskc;
} SOFTKEYMENU;

typedef struct _SOFTKEYNOTIFY {
	LPCTSTR pszTitle;
	SOFTKEYCMD skc;
} SOFTKEYNOTIFY;

#define NOTIF_NUM_SOFTKEYS			2

typedef struct _SHNOTIFICATIONDATAEX {
	DWORD    cbStruct;     // for verification and versioning
	DWORD    dwID;         // identifier for this particular
						  // notification
	SHNP     npPriority;   // priority
	DWORD    csDuration;   // duration of the notification
						  // (usage depends on prior)
	HICON    hicon;        // the icon for the notification
	DWORD    grfFlags;     // flags - see SHNF_ flags below
	CLSID    clsid;        // unique identifier for the
						  // notification class
	HWND     hwndSink;     // window to receive command choices,
						  // dismiss, etc.
	LPCTSTR  pszHTML;      // HTML content for the bubble
	LPCTSTR  pszTitle;     // Optional title for bubble
	LPARAM   lParam;       // User-defined parameter
	// From here, this is WM 5.0 stuff
	union
	{                      // Defines the softkey bar for the
						  // notification
	   SOFTKEYMENU skm;   // Either pass an HMENU in skn
						  // (and set SHNF_HASMENU)
						  // or two softkeys in rgskn.
	   SOFTKEYNOTIFY rgskn[NOTIF_NUM_SOFTKEYS];
	};
	// Text to put on SK2 on the Today screen. If NULL, will
	// default to "Notification"
	LPCTSTR    pszTodaySK;
	// What to execute when SK2 is pressed. If NULL, the toast
	// will be displayed.
	LPCTSTR    pszTodayExec;
} SHNOTIFICATIONDATAEX;


//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
