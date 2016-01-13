/*****************************************************************
Name:				SystemFeaturesDlg.cpp	

Description:   This cpp file controls the System Features Dialog
			   the information displayed on this dialog comes from
			   the main dialogs call to the hardware.


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
/* NONE */

/*****************************************************************
Project Files Included
*****************************************************************/
#include "stdafx.h"
#include "CombinedAPISample.h"
#include "SystemFeaturesDlg.h"
#include "INIFileRW.h"
#include "CommandHandling.h"

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
// CSystemFeaturesDlg dialog


CSystemFeaturesDlg::CSystemFeaturesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSystemFeaturesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSystemFeaturesDlg)
	m_nNoActivePorts = 0;
	m_nNoActiveWireless = 0;
	m_nNoPassivePorts = 0;
	m_nNoTIPDetectionPorts = 0;
	m_szVersionInfo = _T("");
	//}}AFX_DATA_INIT
}


void CSystemFeaturesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemFeaturesDlg)
	DDX_Text(pDX, IDC_ACTIVE_PORTS, m_nNoActivePorts);
	DDX_Text(pDX, IDC_ACTIVE_WIRELESS, m_nNoActiveWireless);
	DDX_Text(pDX, IDC_PASSIVE_PORTS, m_nNoPassivePorts);
	DDX_Text(pDX, IDC_TIP_DETECTION, m_nNoTIPDetectionPorts);
	DDX_Text(pDX, IDC_VERSION_INFO, m_szVersionInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSystemFeaturesDlg, CDialog)
	//{{AFX_MSG_MAP(CSystemFeaturesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemFeaturesDlg message handlers
/*****************************************************************
			OnInitDialog
			VISUAL C++ FUNCTION CALL
*****************************************************************/
BOOL CSystemFeaturesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	/*
	 * m_nTypeOfSystem = 1 means AURORA System else POLARIS System
	 * default dialog configuration is POLARIS
	 */
	if ( m_nTypeOfSystem == AURORA_SYSTEM ) 
	{	
		GetDlgItem( IDC_ACTIVE_PORTS_LBL )->SetWindowText( "Number of tool ports:" );
		GetDlgItem( IDC_TIP_DETECTION_LBL )->ShowWindow( FALSE );
		GetDlgItem( IDC_TIP_DETECTION )->ShowWindow( FALSE );
		GetDlgItem( IDC_PASSIVE_PORTS_LBL )->ShowWindow( FALSE );
		GetDlgItem( IDC_PASSIVE_PORTS )->ShowWindow( FALSE );
		GetDlgItem( IDC_ACTIVE_WIRELESS_LBL )->ShowWindow( FALSE );
		GetDlgItem( IDC_ACTIVE_WIRELESS )->ShowWindow( FALSE );
	}

	UpdateData( false );
	return TRUE;  
} /* OnInitDialog */
/*****************************************************************
Name:				SetVariables

Inputs:	
		char * pszVersionInfo - the string that contains the info
								pertaining to the firmware version of
								the POLARIS attached to
		int nNoActivePorts -  the number of active ports supported by
							  the POLARIS
		int nNoPassivePorts - the number of passive ports supported by
							  the POLARIS
		int nNoActiveWireless - the number of active wireless ports supported
								by the POLARIS
		int nNoTIPDetection - the number of ports that support Tool-In-Port
							  detection with the POLARIS connected to.

Return Value:
		None.

Description: 
		This routine sets the dialogs member variables with the 
		information gathered from the POLARIS System,  These variables
		are set prior to calling the dialogs DoModal routine.
*****************************************************************/
void CSystemFeaturesDlg::SetVariables( char * pszVersionInfo,
									   int nNoActivePorts,
									   int nNoPassivePorts,
									   int nNoActiveWireless,
									   int nNoTIPDetection,
									   int nTypeofSystem )
{
	m_szVersionInfo = pszVersionInfo;
	m_szVersionInfo.Replace( "\n", "\r\n" ); /* to display in edit box */

	switch( nNoActivePorts )
	{
		case 12:	// TDS supported and present 
		case 4:		// TDS supported but not present
			m_nNoActivePorts = nNoActivePorts - 1;
			break;
		default:
			m_nNoActivePorts = nNoActivePorts;
			break;
	}/* switch */

	m_nNoActiveWireless = nNoActiveWireless;
	m_nNoPassivePorts = nNoPassivePorts;
	m_nNoTIPDetectionPorts = nNoTIPDetection;

	m_nTypeOfSystem = nTypeofSystem;
} /* SetVariables */
/*****************************************************************
Name:				SetVariables

Inputs:	
		char * pszVersionInfo - the string that contains the info
								pertaining to the firmware version of
								the AURORA attached to
		int nNoMagneticPorts -  the number of magnetic ports supported by
							    the AURORA
		int nNoFieldGenerators - the number of FG's supported by
							     the AURORA
		int nNoFieldGeneratorCards - the number of FG Cards supported
								     by the AURORA

Return Value:
		None.

Description: 
		This routine sets the dialogs member variables with the 
		information gathered from the AURORA System,  These variables
		are set prior to calling the dialogs DoModal routine.
*****************************************************************/
void CSystemFeaturesDlg::SetVariables( char * pszVersionInfo,
									   int nNoMagneticPorts,
									   int nNoFieldGenerators,
									   int nNoFieldGeneratorCards,
									   int nTypeofSystem )
{
	m_szVersionInfo = pszVersionInfo;
	m_szVersionInfo.Replace( "\n", "\r\n" ); /* to display in EditBox */

	m_nNoActivePorts = nNoMagneticPorts;
	m_nNoActiveWireless = nNoFieldGenerators;
	m_nNoPassivePorts = nNoFieldGeneratorCards;
	m_nTypeOfSystem = nTypeofSystem;

} /* SetVariables */
/**************************END OF FILE***************************/
