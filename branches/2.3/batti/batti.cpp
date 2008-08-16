/**
 *  batti.cpp : Defines the entry point for the application.
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

#include "StdAfx.h"
#include "resource.h"
#include "../share/helpers.h"
#include "../share/defs.h"
#include "../share/regs.h"
#include "../share/Localization.h"
#include "../share/Config.h"

//#define LOGGING
#ifdef LOGGING
#define LOG(a) \
	if (hLogFile != INVALID_HANDLE_VALUE) { \
		DWORD written; \
		WriteFile(hLogFile, a, strlen(a), &written, NULL); \
	}
#else
#define LOG(a)
#endif

UINT ReadConfigMessage;

// draw
HBITMAP OffscreenBmp = NULL;
HGDIOBJ SaveBmp = NULL;
HDC OffscreenDC = NULL;
int ScreenWd;

// windows related
HINSTANCE HInstance = 0;
HWND HMain = 0;
HWND HTaskBar = 0;

BOOL InfoDlgOpened = FALSE;

HDC ScreenDC = NULL;

// timers
UINT Timer = 1;
UINT ChargeTimer = 2;
UINT BlinkCritTimer = 4;			// Blinking Critical Timer

#ifdef LOGGING
UINT LogTimer = 5;					// logging
#endif

UINT PowerOffTimer = 6;				// power-off timeout
UINT AlwaysOnTopTimer = 8;			// refresh interval for always-on-top
UINT RefreshInfoTimer = 9;			// refresh info timer

// charging
int ChargeXOfs = 0;

// blinking
int BlinkState = 1;					// 1 - on, 0 - off (show foreground, show background)

// events
// current battery status

SYSTEM_POWER_STATUS_EX2 SPS = { 0 };

// Last Battery Status
SYSTEM_POWER_STATUS_EX2 LastSPS = { -1 };
int LastBatteryLifePercent = 0;

// battery states
#define BATTERY_STATE_NORMAL					0
#define BATTERY_STATE_LOW						1
#define BATTERY_STATE_CRITICAL					2
#define BATTERY_STATE_POWEROFF					3
#define BATTERY_STATE_CHARGING					4
#define BATTERY_STATE_ON_AC						5

#ifdef LOGGING
// logging
HANDLE hLogFile = NULL;
#endif

// user-defined messages
#define UWM_POWEROFF							(WM_APP + 0x0001)
#define UWM_ICON_NOTIFY							(WM_APP + 0x0002)

// power-off notification ID
#define POWEROFF_NOTIFICATION_ID				1

// power-off notification
// {4B357C8D-0D38-4134-B422-915B04AE8478}
static const GUID CLSID_PowerOffNotification =
	{ 0x4b357c8d, 0xd38, 0x4134, { 0xb4, 0x22, 0x91, 0x5b, 0x4, 0xae, 0x84, 0x78 } };

SHNOTIFICATIONDATAEX PowerOffSND = { 0 };


// sys tray
NOTIFYICONDATA NID = { 0 };

HFONT HUnderlineFont = NULL;					// underlined font for links
COLORREF LinkClr = RGB(0, 0, 255);
HFONT HBoldFont = NULL;							// bold font for headings

/*
//
UINT PredictorTimer = 6;
double SumX = 0;			// \sum_i x_i
double SumY = 0;			// \sum_i y_i
double SumXY = 0;			// \sum_i x_i y_i
double SumX2 = 0;			// \sum_i x_i^2

// 120 samples every 10 seconds means 20 minutes of history
#define PREDICT_BUFFER_SIZE				120
//#define PREDICT_BUFFER_SIZE			30
DWORD T = 0;								// time
int Period = 10;							// 10 seconds
int SampleCount = 0;						// number of samples in the predict buffer
int SampleIdx = 0;							// index to the predict buffer
BYTE PredictBuffer[PREDICT_BUFFER_SIZE];	// predict buffer
double PredictA = 0.0;
*/


// ****************************************************************************

TCHAR *StringBuffer = NULL;   // buffer pro mnoho stringu
TCHAR *StrAct = StringBuffer;

#define STRING_BUFFER_SIZE				1024

TCHAR *LoadStr(int resID) {
	TCHAR *ret;

	if (Localization != NULL) {
		// first check for localized string
		ret = Localization->GetStr(resID);
		if (ret != NULL) return ret;
	}

	// load from resources
	if (!StringBuffer) ret = _T("ERROR LOADING STRING - INSUFFICIENT MEMORY");
	else {
		if (STRING_BUFFER_SIZE - (StrAct - StringBuffer) < 200) StrAct = StringBuffer;

	RELOAD:
		int size = LoadString(HInstance, resID, StrAct, STRING_BUFFER_SIZE - (StrAct - StringBuffer));
		// size obsahuje pocet nakopirovanych znaku bez terminatoru

		// error je NO_ERROR, i kdyz string neexistuje - nepouzitelne
		if (size != 0/* || error == NO_ERROR*/) {
			if (STRING_BUFFER_SIZE - (StrAct - StringBuffer) == size + 1 && StrAct > StringBuffer) {
				// pokud byl retezec presne na konci bufferu, mohlo
				// jit o oriznuti retezce -- pokud muzeme posunout okno
				// na zacatek bufferu, nacteme string jeste jednou
				StrAct = StringBuffer;
				goto RELOAD;
			}
			else {
				ret = StrAct;
				StrAct += size + 1;
			}
		}
		else {
			ret = _T("ERROR LOADING STRING");
		}
	}

	return ret;
}

void CreateFonts() {
	LOGFONT lf = { 0 };
	GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), (void *) &lf);

	lf.lfHeight = SCALEY(12) + 1;
	lf.lfWeight = FW_NORMAL;
	lf.lfUnderline = TRUE;
	HUnderlineFont = CreateFontIndirect(&lf);

	lf.lfWeight = FW_BOLD;
	lf.lfUnderline = FALSE;
	HBoldFont = CreateFontIndirect(&lf);
}

//
BOOL IsOnWM5() {
	static OSVERSIONINFO osvi = { 0 };
	static gotOsvi = FALSE;

	if (!gotOsvi) {
		osvi.dwOSVersionInfoSize = sizeof(osvi);
		GetVersionEx(&osvi);
		gotOsvi = TRUE;
	}

	return osvi.dwMajorVersion == 5;
}

BOOL IsCharging(SYSTEM_POWER_STATUS_EX2 &sps) {
	return (sps.BatteryFlag & 8);
}

