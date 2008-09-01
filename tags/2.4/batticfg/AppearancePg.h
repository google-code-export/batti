/**
 *  AppearancePg.h : header file
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

#if !defined(AFX_BATTIAPPEARANCEPG_H__C5B2FB13_62F7_4637_81BA_A33522F3BE04__INCLUDED_)
#define AFX_BATTIAPPEARANCEPG_H__C5B2FB13_62F7_4637_81BA_A33522F3BE04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\share\Caption.h"
#include "ColorButton.h"

/////////////////////////////////////////////////////////////////////////////
// CBattiAppearancePg dialog

class CBattiAppearancePg : public CPropertyPage
{
	DECLARE_DYNCREATE(CBattiAppearancePg)

// Construction
public:
	CBattiAppearancePg();
	~CBattiAppearancePg();

// Dialog Data
	//{{AFX_DATA(CBattiAppearancePg)
	enum { IDD = IDD_BATTICFG_APPEARANCE };
	CCaption	m_ctlOptionsLbl;
	CSpinButtonCtrl	m_ctlHeightSpin;
	CSpinButtonCtrl	m_ctlLeftMarginSpin;
	CComboBox	m_ctlColorName;
	CColorButton	m_ctlColor;
	CCaption	m_ctlColorLbl;
	BOOL	m_bHasFrame;
	int		m_nLeftMargin;
	int		m_nHeight;
	BOOL	m_bGradientIndicator;
	BOOL	m_bShowSteps;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBattiAppearancePg)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	struct CColorItem {
		UINT ID;				// string ID for the text
		COLORREF *OrigClr;		// pointer to the original color
		COLORREF Clr;			// current color

		CColorItem() {
			ID = -1;
			OrigClr = NULL;
			Clr = 0;
		}

		void Set(UINT id, COLORREF *origClr, COLORREF clr) {
			ID = id;
			OrigClr = origClr;
			Clr = clr;
		}
	};

	CColorItem *m_arColors;

	BOOL GetColor(COLORREF &clr);

	// Generated message map functions
	//{{AFX_MSG(CBattiAppearancePg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClr();
	afx_msg void OnSelendokColorName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATTIAPPEARANCEPG_H__C5B2FB13_62F7_4637_81BA_A33522F3BE04__INCLUDED_)
