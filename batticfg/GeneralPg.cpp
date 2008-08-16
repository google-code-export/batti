/**
 *  GeneralPg.cpp : implementation file
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
#include "GeneralPg.h"

#include "../share/Config.h"
#include "../share/Localization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBattiGeneralPg dialog

CBattiGeneralPg::CBattiGeneralPg() : CPropertyPage(CBattiGeneralPg::IDD) {
	//{{AFX_DATA_INIT(CBattiGeneralPg)
	m_bChargeIndicator = Config.ChargeIndicator;
	m_nTapAction = Config.TapAction;
	m_bBlinkingCritical = Config.BlinkingCritical;
	m_bAlwaysOnTop = Config.AlwaysOnTop;
	m_bSystrayIcon = Config.SysTrayIcon;
	//}}AFX_DATA_INIT

	TCHAR *strTitle = Localization->GetStr(IDD);
	if (strTitle != NULL) {
		m_psp.dwFlags |= PSP_USETITLE;
		m_psp.pszTitle = strTitle;
	}
}

CBattiGeneralPg::~CBattiGeneralPg() {
}

void CBattiGeneralPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBattiGeneralPg)
//	DDX_Control(pDX, IDC_ACTION_ON_TAP_LBL, m_ctlActionOnTapLbl);
	DDX_Control(pDX, IDC_TAP_ACTION, m_ctlTapAction);
	DDX_Control(pDX, IDC_OPTIONS_LBL, m_ctlOptionsLbl);
	DDX_Check(pDX, IDC_CHARGE_INDICATOR, m_bChargeIndicator);
	DDX_CBIndex(pDX, IDC_TAP_ACTION, m_nTapAction);
	DDX_Check(pDX, IDC_BLINKING, m_bBlinkingCritical);
	DDX_Check(pDX, IDC_ALWAYSONTOP, m_bAlwaysOnTop);
	DDX_Check(pDX, IDC_SYSTRAY_ICON, m_bSystrayIcon);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBattiGeneralPg, CPropertyPage)
	//{{AFX_MSG_MAP(CBattiGeneralPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBattiGeneralPg message handlers

struct CActionData {
	UINT ID;
	DWORD Data;

	CActionData(UINT id, DWORD data) {
		ID = id;
		Data = data;
	}
};

BOOL CBattiGeneralPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	LocalizeDialog(GetSafeHwnd(), IDD);

	CenterWindow(GetDesktopWindow());	// center to the hpc screen

	// insert action strings
	// indices must match the TAP_ACTION_XXX
	CActionData actions[] = {
		CActionData(IDS_NO_ACTION, TAP_ACTION_NOTHING),
		CActionData(IDS_SHOW_INFO, TAP_ACTION_SHOW_INFO),
		CActionData(IDS_OPEN_CONFIG, TAP_ACTION_OPEN_CONFIG)
	};
	for (int i = 0; i < countof(actions); i++) {
		CLocalizedString sAction;
		sAction.LoadString(actions[i].ID);
		int item = m_ctlTapAction.AddString(sAction);
		m_ctlTapAction.SetItemData(item, actions[i].Data);

		if (actions[i].Data == (UINT) Config.TapAction)
			m_ctlTapAction.SetCurSel(item);
	}
	// nothing selected -> select the first item
	if (m_ctlTapAction.GetCurSel() == CB_ERR)
		m_ctlTapAction.SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBattiGeneralPg::OnOK() {
	if (UpdateData()) {
		Config.ChargeIndicator = m_bChargeIndicator;
		Config.BlinkingCritical = m_bBlinkingCritical;
		Config.AlwaysOnTop = m_bAlwaysOnTop;
		Config.SysTrayIcon = m_bSystrayIcon;

		Config.TapAction = m_ctlTapAction.GetItemData(m_nTapAction);
	}

	CPropertyPage::OnOK();
}

BOOL CBattiGeneralPg::OnSetActive() {
	Config.Page = 0;

	return CPropertyPage::OnSetActive();
}