BOOL IsOnAC(SYSTEM_POWER_STATUS_EX2 &sps) {
	return (sps.ACLineStatus == 1);
}

void UpdateBatteryStatus() {
	SYSTEM_POWER_STATUS_EX2 sps;
	DWORD dwError = GetSystemPowerStatusEx2(&sps, sizeof(sps), TRUE);
	if (dwError != 0)
		SPS = sps;
}

int GetBatteryState(SYSTEM_POWER_STATUS_EX2 &sps) {
	if (IsCharging(sps))
		return BATTERY_STATE_CHARGING;
	else if (IsOnAC(sps))
		return BATTERY_STATE_ON_AC;
	else {
		if (sps.BatteryLifePercent <= Config.PowerOffLevel)
			return BATTERY_STATE_POWEROFF;
		else if (sps.BatteryLifePercent <= Config.CriticalLevel)
			return BATTERY_STATE_CRITICAL;
		else if (sps.BatteryLifePercent <= Config.LowLevel)
			return BATTERY_STATE_LOW;
		else if (sps.BatteryLifePercent <= 100)
			return BATTERY_STATE_NORMAL;
		else
			return -1;
	}
}


void OpenConfig() {
	TCHAR cfgPathName[MAX_PATH];
	swprintf(cfgPathName, _T("%s\\%s"), Config.InstallDir, BATTI_CONFIG_FILENAME);

	CreateProcess(cfgPathName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
}


//

void ShowInfo(HWND hDlg) {
	//
	SYSTEM_POWER_STATUS_EX2 sps = { 0 };
	DWORD dwError = GetSystemPowerStatusEx2(&sps, sizeof(sps), TRUE);

	//
	TCHAR *sText;
	TCHAR str[256];

	// MAIN battery

	if (IsCharging(sps)) {
		sText = LoadStr(IDS_CHARGING);
	}
	else if (IsOnAC(sps)) {
		sText = LoadStr(IDS_ON_AC);
	}
	else {
		sText = LoadStr(IDS_REMAINING);
	}
	SendDlgItemMessage(hDlg, IDC_STATUS, WM_SETTEXT, 0, (LPARAM) sText);

	// main battery percent
	if (sps.BatteryLifePercent != BATTERY_PERCENTAGE_UNKNOWN) {
		swprintf(str, _T("%d %%"), sps.BatteryLifePercent);
		sText = str;
		SendDlgItemMessage(hDlg, IDC_MAIN_PERCENT, PBM_SETPOS, sps.BatteryLifePercent, 0);
	}
	else
		sText = _T("");
	SendDlgItemMessage(hDlg, IDC_MAIN_PERCENT_NUM, WM_SETTEXT, 0, (LPARAM) sText);

	// voltage
	swprintf(str, _T("%.3lf V"), (Config.CVoltage * sps.BatteryVoltage) / 1000.0);
	sText = str;
	SendDlgItemMessage(hDlg, IDC_VOLTAGE, WM_SETTEXT, 0, (LPARAM) sText);

	// temperature
	if (sps.BatteryTemperature != 0.0) {
		ShowWindow(GetDlgItem(hDlg, IDC_C_TEMPERATURE), SW_SHOW);
		ShowWindow(GetDlgItem(hDlg, IDC_TEMPERATURE), SW_SHOW);

		swprintf(str, _T("%.1lf °C"), (Config.CTemp * sps.BatteryTemperature) / 10.0);
		sText = str;
		SendDlgItemMessage(hDlg, IDC_TEMPERATURE, WM_SETTEXT, 0, (LPARAM) sText);
	}
	else {
		ShowWindow(GetDlgItem(hDlg, IDC_C_TEMPERATURE), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_TEMPERATURE), SW_HIDE);
	}

	// chemistry
	switch (sps.BatteryChemistry) {
		case BATTERY_CHEMISTRY_ALKALINE: sText = _T("Alkaline"); break;
		case BATTERY_CHEMISTRY_NICD: sText = _T("NiCd"); break;
		case BATTERY_CHEMISTRY_NIMH: sText = _T("NiMH"); break;
		case BATTERY_CHEMISTRY_LION: sText = _T("LiIon"); break;
		case BATTERY_CHEMISTRY_LIPOLY: sText = _T("LiPoly"); break;
		case BATTERY_CHEMISTRY_UNKNOWN:
		default:
			sText = _T("");
			break;
	}
	SendDlgItemMessage(hDlg, IDC_CHEMISTRY, WM_SETTEXT, 0, (LPARAM) sText);

	// BACKUP battery

	if (sps.BackupBatteryFlag == BATTERY_FLAG_NO_BATTERY) {
		ShowWindow(GetDlgItem(hDlg, IDC_BACKUP_PERCENT_NUM), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_C_BACKUP_VOLTAGE), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_BACKUP_VOLTAGE), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_BACKUP_PERCENT), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_C_BACKUP_BATTERY), SW_HIDE);
		// TODO: hide backup battery icon
	}
	else {
		// percent
		if (sps.BackupBatteryLifePercent != BATTERY_PERCENTAGE_UNKNOWN) {
			swprintf(str, _T("%d %%"), sps.BackupBatteryLifePercent);
			sText = str;
			SendDlgItemMessage(hDlg, IDC_BACKUP_PERCENT, PBM_SETPOS, sps.BackupBatteryLifePercent, 0);
		}
		else
			sText = _T("");
		SendDlgItemMessage(hDlg, IDC_BACKUP_PERCENT_NUM, WM_SETTEXT, 0, (LPARAM) sText);

		// voltage
		swprintf(str, _T("%.3lf V"), (Config.CBackupVoltage * sps.BackupBatteryVoltage) / 1000.0);
		sText = str;
		SendDlgItemMessage(hDlg, IDC_BACKUP_VOLTAGE, WM_SETTEXT, 0, (LPARAM) sText);
	}
}

