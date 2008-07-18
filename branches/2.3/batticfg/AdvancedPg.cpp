/**
 *  AdvancedPg.cpp : implementation file
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
#include "AdvancedPg.h"

#include "..\share\Config.h"
#include "..\share\Localization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBattiAdvancedPg property page

IMPLEMENT_DYNCREATE(CBattiAdvancedPg, CPropertyPage)

CBattiAdvancedPg::CBattiAdvancedPg() : CPropertyPage(CBattiAdvancedPg::IDD) {
	//{{AFX_DATA_INIT(CBattiAdvancedPg)
	m_nCritical = Config.CriticalLevel;
	m_nLow = Config.LowLevel;
	m_nPowerOff = Config.PowerOffLevel;
	//}}AFX_DATA_INIT

	TCHAR *strTitle = Localization->GetStr(IDD);
	if (strTitle != NULL) {
		m_psp.dwFlags |= PSP_USETITLE;
		m_psp.pszTitle = strTitle;
	}

	UINT nIDs[SOUND_EVENT_COUNT] = { 
		IDS_BATTERY_LOW,
		IDS_BATTERY_CRITICAL,
		IDS_CHARGING_STARTED,
		IDS_CHARGING_ENDS,
		IDS_POWER_OFF
	};
	m_arSounds = new CSoundEventItem[SOUND_EVENT_COUNT];
	for (int i = 0; i < SOUND_EVENT_COUNT; i++) {
		m_arSounds[i].Set(nIDs[i], &Config.SoundEvents[i], Config.SoundEvents[i]);
	}

	CDC *pDC = GetDC();
	if (pDC->GetDeviceCaps(LOGPIXELSX) > 130) {
		m_ctlPlay.LoadBitmaps(MAKEINTRESOURCE(IDB_PLAY_HI));
		m_ctlStop.LoadBitmaps(MAKEINTRESOURCE(IDB_STOP_HI));
	}
	else {
		m_ctlPlay.LoadBitmaps(MAKEINTRESOURCE(IDB_PLAY));
		m_ctlStop.LoadBitmaps(MAKEINTRESOURCE(IDB_STOP));
	}

	ReleaseDC(pDC);
}

CBattiAdvancedPg::~CBattiAdvancedPg() {
	delete [] m_arSounds;
}

void CBattiAdvancedPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBattiAdvancedPg)
	DDX_Control(pDX, IDC_PLAY, m_ctlPlay);
	DDX_Control(pDX, IDC_STOP, m_ctlStop);
	DDX_Control(pDX, IDC_SOUNDS_LBL, m_ctlSoundsLbl);
	DDX_Control(pDX, IDC_SOUNDFILE, m_ctlSoundFile);
	DDX_Control(pDX, IDC_SOUNDEVENTS, m_ctlSoundEvents);
	DDX_Control(pDX, IDC_PLAYSOUND, m_ctlPlaySound);
	DDX_Control(pDX, IDC_LEVELS_LBL, m_ctlLevelsLbl);
	DDX_Control(pDX, IDC_LOW_SPIN, m_ctlLowSpin);
	DDX_Control(pDX, IDC_CRITICAL_SPIN, m_ctlCriticalSpin);
	DDX_Control(pDX, IDC_POWEROFF_SPIN, m_ctlPowerOffSpin);
	DDX_Text(pDX, IDC_CRITICAL, m_nCritical);
	DDV_MinMaxInt(pDX, m_nCritical, 1, 100);
	DDX_Text(pDX, IDC_LOW, m_nLow);
	DDV_MinMaxInt(pDX, m_nLow, 1, 100);
	DDX_Text(pDX, IDC_POWEROFF, m_nPowerOff);
	DDV_MinMaxInt(pDX, m_nPowerOff, 1, 100);
	//}}AFX_DATA_MAP

	if (m_nCritical > m_nLow) {
		CLocalizedString sMsg;
		sMsg.LoadString(IDS_CRITICAL_BIGGER_THAN_LOW);
		AfxMessageBox(sMsg);
		pDX->Fail();
	}

	if (m_nPowerOff > m_nCritical) {
		CLocalizedString sMsg;
		sMsg.LoadString(IDS_POWEROFF_BIGGER_THAN_CRITICAL);
		AfxMessageBox(sMsg);
		pDX->Fail();
	}
}


BEGIN_MESSAGE_MAP(CBattiAdvancedPg, CPropertyPage)
	//{{AFX_MSG_MAP(CBattiAdvancedPg)
	ON_CBN_SELENDOK(IDC_SOUNDEVENTS, OnSelendokSoundevents)
	ON_CBN_SELENDOK(IDC_SOUNDFILE, OnSelendokSoundfile)
	ON_BN_CLICKED(IDC_PLAYSOUND, OnPlaysound)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBattiAdvancedPg message handlers

BOOL CBattiAdvancedPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	LocalizeDialog(GetSafeHwnd(), IDD);

	CenterWindow(GetDesktopWindow());	// center to the hpc screen

	// insert sound events
	for (int i = 0; i < SOUND_EVENT_COUNT; i++) {
		CLocalizedString strName;
		strName.LoadString(m_arSounds[i].ID);
		int item = m_ctlSoundEvents.AddString(strName);
		m_ctlSoundEvents.SetItemData(item, (DWORD) &(m_arSounds[i]));
	}

	// insert sound files
	InsertSoundFiles();

	m_ctlSoundEvents.SetCurSel(0);
	OnSelendokSoundevents();

	m_ctlLowSpin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlLowSpin.SetRange(0, 100);

	m_ctlCriticalSpin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlCriticalSpin.SetRange(0, 100);

	m_ctlPowerOffSpin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlPowerOffSpin.SetRange(0, 100);

	m_ctlPlay.SizeToContent();
	m_ctlStop.SizeToContent();

	UpdateControls();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBattiAdvancedPg::UpdateControls() {
	if (m_ctlPlaySound.IsWindowEnabled() && m_ctlPlaySound.GetCheck() == BST_CHECKED) {
		m_ctlPlay.EnableWindow();
		m_ctlStop.EnableWindow();
	}
	else {
		m_ctlPlay.EnableWindow(FALSE);
		m_ctlStop.EnableWindow(FALSE);
	}
}


void CBattiAdvancedPg::OnOK() {
	if (UpdateData()) {
		// save values to the config
		for (int i = 0; i < SOUND_EVENT_COUNT; i++)
			*(m_arSounds[i].OrigEvent) = m_arSounds[i].Event;
	}

	Config.CriticalLevel = m_nCritical;
	Config.LowLevel = m_nLow;
	Config.PowerOffLevel = m_nPowerOff;

	CPropertyPage::OnOK();
}

BOOL CBattiAdvancedPg::OnSetActive() {
	Config.Page = 1;
	return CPropertyPage::OnSetActive();
}

void CBattiAdvancedPg::OnSelendokSoundevents() {
	int curSel = m_ctlSoundEvents.GetCurSel();
	CSoundEventItem *sei = (CSoundEventItem *) m_ctlSoundEvents.GetItemData(curSel);

	m_ctlPlaySound.SetCheck(sei->Event.Enabled);
	m_ctlSoundFile.EnableWindow(sei->Event.Enabled);

	if (wcslen(sei->Event.FileName) <= 0) {
		m_ctlSoundFile.SetCurSel(0);
		OnSelendokSoundfile();
	}
	else {
		if (m_ctlSoundFile.SelectString(0, sei->Event.FileName) == CB_ERR)
			m_ctlSoundFile.SetCurSel(0);
	}

	UpdateControls();
}

void CBattiAdvancedPg::OnSelendokSoundfile() {
	int curSel = m_ctlSoundEvents.GetCurSel();
	CSoundEventItem *sei = (CSoundEventItem *) m_ctlSoundEvents.GetItemData(curSel);

	// Save selected filename
	TCHAR fileName[512] = { 0 };
	m_ctlSoundFile.GetWindowText(fileName, 511);

	int len = wcslen(fileName);
	delete [] sei->Event.FileName;
	sei->Event.FileName = new TCHAR [len + 1];
	wcscpy(sei->Event.FileName, fileName);

	UpdateControls();
}

void CBattiAdvancedPg::OnPlaysound() {
	int curSel = m_ctlSoundEvents.GetCurSel();
	CSoundEventItem *sei = (CSoundEventItem *) m_ctlSoundEvents.GetItemData(curSel);
	sei->Event.Enabled = m_ctlPlaySound.GetCheck() == BST_CHECKED;
	m_ctlSoundFile.EnableWindow(sei->Event.Enabled);

	UpdateControls();
}

void CBattiAdvancedPg::InsertSoundFiles() {
	// files
	CString strPath = _T("\\windows\\");
	CString mask;
	mask.Format(_T("%s*.wav"), strPath);

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(mask, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				CString strFileName = CString(fd.cFileName);
				int nPos = strFileName.ReverseFind('.');
				if (nPos != -1)
					strFileName = strFileName.Left(nPos);
				int item = m_ctlSoundFile.AddString(strFileName);
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
}

void CBattiAdvancedPg::OnPlay() {
	int curSel = m_ctlSoundEvents.GetCurSel();
	CSoundEventItem *sei = (CSoundEventItem *) m_ctlSoundEvents.GetItemData(curSel);

	CString strFileName;
	strFileName.Format(_T("\\windows\\%s"), sei->Event.FileName);

	PlaySound(strFileName, NULL, SND_ASYNC | SND_NODEFAULT | SND_NOWAIT);

}

void CBattiAdvancedPg::OnStop() {
	PlaySound(NULL, NULL, SND_ASYNC);
}
