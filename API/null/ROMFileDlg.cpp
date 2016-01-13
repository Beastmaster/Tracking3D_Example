/*****************************************************************
Name:			ROMFileDlg.cpp

Description:    this cpp controls the ROM File dialog.  It
				controls writting the ROM files to and reading
				them from the INI file.


All Northern Digital Inc. ("NDI") Media and/or Sample Code and/or
Sample Code Documentation (collectively referred to as "Sample Code")
is licensed and provided "as is" without warranty of any kind.  The
licensee, by use of the Sample Code, warrants to NDI that the Sample
Code is fit for the use and purpose for which the licensee intends to
use the Sample Code. NDI makes no warranties, express or implied, that
the functions contained in the Sample Code will meet the licensee's
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
or business interruption, however caused.  In no event shall NDI's
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
Project Files Included
*****************************************************************/
#include "stdafx.h"
#include "CombinedAPISample.h"
#include "ROMFileDlg.h"
#include "INIFileRW.h"
#include "CommandHandling.h"

/*****************************************************************
C Library Files Included
*****************************************************************/
#include "io.h"

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
/* NONE */


/////////////////////////////////////////////////////////////////////////////
// CROMFileDlg dialog


CROMFileDlg::CROMFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CROMFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CROMFileDlg)
	m_szPortID = _T("");
	m_szROMFile = _T("");
	m_bChangesSaved = FALSE;
	m_nNoActivePorts = 0;
	m_nNoPassivePorts = 0;
	m_nNoMagneticPorts = 0;
	//}}AFX_DATA_INIT
}


void CROMFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CROMFileDlg)
	DDX_Control(pDX, IDC_ROM_FILE, m_ctlROMFile);
	DDX_Control(pDX, IDC_PORT_LISTING, m_ctlROMFileList);
	DDX_Text(pDX, IDC_PORT, m_szPortID);
	DDX_Text(pDX, IDC_ROM_FILE, m_szROMFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CROMFileDlg, CDialog)
	//{{AFX_MSG_MAP(CROMFileDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_CLOSE, OnCloseButton)
	ON_NOTIFY(NM_CLICK, IDC_PORT_LISTING, OnClickPortListing)
	ON_NOTIFY(LVN_KEYDOWN, IDC_PORT_LISTING, OnKeydownPortListing)
	ON_BN_CLICKED(IDC_ACCEPT, OnAccept)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CROMFileDlg message handlers
/*****************************************************************
				OnInitDialog
			VISUAL C++ FUNCTION CALL
*****************************************************************/

BOOL CROMFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctlROMFileList.InsertColumn( 0, "Port ID", LVCFMT_LEFT, 100, -1 );
	m_ctlROMFileList.InsertColumn( 1, "SROM Image File", LVCFMT_LEFT, 310, -1 );

	m_ctlROMFileList.SetExtendedStyle(LVS_EX_FULLROWSELECT );
	
	FillROMFileTable();

	m_nROMSelection = 0;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/*****************************************************************
				OnClose
			VISUAL C++ FUNCTION CALL
*****************************************************************/
void CROMFileDlg::OnClose() 
{
	CDialog::OnClose();
}

