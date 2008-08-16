/**
 *  AppearancePg.cpp : implementation file
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
#include "AppearancePg.h"
#include "ColorPickerDlg.h"

#include "../share/Config.h"
#include "../share/Localization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COLOR_COUNT						10

/////////////////////////////////////////////////////////////////////////////
// CBattiAppearancePg property page

IMPLEMENT_DYNCREATE(CBattiAppearancePg, CPropertyPage)

CBattiAppearancePg::CBattiAppearancePg() : CPropertyPage(CBattiAppearancePg::IDD) {
	//{{AFX_DATA_INIT(CBattiAppearancePg)
	m_bHasFrame = Config.HasFrame;
	m_nLeftMargin = Config.LeftMargin;
	m_nHeight = Config.Height;
	m_bGradientIndicator = Config.GradientIndicator;
	m_bShowSteps = Config.ShowPercentageSteps;
	//}}AFX_DATA_INIT

	// !!! UPDATE the COLOR_COUNT constant at the beginnging of this file !!!!
	m_arColors = new CColorItem[COLOR_COUNT];
	m_arColors[0].Set(IDS_FOREGROUND, &Config.ClrFg, Config.ClrFg);
	m_arColors[1].Set(IDS_BACKGROUND, &Config.ClrBg, Config.ClrBg);
	m_arColors[2].Set(IDS_LOW_FG, &Config.ClrFgLow, Config.ClrFgLow);
	m_arColors[3].Set(IDS_LOW_BG, &Config.ClrBgLow, Config.ClrBgLow);
	m_arColors[4].Set(IDS_CRITICAL_FG, &Config.ClrFgCritical, Config.ClrFgCritical);
	m_arColors[5].Set(IDS_CRITICAL_BG, &Config.ClrBgCritical, Config.ClrBgCritical);
	m_arColors[6].Set(IDS_CHARGING_FG, &Config.ClrFgCharging, Config.ClrFgCharging);
	m_arColors[7].Set(IDS_CHARGING_BG, &Config.ClrBgCharging, Config.ClrBgCharging);
	m_arColors[8].Set(IDS_AC, &Config.ClrFgAC, Config.ClrFgAC);
	m_arColors[9].Set(IDS_FRAME, &Config.ClrFrame, Config.ClrFrame);

	TCHAR *strTitle = Localization->GetStr(IDD);
	if (strTitle != NULL) {
		m_psp.dwFlags |= PSP_USETITLE;
		m_psp.pszTitle = strTitle;
	}
}

CBattiAppearancePg::~CBattiAppearancePg() {
	delete [] m_arColors;
}

void CBattiAppearancePg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBattiAppearancePg)
	DDX_Control(pDX, IDC_OPTIONS_LBL, m_ctlOptionsLbl);
	DDX_Control(pDX, IDC_HEIGHT_SPIN, m_ctlHeightSpin);
	DDX_Control(pDX, IDC_LEFTMARGIN_SPIN, m_ctlLeftMarginSpin);
	DDX_Control(pDX, IDC_COLOR_NAME, m_ctlColorName);
	DDX_Control(pDX, IDC_CLR, m_ctlColor);
	DDX_Control(pDX, IDC_COLOR_LBL, m_ctlColorLbl);
	DDX_Check(pDX, IDC_HAS_FRAME, m_bHasFrame);
	DDX_Text(pDX, IDC_LEFTMARGIN, m_nLeftMargin);
	DDX_Text(pDX, IDC_HEIGHT, m_nHeight);
	DDV_MinMaxInt(pDX, m_nHeight, 1, 26);
	DDX_Check(pDX, IDC_GRADIENT, m_bGradientIndicator);
	DDX_Check(pDX, IDC_STEPS, m_bShowSteps);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBattiAppearancePg, CPropertyPage)
	//{{AFX_MSG_MAP(CBattiAppearancePg)
	ON_BN_CLICKED(IDC_CLR, OnClr)
	ON_CBN_SELENDOK(IDC_COLOR_NAME, OnSelendokColorName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBattiAppearancePg message handlers

void CBattiAppearancePg::OnOK() {
	Config.LeftMargin = m_nLeftMargin;
	Config.Height = m_nHeight;

	// save values to the config
	for (int i = 0; i < COLOR_COUNT; i++)
		*(m_arColors[i].OrigClr) = m_arColors[i].Clr;

	Config.HasFrame = m_bHasFrame;
	Config.ShowPercentageSteps = m_bShowSteps;
	Config.GradientIndicator = m_bGradientIndicator;

	CPropertyPage::OnOK();
}

BOOL CBattiAppearancePg::OnSetActive() {
	Config.Page = 2;

	return CPropertyPage::OnSetActive();
}

BOOL CBattiAppearancePg::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	LocalizeDialog(GetSafeHwnd(), IDD);

	m_ctlLeftMarginSpin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION, 0);
	m_ctlLeftMarginSpin.SetRange(0, 1000);

	m_ctlHeightSpin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION, 0);
	m_ctlHeightSpin.SetRange(1, 30);

	// insert colors
	for (int i = 0; i < COLOR_COUNT; i++) {
		CLocalizedString strName;
		strName.LoadString(m_arColors[i].ID);
		int item = m_ctlColorName.AddString(strName);
		m_ctlColorName.SetItemData(item, (DWORD) &(m_arColors[i]));
	}
	m_ctlColorName.SetCurSel(0);
	OnSelendokColorName();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CBattiAppearancePg::GetColor(COLORREF &clr) {
	CColorPickerDlg dlg(clr);
	if (dlg.DoModal() == IDOK) {
		clr = dlg.m_clrColor;
		return TRUE;
	}
	else
		return FALSE;
}

void CBattiAppearancePg::OnClr() {
	int curSel = m_ctlColorName.GetCurSel();
	CColorItem *ci = (CColorItem *) m_ctlColorName.GetItemData(curSel);
	GetColor(ci->Clr);
	m_ctlColor.SetColor(ci->Clr);
}

void CBattiAppearancePg::OnSelendokColorName() {
	int curSel = m_ctlColorName.GetCurSel();
	CColorItem *ci = (CColorItem *) m_ctlColorName.GetItemData(curSel);
	m_ctlColor.SetColor(ci->Clr);
}

