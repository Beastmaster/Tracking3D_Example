/*****************************************************************
Name:			NewAlertFlagsDlg.cpp

Description:    This routine contains all the routines used to
				convert data.


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
#include "stdafx.h"
#include "combinedapisample.h"

#include "CommandHandling.h"
#include "NewAlertFlagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewAlertFlagsDlg dialog


CNewAlertFlagsDlg::CNewAlertFlagsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewAlertFlagsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewAlertFlagsDlg)
	//}}AFX_DATA_INIT
}


void CNewAlertFlagsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewAlertFlagsDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewAlertFlagsDlg, CDialog)
	//{{AFX_MSG_MAP(CNewAlertFlagsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewAlertFlagsDlg message handlers

BOOL CNewAlertFlagsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_0x00000001)->EnableWindow(m_dtNewAlerts.bFatalParamterFault);
	GetDlgItem(IDC_0x00000002)->EnableWindow(m_dtNewAlerts.bSensorParameterFault);
	GetDlgItem(IDC_0x00000004)->EnableWindow(m_dtNewAlerts.bMainVoltageFault);
	GetDlgItem(IDC_0x00000008)->EnableWindow(m_dtNewAlerts.bSensorVoltageFault);
	GetDlgItem(IDC_0x00000010)->EnableWindow(m_dtNewAlerts.bIlluminatorVoltageFault);
	GetDlgItem(IDC_0x00000020)->EnableWindow(m_dtNewAlerts.bIlluminatorCurrentFault);
	GetDlgItem(IDC_0x00000040)->EnableWindow(m_dtNewAlerts.bLeftSensorTempFault);
	GetDlgItem(IDC_0x00000080)->EnableWindow(m_dtNewAlerts.bRightSensorTempFault);
	GetDlgItem(IDC_0x00000100)->EnableWindow(m_dtNewAlerts.bMainBoardTempFault);
	GetDlgItem(IDC_0x00100000)->EnableWindow(m_dtNewAlerts.bBatteryFaultAlarm);
	GetDlgItem(IDC_0x00200000)->EnableWindow(m_dtNewAlerts.bBumpDetectedAlarm);
	GetDlgItem(IDC_0x00400000)->EnableWindow(m_dtNewAlerts.bCableFaultAlarm);
	GetDlgItem(IDC_0x00800000)->EnableWindow(m_dtNewAlerts.bFirmwareIncompatible);
	GetDlgItem(IDC_0x02000000)->EnableWindow(m_dtNewAlerts.bInternalFlashFull);
	GetDlgItem(IDC_0x04000000)->EnableWindow(m_dtNewAlerts.bLaserBatteryFaultAlarm);
	GetDlgItem(IDC_0x01000000)->EnableWindow(m_dtNewAlerts.bNonFatalParamFault);
	GetDlgItem(IDC_0x20000000)->EnableWindow(m_dtNewAlerts.bTempTooHigh);
	GetDlgItem(IDC_0x40000000)->EnableWindow(m_dtNewAlerts.bTempTooLow);

	UpdateData( false );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
