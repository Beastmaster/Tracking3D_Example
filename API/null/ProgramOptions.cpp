/*****************************************************************
Name:			ProgramOptions.cpp

Description:    This cpp controls the Program Options dialog. This
				dialog is used to set program flags and options.


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
C Library Files Included
*****************************************************************/

/*****************************************************************
Project Files Included
*****************************************************************/
#include "stdafx.h"
#include "CombinedAPISample.h"
#include "ProgramOptions.h"
#include "INIFileRW.h"

/*****************************************************************
Defines
*****************************************************************/
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************
Global Variables
*****************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CProgramOptions dialog


CProgramOptions::CProgramOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CProgramOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgramOptions)
	m_bBeepOnError = TRUE;
	m_bBeepOnWarning = TRUE;
	m_bClearLogFile = FALSE;
	m_bDateTimeStampFile = FALSE;
	m_szLogFile = _T("");
	m_bLogToFile = TRUE;
	m_nNoErrorBeeps = 1;
	m_nNoWarningBeeps = 1;
	m_bReportWhileTracking = FALSE;
	//}}AFX_DATA_INIT
}


void CProgramOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgramOptions)
	DDX_Check(pDX, IDC_BEEP_ERROR, m_bBeepOnError);
	DDX_Check(pDX, IDC_BEEP_ON_WARNING, m_bBeepOnWarning);
	DDX_Check(pDX, IDC_CLEAR_FILE, m_bClearLogFile);
	DDX_Check(pDX, IDC_DATETIME_STAMP, m_bDateTimeStampFile);
	DDX_Text(pDX, IDC_LOG_FILE, m_szLogFile);
	DDX_Check(pDX, IDC_LOG_TO_FILE, m_bLogToFile);
	DDX_Text(pDX, IDC_NO_ERROR_BEEPS, m_nNoErrorBeeps);
	DDV_MinMaxInt(pDX, m_nNoErrorBeeps, 1, 9);
	DDX_Text(pDX, IDC_NO_WARNING_BEEPS, m_nNoWarningBeeps);
	DDV_MinMaxInt(pDX, m_nNoWarningBeeps, 1, 9);
	DDX_Check(pDX, IDC_REPORT_WHILE_TRACKING, m_bReportWhileTracking);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgramOptions, CDialog)
	//{{AFX_MSG_MAP(CProgramOptions)
	ON_BN_CLICKED(IDC_LOG_TO_FILE, OnLogToFile)
	ON_BN_CLICKED(IDC_BEEP_ERROR, OnBeepError)
	ON_BN_CLICKED(IDC_BEEP_ON_WARNING, OnBeepOnWarning)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgramOptions message handlers
