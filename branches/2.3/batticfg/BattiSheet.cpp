/**
 *  CeDialog.cpp : implementation file
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
#include "batticfg.h"
#include "BattiSheet.h"

#include "..\share\Config.h"
#include "..\share\Localization.h"

#include "AboutPg.h"
#include "LicensePg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBattiSheet

IMPLEMENT_DYNAMIC(CBattiSheet, CCePropertySheet)

CBattiSheet::CBattiSheet(UINT iSelectPage)
	:CCePropertySheet(IDS_BATTI, NULL, iSelectPage)
{
	SetMenu(IDR_MAIN);
	Init();

	CLocalizedString sTitle;
	if (sTitle.LoadString(IDS_OPTIONS))
		m_strTitle = sTitle;
}

CBattiSheet::~CBattiSheet() {
}


BEGIN_MESSAGE_MAP(CBattiSheet, CCePropertySheet)
	//{{AFX_MSG_MAP(CBattiSheet)
	ON_COMMAND(ID_APP_ABOUT, OnAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBattiSheet message handlers

void CBattiSheet::Init() {
	AddPage(&m_pgGeneral);
	AddPage(&m_pgAdvanced);
	AddPage(&m_pgAppearance);
}

BOOL CBattiSheet::OnInitDialog() {
	CCePropertySheet::OnInitDialog();
	
	SHINITDLGINFO sid;
	sid.dwMask   = SHIDIM_FLAGS;
	sid.dwFlags  = SHIDIF_SIZEDLGFULLSCREEN;
	sid.hDlg     = GetSafeHwnd();
	SHInitDialog(&sid);
	
	//
	LocalizeMenubar(m_hwndCmdBar);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBattiSheet::OnAbout() {
	CAboutPg pgAbout;
	CLicensePg pgLicense;

	CCePropertySheet sheet(IDS_ABOUT);
	sheet.AddPage(&pgAbout);
	sheet.AddPage(&pgLicense);
	sheet.SetMenu(IDR_CANCEL);
	sheet.DoModal();
}
