/**
 *  setup.cpp : Defines the entry point for the DLL application.
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

#include "stdafx.h"
#include "ce_setup.h"
#include "../share/defs.h"
#include "../share/helpers.h"
#include "../share/regs.h"
#include "../share/Config.h"
#include "../share/Localization.h"
#include "resource.h"

LPCTSTR szSeconds = _T("Seconds");
HINSTANCE HInstance;


BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	HInstance = (HINSTANCE) hModule;
    return TRUE;
}

void TerminateBatti() {
	// find batti window
	HWND hBatti = FindBattiWindow();
	if (hBatti != NULL) {
		PostMessage(hBatti, WM_CLOSE, 0, 0);

/*		// end the app
		DWORD processID;
		DWORD threadID = GetWindowThreadProcessId(hBatti, &processID);
		HANDLE hProcess = OpenProcess(0, FALSE, processID);
		if (hProcess != NULL) {
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
		}
*/	}
}

codeINSTALL_INIT
Install_Init(HWND hwndParent, BOOL fFirstCall, BOOL fPreviouslyInstalled, LPCTSTR pszInstallDir) {
	TerminateBatti();	

	return codeINSTALL_INIT_CONTINUE;
}

void Migrate() {
	HKEY hConfig;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, szRoot, 0, 0, &hConfig) == ERROR_SUCCESS) {
		DWORD dwType;
		DWORD dwSize = sizeof(COLORREF);
		COLORREF clr;

		// read foreground color and use it for foreground AC color
		if (RegQueryValueEx(hConfig, szClrFg, NULL, &dwType, (LPBYTE) &clr, &dwSize) == ERROR_SUCCESS) {
			RegSetValueEx(hConfig, szClrFgAC, 0, REG_DWORD, (LPBYTE) &clr, dwSize);
		}

		DWORD dwValue;
		// if 'seconds' key found -> delete it (it is not further needed)
		dwSize = sizeof(DWORD);
		if (RegQueryValueEx(hConfig, szSeconds, NULL, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) {
			RegDeleteValue(hConfig, szSeconds);
		}

		// if action is 'Open config' change it to 'Show info'
		if (RegQueryValueEx(hConfig, szAction, NULL, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) {
			if (dwValue == TAP_ACTION_OPEN_CONFIG) {
				dwValue = TAP_ACTION_SHOW_INFO;
				RegSetValueEx(hConfig, szAction, 0, REG_DWORD, (LPBYTE) &dwValue, dwSize);
			}
		}

		RegCloseKey(hConfig);
	}
}

codeINSTALL_EXIT
Install_Exit(HWND hwndParent, LPCTSTR pszInstallDir, WORD cFailedDirs, WORD cFailedFiles,
	WORD cFailedRegKeys, WORD cFailedRegVals, WORD cFailedShortcuts) {

	// Migrate
	Migrate();

	// write install path
	HKEY hRoot;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, szRoot, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hRoot, &dwDisposition) != ERROR_SUCCESS)
		return codeINSTALL_EXIT_UNINSTALL;

	if (RegSetValueEx(hRoot, szInstallDir, 0, REG_SZ, (LPBYTE) pszInstallDir, sizeof(TCHAR) * (lstrlen(pszInstallDir) + 1)) != ERROR_SUCCESS) {
		RegCloseKey(hRoot);
		return codeINSTALL_EXIT_UNINSTALL;
	}
	RegCloseKey(hRoot);


	PROCESS_INFORMATION pi;
	TCHAR pathName[MAX_PATH + 2];	// include surrounding quotes
	swprintf(pathName, _T("\"%s\\%s\""), pszInstallDir, BATTI_FILENAME);

	// run application on startup
	TCHAR szStartupFolderPath[MAX_PATH];
	::SHGetSpecialFolderPath(NULL, szStartupFolderPath, CSIDL_STARTUP, FALSE);

	TCHAR linkPathName[MAX_PATH];
	swprintf(linkPathName, _T("%s\\batti.lnk"), szStartupFolderPath);
	::SHCreateShortcut(linkPathName, pathName);

	// run the application 
	swprintf(pathName, _T("%s\\%s"), pszInstallDir, BATTI_FILENAME);
	CreateProcess(pathName, _T("/nodelay"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);

	// run config after install
	swprintf(pathName, _T("%s\\%s"), pszInstallDir, BATTI_CONFIG_FILENAME);
	CreateProcess(pathName, _T(""), NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);

	return codeINSTALL_EXIT_DONE;
}

codeUNINSTALL_INIT
Uninstall_Init(HWND hwndParent, LPCTSTR pszInstallDir) {
	TerminateBatti();

	// remove the link
	TCHAR szStartupFolderPath[MAX_PATH];
	::SHGetSpecialFolderPath(NULL, szStartupFolderPath, CSIDL_STARTUP, FALSE);
	TCHAR link[] = L"\\batti.lnk";
	TCHAR *linkPathName = new TCHAR[lstrlen(szStartupFolderPath) + lstrlen(link) + 1];
	wcscpy(linkPathName, szStartupFolderPath);
	wcscat(linkPathName, link);

	codeUNINSTALL_INIT ret;
	if (DeleteFile(linkPathName))
		ret = codeUNINSTALL_INIT_CONTINUE;
	else
		ret = codeUNINSTALL_INIT_CANCEL;

	delete [] linkPathName;

	//
	CLocalization loc;
	loc.Init();

	TCHAR s[MAX_PATH];
	TCHAR *str = loc.GetStr(IDS_KEEP_SETTINGS);
	if (str == NULL) {
		LoadString(HInstance, IDS_KEEP_SETTINGS, s, MAX_PATH);
		str = s + 0;
	}

	if (::MessageBox(hwndParent, str, _T("Batti"), MB_YESNO | MB_SETFOREGROUND) == IDNO) {
		// remove all registry keys (config, etc.)
		RegDeleteKey(HKEY_CURRENT_USER, szRoot);
	}

	return ret;
}

codeUNINSTALL_EXIT
Uninstall_Exit(HWND hwndParent) {
	return codeUNINSTALL_EXIT_DONE;
}
