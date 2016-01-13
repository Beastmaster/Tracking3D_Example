/*****************************************************************
Name:               SystemFeaturesDlg.h

Description:	This file is the header for the System Features Dialog


All Northern Digital Inc. ("NDI") Media and/or Sample Code and/or
Sample Code Documentation (collectively referred to as "Sample Code")
is licensed and provided "as is" without warranty of any kind.  The
licensee, by use of the Sample Code, warrants to NDI that the Sample
Code is fit for the use and purpose for which the licensee intends to
use the Sample Code. NDI makes no warranties, express or implied, that
the functions contained in the Sample Code will meet the licensee’s
requirements or that the operation of the programs contained therein
will be error free.  This warranty as expressed herein is exclusive
and NDI expressly disclaims any and all express and/or implied, in fact
or in law, warranties, representations, and conditions of every kind
pertaining in any way to the Sample Code licensed and provided by NDI
hereunder, including without limitation, each warranty and/or condition
of quality, merchantability, description, operation, adequacy,
suitability, fitness for particular purpose, title, interference with
use or enjoyment, and/or non infringement, whether express or implied
by statute, common law, usage of trade, course of dealing, custom, or
otherwise.  No NDI dealer, distributor, agent or employee is authorized
to make any modification or addition to this warranty.

In no event shall NDI nor any of its employees be liable for any direct,
indirect, incidental, special, exemplary, or consequential damages,
sundry damages or any damages whatsoever, including, but not limited to,
procurement of substitute goods or services, loss of use, data or profits,
or business interruption, however caused.  In no event shall NDI’s
liability to the licensee exceed the amount paid by the licensee for the S
ample Code or any NDI products that accompany the Sample Code. The said
imitations and exclusions of liability shall apply whether or not any
such damages are construed as arising from a breach of a representation,
warranty, guarantee, covenant, obligation, condition or fundamental term
or on any theory of liability, whether in contract, strict liability, or
tort (including negligence or otherwise) arising in any way out of the
use of the Sample Code even if advised of the possibility of such damage.
In no event shall NDI be liable for any claims, losses, damages, judgments,
costs, awards, expenses or liabilities of any kind whatsoever arising
directly or indirectly from any injury to person or property, arising
from the Sample Code or any use thereof.


Copyright (C) 2002, 2003, Northern Digital Inc. All rights reserved.


*****************************************************************/
#if !defined(AFX_SYSTEMFEATURESDLG_H__47C6EEAA_A445_4F63_946A_A10925A6203F__INCLUDED_)
#define AFX_SYSTEMFEATURESDLG_H__47C6EEAA_A445_4F63_946A_A10925A6203F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SystemFeaturesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSystemFeaturesDlg dialog

class CSystemFeaturesDlg : public CDialog
{
// Construction
public:
	CSystemFeaturesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSystemFeaturesDlg)
	enum { IDD = IDD_SYSTEM_FEATURES };
	int		m_nNoActivePorts;
	int		m_nNoActiveWireless;
	int		m_nNoPassivePorts;
	int		m_nNoTIPDetectionPorts;
	CString	m_szVersionInfo;
	int		m_nTypeOfSystem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemFeaturesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSystemFeaturesDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetVariables( char * pszVersionInfo,
					   int nNoActivePorts,
					   int nNoPassivePorts,
					   int nNoActiveWireless,
					   int nNoTIPDetection,
					   int nTypeofSystem );
	void SetVariables( char * pszVersionInfo,
					   int nNoMagneticPorts,
					   int nNoFieldGenerators,
					   int nNoFieldGeneratorCards,
					   int nTypeofSystem );

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMFEATURESDLG_H__47C6EEAA_A445_4F63_946A_A10925A6203F__INCLUDED_)
/************************END OF FILE*****************************/