BOOL CALLBACK DialogInfoProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// menu bar
	BOOL fSuccess;
	SHMENUBARINFO mbi = { 0 };
	HWND hwndCmdBar;
	LRESULT res;

	SHINITDLGINFO shidi = { 0 };

	switch (uMsg) {
		case WM_INITDIALOG:
			LocalizeDialog(hwndDlg, IDD_INFO);

			mbi.cbSize = sizeof(mbi);
			mbi.hInstRes = HInstance;
			mbi.hwndParent = hwndDlg;
			mbi.dwFlags = SHCMBF_HMENU;
			mbi.nToolBarId = IDR_CLOSE;

			fSuccess = SHCreateMenuBar(&mbi);
			hwndCmdBar = mbi.hwndMB;
			LocalizeMenubar(hwndCmdBar);

			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hwndDlg;
			SHInitDialog(&shidi);

			SHDoneButton(hwndDlg, SHDB_SHOW);

			SendDlgItemMessage(hwndDlg, IDC_MAIN_PERCENT, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
			SendDlgItemMessage(hwndDlg, IDC_BACKUP_PERCENT, PBM_SETRANGE, 0, MAKELPARAM(0, 100));

			ShowInfo(hwndDlg);
			SetTimer(hwndDlg, RefreshInfoTimer, 2000, NULL);

			SendMessage(GetDlgItem(hwndDlg, IDC_C_MAIN_BATTERY), WM_SETFONT, (WPARAM) HBoldFont, MAKELPARAM(0, 0));
			SendMessage(GetDlgItem(hwndDlg, IDC_C_BACKUP_BATTERY), WM_SETFONT, (WPARAM) HBoldFont, MAKELPARAM(0, 0));

			SendMessage(GetDlgItem(hwndDlg, IDC_SETTINGS), WM_SETFONT, (WPARAM) HUnderlineFont, MAKELPARAM(0, 0));

			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDCANCEL) {
				EndDialog(hwndDlg, IDCANCEL);
			}
			else if (LOWORD(wParam) == IDOK) {
				EndDialog(hwndDlg, IDOK);
			}
			else if (LOWORD(IDC_SETTINGS) == IDC_SETTINGS) {
				EndDialog(hwndDlg, IDCANCEL);
				OpenConfig();
			}
			return TRUE;

		case WM_TIMER:
			if (wParam == RefreshInfoTimer) {
				ShowInfo(hwndDlg);
			}
			return 0;

		case WM_DESTROY:
			KillTimer(hwndDlg, RefreshInfoTimer);
			InfoDlgOpened = FALSE;
			return 0;

		case WM_CTLCOLORSTATIC:
			res = DefWindowProc(hwndDlg, uMsg, wParam, lParam);
			if ((HWND) lParam == GetDlgItem(hwndDlg, IDC_SETTINGS)) {
				HDC hDC = (HDC) wParam;
				SetTextColor(hDC, LinkClr);
			}
			return res;

		default:
			return FALSE;
	}
}


void OpenInfo() {
	if (!InfoDlgOpened) {
		InfoDlgOpened = TRUE;
		DialogBox(HInstance, MAKEINTRESOURCE(IDD_INFO), NULL, DialogInfoProc);
	}
}

void PowerOffNotification() {
	TCHAR notification[256];
	swprintf(notification, LoadStr(IDS_NOTIFICATION), Config.PowerOffTimeout);

	TCHAR buf[512];
	memset(&PowerOffSND, 0, sizeof(PowerOffSND));
	if (IsOnWM5()) {
		PowerOffSND.cbStruct = sizeof(SHNOTIFICATIONDATAEX);
		swprintf(buf,
			L"<html><body>"
			L"<p><form method=\"POST\" action=>"
			L"<p>%s</p>"
			L"</body></html>",
			notification
		);

		PowerOffSND.rgskn[0].pszTitle = LoadStr(IDS_HIDE);
		PowerOffSND.rgskn[0].skc.wpCmd = ID_HIDE;
		PowerOffSND.rgskn[0].skc.grfFlags = NOTIF_SOFTKEY_FLAGS_DISMISS;
		PowerOffSND.rgskn[1].pszTitle = LoadStr(IDS_POWEROFF);
		PowerOffSND.rgskn[1].skc.wpCmd = ID_POWEROFF;
		PowerOffSND.rgskn[1].skc.grfFlags = NOTIF_SOFTKEY_FLAGS_DISMISS;
	}
	else {
		PowerOffSND.cbStruct = sizeof(SHNOTIFICATIONDATA);
		swprintf(buf,
			L"<html><body>"
			L"<p><form method=\"POST\" action=>"
			L"<p>%s</p>"
			L"<hr size=1/>"
			L"<p align=right>"
			L"<input type=button name='cmd:%d' value='%s'>"
			L"<input type=button name='cmd:%d' value='%s'>"
			L"</p>"
			L"</form></p>"
			L"</body></html>",
			notification,
			ID_POWEROFF, LoadStr(IDS_POWEROFF),
			ID_HIDE, LoadStr(IDS_HIDE));
	}


	PowerOffSND.grfFlags = SHNF_DISPLAYON | SHNF_FORCEMESSAGE;
	PowerOffSND.dwID = POWEROFF_NOTIFICATION_ID;
	PowerOffSND.npPriority = SHNP_INFORM;
	PowerOffSND.csDuration = Config.PowerOffTimeout;
	PowerOffSND.hicon = (HICON) LoadImage(HInstance, MAKEINTRESOURCE(IDI_POWEROFF), IMAGE_ICON, SCALEX(16), SCALEY(16), 0);
	PowerOffSND.hwndSink = HMain;
	PowerOffSND.pszHTML = buf;
	PowerOffSND.pszTitle = LoadStr(IDS_BATTI_NOTICIFATION);
	PowerOffSND.lParam = 0;
	PowerOffSND.clsid = CLSID_PowerOffNotification;
	SHNotificationAdd((SHNOTIFICATIONDATA *) &PowerOffSND);

	SetTimer(HMain, PowerOffTimer, Config.PowerOffTimeout * 1000, NULL);

	// TODO: Find HTML window and allow JS execution
}

void OnAction() {
	switch (Config.TapAction) {
		case TAP_ACTION_OPEN_CONFIG:	OpenConfig(); break;
		case TAP_ACTION_SHOW_INFO:		OpenInfo(); break;
		default: break;
	}
}

void TurnOffDevice() {
	// Send keypresses that mean power off
	keybd_event(VK_OFF, 0, KEYEVENTF_SILENT, 0);
	keybd_event(VK_OFF, 0, (KEYEVENTF_KEYUP | KEYEVENTF_SILENT), 0);
}

void ExecuteSoundEvent(CSoundEvent *event) {
	if (event->Enabled) {
		TCHAR fileName[MAX_PATH + 1];
		swprintf(fileName, _T("\\windows\\%s.wav"), event->FileName);
		PlaySound(fileName, NULL, SND_ASYNC | SND_FILENAME | SND_NOWAIT);
	}
}