/*****************************************************************
Name:				OnBrowse

Inputs:
	None.

Return Value:
	None.

Description:   This function controls what happens when the "..."
			   button is pressed.  This button brings up a file dialog
			   box, so the user can select a ROM file.
*****************************************************************/
void CROMFileDlg::OnBrowse() 
{
	UpdateData( TRUE );
	/* set for *.rom extension */
	CFileDialog m_pFileDialog(TRUE, NULL, "*.rom;*.ROM");

	/* if the user pressed the "Open" button, set the file name */
	if (m_pFileDialog.DoModal() == IDOK)
	{
		m_szROMFile = m_pFileDialog.GetPathName();
	} /* if */

	UpdateData( false );
} /* OnBrowse */
/*****************************************************************
Name:				OnCloseButton

Inputs:
	None.

Return Value:
	None.

Description:   This function controls what happens when the Close
			   button is pressed.  If the form has information in
			   the ROM file selection area, the user is asked if
			   they wish to save their changes.
*****************************************************************/
void CROMFileDlg::OnCloseButton() 
{
	bool
		bOkToClose = TRUE;
	char
		szErrorMsg[MAX_PATH + 30];

	UpdateData(true);
	/*
	 * if there is a file name of TTCFG is selected, ask if 
	 * they wish to save their changes
	 */
	if ( ((m_szROMFile.GetLength() > 0) && 
		  m_szROMFile.Find("TDS Expansion Port", 0)) )
	{
		if ( MessageBox( "Do you wish to save your changes?", 
			 "Save Changes", MB_ICONQUESTION|MB_YESNO|MB_SETFOREGROUND ) == IDYES )
		{
			/*
			 * TDS expansion port shall not be treated as a normal port.
			 */
			if ( m_szROMFile.GetLength() > 0 )
			{
				/*
				 * make sure we can access the file
				 */
				if ( _access(m_szROMFile, 0 ) < 0 )
				{
					sprintf( szErrorMsg, "File %s could not be found.", m_szROMFile );
					MessageBox( szErrorMsg, "Invalid SROM Image File", MB_ICONERROR|MB_OK|MB_SETFOREGROUND );
					GetDlgItem( IDC_ROM_FILE )->SetFocus();
					bOkToClose = FALSE;
				}/* if */
			} /* if */	
		} /* if */
	} /* if */

	if ( bOkToClose )
	{
		if ( m_bChangesSaved )
		{
			MessageBox( "You must re-initialize your system before SROM Image File\n"
						"changes will be applied.", "SROM Image Files", MB_OK|MB_ICONINFORMATION );

			/* Force to re-initialize the system  */
			AfxGetMainWnd( )->GetDlgItem( IDC_ACTIVATE_PORTS )->EnableWindow( FALSE );
			AfxGetMainWnd( )->GetDlgItem( IDC_TRACKING_BUT )->EnableWindow( FALSE );
			AfxGetMainWnd( )->GetMenu()->EnableMenuItem( ID_SYSTEM_ACTIVATEPORTS, MF_GRAYED );
		} /* if */
		CDialog::OnCancel();	
	}
} /* OnCloseButton */
/*****************************************************************
Name:				OnClickPortListing

Inputs:
	Visual C++ specifics...

Return Value:
	None.

Description:   This routine control what happens when the user
				clicks in the Port Listing for ROM Files.  It
				fills the form with the appropriate information as
				taken from the list.
*****************************************************************/
void CROMFileDlg::OnClickPortListing(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData( true );

	/* get the text and values for the selected item */
	m_szPortID = m_ctlROMFileList.GetItemText( m_ctlROMFileList.GetSelectionMark(), 0 ); 
	m_szROMFile = m_ctlROMFileList.GetItemText( m_ctlROMFileList.GetSelectionMark(), 1 );
	m_nROMSelection = m_ctlROMFileList.GetSelectionMark();
	UpdateData( false );

	/* set the appropriate dialog items */
	/*
	 * Handle the special TDS expansion port case.
	 * - Also make sure it's an aurora system!
	 */
	if( (m_nTypeofSystem != AURORA_SYSTEM) && (strstr(m_szPortID, "Port 4") != NULL) )
	{
		GetDlgItem( IDC_ROM_FILE )->ShowWindow( FALSE );
		GetDlgItem( IDC_ROM_LABEL )->ShowWindow( FALSE );
		GetDlgItem( IDC_BROWSE )->ShowWindow( FALSE );		
	}
	else
	{
		GetDlgItem( IDC_ROM_FILE )->ShowWindow( TRUE );
		GetDlgItem( IDC_ROM_LABEL )->ShowWindow( TRUE );
		GetDlgItem( IDC_BROWSE )->ShowWindow( TRUE );

		GetDlgItem( IDC_BROWSE )->EnableWindow( TRUE );	
		GetDlgItem( IDC_ACCEPT )->EnableWindow( TRUE );	
		m_ctlROMFile.SetReadOnly( FALSE );
	}/* else */


	*pResult = 0;
} /* OnClickPortListing */
/*****************************************************************
Name:				FillROMFileTable

Inputs:
	None.

Return Value:
	None.

Description:   This routine handles the filling of the ROM file
			   listing based on the type of system you are attached
			   to.
*****************************************************************/
void CROMFileDlg::FillROMFileTable( )
{
	char
		pszROMFileName[MAX_PATH];
	char
		pszPortID[20];
	int
		i = 0;

	/* for the number of passive ports supported */
	for ( i = m_nNoPassivePorts; i > 0; i-- )
	{
		sprintf( pszPortID, "Wireless Tool %02d", i );
		ReadINIParm( "POLARIS SROM Image Files", pszPortID, "", MAX_PATH, pszROMFileName );
		AddItemToList( pszPortID, pszROMFileName );
	} /* for */

	/* 
	 * if the system supports 4 active ports show 12 
	 * for the case that the TDS is plugged in and active
	 */
	if ( m_nNoActivePorts == 4 )
		m_nNoActivePorts = 12;

	/* for the number of active ports supported */
	for ( i = m_nNoActivePorts; i > 0; i-- )
	{
		/*
		 * TDS expansion port shall not be treated as a normal port.
		 */
		if( i == 4 )
		{
			sprintf( pszPortID, "Port %d", i );
			AddItemToList( pszPortID, "TDS Expansion Port" );
			continue;
		}/* if */

		sprintf( pszPortID, "Port %d", i );
		ReadINIParm( "POLARIS SROM Image Files", pszPortID, "", MAX_PATH, pszROMFileName );
		AddItemToList( pszPortID, pszROMFileName );
	} /* for */

	/* for the number of magnetic ports supported */
	for ( i = m_nNoMagneticPorts; i > 0; i-- )
	{
		sprintf( pszPortID, "Port %d", i );
		ReadINIParm( "AURORA SROM Image Files", pszPortID, "", MAX_PATH, pszROMFileName );
		AddItemToList( pszPortID, pszROMFileName );
	} /* for */
} /* FillROMFileTable */
/*****************************************************************
Name:				AddItemToList

Inputs:
	char * pszPortID - the Port ID of the item being added to list
	char * pszROMFileName - the ROM file to be added to the list attached
							to pszPortID

Return Value:
	None.

Description:   This routine adds the inputs to the list of ROM files
			   in the spot the corresponds to the specified pszPortID.
*****************************************************************/
void CROMFileDlg::AddItemToList( char * pszPortID, char * pszROMFileName )
{
	m_ctlROMFileList.InsertItem( 0, NULL );
	m_ctlROMFileList.SetItemText( 0, 0, pszPortID );
	m_ctlROMFileList.SetItemText( 0, 1, pszROMFileName );
} /* AddItemToList */
/*****************************************************************
Name:				SaveItemToINI

Inputs:
	char * pszPortID - the Port ID of the item being saved to the INI
	char * pszROMFileName - the ROM file to be saved to the INI attached
							to pszPortID

Return Value:
	None.

Description:   This routine saves the specified Port ID and ROM file
			   to the INI file under the correct section.
*****************************************************************/
void CROMFileDlg::SaveItemToINI( char * pszPortID, char * pszROMFileName )
{
	/* if Magnetic Ports is greater than zero, attached to AURORA */
	if ( m_nNoMagneticPorts > 0 )
		WriteINIParm( "AURORA SROM Image Files", pszPortID, pszROMFileName );
	else
		WriteINIParm( "POLARIS SROM Image Files", pszPortID, pszROMFileName );
} /* SaveItemToINI */
/*****************************************************************
Name:			OnKeydownPortListing	

Inputs:
	Visual C++ specifics....

Return Value:
	None.

Description:   This routine controls the key presses for the list.
				It controls the up, down, left, right arrows and
				the home, end. pageup and pagedown keys.
*****************************************************************/
void CROMFileDlg::OnKeydownPortListing(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int
		nSelection = 0;

	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	nSelection = m_ctlROMFileList.GetSelectionMark();

	switch ( pLVKeyDow->wVKey )
	{
		case VK_UP:
			{
				if ( nSelection > 0 )
					nSelection--;
				break;
			} /* VK_UP */
		case VK_DOWN:
			{
				if ( nSelection < m_ctlROMFileList.GetItemCount() - 1 )
					nSelection++;
				break;
			} /* VK_DOWN */
		case VK_PRIOR:
			{
				nSelection-=m_ctlROMFileList.GetCountPerPage()-1;
				if (nSelection < 0)
					nSelection = 0;
				break;
			} /* VK_PRIOR */
		case VK_NEXT:
			{
				nSelection+=m_ctlROMFileList.GetCountPerPage()-1;
				if (nSelection > (m_ctlROMFileList.GetItemCount() -1 ))
					nSelection = m_ctlROMFileList.GetItemCount() -1;
				break;
			} /* VK_NEXT */
		case VK_HOME:
			{
				nSelection = 0;
				break;
			} /* VK_HOME */
		case VK_END:
			{
				nSelection = m_ctlROMFileList.GetItemCount() -1;
				break;
			} /* VK_END */
	};/* switch */

	m_szPortID = m_ctlROMFileList.GetItemText( nSelection, 0 ); 
	m_szROMFile = m_ctlROMFileList.GetItemText( nSelection, 1 );


	m_nROMSelection = nSelection;

	GetDlgItem( IDC_BROWSE )->EnableWindow( TRUE );	
	GetDlgItem( IDC_ACCEPT )->EnableWindow( TRUE );	
	m_ctlROMFile.SetReadOnly( FALSE );

	UpdateData( false );

	*pResult = 0;
} /* OnKeydownPortListing */

