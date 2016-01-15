/*****************************************************************
Name:               CommandHandling.h

Description:	This file is the header for the CommandHandling
				class. It defines everything required for the class.


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
#ifndef _COMMANDHANDLING_H_
#define _COMMANDHANDLING_H_
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <time.h>
//#include <afxtempl.h>  //MFC lib
#include "APIStructures.h"
#include "Comm32.h"

/*****************************************************************
Defines   
*****************************************************************/
#define NUM_COM_PORTS		10	/* number of com ports */
#define POLARIS_SYSTEM		1	/* type of system, POLARIS */
#define AURORA_SYSTEM		2	/* or AURORA */
#define ACCEDO_SYSTEM		3	/* or ACCEDO */
#define VICRA_SYSTEM		4	/* or VICRA */
#define SPECTRA_SYSTEM	    5	/* or SPECTRA */

/*****************************************************************
Structures
*****************************************************************/
/* NONE */

/*****************************************************************
Routine Definitions
*****************************************************************/
class CCommandHandling
{
public:
	CCommandHandling();
	virtual ~CCommandHandling();

	int nCloseComPorts();
	int nOpenComPort( int nPort );
	int nHardWareReset(bool bWireless);
	int nSetSystemComParms( int nBaudRate, 
						    int nDataBits, 
						    int nParity, 
						    int nStopBits, 
						    int nHardware);
	int nSetCompCommParms(int  nBaud,
		 				  int nDataBits,
						  int nParity,
						  int nStop,
						  int nFlowControl);
	int nBeepSystem( int nBeeps );
	int nInitializeSystem();
	int nSetFiringRate();
	int nGetSystemInfo();
	int nInitializeAllPorts();
	int nInitializeHandle( int nHandle );
	int nEnableAllPorts();
	int nEnableOnePorts( int nPortHandle );
	int nDisablePort( int nPortHandle );
	int nActivateAllPorts();
	int nLoadTTCFG( char * pszPortID );
	int nGetHandleForPort( char * pszPortID );
	int nLoadVirtualSROM( char * pszFileName, 
					      char * pszPhysicalPortID, 
						  bool bPassive );
	int nFreePortHandles();
	int nGetPortInformation(int nPortHandle);
	int nStartTracking();
	int nGetTXTransforms(bool bReportOOV);
	int nGetBXTransforms(bool bReportOOV);
	int nStopTracking();
	int nGetAlerts(bool bNewAlerts);

	void ErrorMessage();
	void WarningMessage();
	int CreateTimeoutTable();
	int nLookupTimeout( char *szCommand );

/*****************************************************************
Variables
*****************************************************************/
	bool
		m_bLogToFile,				/* log to file */
		m_bDateTimeStampFile;		/* include date and time stamp in log file */
	char
		m_szLogFile[_MAX_PATH];		/* log file name */

	SystemInformation
		m_dtSystemInformation;		/* System Information variable - structure */

	HandleInformation
		m_dtHandleInformation[NO_HANDLES];	/* Handle Information varaible - structure */

	int
		m_nRefHandle; /* the handle for the tool acting as the reference tool */

	DiagNewAlertFlags
		m_dtNewAlerts; /* alert information */

	std::map<std::string, int>
		m_dtTimeoutValues;

protected:
/*****************************************************************
Routine Definitions
*****************************************************************/
	void ApplyXfrms();
	int nSendMessage( char * pszCommand, bool bAddCRC );
	int nGetResponse();
	int nGetBinaryResponse( );
	int nVerifyResponse( char * pszReply, bool bCheckCRC );
	int nCheckResponse( int nResponse );
	void LogToFile(int nDirection,char *psz);

	void InitCrcTable();
	unsigned int CalcCrc16( unsigned int crc, int data );
	unsigned CalcCRCByLen( char *pszString, int nLen );
	int SystemCheckCRC(char *psz);
	unsigned int SystemGetCRC(char *psz, int nLength);

	int nAddCRCToCommand( char * pszCommandString );
	int nAddCRToCommand( char * pszCommandString );
	int nBuildCommand( char * pszCommandString, bool bAddCRC );

/*****************************************************************
Variables
*****************************************************************/
	Comm32Port
		*pCOMPort;					/* pointer to the Comm32 class */

	char
		m_szLastReply[MAX_REPLY_MSG],	/* Last reply received from the system */
		m_szCommand[MAX_COMMAND_MSG];		/* command to send to the system */
	bool
		m_bClearLogFile,				/* clear log file on intialization */
		m_bDisplayErrorsWhileTracking;	/* display the error while tracking */
	int
		m_nTimeout,
		m_nDefaultTimeout;						/* timeout value in seconds */
	bool
		bComPortOpen[NUM_COM_PORTS];	/* array of com ports - if true they are open */
	int
		m_nPortsEnabled;				/* the number of port enable by nEnableAllPorts */
};
/************************END OF FILE*****************************/

#endif