// sys tray

BOOL AddTrayIcon(HWND hWnd) {
	memset(&NID, 0, sizeof(NID));
	NID.cbSize = sizeof(NID);
	NID.hWnd = hWnd;
	NID.uID = IDR_MAIN;
	if (IsOnAC(SPS))
		NID.hIcon = (HICON) ::LoadImage(HInstance, MAKEINTRESOURCE(IDI_TRAY_AC), IMAGE_ICON, SCALEX(16), SCALEY(16), 0);
	else
		NID.hIcon = (HICON) ::LoadImage(HInstance, MAKEINTRESOURCE(IDI_TRAY_BATTERY), IMAGE_ICON, SCALEX(16), SCALEY(16), 0);
	NID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	NID.uCallbackMessage = UWM_ICON_NOTIFY;
	wcscpy(NID.szTip, ::LoadStr(IDS_BATTI));

	return Shell_NotifyIcon(NIM_ADD, &NID);
}

BOOL RemoveTrayIcon() {
    NID.uFlags = 0;
    return Shell_NotifyIcon(NIM_DELETE, &NID);
}

BOOL ModifyTrayIcon(HWND hWnd, UINT nIcon) {
	memset(&NID, 0, sizeof(NID));
	NID.cbSize = sizeof(NID);
	NID.hWnd = HMain;
	NID.uID = IDR_MAIN;
	NID.uFlags = NIF_ICON;
	NID.hIcon = (HICON) ::LoadImage(HInstance, MAKEINTRESOURCE(nIcon), IMAGE_ICON, SCALEX(16), SCALEY(16), 0);
	return Shell_NotifyIcon(NIM_MODIFY, &NID);
}

//
// Paint
//

void CreateOffscreenBuffer(int wd, int ht) {
	HDC scrDC = ::GetDC(NULL);
	OffscreenBmp = CreateCompatibleBitmap(scrDC, wd, ht);
	OffscreenDC = CreateCompatibleDC(scrDC);
	SaveBmp = SelectObject(OffscreenDC, OffscreenBmp);
	::ReleaseDC(NULL, scrDC);
}

void DestroyOffscreenBuffer() {
	SelectObject(OffscreenDC, SaveBmp);
	DeleteObject(OffscreenBmp);
	DeleteDC(OffscreenDC);
}

void DrawSolidRect(HDC hDC, int left, int top, int right, int bottom, COLORREF clr) {
	HPEN pen = CreatePen(PS_NULL, 1, RGB(255, 255, 255));
	HGDIOBJ oldPen = SelectObject(hDC, pen);

	HBRUSH br = CreateSolidBrush(clr);
	HGDIOBJ oldBrush = SelectObject(hDC, br);

	Rectangle(hDC, left, top, right, bottom);

	SelectObject(hDC, oldBrush);
	DeleteObject(br);

	SelectObject(hDC, oldPen);
	DeleteObject(pen);
}

void DrawFlat(HDC hDC, RECT &rc, RECT &rcClip, SYSTEM_POWER_STATUS_EX2 &sps) {
	// draw bar
	COLORREF clrFg;
	COLORREF clrBg;

	int batState = GetBatteryState(sps);
	switch (batState) {
		case BATTERY_STATE_CRITICAL:
		case BATTERY_STATE_POWEROFF:
			// critical state
			if (Config.BlinkingCritical && BlinkState == 1)
				clrFg = Config.ClrBgCritical;
			else
				clrFg = Config.ClrFgCritical;
			clrBg = Config.ClrBgCritical;
			break;

		case BATTERY_STATE_LOW:
			// low state
			clrFg = Config.ClrFgLow;
			clrBg = Config.ClrBgLow;
			break;

		case BATTERY_STATE_NORMAL:
		default:
			// normal state
			clrFg = Config.ClrFg;
			clrBg = Config.ClrBg;
			break;

		case BATTERY_STATE_CHARGING:
			clrFg = Config.ClrFgCharging;
			clrBg = Config.ClrBgCharging;
			break;

		case BATTERY_STATE_ON_AC:
			clrFg = Config.ClrFgAC;
			clrBg = Config.ClrBg;
			break;
	}

	RECT r;
	int wd = rc.right - rc.left;
	int percent;
	if (Config.ChargeIndicator && IsCharging(sps))
		percent = ChargeXOfs;
	else
		percent = sps.BatteryLifePercent;

	int x = (int) (rc.left + (percent * wd / 100.0));
	RECT rcFg = { rc.left, rc.top, x, rc.bottom };
	IntersectRect(&r, &rcFg, &rcClip);
	DrawSolidRect(hDC, r.left, r.top, r.right, r.bottom, clrFg);

	RECT rcBg = { x, rc.top, rc.right, rc.bottom };
	IntersectRect(&r, &rcBg, &rcClip);
	DrawSolidRect(hDC, r.left, r.top, r.right, r.bottom, clrBg);
}

void DrawGradRect(HDC hDC, int left, int top, int right, int bottom, COLORREF clr1, COLORREF clr2, int st = 0, int wd = -1) {
	int r1 = GetRValue(clr1);
	int g1 = GetGValue(clr1);
	int b1 = GetBValue(clr1);

	int r2 = GetRValue(clr2);
	int g2 = GetGValue(clr2);
	int b2 = GetBValue(clr2);

	int w = right - left;
	if (w <= 0) return;

	if (wd == -1)
		wd = w;
	for (int i = left, j = st; i <= right; i++, j++) {
		int r = r1 + (j * (r2-r1) / wd);
		int g = g1 + (j * (g2-g1) / wd);
		int b = b1 + (j * (b2-b1) / wd);
		COLORREF rgb = RGB(r, g, b);

		HPEN pen = CreatePen(PS_SOLID, 1, rgb);
		HGDIOBJ oldPenStep = SelectObject(hDC, pen);

		POINT pts[] = {
			{ i, top },
			{ i, bottom },
		};
		Polyline(hDC, pts, countof(pts));

		SelectObject(hDC, oldPenStep);
		DeleteObject(pen);
	}
}

