/*
Author: QIN Shuo
Date:   2016/1/4
Organization: RC-MIC(CUHK)

Description:
	Reconstruct the API command handling
	Inhert from class: CCommandHandling

Attention:
	You should copy the file:
		TrackerPolarisVicra.ini
	To the .exe folder. Because the programe
	search for this configuration file first.

	There are 3 ROM files, and you should put 
	the path of these rom files to the right
	place.
*/



#ifndef _POLARISVICRACONFIGURATION_H_
#define _POLARISVICRACONFIGURATION_H_

#include "APIStructures.h"
#include "Comm32.h"
#include "CommandHandling.h"
#include "Conversions.h"
#include "INIFileRW.h"
#include "resource.h"

#include "../TrackingClass/TrackerBase.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <utility> 
#include <algorithm> 

// Tracker operating mode defines
#define MODE_PRE_INIT		0x01
#define MODE_INIT			0x02
#define MODE_TRACKING		0x03
#define MODE_ACTIVATED		0x04


#define POLARIS_SROM "POLARIS SROM Image Files"

#define EN_INFO_POLARIS 0			// enable output information

/*
User Manual:

1. Configure Tracker
2. 

*/
class PloarisVicraConfiguration: public CCommandHandling , public TrackerBase
{
public:
	PloarisVicraConfiguration();
	~PloarisVicraConfiguration();

	//====== Basic functions =======//
	/*
	1. Configure Tracker
	2. StartTracking
	3. StopTracking
	*/ 
	virtual int  StartTracking();
	virtual int  StopTracking();
	virtual int  GetTrackingStatus();
	virtual int  ConfigureTracker();
	virtual int  GetToolValidation(int);
	virtual int  GetTransformValidation(int);
	virtual QIN_Transform_Type* GetTransform(int index);
	virtual int GetTransform(int, double**);
	virtual void Beep(int);
	
	// manual GET functions for system parameters
	std::string GetSystemMode(){ return m_szSystemMode; };			/* system operating mode */
	std::string GetSerialNo(){ return m_szSerialNo; };			    /* serial number */
	std::string GetTypeOfSystem(){ return m_szTypeOfSystem; };		/* system type */
	std::string GetToolReversion(){ return m_szToolRev; };			/* tool reversion */
	std::string GetToolType(){ return m_szToolType; };			    /* tool type */
	std::string GetPartNumber(){ return m_szPartNumber; };			/* part number */
	std::string GetManufID(){ return m_szManufID; };			    /* manufacturer ID */

	// manual specified parameters
	void SetBaudRate(int);	
	void SetnDataBits(int);
	void SetnParity(int);
	void SetnStop(int);
	void SetnFlowControl(int);
	void SetnHardware(int);
	void ResetHardare(int);
	void SetWireless(int);
	void AddSROMFile(std::string);
	int  SetConfigureFile(std::string);

private:
	std::string m_ConfigFile;
	bool
		m_bResetHardware,		/* reset hardware variable */
		m_bWireless,			/* uses the wireless compatible settings */
		m_bSystemInitialized,	/* is the system initialized */
		m_bStopTracking,        /* thread flag for stop tracking */
		m_bIsTracking,			/* tracking status flag*/
		m_bPortsActivated,		/* are ports activated */
		m_bPortEnabled,			/* port enable status */
		m_bPortInitialized,		/* port initialization flag */
		
		m_bUseEulerAngles,		// display euler mode
		m_bUse0x0800Option;		// use 0x0800opetion

	int
		m_nCOMPort,				/* the current com port number */
		m_nTrackingMode,		// 0: TX mode,   1: BX mode
		m_szFrameNumber;		//display purpose
	//handle information
	std::string
		m_szVersionInfo,		// Tracking device info
		m_szSystemMode,			/* system operating mode */
		m_szSerialNo,			/* serial number */
		m_szTypeOfSystem,		/* system type */
		m_szToolRev,			/* tool reversion */
		m_szToolType,			/* tool type */
		m_szPartNumber,			/* part number */
		m_szManufID;			/* manufacturer ID */

	std::vector< int > m_PortID;
	std::string m_OperatingStatus;
	std::string m_TempetureStatus;
	std::string m_TransformStatus;
	int         m_DiagnosticStatus;
	std::map<int , TransformInformation*> m_Transform_Map;   // < handle number, transform>

	void SetMode(int nMode);
	int nActivatePorts();
	int nGetSystemTransformData();

};




#endif