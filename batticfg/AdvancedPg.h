/**
 *  AdvancedPg.h : header file
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

#if !defined(AFX_BATTIADVANCEDPG_H__710B607B_8DA7_4EF8_A66C_CC19282BF811__INCLUDED_)
#define AFX_BATTIADVANCEDPG_H__710B607B_8DA7_4EF8_A66C_CC19282BF811__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../share/Config.h"
#include "../share/Caption.h"

/////////////////////////////////////////////////////////////////////////////
// CBattiAdvancedPg dialog

class CBattiAdvancedPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CBattiAdvancedPg)

// Construction
public:
	CBattiAdvancedPg();
	~CBattiAdvancedPg();

// Dialog Data
	//{{AFX_DATA(CBattiAdvancedPg)
	enum { IDD = IDD_BATTICFG_ADVANCED };
	CBitmapButton	m_ctlPlay;
	CBitmapButton	m_ctlStop;
	CCaption    m_ctlSoundsLbl;
	CComboBox	m_ctlSoundFile;
	CComboBox	m_ctlSoundEvents;
	CButton	m_ctlPlaySound;
	CCaption	m_ctlLevelsLbl;
	CSpinButtonCtrl	m_ctlLowSpin;
	CSpinButtonCtrl	m_ctlCriticalSpin;
	CSpinButtonCtrl	m_ctlPowerOffSpin;
	int		m_nCritical;
	int		m_nLow;
	int		m_nPowerOff;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBattiAdvancedPg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	struct CSoundEventItem {
		UINT ID;					// string ID for the text
		CSoundEvent *OrigEvent;		// pointer to the original color
		CSoundEvent Event;			// current color

		CSoundEventItem() {
			ID = -1;
			OrigEvent = NULL;
		}

		void Set(UINT id, CSoundEvent *origEvent, CSoundEvent &event) {
			ID = id;
			OrigEvent = origEvent;
			Event = event;
		}
	};

	CSoundEventItem *m_arSounds;
	void InsertSoundFiles();

	// Generated message map functions
	//{{AFX_MSG(CBattiAdvancedPg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelendokSoundevents();
	afx_msg void OnSelendokSoundfile();
	afx_msg void OnPlaysound();
	afx_msg void OnPlay();
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATTISOUNDPG_H__710B607B_8DA7_4EF8_A66C_CC19282BF811__INCLUDED_)