void DrawGradient(HDC hDC, RECT &rc, RECT &rcClip, SYSTEM_POWER_STATUS_EX2 &sps) {
	int wd = rc.right - rc.left;
	int batPercent;

	int batState = GetBatteryState(sps);
	if (batState == BATTERY_STATE_ON_AC ||
		(batState == BATTERY_STATE_CHARGING && !Config.ChargeIndicator))
	{
		batPercent = sps.BatteryLifePercent;
		int halfX = (int) (rc.left + (wd * (batPercent / 2) / 100.0));
		int batX = (int) (rc.left + (batPercent * wd / 100.0));

		COLORREF clr1;
		if (batState == BATTERY_STATE_ON_AC)
			clr1 = Config.ClrFgAC;
		else
			clr1 = Config.ClrFgCharging;
		double h, s, l;
		rgb2hsl(clr1, h, s, l);
		l = 0.8;
		COLORREF clr2 = hsl2rgb(h, s, l);
		int w = halfX - rc.left;

		RECT r;
		RECT rcP[] = {
			{ rc.left, rc.top, halfX, rc.bottom },
			{ halfX, rc.top, batX, rc.bottom }
		};

		IntersectRect(&r, &rcP[0], &rcClip);
		DrawGradRect(hDC, r.left, r.top, r.right, r.bottom, clr1, clr2, r.left - rc.left, w);

		IntersectRect(&r, &rcP[1], &rcClip);
		DrawGradRect(hDC, r.left, r.top, r.right, r.bottom, clr2, clr1, r.left - halfX, w);

		// bg
		RECT rcBg = { batX, rc.top, rc.right, rc.bottom };
		IntersectRect(&r, &rcBg, &rcClip);
		DrawSolidRect(hDC, r.left, r.top, r.right, r.bottom, Config.ClrBgCharging);
	}
	else {
		RECT r;
		int critX = (int) (rc.left + (wd * Config.CriticalLevel / 100.0));
		int lowX = (int) (rc.left + (wd * Config.LowLevel / 100.0));

		if (Config.ChargeIndicator && IsCharging(sps))
			batPercent = ChargeXOfs;
		else
			batPercent = sps.BatteryLifePercent;

		COLORREF clrFg, clrBg;
		int batX = (int) (rc.left + (batPercent * wd / 100.0));
		int left = rcClip.left;

		RECT rcCrit = { rc.left, rc.top, min(critX, batX), rc.bottom };
		IntersectRect(&r, &rcCrit, &rcClip);

		if (batState == BATTERY_STATE_CRITICAL) {
			if (Config.BlinkingCritical && BlinkState == 1)
				clrFg = Config.ClrBgCritical;
			else
				clrFg = Config.ClrFgCritical;
			clrBg = Config.ClrBgCritical;
		}
		else {
			clrFg = Config.ClrFgCritical;
			clrBg = Config.ClrBg;
		}

		DrawSolidRect(hDC, r.left, r.top, r.right, r.bottom, clrFg);

		if (batX > critX) {
			RECT rcLow = { critX, rc.top, min(lowX, batX), rc.bottom };
			IntersectRect(&r, &rcLow, &rcClip);

			int w = wd * (Config.LowLevel - Config.CriticalLevel) / 100;
			DrawGradRect(hDC, r.left, r.top, r.right, r.bottom, Config.ClrFgCritical, Config.ClrFgLow, r.left - critX, w);
		}

		if (batX > lowX) {
			RECT rcNorm = { lowX, rc.top, min(rc.left, batX), rc.bottom };
			IntersectRect(&r, &rcNorm, &rcClip);

			int w = wd * (100 - Config.LowLevel) / 100;
			DrawGradRect(hDC, r.left, r.top, r.right, r.bottom, Config.ClrFgLow, Config.ClrFg, r.left - lowX, w);
		}

		if (batX < rc.right) {
			// bg
			RECT rcBg = { batX, rc.top, rc.right, rc.bottom };
			IntersectRect(&r, &rcBg, &rcClip);
			DrawSolidRect(hDC, r.left, r.top, r.right, r.bottom, clrBg);
		}
	}
}

void Paint(HDC hDC, RECT &rc, RECT &rcClip, SYSTEM_POWER_STATUS_EX2 &sps) {
//	int saveDC = SaveDC(hDC);

	if (sps.BatteryLifePercent >= 0 && sps.BatteryLifePercent <= 100)
		LastBatteryLifePercent = sps.BatteryLifePercent;
	else
		sps.BatteryLifePercent = LastBatteryLifePercent;

	// frame
	if (Config.HasFrame) {
		rc.bottom--;
		// frame
		HPEN penFrame = CreatePen(PS_SOLID, 1, Config.ClrFrame);
		HGDIOBJ oldPenFrame = SelectObject(hDC, penFrame);

		POINT pts[] = {
			{ rc.left, rc.bottom },
			{ rc.right - 1, rc.bottom },
			{ rc.right - 1, rc.top - 1 }
		};
		Polyline(hDC, pts, countof(pts));

		SelectObject(hDC, oldPenFrame);
		DeleteObject(penFrame);
	}

	if (Config.GradientIndicator)
		DrawGradient(hDC, rc, rcClip, sps);
	else
		DrawFlat(hDC, rc, rcClip, sps);

	// steps
	if (Config.ShowPercentageSteps) {
		int batPercent;
		if (Config.ChargeIndicator && IsCharging(sps))
			batPercent = ChargeXOfs;
		else
			batPercent = sps.BatteryLifePercent;

		double onePercent = (double) (rc.right - rc.left) / 100.0;
		for (int i = 0; i <= batPercent; i += Config.PercentageStep) {
			int x = rc.left + (int) (i * onePercent);

			if (rcClip.left <= x && x <= rcClip.right) {
				POINT pts[] = {
					{ x, rc.top },
					{ x, rc.bottom },
				};

				COLORREF c = GetPixel(hDC, x, rc.top);

				double h, s, l;
				rgb2hsl(c, h, s, l);

				l = l * 2 / 3.0;
				c = hsl2rgb(h, s, l);

				HPEN penStep = CreatePen(PS_SOLID, 1, c);
				HGDIOBJ oldPenStep = SelectObject(hDC, penStep);

				Polyline(hDC, pts, countof(pts));

				SelectObject(hDC, oldPenStep);
				DeleteObject(penStep);
			}
		}
	}

//	RestoreDC(hDC, saveDC);
}


//
// OnPaint
//
void OnPaint(HWND hWnd) {
	// paint
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hWnd, &ps);

	RECT rc;
	GetClientRect(hWnd, &rc);

	RECT rcClip;
	GetClipBox(hDC, &rcClip);
	Paint(OffscreenDC, rc, rcClip, SPS);

	int wd = rcClip.right - rcClip.left;
	int ht = rcClip.bottom - rcClip.top;
	BitBlt(hDC, rcClip.left, rcClip.top, wd, ht, OffscreenDC, rcClip.left, rcClip.top, SRCCOPY);

	EndPaint(hWnd, &ps);
}

