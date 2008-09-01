/**
 *  Config.h: interface for the CConfig class.
 *
 *  Copyright (C) 2008  David Andrs <pda@jasnapaka.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#if !defined(_CONFIG_H_)
#define _CONFIG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// tap action
#define TAP_ACTION_OPEN_CONFIG				0
#define TAP_ACTION_NOTHING					1
#define TAP_ACTION_SHOW_INFO				2

// config
#define CONFIG_DEFAULT_COLOR_BG				RGB(75, 75, 75)
#define CONFIG_DEFAULT_COLOR_FG				RGB(50, 160, 50)
//#define CONFIG_DEFAULT_COLOR_FGAC			RGB(190, 124, 229)
#define CONFIG_DEFAULT_COLOR_FGAC			RGB(5, 95, 40)

#define CONFIG_DEFAULT_COLOR_BG_LOW			RGB(75, 75, 75)
#define CONFIG_DEFAULT_COLOR_FG_LOW			RGB(240, 240, 40)

#define CONFIG_DEFAULT_COLOR_BG_CRITICAL	RGB(75, 75, 75)
#define CONFIG_DEFAULT_COLOR_FG_CRITICAL	RGB(220, 35, 55)

#define CONFIG_DEFAULT_COLOR_BG_CHARGING	RGB(90, 25, 130)
#define CONFIG_DEFAULT_COLOR_FG_CHARGING	RGB(120, 35, 175)

/*
#define CONFIG_DEFAULT_COLOR_BG				RGB(75, 75, 75)
#define CONFIG_DEFAULT_COLOR_FG				RGB(5, 130, 60)
#define CONFIG_DEFAULT_COLOR_FGAC			RGB(190, 124, 229)
//#define CONFIG_DEFAULT_COLOR_FGAC			RGB(97, 136, 255)

#define CONFIG_DEFAULT_COLOR_BG_LOW			RGB(75, 75, 75)
#define CONFIG_DEFAULT_COLOR_FG_LOW			RGB(245, 210, 100)

#define CONFIG_DEFAULT_COLOR_BG_CRITICAL	RGB(75, 75, 75)
#define CONFIG_DEFAULT_COLOR_FG_CRITICAL	RGB(220, 35, 54)

#define CONFIG_DEFAULT_COLOR_BG_CHARGING	RGB(0, 36, 156)
#define CONFIG_DEFAULT_COLOR_FG_CHARGING	RGB(99, 138, 255)
*/

#define CONFIG_DEFAULT_COLOR_FRAME			RGB(0, 0, 0)

#define CONFIG_DEFAULT_CRITICAL_LEVEL		10
#define CONFIG_DEFAULT_LOW_LEVEL			30
#define CONFIG_DEFAULT_POWER_OFF_LEVEL		5
#define CONFIG_DEFAULT_POWER_OFF_TIMEOUT	10
#define CONFIG_DEFAULT_SNOOZE_INTERVAL		60

#define CONFIG_DEFAULT_LEFT_MARGIN			0
#define CONFIG_DEFAULT_HEIGHT               3

#define CONFIG_DEFAULT_CHARGE_INDICATOR		TRUE
#define CONFIG_DEFAULT_BATTERY_CAPACITY		0
#define CONFIG_DEFAULT_TAP_ACTION			TAP_ACTION_SHOW_INFO
#define CONFIG_DEFAULT_BLINKING_CRITICAL	TRUE
#define CONFIG_DEFAULT_ALWAYSONTOP			FALSE
#define CONFIG_DEFAULT_SHOWICON				FALSE
#define CONFIG_DEFAULT_SYSTRAYICON			FALSE

#define CONFIG_DEFAULT_HAS_FRAME			TRUE
#define CONFIG_DEFAULT_SHOWPERCENTAGESTEPS	FALSE
#define CONFIG_DEFAULT_PERCENTAGESTEP		5
#define CONFIG_DEFAULT_GRADIENT_INDICATOR	TRUE

#define CONFIG_DEFAULT_STARTUP_DELAY		0

#define CONFIG_DEFAULT_PAGE					0

// sound events
#define SOUND_EVENT_COUNT					5

#define SOUND_EVENT_BATTERY_LOW				0
#define SOUND_EVENT_BATTERY_CRITICAL		1
#define SOUND_EVENT_CHARGING_STARTED		2
#define SOUND_EVENT_BATERY_CHARGED			3
#define SOUND_EVENT_POWER_OFF				4