/*****************************************************************
Name:				OnAccept

Inputs:
	Visual C++ specifics...

Return Value:
	None.

Description:   This routine control what happens when the user
			   presses the Save button, it saves the current listed
			   SROM Image file to the INI file
*****************************************************************/
void CROMFileDlg::OnAccept() 
{
	char
		szErrorMsg[MAX_PATH + 30];
	bool
		bValidFile = TRUE;

	/* update the information */
	UpdateData( true );

	/* if a file has been selected, check to see if that file exists */
	if ( m_szROMFile.GetLength() > 0 )
	{
		if ( _access(m_szROMFile, 0 ) < 0)
		{
			sprintf( szErrorMsg, "File %s could not be found.", m_szROMFile );
			MessageBox( szErrorMsg, "Invalid SROM Image File", MB_ICONERROR|MB_OK|MB_SETFOREGROUND );
			GetDlgItem( IDC_ROM_FILE )->SetFocus();
			bValidFile = FALSE;
		} /* if */
	} /* if */	

	/*
	 * if this is a valid file set the
	 * dialog options accordingly 
	 */
	if ( bValidFile )
	{
		m_ctlROMFileList.SetItemText( m_nROMSelection, 1, m_szROMFile );

		GetDlgItem( IDC_BROWSE )->EnableWindow( FALSE );
		m_ctlROMFileList.EnableWindow( TRUE );
		m_ctlROMFile.SetReadOnly( TRUE );
		SaveItemToINI( m_szPortID.GetBuffer(0), (LPSTR) (LPCSTR)m_szROMFile );
		m_bChangesSaved = TRUE;
		m_szROMFile = "";
		UpdateData( false );
	} /* if */	
} /* On Accept */