//
// OnPaintOnTop
//
void OnPaintOnTop() {
	// paint
	RECT rc = { SCALEX(Config.LeftMargin), 0, GetDeviceCaps(ScreenDC, HORZRES), SCALEY(Config.Height) };
	RECT rcClip = rc;

	int wd = rc.right - rc.left;
	int ht = rc.bottom - rc.top;

	Paint(OffscreenDC, rc, rcClip, SPS);
	BitBlt(ScreenDC, 0, 0, wd, ht, OffscreenDC, 0, 0, SRCCOPY);
}

void BattiInvalidateRect(HWND hWnd, const RECT *lpRect, BOOL bErase) {
	if (Config.AlwaysOnTop)
		OnPaintOnTop();
	else
		InvalidateRect(hWnd, lpRect, bErase);
}

void CheckEvents() {
	// check events
	int prevState = GetBatteryState(LastSPS);
	int curState = GetBatteryState(SPS);

	// entering LOW battery state
	if (prevState != BATTERY_STATE_LOW && curState == BATTERY_STATE_LOW) {
		ExecuteSoundEvent(&Config.SoundEvents[SOUND_EVENT_BATTERY_LOW]);
		BattiInvalidateRect(HMain, NULL, TRUE);
	}
	// entering CRITICAL battery state
	if (prevState != BATTERY_STATE_CRITICAL && curState == BATTERY_STATE_CRITICAL) {
		ExecuteSoundEvent(&Config.SoundEvents[SOUND_EVENT_BATTERY_CRITICAL]);
		BattiInvalidateRect(HMain, NULL, TRUE);

		KillTimer(HMain, BlinkCritTimer);
		if (Config.BlinkingCritical)
			SetTimer(HMain, BlinkCritTimer, BLINK_INTERVAL, NULL);
	}
	// leaving CRITICAL battery state
	if (prevState == BATTERY_STATE_CRITICAL && curState != BATTERY_STATE_CRITICAL) {
		KillTimer(HMain, BlinkCritTimer);
	}

	// entering POWEROFF battery state
	if (prevState != BATTERY_STATE_POWEROFF && curState == BATTERY_STATE_POWEROFF) {
		ExecuteSoundEvent(&Config.SoundEvents[SOUND_EVENT_POWER_OFF]);
		PostMessage(HMain, UWM_POWEROFF, 0, 0);
	}

	// leaving POWEROFF battery state
	if (prevState == BATTERY_STATE_POWEROFF && curState != BATTERY_STATE_POWEROFF) {
		SHNotificationRemove(&CLSID_PowerOffNotification, POWEROFF_NOTIFICATION_ID);
	}

	// charging started
	if (!IsCharging(LastSPS) && IsCharging(SPS)) {
		ExecuteSoundEvent(&Config.SoundEvents[SOUND_EVENT_CHARGING_STARTED]);

		// start charging timer
		ChargeXOfs = 0;
		SetTimer(HMain, ChargeTimer, CHARGE_INTERVAL, NULL);
		BattiInvalidateRect(HMain, NULL, TRUE);
	}

	// charging stopped
	if (IsCharging(LastSPS) && !IsCharging(SPS)) {
		// battery charged
		if (SPS.BatteryLifePercent == 100) {
			ExecuteSoundEvent(&Config.SoundEvents[SOUND_EVENT_BATERY_CHARGED]);
		}

		KillTimer(HMain, ChargeTimer);
		BattiInvalidateRect(HMain, NULL, TRUE);

/*		// charging ends -> empty the predict buffer
		T = 0;
		SampleCount = 0;
		SampleIdx = 0;
		memset(PredictBuffer, 0, sizeof(PredictBuffer));
*/
	}
	// on AC
	if (!IsOnAC(LastSPS) && IsOnAC(SPS)) {
		// modify icon
		ModifyTrayIcon(HMain, IDI_TRAY_AC);
		BattiInvalidateRect(HMain, NULL, TRUE);
	}
	// on battery
	if (IsOnAC(LastSPS) && !IsOnAC(SPS)) {
		// modify icon
		ModifyTrayIcon(HMain, IDI_TRAY_BATTERY);
		BattiInvalidateRect(HMain, NULL, TRUE);
	}

	if (LastSPS.BatteryLifePercent != SPS.BatteryLifePercent) {
		// redraw
		RECT rc;
		GetClientRect(HMain, &rc);
		int wd = rc.right - rc.left;

		rc.left = (wd * (LastSPS.BatteryLifePercent)) / 100;
		rc.right = (wd * (SPS.BatteryLifePercent)) / 100;

		BattiInvalidateRect(HMain, &rc, TRUE);
	}

	LastSPS = SPS;
}

void RepositionWindow() {
	// get taskbar size and reposition the window
	RECT rc = { 0 };
	GetClientRect(HTaskBar, &rc);
	rc.left += SCALEX(Config.LeftMargin);

	int wd = rc.right - rc.left;
	int ht = SCALEY(Config.Height);
	DestroyOffscreenBuffer();
	CreateOffscreenBuffer(wd, ht);

	SetWindowPos(HMain, NULL, rc.left, rc.top, rc.right - rc.left, SCALEY(Config.Height), SWP_NOZORDER);
}