// refresh interval for AlwaysOnTop (in miliseconds)
#define ALWAYS_ON_TOP_UPDATE_INTERVAL		1000
// general update interval is 2 seconds
#define GENERAL_UPDATE_INTERVAL				2000
#define CHARGE_INTERVAL						500
#define BLINK_INTERVAL						750


/////////////////////////////////////////////////////////////////////
// sound events

struct CSoundEvent {
	BOOL Enabled;
	TCHAR *FileName;

	CSoundEvent() {
		Enabled = FALSE;
		FileName = new TCHAR[1];
		FileName[0] = '\0';
	}

	CSoundEvent(const CSoundEvent &o) {
		Enabled = o.Enabled;
		if (FileName != NULL) {
			delete [] FileName;
			FileName = new TCHAR [wcslen(o.FileName) + 1];
			wcscpy(FileName, o.FileName);
		}
		else
			FileName = o.FileName;
	}

	CSoundEvent &operator=(const CSoundEvent &o) {
		delete [] FileName;

		Enabled = o.Enabled;
		if (FileName != NULL) {
			delete [] FileName;
			FileName = new TCHAR [wcslen(o.FileName) + 1];
			wcscpy(FileName, o.FileName);
		}
		else
			FileName = o.FileName;

		return *this;
	}

	virtual ~CSoundEvent() {
		delete [] FileName;
	}
};


/////////////////////////////////////////////////////////////////////

class CConfig {
public:
	TCHAR *InstallDir;

	CSoundEvent SoundEvents[SOUND_EVENT_COUNT];

	COLORREF ClrFrame;
	COLORREF ClrFg;
	COLORREF ClrBg;
	COLORREF ClrFgAC;

	COLORREF ClrFgLow;
	COLORREF ClrBgLow;
	COLORREF ClrFgCritical;
	COLORREF ClrBgCritical;
	COLORREF ClrFgCharging;
	COLORREF ClrBgCharging;

	int LeftMargin;
	int Height;


	BYTE LowLevel;
	BYTE CriticalLevel;
	BYTE PowerOffLevel;

	BOOL ChargeIndicator;
	BOOL BlinkingCritical;
	int TapAction;
	BOOL AlwaysOnTop;
	BOOL ShowIcon;
	BOOL SysTrayIcon;
	BOOL ShowPercentageSteps;
	int PercentageStep;

	int PowerOffTimeout;
	int StartupDelay;
	int SnoozeInterval;

	BOOL HasFrame;
	BOOL GradientIndicator;

	int Page;

	// correction coefficients
	double CVoltage, CTemp, CBackupVoltage;

	CConfig();
	virtual ~CConfig();

	void Save();
	void Load();

	void LoadColors();
	void LoadCoefs();
};

///////////////////////////////////////////////////////////////////////////////

extern CConfig Config;

extern LPCTSTR szRoot;

extern LPCTSTR szClrFrame;
extern LPCTSTR szClrBg;
extern LPCTSTR szClrFg;
extern LPCTSTR szClrFgAC;
extern LPCTSTR szClrBgLow;
extern LPCTSTR szClrFgLow;
extern LPCTSTR szClrBgCritical;
extern LPCTSTR szClrFgCritical;
extern LPCTSTR szClrBgCharging;
extern LPCTSTR szClrFgCharging;

extern LPCTSTR szLowLevel;
extern LPCTSTR szCriticalLevel;
extern LPCTSTR szPowerOffLevel;
extern LPCTSTR szPowerOffTimeout;
extern LPCTSTR szSnoozeInterval;

extern LPCTSTR szLeftMargin;
extern LPCTSTR szHeight;
extern LPCTSTR szChargeIndicator;
//extern LPCTSTR szBatteryCapacity;
extern LPCTSTR szBlinkingCritical;
extern LPCTSTR szAlwaysOnTop;
extern LPCTSTR szShowIcon;

extern LPCTSTR szStartupDelay;

extern LPCTSTR szAction;

extern LPCTSTR szHasFrame;
//extern LPCTSTR szPredictA;

extern LPCTSTR szPage;
extern LPCTSTR szInstallDir;

extern LPCTSTR szSoundEvents;
extern LPCTSTR szEnabled;
extern LPCTSTR szFileName;

extern LPCTSTR szCVoltage;
extern LPCTSTR szCTemp;
extern LPCTSTR szCBackupVoltage;

#endif // !defined(AFX_CONFIG_H__3AC5942A_61A5_4469_ADFE_264704448F3C__INCLUDED_)
