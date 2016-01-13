/*****************************************************************
Name:			ComPortSettings.cpp

Description:    This file controls the COM Port settings dialog.
				It is with this dialog the user sets the COM Port
				settings.  This dialog directly interacts with the
				ini file.


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
/* none */

/*****************************************************************
Project Files Included
*****************************************************************/
#include "ComPortSettings.h"
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
/* none */

/////////////////////////////////////////////////////////////////////////////
// CComPortSettings dialog


CComPortSettings::CComPortSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CComPortSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComPortSettings)
	m_bHardware = FALSE;
	m_bReset = TRUE;
	m_bWireless = FALSE;
	//}}AFX_DATA_INIT
}

	/* 
	 * use m_bWireless to tell the application if the port is wireless
	 * NOTE: the wireless port communication shouldn't use hardware
	 * handshaking or send a SERIAL BREAK
	 */
void CComPortSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComPortSettings)
	DDX_Control(pDX, IDC_COM_PORTS, m_ctlCOMPorts);
	DDX_Control(pDX, IDC_BAUD_RATE, m_ctlBaudRate);
	DDX_Control(pDX, IDC_PARITY, m_ctlParity);
	DDX_Control(pDX, IDC_STOP_BITS, m_ctlStopBits);
	DDX_Control(pDX, IDC_DATA_BITS, m_ctlDataBits);
	DDX_Check(pDX, IDC_HARDWARE, m_bHardware);
	DDX_Check(pDX, IDC_RESET, m_bReset);
	DDX_Check(pDX, IDC_WIRELESS, m_bWireless);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComPortSettings, CDialog)
	//{{AFX_MSG_MAP(CComPortSettings)
	ON_BN_CLICKED(IDC_WIRELESS, OnWireless)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComPortSettings message handlers
/*****************************************************************
				OnInitDialog
			VISUAL C++ FUNCTION CALL
*****************************************************************/
BOOL CComPortSettings::OnInitDialog()
{
	char cCOMPort[10];
	int
		nCOMPort,
		nBaudRate,
		nStopBits,
		nParity,
		nDataBits;

	CDialog::OnInitDialog();

	ReadINIParm( "Communication", "Baud Rate", "0", &nBaudRate );
	ReadINIParm( "Communication", "Stop Bits", "0", &nStopBits );
	ReadINIParm( "Communication", "Parity", "0", &nParity );
	ReadINIParm( "Communication", "Data Bits", "0", &nDataBits );
	ReadINIParm( "Communication", "Hardware", "0", &m_bHardware );
	ReadINIParm( "Communication", "COM Port", "0", &nCOMPort );
	ReadINIParm( "Communication", "Reset Hardware", "0", &m_bReset );
	ReadINIParm( "Communication", "Wireless", "0", &m_bWireless );

	for ( int i = 1; i < 20; i++ )
	{
		sprintf( cCOMPort, "COM%d", i );
		m_ctlCOMPorts.AddString( cCOMPort );
	} /* for */
	m_ctlCOMPorts.SetCurSel( nCOMPort );

	m_ctlBaudRate.AddString( "9600" );
	m_ctlBaudRate.AddString( "14400" );
	m_ctlBaudRate.AddString( "19200" );
	m_ctlBaudRate.AddString( "38400" );
	m_ctlBaudRate.AddString( "57600" );
	m_ctlBaudRate.AddString( "115200" );
	m_ctlBaudRate.AddString( "921600" );
	m_ctlBaudRate.AddString( "1228739" );
	m_ctlBaudRate.SetCurSel( nBaudRate );

	m_ctlParity.AddString( "None" );
	m_ctlParity.AddString( "Odd" );
	m_ctlParity.AddString( "Even" );
	m_ctlParity.SetCurSel( nParity );

	m_ctlStopBits.AddString( "1 bit" );
	m_ctlStopBits.AddString( "2 bits" );
	m_ctlStopBits.SetCurSel( nStopBits );

	m_ctlDataBits.AddString( "8 bits" );
	m_ctlDataBits.AddString( "7 bits" );
	m_ctlDataBits.SetCurSel( nDataBits );

	UpdateData( false );

	OnWireless();

	return TRUE;
} /* OnInitDialog */
/*****************************************************************
Name:				OnOK

Inputs:
	None.

Return Value:
	None.

Description:   This routine controls the pushing of the OK button
			   It makes sure all fields are valid and then saves
			   the information to the INI file.
*****************************************************************/
void CComPortSettings::OnOK()
{
	int
		nBaudRate = 0;

	if (UpdateData( true ))
	{
		CDialog::OnOK();

		nBaudRate = m_ctlBaudRate.GetCurSel();

		WriteINIParm( "Communication", "Baud Rate", nBaudRate );
		WriteINIParm( "Communication", "Stop Bits", m_ctlStopBits.GetCurSel() );
		WriteINIParm( "Communication", "Parity", m_ctlParity.GetCurSel() );
		WriteINIParm( "Communication", "Data Bits", m_ctlDataBits.GetCurSel() );
		WriteINIParm( "Communication", "COM Port", m_ctlCOMPorts.GetCurSel() );
		WriteINIParm( "Communication", "Reset Hardware", m_bReset );
		WriteINIParm( "Communication", "Hardware", m_bHardware );
		WriteINIParm( "Communication", "Wireless", m_bWireless );
	} /* if */
} /* OnOK */
/*****************************************************************
Name:				OnCancel

Inputs:
	None.

Return Value:
	None.

Description:   This routine handles the pressing of the cancel button.
*****************************************************************/
void CComPortSettings::OnCancel()
{
	CDialog::OnCancel();
} /* OnCancel */

/*****************************************************************
Name:				OnWireless

Inputs:
	None.

Return Value:
	None.

Description:   This option disables hardware handshaking and sending
			   a serial break to the position sensor when the port
			   is specified as a wireless port.
			   These two differences are important things to note
			   when communicating wirelessly to the Vicra.
*****************************************************************/
void CComPortSettings::OnWireless() 
{
	UpdateData( true );

	GetDlgItem( IDC_RESET )->ShowWindow( !m_bWireless );
	GetDlgItem( IDC_WIRELESS_NOTE )->ShowWindow( m_bWireless );

	UpdateData( false );
}