int OnTimer(UINT timerId) {
	if (timerId == Timer) {
		UpdateBatteryStatus();
		CheckEvents();

		if (IsWindow(HMain)) {
			// update window size (to reflect landscape/portrait mode)
			int wd = GetDeviceCaps(ScreenDC, HORZRES);
			if (ScreenWd != wd) {
				RepositionWindow();
				ScreenWd = wd;
				BattiInvalidateRect(HMain, NULL, FALSE);
			}
		}
 		return 0;
	}
	else if (timerId == ChargeTimer) {
		if (Config.ChargeIndicator && IsCharging(SPS)) {
			// charging
			int tmp = ChargeXOfs;
			ChargeXOfs += 5;
			if (ChargeXOfs >= SPS.BatteryLifePercent + 5)
				ChargeXOfs = 0;
			else if (ChargeXOfs >= SPS.BatteryLifePercent)
				// show the last (incomplete) step
				ChargeXOfs = SPS.BatteryLifePercent;

			RECT rc;
			GetClientRect(HMain, &rc);
			int wd = rc.right - rc.left;

			if (tmp < ChargeXOfs) {
				rc.left = (wd * tmp) / 100;
				rc.right = (wd * ChargeXOfs) / 100;
			}
			else {
				rc.left = (wd * ChargeXOfs) / 100;
				rc.right = (wd * tmp) / 100;
			}
			BattiInvalidateRect(HMain, &rc, FALSE);
		}

		return 0;
	}
	else if (timerId == BlinkCritTimer) {
		if (SPS.ACLineStatus == 0 && SPS.BatteryLifePercent <= Config.CriticalLevel) {
			// we are in the critical level and not on AC power source
			BlinkState = (BlinkState + 1) % 2;

			RECT rc;
			GetClientRect(HMain, &rc);
			int wd = rc.right - rc.left;
			rc.right = (wd * SPS.BatteryLifePercent) / 100;

			BattiInvalidateRect(HMain, &rc, TRUE);
		}
		return 0;
	}
	else if (timerId == PowerOffTimer) {
		// turn the device off
		KillTimer(HMain, PowerOffTimer);
		SHNotificationRemove(&CLSID_PowerOffNotification, POWEROFF_NOTIFICATION_ID);
		TurnOffDevice();
		return 0;
	}
	else if (timerId == AlwaysOnTopTimer) {
		OnPaintOnTop();
		return 0;
	}
#ifdef LOGGING
	else if (timerId == LogTimer) {
/*		SYSTEM_POWER_STATUS_EX2 sps;
		GetSystemPowerStatusEx2(&sps, sizeof(sps), TRUE);

		char a[2048];
		SYSTEMTIME now;
		GetLocalTime(&now);
		sprintf(a, "%02d:%02d:%02d %d %d - %d%%, %dmV, %dmA, AVG: %dmA, %dmAh\n", now.wHour, now.wMinute, now.wSecond,
			sps.ACLineStatus, sps.BatteryFlag,
			sps.BatteryLifePercent, sps.BatteryVoltage, sps.BatteryCurrent, sps.BatteryAverageCurrent,
			sps.BatterymAHourConsumed);
		DWORD written;
		WriteFile(hLogFile, a, strlen(a), &written, NULL);
*/
	}
#endif
/*
	else if (timerId == PredictorTimer) {
		SYSTEM_POWER_STATUS_EX2 sps;
		GetSystemPowerStatusEx2(&sps, sizeof(sps), TRUE);

		if (sps.ACLineStatus == 0) {
			if (sps.BatteryLifePercent != 0xFF) {
				T++;

				BYTE value;
				if (SampleCount < PREDICT_BUFFER_SIZE) {
					SampleCount++;
				}
				else {
					value = PredictBuffer[SampleIdx];
					DWORD t = T - PREDICT_BUFFER_SIZE;

					SumY -= value;
					SumX -= t;
					SumX2 -= t * t;
					SumXY -= value * t;
				}

				value = sps.BatteryLifePercent;

				// insert new value into the buffer
				PredictBuffer[SampleIdx] = value;

				SumY += value;
				SumX += T;
				SumX2 += T * T;
				SumXY += value * T;

				double da = ((SampleCount * SumXY) - (SumX * SumY)) / ((SampleCount * SumX2) - (SumX * SumX));
				if (da != 0) {
					PredictA = da;

					HKEY hConfig;
					if (RegOpenKeyEx(HKEY_CURRENT_USER, szRoot, 0, 0, &hConfig) == ERROR_SUCCESS) {
						RegSetValueEx(hConfig, szPredictA, 0, REG_BINARY, (BYTE *) &PredictA, sizeof(double));
						RegCloseKey(hConfig);
					}
				}

				SampleIdx = (SampleIdx + 1) % PREDICT_BUFFER_SIZE;

#ifdef LOGGING
				SYSTEMTIME now;
				GetLocalTime(&now);

				char a[2048];
				DWORD written;
				sprintf(a, "%02d:%02d:%02d ", now.wHour, now.wMinute, now.wSecond);
				WriteFile(hLogFile, a, strlen(a), &written, NULL);

/*				for (int i = 0; i < PREDICT_BUFFER_SIZE; i++) {
					sprintf(a, "%03d ", PredictBuffer[i]);
					WriteFile(hLogFile, a, strlen(a), &written, NULL);
				}

				sprintf(a, "| %d | %lf\n", SampleIdx, da);
*/
/*				sprintf(a, "| %d | %lf\n", SampleCount, da);
				WriteFile(hLogFile, a, strlen(a), &written, NULL);
#endif
			}
		}
	}
*/

	return 1;
}

LRESULT OnCommand(WORD wNotifyCode, WORD wID, LPARAM lParam) {
	switch (wID) {
		// low power notification dialog
		case ID_POWEROFF:
			SHNotificationRemove(&CLSID_PowerOffNotification, POWEROFF_NOTIFICATION_ID);
			KillTimer(HMain, PowerOffTimer);
			TurnOffDevice();
			break;

		case ID_HIDE:
			KillTimer(HMain, PowerOffTimer);
			break;

		case ID_SETTINGS:
		case ID_APP_SETTINGS:
			OpenConfig();
			break;

		case ID_APP_INFO:
			OpenInfo();
			break;

		case ID_APP_EXIT:
			PostQuitMessage(0);
			break;
	}

	return 0;
}

LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam) {
    if (wParam != NID.uID)
        return 0L;

	if (LOWORD(lParam) == WM_LBUTTONUP) {
		HMENU hMenu = ::LoadMenu(HInstance, MAKEINTRESOURCE(NID.uID));
		if (!hMenu)
			return 0;

		HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
		if (!hSubMenu) {
			::DestroyMenu(hMenu);        // Be sure to Destroy Menu Before Returning
			return 0;
		}
		LocalizeMenu(hSubMenu);

		// Display and track the popup menu
		POINT pos;
		HDC hdc = GetDC(NULL);
		pos.x = GetDeviceCaps(hdc, HORZRES);
		pos.y = GetDeviceCaps(hdc, VERTRES) - SCALEY(26);
		if (IsOnWM5())
			pos.y -= SCALEY(20);
		ReleaseDC(NULL, hdc);

		::SetForegroundWindow(NID.hWnd);
		::TrackPopupMenu(hSubMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, pos.x, pos.y, 0, HMain, NULL);

		// BUGFIX: See "PRB: Menus for Notification Icons Don't Work Correctly"
//		::PostMessage(HMain, WM_NULL, 0, 0);

		DestroyMenu(hMenu);
	}

	return 1;
}