/*****************************************************************
				OnInitDialog
			VISUAL C++ FUNCTION CALL
*****************************************************************/
BOOL CProgramOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillForm();
	OnLogToFile();
	OnBeepError();
	OnBeepOnWarning();
	
	return TRUE;
}
/*****************************************************************
Name:		OnOK		

Inputs:
	None.

Return Value:
	None.

Description:   This routine handles the pushing of the OK button
			   on this dialog.
*****************************************************************/
void CProgramOptions::OnOK() 
{
	FILE
		*pfOut;
	char
		szErrorMessage[MAX_PATH + 30];
	bool
		bFileValid = TRUE;

	if (UpdateData( true ))
	{
		if ( m_szLogFile.GetLength() > 0 )
		{
			pfOut = fopen(m_szLogFile,"a+t");
			if(pfOut == NULL)
			{
				sprintf( szErrorMessage, "Invalid File Name: %s.", m_szLogFile );
				MessageBox( szErrorMessage, "Invalid File Name", MB_ICONERROR|MB_OK|MB_SETFOREGROUND );
				GetDlgItem( IDC_LOG_FILE )->SetFocus();
				bFileValid = FALSE;
			} /* if */
			else
				fclose( pfOut );
		} /* if */

		if( m_bLogToFile && m_szLogFile.GetLength() == 0 )
		{
			MessageBox( "Please specify a log file.", "Invalid File Name",
						MB_ICONERROR|MB_OK|MB_TOPMOST|MB_SYSTEMMODAL );
			GetDlgItem( IDC_LOG_FILE )->SetFocus();
			bFileValid = FALSE;
		}/* if */

		if ( bFileValid )
		{
			WriteINIParm( "Logging Options", "Log File Name", (LPSTR) (LPCSTR)m_szLogFile );
			WriteINIParm( "Logging Options", "Log To File", m_bLogToFile );
			WriteINIParm( "Logging Options", "Clear File", m_bClearLogFile );
			WriteINIParm( "Logging Options", "Date Time Stamp", m_bDateTimeStampFile );

			WriteINIParm( "Beeping Options", "Beep On Error", m_bBeepOnError );
			WriteINIParm( "Beeping Options", "No Error Beeps", m_nNoErrorBeeps );
			WriteINIParm( "Beeping Options", "Beep On Warning", m_bBeepOnWarning );
			WriteINIParm( "Beeping Options", "No Warning Beeps", m_nNoWarningBeeps );

			WriteINIParm( "Reporting Options", "Report While Tracking", m_bReportWhileTracking );
		
			CDialog::OnOK();
		} /* if */
	} /* if */
} /* OnOK */
/*****************************************************************
Name:		OnLogToFile()		

Inputs:
	None.

Return Value:
	None.

Description:   This routine handles what happens whe the Log To File
			   check box is select or unselected.
*****************************************************************/
void CProgramOptions::OnLogToFile() 
{
	UpdateData( true );
	/* active or de-activate the appropriate fields */
	GetDlgItem( IDC_LOG_FILE )->EnableWindow( m_bLogToFile );
	GetDlgItem( IDC_BROWSE )->EnableWindow( m_bLogToFile );
	GetDlgItem( IDC_CLEAR_FILE )->EnableWindow( m_bLogToFile );
	GetDlgItem( IDC_DATETIME_STAMP )->EnableWindow( m_bLogToFile );
} /* OnLogToFile */
/*****************************************************************
Name:		OnBeepError()		

Inputs:
	None.

Return Value:
	None.

Description:    This routine handles what occurs when the "Beep on
				Error" check box is clicked.
*****************************************************************/
void CProgramOptions::OnBeepError() 
{
	UpdateData( true );
	/* active or de-activate the appropriate fields */
	GetDlgItem( IDC_NO_ERROR_BEEPS )->EnableWindow( m_bBeepOnError );
} /* OnBeepError */
/*****************************************************************
Name:		OnBeepOnWarning()		

Inputs:
	None.

Return Value:
	None.

Description:    This routine handles what occurs when the "Beep on
				Warning" check box is clicked.
*****************************************************************/
void CProgramOptions::OnBeepOnWarning() 
{
	UpdateData( true );
	/* active or de-activate the appropriate fields */	
	GetDlgItem( IDC_NO_WARNING_BEEPS )->EnableWindow( m_bBeepOnWarning );	
} /* OnBeepOnWarning */
/*****************************************************************
Name:				FillForm()

Inputs:
	None.

Return Value:
	None.

Description:    This routine gathers the information from the INI
				file and displays it in the form.
*****************************************************************/
void CProgramOptions::FillForm() 
{
	char
		szFileName[MAX_PATH];
	ReadINIParm( "Logging Options", "Log File Name", "", MAX_PATH, szFileName );
	m_szLogFile = szFileName;
	ReadINIParm( "Logging Options", "Log To File", "", &m_bLogToFile );
	ReadINIParm( "Logging Options", "Clear File", "", &m_bClearLogFile );
	ReadINIParm( "Logging Options", "Date Time Stamp", "", &m_bDateTimeStampFile );

	ReadINIParm( "Beeping Options", "Beep On Error", "0", &m_bBeepOnError );
	ReadINIParm( "Beeping Options", "No Error Beeps", "1", &m_nNoErrorBeeps );
	ReadINIParm( "Beeping Options", "Beep On Warning", "0", &m_bBeepOnWarning );
	ReadINIParm( "Beeping Options", "No Warning Beeps", "1", &m_nNoWarningBeeps );

	ReadINIParm( "Reporting Options", "Report While Tracking", "1", &m_bReportWhileTracking );

	UpdateData( false );
} /* FillForm */
/*****************************************************************
Name:				OnBrowse()

Inputs:
	None.

Return Value:
	None.

Description:    This routine controls the pushing of the "..." button.
				It brings up a file dialog box that allow the user to
				select a log file location.
*****************************************************************/
void CProgramOptions::OnBrowse() 
{
	UpdateData( TRUE );
	CFileDialog m_pFileDialog(TRUE, NULL, "*.*");

	if (m_pFileDialog.DoModal() == IDOK )
	{
		m_szLogFile = m_pFileDialog.GetPathName();
	} /* if */

	UpdateData( false );	
} /* OnBrowse */
/*****************************************************************
Name:		OnCancel		

Inputs:
	None.

Return Value:
	None.

Description:   This routine handles the pushing of the Cancel button
			   on this dialog.
*****************************************************************/
void CProgramOptions::OnCancel() 
{
	FILE
		*pfOut;
	char
		szErrorMessage[MAX_PATH + 30];
	int
		nReturn = IDNO;


	if (UpdateData( true ))
	{
		if ( m_szLogFile.GetLength() > 0 )
		{
			pfOut = fopen(m_szLogFile,"a+t");
			if(pfOut == NULL)
			{
				sprintf( szErrorMessage, "Invalid File Name: %s.\n"
										 "The COM Stream will not be logged until this is fixed.\n"
										 "Do you want to fix this before closing this dialog?"
										 ,m_szLogFile );
				if (::MessageBox( m_hWnd, szErrorMessage, "Invalid File Name", 
								MB_ICONERROR|MB_YESNO|MB_SETFOREGROUND ) == IDYES )
				{
					nReturn = IDYES;
					GetDlgItem( IDC_LOG_FILE )->SetFocus();
				}
			} /* if */
			else
				fclose( pfOut );
		} /* if */
	}
	if ( nReturn == IDNO )
		CDialog::OnCancel();
}
/**************************END OF FILE***************************/
