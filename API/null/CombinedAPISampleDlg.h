/*****************************************************************
Name:               CombinedAPISampleDlg.h

Description:	This is the header file for the main dialog of this
				program.  It handles all user interaction with the
				system.

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

/*****************************************************************
Defines   
*****************************************************************/
#if !defined(AFX_COMBINEDAPISAMPLEDLG_H__323DA984_1509_4772_A4D6_996468149080__INCLUDED_)
#define AFX_COMBINEDAPISAMPLEDLG_H__323DA984_1509_4772_A4D6_996468149080__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_FILL_LIST		WM_USER+1
#define WM_COM_PORT_TO		WM_USER+2
#define MODE_PRE_INIT		0x01
#define MODE_INIT			0x02
#define MODE_TRACKING		0x03
#define MODE_ACTIVATED		0x04

#include "CommandHandling.h"	/* included the command handling header */
/////////////////////////////////////////////////////////////////////////////
// CCombinedAPISampleDlg dialog

class CCombinedAPISampleDlg : public CDialog
{
// Construction
public:
	CCombinedAPISampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCombinedAPISampleDlg)
	enum { IDD = IDD_COMBINEDAPISAMPLE_DIALOG };
	CEdit	m_ctlFrameNo;
	CComboBox	m_ctlPortHandleCB;
	CComboBox	m_ctlRefHandleCB;
	CListCtrl	m_ctlTrackingList;
	CString	m_szFrameNumber;
	BOOL	m_bInterference;
	CString	m_szSystemMode;
	BOOL	m_bUse0x0800Option;
	BOOL	m_bUseEulerAngles;
	int		m_nTrackingMode;
	CString	m_szManufID;
	BOOL	m_bPortEnabled;
	BOOL	m_bPortInitialized;
	CString	m_szSerialNo;
	CString	m_szToolRev;
	CString	m_szToolType;
	CString	m_szPartNumber;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCombinedAPISampleDlg)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCombinedAPISampleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnResetSystem();
	afx_msg void OnInitializeSystem();
	afx_msg void OnActivatePorts();
	afx_msg void OnTrackingBut();
	afx_msg LONG nGetSystemTransformData( UINT wParam, LONG lParam );
	afx_msg LONG nComPortTimeout( UINT wParam, LONG lParam );
	afx_msg void OnFileExit();
	afx_msg void OnSettingsComportsettings();
	afx_msg void OnSettingsRomfilesettings();
	afx_msg void OnSystemActivateports();
	afx_msg void OnSystemInitializesystem();
	afx_msg void OnSystemStarttracking();
	afx_msg void OnSystemStoptracking();
	afx_msg void OnViewSystemproperties();
	afx_msg void OnViewAlertFlags();
	afx_msg void OnSelchangePortHandles();
	afx_msg void OnSelchangeRefPortHandle();
	afx_msg void OnOptionsIlluminatorfiringrate();
	afx_msg void OnOptionsProgramoptions();
	afx_msg void OnHelpAboutcombinedapisample();
	afx_msg void OnPortEnabled();
	afx_msg void OnDiagnosticsPending();
	afx_msg void OnEulerAngles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

/*****************************************************************
Structures
*****************************************************************/
/* NONE */

/*****************************************************************
Variables
*****************************************************************/
	bool
		m_bResetHardware,		/* reset hardware variable */
		m_bWireless,			/* uses the wireless compatible settings */
		m_bSystemInitialized,	/* is the system initialized */
		m_bPortsActivated;		/* are ports activated */
	int
		m_nCOMPort;				/* the current com port number */

	CCommandHandling
		*pCommandHandling;		/* point to the command handling class */

	HACCEL 
		m_hAccel;				/* the main dialogs accelerator */

	CMenu
		*pMainMenu;				/* point to the main menu for the form */

/*****************************************************************
Routine Definitions
*****************************************************************/
	int nActivatePorts();		/* Activate all ports */
	int nStartTracking();		/* start tracking */
	int nStopTracking();		/* stop tracking */
	void SetMode( int nMode );	/* set the mode of the dialog */
};
/************************END OF FILE*****************************/

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBINEDAPISAMPLEDLG_H__323DA984_1509_4772_A4D6_996468149080__INCLUDED_)