//
// MainWndProc
//
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == ReadConfigMessage) {
		BOOL sysTrayIcon = Config.SysTrayIcon;
		int leftMargin = Config.LeftMargin;
		int height = Config.Height;

		Config.Load();
		Config.LoadColors();

		// position of a Batti window need to be changed
		if (leftMargin != Config.LeftMargin || height != Config.Height)
			RepositionWindow();
		// redraw
		BattiInvalidateRect(HMain, NULL, TRUE);

		// always on top timer
		KillTimer(HMain, AlwaysOnTopTimer);
		if (Config.AlwaysOnTop)
			SetTimer(HMain, AlwaysOnTopTimer, ALWAYS_ON_TOP_UPDATE_INTERVAL, NULL);

		if (sysTrayIcon != Config.SysTrayIcon) {
			if (Config.SysTrayIcon)
				AddTrayIcon(HMain);
			else
				RemoveTrayIcon();
		}

		return 0;
	}
	else {
		switch (msg) {
			case WM_ERASEBKGND:
				return 0;

			case WM_PAINT:
				if (Config.AlwaysOnTop) {
					// validate the underlaying window to prevent permanent redrawing
					RECT rc = { 0 };
					GetClientRect(hWnd, &rc);
					ValidateRect(hWnd, &rc);
				}
				else
					OnPaint(hWnd);

				return 0;

			case WM_TIMER:
				return OnTimer(wParam);

			case WM_LBUTTONDOWN:
				OnAction();
				return 0;

			case WM_COMMAND:
				return OnCommand(HIWORD(wParam), LOWORD(wParam), lParam);

			case WM_SIZE:
				InvalidateRect(hWnd, NULL, FALSE);
				break;

			case WM_CLOSE:
				PostQuitMessage(0);
				break;

			case UWM_POWEROFF:
				PowerOffNotification();
				return 0;

			case UWM_ICON_NOTIFY:
				return OnTrayNotify(wParam, lParam);
		} // switch

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	HInstance = hInstance;

	HANDLE hMutex = CreateMutex(NULL, TRUE, WINDOW_CLASS WINDOW_NAME);
	if (hMutex == NULL)
		return 0;
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return 0;

	ReadConfigMessage = RegisterWindowMessage(READ_CONFIG_MESSAGE);

 	HIDPI_InitScaling();

	// task bar
	HTaskBar = FindTaskBar();

	if (!StringBuffer) StrAct = StringBuffer = new TCHAR [STRING_BUFFER_SIZE];
	if (!StringBuffer)
		return FALSE;

	// get screen device context for AlwaysOnTop painting
	ScreenDC = GetDC(NULL);

	// config
	Config.Load();
	Config.LoadColors();
	Config.LoadCoefs();

	if (wcscmp(lpCmdLine, _T("/nodelay")) != 0) {
		// start-up delay
		Sleep(Config.StartupDelay * 1000);
	}

	if (!Localization) {
		Localization = new CLocalization();
		Localization->Init();
	}

	// register class
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = MainWndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
	wc.lpszClassName = WINDOW_CLASS;
	RegisterClass(&wc);

	//
	ScreenWd = GetDeviceCaps(ScreenDC, HORZRES);

	// create window
	RECT rc = { 0 };
	GetClientRect(HTaskBar, &rc);
	rc.left += SCALEX(Config.LeftMargin);

	int wd = rc.right - rc.left;
	int ht = SCALEY(Config.Height);
	HMain = CreateWindowEx(WS_EX_TOPMOST, WINDOW_CLASS, WINDOW_NAME, WS_CHILD | WS_VISIBLE,
		rc.left, rc.top, wd, ht, HTaskBar, NULL, hInstance, NULL);
	SetWindowPos(HMain, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// offscreen buffer
	CreateOffscreenBuffer(wd, ht);

	if (Config.SysTrayIcon)
		AddTrayIcon(HMain);

	if (lpCmdLine != 0 && wcslen(lpCmdLine) != 0) {
		COPYDATASTRUCT cs;
		cs.dwData = 0;
		cs.cbData = 2 + 2 * wcslen(lpCmdLine);
		cs.lpData = lpCmdLine;
		SendMessage(HMain, WM_COPYDATA, NULL, (LPARAM) &cs);
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);

#ifdef LOGGING
//	hLogFile = CreateFile(_T("\\Storage Card\\Program Files\\Batti\\batti.log"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	hLogFile = CreateFile(_T("\\Program Files\\Batti\\batti.log"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	SetTimer(HMain, LogTimer, 10000, NULL);

/*	if (hLogFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, _T("Hovno"), _T("DBG"), MB_OK);
	}
	else
		MessageBox(NULL, _T("Ok"), _T("DBG"), MB_OK);
*/
#endif

	CreateFonts();

	SetTimer(HMain, Timer, GENERAL_UPDATE_INTERVAL, NULL);

	memset(&LastSPS, 0, sizeof(LastSPS));
	LastSPS.BatteryLifePercent = -1;
	UpdateBatteryStatus();
	CheckEvents();

	ShowWindow(HMain, SW_SHOW);
	UpdateWindow(HMain);

	if (Config.AlwaysOnTop) {
		SetTimer(HMain, AlwaysOnTopTimer, ALWAYS_ON_TOP_UPDATE_INTERVAL, NULL);
		BattiInvalidateRect(HMain, NULL, TRUE);
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	KillTimer(HMain, Timer);
	KillTimer(HMain, ChargeTimer);
	KillTimer(HMain, BlinkCritTimer);
	KillTimer(HMain, PowerOffTimer);
	KillTimer(HMain, AlwaysOnTopTimer);

#ifdef LOGGING
	KillTimer(HMain, LogTimer);
	CloseHandle(hLogFile);
#endif

	delete Localization;

	// free memory
	delete [] StringBuffer;
	StringBuffer = NULL;

	DestroyOffscreenBuffer();

	// release screen device context
	ReleaseDC(NULL, ScreenDC);

	SHNotificationRemove(&CLSID_PowerOffNotification, POWEROFF_NOTIFICATION_ID);

	if (Config.SysTrayIcon)
		RemoveTrayIcon();

	DeleteObject(HUnderlineFont);
	DeleteObject(HBoldFont);

	return msg.wParam;
}

