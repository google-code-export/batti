/**
 *  BattiCfg.cpp : Defines the class behaviors for the application.
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
#include "BattiCfg.h"
#include "BattiSheet.h"

#include "../share/Config.h"
#include "../share/Localization.h"
#include "../share/defs.h"
#include "../share/helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT ReadConfigMessage;

/////////////////////////////////////////////////////////////////////////////
// CBatticfgApp

BEGIN_MESSAGE_MAP(CBatticfgApp, CWinApp)
	//{{AFX_MSG_MAP(CBatticfgApp)
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatticfgApp construction

CBatticfgApp::CBatticfgApp()
	: CWinApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBatticfgApp object

CBatticfgApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBatticfgApp initialization

BOOL CBatticfgApp::InitInstance() {
	// Standard initialization
	HIDPI_InitScaling();

//	GetDataMessage = RegisterWindowMessage(BATTI_GET_DATA_MESSAGE);
	ReadConfigMessage = RegisterWindowMessage(READ_CONFIG_MESSAGE);

	Config.Load();
	Config.LoadColors();

	Localization = new CLocalization();
	Localization->Init();

	CBattiSheet sheet(Config.Page);
	sheet.SetActivePage(Config.Page);
	if (sheet.DoModal()) {
		Config.Save();

		// notify batti's windows to take changes
		HWND hBatti = FindBattiWindow();
		::SendMessage(hBatti, ReadConfigMessage, 0, 0);
	}

	delete Localization;

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
