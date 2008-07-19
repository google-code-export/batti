/**
 *  Config.cpp: implementation of the CConfig class.
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

//#include "stdafx.h"
#include <windows.h>
#include "Config.h"
//#include "defs.h"
#include "regs.h"

CConfig Config;

LPCTSTR szRoot = _T("Software\\DaProfik\\Batti");

LPCTSTR szClrFrame = _T("ColorFrame");
LPCTSTR szClrBg = _T("ColorBg");
LPCTSTR szClrFg = _T("ColorFg");
LPCTSTR szClrFgAC = _T("ColorFgAC");
LPCTSTR szClrBgLow = _T("ColorBgLow");
LPCTSTR szClrFgLow = _T("ColorFgLow");
LPCTSTR szClrBgCritical = _T("ColorBgCritical");
LPCTSTR szClrFgCritical = _T("ColorFgCritical");
LPCTSTR szClrBgCharging = _T("ColorBgCharging");
LPCTSTR szClrFgCharging = _T("ColorFgCharging");

LPCTSTR szLowLevel = _T("LowLevel");
LPCTSTR szCriticalLevel = _T("CriticalLevel");
LPCTSTR szPowerOffLevel = _T("PowerOffLevel");
LPCTSTR szPowerOffTimeout = _T("PowerOffTimeout");
LPCTSTR szSnoozeInterval = _T("SnoozeInterval");

LPCTSTR szLeftMargin = _T("LeftMargin");
LPCTSTR szHeight = _T("Height");
LPCTSTR szChargeIndicator = _T("ChargeIndicator");
//LPCTSTR szBatteryCapacity = _T("BatteryCapacity");
LPCTSTR szBlinkingCritical = _T("BlinkingCritical");
LPCTSTR szAlwaysOnTop = _T("AlwaysOnTop");
//LPCTSTR szShowIcon = _T("ShowIcon");
LPCTSTR szSysTrayIcon = _T("SysTrayIcon");

LPCTSTR szStartupDelay = _T("StartupDelay");


LPCTSTR szAction = _T("Action");

LPCTSTR szHasFrame = _T("HasFrame");
LPCTSTR szShowPercentageSteps = _T("ShowPercentageSteps");
LPCTSTR szPercentageStep = _T("PercentageSteps");
LPCTSTR szGradientIndicator = _T("GradientIndicator");

//LPCTSTR szPredictA = _T("A");

LPCTSTR szPage = _T("Page");
LPCTSTR szInstallDir = _T("InstallDir");

LPCTSTR szSoundEvents = _T("SoundEvents");
LPCTSTR szEnabled = _T("Enabled");
LPCTSTR szFileName = _T("FileName");

LPCTSTR szCVoltage = _T("cV");
LPCTSTR szCTemp = _T("cT");
LPCTSTR szCBackupVoltage = _T("cBV");

//////////////////////////////////////////////////////////////////////

double RegReadCoef(HKEY hKey, LPCTSTR szValueName) {
	TCHAR *s = NULL;
	s = RegReadString(hKey, szValueName, _T("1.0"));
	double d;
	swscanf(s, _T("%lf"), &d);
	delete [] s;

	return d;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig() {
	InstallDir = NULL;

	ClrBg = CONFIG_DEFAULT_COLOR_BG;
	ClrFg = CONFIG_DEFAULT_COLOR_FG;
	ClrFgAC = CONFIG_DEFAULT_COLOR_FGAC;
	ClrFrame = CONFIG_DEFAULT_COLOR_FRAME;
	ClrBgLow = CONFIG_DEFAULT_COLOR_BG_LOW;
	ClrFgLow = CONFIG_DEFAULT_COLOR_FG_LOW;
	ClrBgCritical = CONFIG_DEFAULT_COLOR_BG_CRITICAL;
	ClrFgCritical = CONFIG_DEFAULT_COLOR_FG_CRITICAL;
	ClrBgCharging = CONFIG_DEFAULT_COLOR_BG_CHARGING;
	ClrFgCharging = CONFIG_DEFAULT_COLOR_FG_CHARGING;

	CriticalLevel = CONFIG_DEFAULT_CRITICAL_LEVEL;
	LowLevel = CONFIG_DEFAULT_LOW_LEVEL;
	PowerOffLevel = CONFIG_DEFAULT_POWER_OFF_LEVEL;

	Height = CONFIG_DEFAULT_HEIGHT;
	LeftMargin = CONFIG_DEFAULT_LEFT_MARGIN;
	ChargeIndicator = CONFIG_DEFAULT_CHARGE_INDICATOR;
//	BatteryCapacity =  CONFIG_DEFAULT_BATTERY_CAPACITY;
	BlinkingCritical = CONFIG_DEFAULT_BLINKING_CRITICAL;
	AlwaysOnTop = CONFIG_DEFAULT_ALWAYSONTOP;
//	ShowIcon = CONFIG_DEFAULT_SHOWICON;
	ShowPercentageSteps = CONFIG_DEFAULT_SHOWPERCENTAGESTEPS;
	PercentageStep = CONFIG_DEFAULT_PERCENTAGESTEP;

	StartupDelay = CONFIG_DEFAULT_STARTUP_DELAY;
	SysTrayIcon = CONFIG_DEFAULT_SYSTRAYICON;

	HasFrame = CONFIG_DEFAULT_HAS_FRAME;
	GradientIndicator = CONFIG_DEFAULT_GRADIENT_INDICATOR;

	Page = CONFIG_DEFAULT_PAGE;

	CVoltage = 1.0;
	CTemp = 1.0;
	CBackupVoltage = 1.0;
}

CConfig::~CConfig() {
}

void CConfig::Save() {
	DWORD dwDisposition;
	HKEY hApp;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, szRoot, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hApp, &dwDisposition) == ERROR_SUCCESS) {
		RegWriteDword(hApp, szClrBg, ClrBg);
		RegWriteDword(hApp, szClrFg, ClrFg);
		RegWriteDword(hApp, szClrFgAC, ClrFgAC);
		RegWriteDword(hApp, szClrFrame, ClrFrame);
		RegWriteDword(hApp, szClrBgLow, ClrBgLow);
		RegWriteDword(hApp, szClrFgLow, ClrFgLow);
		RegWriteDword(hApp, szClrBgCritical, ClrBgCritical);
		RegWriteDword(hApp, szClrFgCritical, ClrFgCritical);
		RegWriteDword(hApp, szClrBgCharging, ClrBgCharging);
		RegWriteDword(hApp, szClrFgCharging, ClrFgCharging);

		RegWriteDword(hApp, szCriticalLevel, CriticalLevel);
		RegWriteDword(hApp, szLowLevel, LowLevel);
		RegWriteDword(hApp, szPowerOffLevel, PowerOffLevel);

		RegWriteDword(hApp, szLeftMargin, LeftMargin);
		RegWriteDword(hApp, szHeight, Height);
		RegWriteDword(hApp, szChargeIndicator, ChargeIndicator);
		RegWriteDword(hApp, szAction, TapAction);
//		RegWriteDword(hApp, szBatteryCapacity, BatteryCapacity);
		RegWriteDword(hApp, szBlinkingCritical, BlinkingCritical);
		RegWriteDword(hApp, szAlwaysOnTop, AlwaysOnTop);
//		RegWriteDword(hApp, szShowIcon, ShowIcon);
		RegWriteDword(hApp, szSysTrayIcon, SysTrayIcon);
		RegWriteDword(hApp, szShowPercentageSteps, ShowPercentageSteps);
		RegWriteDword(hApp, szPercentageStep, PercentageStep);

		RegWriteDword(hApp, szHasFrame, HasFrame);
		RegWriteDword(hApp, szGradientIndicator, GradientIndicator);

		RegWriteDword(hApp, szPage, Page);

		// sound events
		RegDeleteKey(hApp, szSoundEvents);
		HKEY hSoundEvents;
		if (RegCreateKeyEx(hApp, szSoundEvents, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hSoundEvents, &dwDisposition) == ERROR_SUCCESS) {
			for (int i = 0; i < SOUND_EVENT_COUNT; i++) {
				TCHAR sEvent[4];
				swprintf(sEvent, _T("%d"), i);

				HKEY hEvent;
				if (RegCreateKeyEx(hSoundEvents, sEvent, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hEvent, &dwDisposition) == ERROR_SUCCESS) {
					RegWriteDword(hEvent, szEnabled, SoundEvents[i].Enabled);
					RegWriteString(hEvent, szFileName, SoundEvents[i].FileName);
					RegCloseKey(hEvent);
				}
			}
			RegCloseKey(hSoundEvents);
		}

		RegCloseKey(hApp);
	}
}

void CConfig::Load() {
	HKEY hApp;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, szRoot, 0, 0, &hApp) == ERROR_SUCCESS) {
		delete [] InstallDir;
		InstallDir = RegReadString(hApp, szInstallDir, _T(""));

		CriticalLevel = (BYTE) RegReadDword(hApp, szCriticalLevel, CONFIG_DEFAULT_CRITICAL_LEVEL);
		LowLevel = (BYTE) RegReadDword(hApp, szLowLevel, CONFIG_DEFAULT_LOW_LEVEL);
		PowerOffLevel = (BYTE) RegReadDword(hApp, szPowerOffLevel, CONFIG_DEFAULT_POWER_OFF_LEVEL);

		LeftMargin = RegReadDword(hApp, szLeftMargin, CONFIG_DEFAULT_LEFT_MARGIN);
		Height = RegReadDword(hApp, szHeight, CONFIG_DEFAULT_HEIGHT);
		ChargeIndicator = RegReadDword(hApp, szChargeIndicator, CONFIG_DEFAULT_CHARGE_INDICATOR);
		TapAction = RegReadDword(hApp, szAction, CONFIG_DEFAULT_TAP_ACTION);
		BlinkingCritical = RegReadDword(hApp, szBlinkingCritical, CONFIG_DEFAULT_BLINKING_CRITICAL);
		AlwaysOnTop = RegReadDword(hApp, szAlwaysOnTop, CONFIG_DEFAULT_ALWAYSONTOP);
//		ShowIcon = RegReadDword(hApp, szShowIcon, CONFIG_DEFAULT_SHOWICON);
		SysTrayIcon = RegReadDword(hApp, szSysTrayIcon, CONFIG_DEFAULT_SYSTRAYICON);
		ShowPercentageSteps = RegReadDword(hApp, szShowPercentageSteps, CONFIG_DEFAULT_SHOWPERCENTAGESTEPS);
		PercentageStep = RegReadDword(hApp, szPercentageStep, CONFIG_DEFAULT_PERCENTAGESTEP);

		PowerOffLevel = (BYTE) RegReadDword(hApp, szPowerOffLevel, CONFIG_DEFAULT_POWER_OFF_LEVEL);
		PowerOffTimeout = RegReadDword(hApp, szPowerOffTimeout, CONFIG_DEFAULT_POWER_OFF_TIMEOUT);
		SnoozeInterval = RegReadDword(hApp, szSnoozeInterval, CONFIG_DEFAULT_SNOOZE_INTERVAL);

//		BatteryCapacity = RegReadDword(hApp, szBatteryCapacity, CONFIG_DEFAULT_BATTERY_CAPACITY);

		HasFrame = RegReadDword(hApp, szHasFrame, CONFIG_DEFAULT_HAS_FRAME);
		GradientIndicator = RegReadDword(hApp, szGradientIndicator, CONFIG_DEFAULT_GRADIENT_INDICATOR);

		Page = RegReadDword(hApp, szPage, CONFIG_DEFAULT_PAGE);

		// sound events
		HKEY hSoundEvents;
		if (RegOpenKeyEx(hApp, szSoundEvents, 0, 0, &hSoundEvents) == ERROR_SUCCESS) {
			for (int i = 0; i < SOUND_EVENT_COUNT; i++) {
				TCHAR sEvent[4];
				swprintf(sEvent, _T("%d"), i);

				HKEY hEvent;
				if (RegOpenKeyEx(hSoundEvents, sEvent, 0, 0, &hEvent) == ERROR_SUCCESS) {
					SoundEvents[i].Enabled = RegReadDword(hEvent, szEnabled, FALSE);
					
					delete [] SoundEvents[i].FileName;
					SoundEvents[i].FileName = RegReadString(hEvent, szFileName, _T(""));

					RegCloseKey(hEvent);
				}
				else {
					SoundEvents[i].Enabled = FALSE;
					SoundEvents[i].FileName = new TCHAR[1];
					SoundEvents[i].FileName[0] = '\0';
				}
			}
			RegCloseKey(hSoundEvents);
		}

		// tweaks
		StartupDelay = RegReadDword(hApp, szStartupDelay, CONFIG_DEFAULT_STARTUP_DELAY);

		RegCloseKey(hApp);
	}
}

void CConfig::LoadCoefs() {
	HKEY hApp;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, szRoot, 0, 0, &hApp) == ERROR_SUCCESS) {
		// correction coefficients
		CVoltage = RegReadCoef(hApp, szCVoltage);
		CTemp = RegReadCoef(hApp, szCTemp);
		CBackupVoltage = RegReadCoef(hApp, szCBackupVoltage);		

		RegCloseKey(hApp);
	}
}

void CConfig::LoadColors() {
	HKEY hApp;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, szRoot, 0, 0, &hApp) == ERROR_SUCCESS) {
		ClrBg = RegReadDword(hApp, szClrBg, CONFIG_DEFAULT_COLOR_BG);
		ClrFg = RegReadDword(hApp, szClrFg, CONFIG_DEFAULT_COLOR_FG);
		ClrFgAC = RegReadDword(hApp, szClrFgAC, CONFIG_DEFAULT_COLOR_FGAC);
		ClrFrame = RegReadDword(hApp, szClrFrame, CONFIG_DEFAULT_COLOR_FRAME);
		ClrBgLow = RegReadDword(hApp, szClrBgLow, CONFIG_DEFAULT_COLOR_BG_LOW);
		ClrFgLow = RegReadDword(hApp, szClrFgLow, CONFIG_DEFAULT_COLOR_FG_LOW);
		ClrBgCritical = RegReadDword(hApp, szClrBgCritical, CONFIG_DEFAULT_COLOR_BG_CRITICAL);
		ClrFgCritical = RegReadDword(hApp, szClrFgCritical, CONFIG_DEFAULT_COLOR_FG_CRITICAL);
		ClrBgCharging = RegReadDword(hApp, szClrBgCharging, CONFIG_DEFAULT_COLOR_BG_CHARGING);
		ClrFgCharging = RegReadDword(hApp, szClrFgCharging, CONFIG_DEFAULT_COLOR_FG_CHARGING);

		RegCloseKey(hApp);
	}
}
