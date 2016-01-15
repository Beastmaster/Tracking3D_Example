#if !defined(AFX_NEWALERTFLAGSDLG_H__B5B05367_9EF8_4792_B42C_996C22D8F6FF__INCLUDED_)
#define AFX_NEWALERTFLAGSDLG_H__B5B05367_9EF8_4792_B42C_996C22D8F6FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewAlertFlagsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewAlertFlagsDlg dialog

class CNewAlertFlagsDlg : public CDialog
{
// Construction
public:
	CNewAlertFlagsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewAlertFlagsDlg)
	enum { IDD = IDD_ALERTS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewAlertFlagsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

public:
	DiagNewAlertFlags
		m_dtNewAlerts;
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewAlertFlagsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWALERTFLAGSDLG_H__B5B05367_9EF8_4792_B42C_996C22D8F6FF__INCLUDED_)
