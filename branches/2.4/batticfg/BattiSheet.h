/**
 *  BattiSheet.h : header file
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

#if !defined(AFX_BATTISHEET_H__CFD7B2A8_55B2_4ABF_A08D_28C20C90722B__INCLUDED_)
#define AFX_BATTISHEET_H__CFD7B2A8_55B2_4ABF_A08D_28C20C90722B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CePropertySheet.h"
#include "GeneralPg.h"
#include "AdvancedPg.h"
#include "AppearancePg.h"

/////////////////////////////////////////////////////////////////////////////
// CBattiSheet

class CBattiSheet : public CCePropertySheet
{
	DECLARE_DYNAMIC(CBattiSheet)

// Construction
public:
	CBattiSheet(UINT iSelectPage);

// Attributes
protected:
	CBattiGeneralPg m_pgGeneral;
	CBattiAdvancedPg m_pgAdvanced;
	CBattiAppearancePg m_pgAppearance;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBattiSheet)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBattiSheet();

	// Generated message map functions
protected:
	void Init();

	//{{AFX_MSG(CBattiSheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATTISHEET_H__CFD7B2A8_55B2_4ABF_A08D_28C20C90722B__INCLUDED_)
