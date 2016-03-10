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

#include "PolarisVicraConfiguration.h"
#include "INIFileRW.h"

PloarisVicraConfiguration::PloarisVicraConfiguration()
{
	m_ConfigFile = "";
	m_bResetHardware = false;		/* reset hardware variable */
	m_bWireless = false;			/* uses the wireless compatible settings */
	m_bSystemInitialized = false;	/* is the system initialized */
	m_bStopTracking = false;        /* thread flag for stop tracking */
	m_bIsTracking = false;			/* tracking status flag*/
	m_bPortsActivated = false;		/* are ports activated */
	m_bPortEnabled = false;			/* port enable status */
	m_bPortInitialized = false;		/* port initialization flag */

	m_bUseEulerAngles = false;      // display euler mode
	m_bUse0x0800Option = false;     // use 0x0800opetion

	m_nCOMPort = 0;				    /* the current com port number */
	m_nTrackingMode = 0;		    //used in getdatafunction (TX/BX mode)
	m_szFrameNumber = 0;		    //display frame number;
	//device information		    
	m_szSystemMode = "";		    /* system operating mode */
	m_szSerialNo = "";			    /* serial number */
	m_szTypeOfSystem = "";			/* system type */
	m_szToolRev = "";			    /* tool reversion */
	m_szToolType = "";			    /* tool type */
	m_szPartNumber = "";		    	/* part number */
	m_szManufID = "";			    /* manufacturer ID */

	m_PortID.clear();
	m_OperatingStatus = "";
	m_TempetureStatus = "";
	m_TransformStatus = "";
	m_DiagnosticStatus = 0;
	m_Transform_Map;
}

PloarisVicraConfiguration::~PloarisVicraConfiguration()
{
	//release transform menory
	for (auto it = m_Transform_Map.begin(); it != m_Transform_Map.end(); ++it)
	{
		delete it->second;
		it->second = NULL;
	}
	m_Transform_Map.clear();
}

/*
Description:
	Start Tracking

Return Value:
	0:  success
	1:  fail
*/
int PloarisVicraConfiguration::StartTracking()
{
	if (this->nStartTracking())
	{
		/*
		* if we can start tracking, start the tracking thread and set the mode.
		*/
		m_bIsTracking = TRUE;
		m_bStopTracking = FALSE;

		m_szSystemMode = "System Tracking";

		SetMode(MODE_TRACKING);

		//Get data onece to refresh the buffs
		nGetSystemTransformData();
		std::cout << "Tracker Device Information:"  << std::endl;
		std::cout << "SystemMode:\t" << m_szSystemMode << std::endl;
		std::cout << "SerialNo:\t"   << m_szSerialNo   << std::endl;
		std::cout << "ToolRev:\t"    << m_szToolRev    << std::endl;
		std::cout << "ToolType:\t"   << m_szToolType   << std::endl;
		std::cout << "PartNumber:\t" << m_szPartNumber << std::endl;
		std::cout << "ManufID:\t"    << m_szManufID    << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Cannot Start Tracking: Device ERROR! " << std::endl;
		return 1;
	}
}

/*
Description:
	Stop tracking device, if you want to start another
	tracking process, you should call configure tracker
	and start function.
Return Value:
	0:	success
	1:	fail
*/
int PloarisVicraConfiguration::StopTracking()
{
	if (this->nStopTracking())
	{
		/*
		* set the variable that will stop the thread.
		* set the text on the dialog and put program in proper mode.
		*/
		m_bIsTracking = false;
		m_bStopTracking = true;

		m_szSystemMode = "System Initialized";
		SetMode(MODE_INIT);
		this->nCloseComPorts();
		return 0;
	}
	return 1;
}

/*
Description:
	Set configure file (.ini file)
	If this function is called, REAMINIParm/WRITEINIParm 
*/
int  PloarisVicraConfiguration::SetConfigureFile(std::string m_ConfigFile)
{
	SetINIFileName(m_ConfigFile.c_str());  // this is a external function defined in INIFileRW.h
	return 0;
}


/*
Description:
	This function implement the virtual fucntion in TrackerBase class
	You should just call this single function to finished the whole
	initialization process.
Return Value:
	0: success
	other: fail
*/
int  PloarisVicraConfiguration::ConfigureTracker()
{
	int
		nBaudRate = 0,
		nStopBits = 0,
		nParity = 0,
		nDataBits = 0,
		nHardware = 0,
		nWireless = 0,
		m_nCOMPort;

	bool
		m_bWireless,
		m_bResetHardware;


	/*
	* read the COM port parameters from the ini file
	*/
	ReadINIParm("Communication", "Baud Rate", "0", &nBaudRate);
	ReadINIParm("Communication", "Stop Bits", "0", &nStopBits);
	ReadINIParm("Communication", "Parity", "0", &nParity);
	ReadINIParm("Communication", "Data Bits", "0", &nDataBits);
	ReadINIParm("Communication", "Hardware", "0", &nHardware);
	ReadINIParm("Communication", "COM Port", "0", &m_nCOMPort);
	ReadINIParm("Communication", "Wireless", "0", &m_bWireless);
	// this feature is useful for debugging only
	ReadINIParm("Communication", "Reset Hardware", "0", &m_bResetHardware);

	//close the port first and then open
	this->nCloseComPorts();
	if (!this->nOpenComPort(m_nCOMPort))
	{
		//cannot open port, post a message here
		std::cout << " COMM Port cannot Open" << std::endl;
		return 1;
	}
	//if you are supposed to reset, call the reset now
	if (m_bResetHardware)
	{
		if (!this->nHardWareReset(m_bWireless))
			return 1;
	}

	/*
	* get the timeout values for the commands
	* this will return an error with all other systems, other than Vicra
	*/
	this->CreateTimeoutTable();

	/*
	* set the System COM Port parameters, then the computers COM Port parameters.
	* if that is successful, initialize the system
	*/
	if (this->nSetSystemComParms(nBaudRate, nDataBits, nParity, nStopBits, nHardware))
	{
		if (this->nSetCompCommParms(nBaudRate, nDataBits, nParity, nStopBits, nHardware))
		{
			if (this->nInitializeSystem())
			{
				/*
				* get the system information
				*/
				if (!this->nGetSystemInfo())
				{
					/*
					* Check system type: Polaris, Polaris Accedo, and Aurora
					*/

					std::cout << "Cannot get device information " << std::endl;
					return 1;
				}

				/*
				Post a message here to indicate system type
				*/
				std::cout << "System Type is: ";
				switch (this->m_dtSystemInformation.nTypeofSystem)
				{
					case		POLARIS_SYSTEM:
					{
						m_szTypeOfSystem = "Polaris system";
						std::cout << "Polaris system" << std::endl;
						break;
					}
					case		AURORA_SYSTEM:
					{
						m_szTypeOfSystem = "Aurora system";
						std::cout << "Aurora system" << std::endl;
						break;
					}
					case		ACCEDO_SYSTEM:
					{
						m_szTypeOfSystem = "Accedo system";
						std::cout << "Accedo system" << std::endl;
						break;
					}
					case		VICRA_SYSTEM:
					{
						m_szTypeOfSystem = "Polaris Vicra system";
						std::cout << "Polaris Vicra system" << std::endl;
						break;
					}
					case		SPECTRA_SYSTEM:
					{
						m_szTypeOfSystem = "Spectra system";
						std::cout << "Spectra system" << std::endl;
						break;
					}
					default:
					{
						m_szTypeOfSystem = "Unknown system";
						std::cout << "Unknown system" << std::endl;
						break;
					}
				}

				/*
				Tracking device version information
				*/
				m_szVersionInfo = m_dtSystemInformation.szVersionInfo;
				std::cout << "Device Version Info:\t"<<m_szVersionInfo << std::endl;

				/*
				Post message of system informations
				*/
				m_szSerialNo = m_dtHandleInformation[1].szSerialNo;
				m_szToolRev = m_dtHandleInformation[1].szRev;
				m_szToolType = m_dtHandleInformation[1].szToolType;
				m_szPartNumber = m_dtHandleInformation[1].szPartNumber;
				m_szManufID = m_dtHandleInformation[1].szManufact;

				/*
				* Set firing rate if system type is Polaris or Polaris Accedo.
				*/
				if (this->m_dtSystemInformation.nTypeofSystem != AURORA_SYSTEM)
				{
					this->nSetFiringRate();
				}
				m_szSystemMode = "System Initialized";
				SetMode(MODE_INIT);
				m_bSystemInitialized = TRUE;

				//show a message that the system has been initilized
				std::cout << "initialization success" << std::endl;

				// ======== activate all rom files  ========== //
				if (nActivatePorts() == 1) // fail
				{
					std::cout << "Cannot activated tool model" << std::endl;
					std::cout << "Initilization process cannot finisheed" << std::endl;
					return 1;
				}
				return 0;
			}
			else
			{
				//Post message here
				std::cout << "Cannot initialized" << std::endl;
				return 1;
			}
		}
		else
		{
			std::cout << "Cannot Set COM parameters" << std::endl;
			return 1;
		}
	}
	else
	{
		std::cout << "Cannot set system comm parameters\nMaybe low temperature, Just wait a minute" << std::endl;
		return 1;
	}
}

/*
Description:
	Check status of tracking, return true if tracking

Return:
	0: "Tracking Mode"
	1: "Pre Init Mode"
	2: "Init Mode"
	3: "Activated Mode"
	4: "Unknown mode"
	5£º error occur
*/
int PloarisVicraConfiguration::GetTrackingStatus()
{
	nGetSystemTransformData();

	if (m_OperatingStatus == "Tracking Mode")
		return 0;
	else if (m_OperatingStatus == "Pre Init Mode")
		return 1;
	else if (m_OperatingStatus == "Init Mode")
		return 2;
	else if (m_OperatingStatus == "Activated Mode")
		return 3;
	else if (m_OperatingStatus == "Unknown mode")
		return 4;
	else 
		return 5;
}

/*
Description:
	Get Transform from tracker sensor
	If cannot get transform, you will get a transform full of 0 value
Input:
	index: the index of the tool count from 0. By order in ini file parameters
Return:
	If the transform is valid, return a pointer
	Else return a pointer to a 0 transform
	(for ease of use)
	If index out of range
*/
QIN_Transform_Type* PloarisVicraConfiguration::GetTransform(int index)
{
	nGetSystemTransformData();
	
	if (m_Transform == NULL)
	{
		m_Transform = new QIN_Transform_Type;
		m_Transform->q0 = 1;
	}
	if (!m_bIsTracking)  // not tracking
	{
		memset(m_Transform, 0, sizeof(QIN_Transform_Type));
		m_Transform->q0 = 1;
		return m_Transform;
	}

	//check is the index in the portID list
	if ( m_PortID.size()<=index )
	{
		memset(m_Transform, 0, sizeof(QIN_Transform_Type));
		m_Transform->q0 = 1;
		return m_Transform;
	}

	int index2 = m_PortID[index];
	auto it = std::find(m_PortID.begin(), m_PortID.end(), index2);
	if (it != m_PortID.end())
	{
		memset(m_Transform, 0, sizeof(QIN_Transform_Type));
		m_Transform->q0 = 1;
		// check transform validation
		if (this->m_dtHandleInformation[index2].Xfrms.ulFlags != TRANSFORM_VALID )
		{
#if EN_INFO_POLARIS
			std::cout << "Transform "<< index <<" Invalid!" << std::endl;
#endif
			return m_Transform;
		}
		if (this->m_dtHandleInformation[index2].HandleInfo.bPartiallyOutOfVolume)
		{
#if EN_INFO_POLARIS ==1
			std::cout << "Tool " << index << " Partially out of volume" << std::endl;
#endif
			return m_Transform;
		}
		else if (this->m_dtHandleInformation[index2].HandleInfo.bOutOfVolume)
		{
#if EN_INFO_POLARIS
			std::cout << "Tool " << index << " Out of volume" << std::endl;
#endif
			return m_Transform;
		}
		else
		{
			std::cout << "Tool " << index << " Within volume" << std::endl;
		}
		
		m_Transform_Map[index2]->translation.x = m_dtHandleInformation[index2].Xfrms.translation.x;
		m_Transform_Map[index2]->translation.y = m_dtHandleInformation[index2].Xfrms.translation.y;
		m_Transform_Map[index2]->translation.z = m_dtHandleInformation[index2].Xfrms.translation.z;
		m_Transform_Map[index2]->rotation.q0 = m_dtHandleInformation[index2].Xfrms.rotation.q0;
		m_Transform_Map[index2]->rotation.qx = m_dtHandleInformation[index2].Xfrms.rotation.	qx;
		m_Transform_Map[index2]->rotation.qy = m_dtHandleInformation[index2].Xfrms.rotation.qy;
		m_Transform_Map[index2]->rotation.qz = m_dtHandleInformation[index2].Xfrms.rotation.qz;
		m_Transform_Map[index2]->fError = m_dtHandleInformation[index2].Xfrms.fError;

		m_Transform->x  = m_Transform_Map[index2]->translation.x;
		m_Transform->y  = m_Transform_Map[index2]->translation.y;
		m_Transform->z  = m_Transform_Map[index2]->translation.z;
		m_Transform->q0 = m_Transform_Map[index2]->rotation.q0;
		m_Transform->qx = m_Transform_Map[index2]->rotation.qx;
		m_Transform->qy = m_Transform_Map[index2]->rotation.qy;
		m_Transform->qz = m_Transform_Map[index2]->rotation.qz;
		m_Transform->error = m_Transform_Map[index2]->fError;
		return m_Transform;
	}
	else
	{
		memset(m_Transform, 0, sizeof(QIN_Transform_Type));
		m_Transform->q0 = 1;
		return m_Transform;
	}
}

/*
Description:
	Check validation of a certain handle
Input:
	Index of certain handle, count from 0
Return:
	0: Success
	1: Fail
*/
int PloarisVicraConfiguration::GetToolValidation( int index)
{
	if (m_PortID.size()<1)
	{
		std::cout << "No tool loaded" << std::endl;
		return 1;
	}
	if (this->m_dtHandleInformation[ m_PortID[index] ].HandleInfo.bInitialized > 0 &&
		this->m_dtHandleInformation[ m_PortID[index] ].szToolType[1] != '8')
	{
		return 0;
	}
	else
	{
		std::cout << "Handle " << index << " invalid" << std::endl;
		return 1;
	}
}
/*
Description:
	Check validation of a certain transform
	Transform is still avaliable when partially/fully missed
Input:
	Index of certain handle, count from 0
Return:
	0: Success
	1: Partially out of volume
	2: Out of volume
	4: Transform invalid
*/
int PloarisVicraConfiguration::GetTransformValidation(int index)
{
	nGetSystemTransformData();
	if (this->m_dtHandleInformation[ m_PortID[index] ].Xfrms.ulFlags == TRANSFORM_VALID)
	{
		if (this->m_dtHandleInformation[m_PortID[index]].HandleInfo.bPartiallyOutOfVolume)
		{
			std::cout << "Tool " << index << " Partially out of volume" << std::endl;
			return 1;
		}
		else if (this->m_dtHandleInformation[m_PortID[index]].HandleInfo.bOutOfVolume)
		{
			std::cout << "Tool " << index << " Out of volume" << std::endl;
			return 2;
		}
		else
		{
			std::cout << "Tool " << index << " Within volume" << std::endl;
			return 0;
		}
		return 0;
	}
	else
	{
		std::cout << "Transform" << index << "Invalid" << std::endl;
		return 4;
	}
}

/*
Input variables:
	Baud Rate = 

*/
void PloarisVicraConfiguration::SetBaudRate(int nBaudRate)
{
	WriteINIParm("Communication", "Baud Rate", nBaudRate);
}

/*
Input variables:
Stop Bits=0
*/
void PloarisVicraConfiguration::SetnStop(int nStopBits)
{
	WriteINIParm("Communication", "Stop Bits", nStopBits);
}
/*
Input variables:
	Parity=0
	0:   None
	1:   Odd
	2:	 Even
*/
void PloarisVicraConfiguration::SetnParity(int nParity)
{
	WriteINIParm("Communication", "Parity", nParity);
}

/*
	Input variables:
	Data Bits=0
	0:   8bits
	1:   7bits
*/
void PloarisVicraConfiguration::SetnDataBits(int nDataBits)
{
	WriteINIParm("Communication", "Data Bits", nDataBits);
}
/*
Input variables:
	COM Port =
	0:  COM1
	1:  COM2
	2:  COM3
	3:  COM4
	4:  COM5
	5:  COM6
	6:  COM7
	7:  COM8
	8:  COM9
	9:  COM10
*/
void PloarisVicraConfiguration::SetnFlowControl(int nCOMPort)
{
	WriteINIParm("Communication", "COM Port", nCOMPort);
}
/*
Input variables:
	Hardware=1
*/
void PloarisVicraConfiguration::SetnHardware(int m_bReset)
{
	WriteINIParm("Communication", "Reset Hardware", m_bReset);
}
/*
Input variables:
	Reset Hardware = 1
*/
void PloarisVicraConfiguration::ResetHardare(int m_bHardware)
{
	WriteINIParm("Communication", "Hardware", m_bHardware);
}
/*
Input variables:
	Wireless=0
*/
void PloarisVicraConfiguration::SetWireless(int m_bWireless)
{
	WriteINIParm("Communication", "Wireless", m_bWireless);
}
/*
Input variables:
	Timeout Time=10  ( Not implemented)
*/


/*
Description:
	input a SROM file and write the result to configuration file(*.ini)
	This function just append a SROM file 
*/
void PloarisVicraConfiguration::AddSROMFile(std::string fullpath)
{
	char pszROMFileName[MAX_PATH];
	char pszROMVariable[MAX_PATH];
	int  pszPortID;

	int max = 0;
	//find existance of
	for (auto temp = m_PortID.begin(); temp != m_PortID.end();++temp)
	{
		max = (max > (*temp)) ? max : (*temp); 
	}
	pszPortID = max + 1;
	memset(pszROMVariable, 0, sizeof(pszROMVariable));
	sprintf(pszROMVariable, "Wireless Tool %02d", pszPortID);

	memset(pszROMFileName,0,sizeof(pszROMFileName));
	strcpy(pszROMFileName,fullpath.c_str());

	WriteINIParm("POLARIS SROM Image Files", pszROMVariable , pszROMFileName);
}


/*
Description: 
	Load SROM file and activate handles

Return Value:
	0: success
	1: fail
*/
int PloarisVicraConfiguration::nActivatePorts()
{
	char
		pszPortID[9];
	int
		i = 0;

	/*
	* if we can active the ports, we then fill the port information
	* on the main dialog
	*/
	if (this->nActivateAllPorts())
	{
		m_bPortsActivated = true;
		m_szManufID = "";
		m_bPortEnabled = FALSE;
		m_bPortInitialized = FALSE;
		m_szSerialNo = "";
		m_szToolRev = "";
		m_szToolType = "";
		m_szPartNumber = "";

		//NO_HANDLES is defined in APIStructures.h, 
		for (int i = 0; i < NO_HANDLES; i++)
		{
			if (this->m_dtHandleInformation[i].HandleInfo.bInitialized == 1)
			{
				sprintf(pszPortID, "%02X", i);
				if (this->m_dtHandleInformation[i].szToolType[1] != '8')
				{
					m_PortID.push_back(i);
					
					//put into map
					TransformInformation* transform_temp = new TransformInformation;
					memset(transform_temp, 0 , sizeof(TransformInformation));
					m_Transform_Map[i] = transform_temp;
				}
			}
		}
		SetMode(MODE_ACTIVATED);
		return 0;
	}

	m_bPortsActivated = false;
	return 1;
}

/*
Description:
	Start tracking and change status flag\

Return Value:
	0: success
	1: fail

int PloarisVicraConfiguration::nStartTracking()
{

}
*/
/*
Return Value:
0: success
1: fail

int PloarisVicraConfiguration::nStopTracking()
{
}
*/



/*
Description:
	Change tracker status

Flag parameters:
	OperatingStatus

Return Value:
	None
*/
void PloarisVicraConfiguration::SetMode(int nMode)
{
	bool
		bTrackingMode = FALSE,
		bPreInitialization = FALSE,
		bInitialized = FALSE;

	switch (nMode)
	{
	case MODE_PRE_INIT:
	{
		bPreInitialization = TRUE;
		break;
	}
	case MODE_INIT:
	{
		bInitialized = TRUE;
		bPreInitialization = FALSE;
		m_bPortsActivated = FALSE;
		break;
	}
	case MODE_ACTIVATED:
	{
		bInitialized = TRUE;
		break;
	}
	case MODE_TRACKING:
	{
		bTrackingMode = TRUE;
		break;
	}
	}

	// Do something in init or pre_init mode
	if (nMode == MODE_PRE_INIT || nMode == MODE_INIT)
	{
		/** clear flag here**/
		bInitialized = TRUE;
		m_szManufID = "";
		m_bPortEnabled = FALSE;
		m_bPortInitialized = FALSE;
		m_szSerialNo = "";
		m_szToolRev = "";
		m_szToolType = "";
		m_szPartNumber = "";
	}

	/**Modify GUI here**/
	switch (nMode)
	{
		case MODE_PRE_INIT:
		{
			m_OperatingStatus = "Pre Init Mode";
			break;
		}
		case MODE_INIT:
		{
			m_OperatingStatus = "Init Mode";
			break;
		}
		case MODE_ACTIVATED:
		{
			m_OperatingStatus = " Activated Mode";
			break;
		}
		case MODE_TRACKING:
		{
			m_OperatingStatus = "Tracking Mode";
			break;
		}
		default:
		{
			m_OperatingStatus = "Unknown mode";
			break;
		}
	}
}

/*
Return:
	0: success
	1: not tracking
	2: Tx mode transform failed
	3: Bx mode transform failed
	4: transformStatus is "Not Tracking"
	5: Port is occupied
*/
int PloarisVicraConfiguration::nGetSystemTransformData()
{
	char
		pszTemp[256];
	std::string
		szCBHandle,
		szPortNo;

	Rotation
		dtEulerRot;

	if (!m_bIsTracking)
	{
		m_TransformStatus = "Not Tracking";
		return 1;
	}


	/*
	* if tracking mode is 0, we are asking for TX data, else we are
	* asking for BX data.
	*/
	if (m_nTrackingMode == 0)
	{
		if (!this->nGetTXTransforms(m_bUse0x0800Option ? true : false))
			return 2;
	} /* if */
	else if (m_nTrackingMode == 1)
	{
		if (!this->nGetBXTransforms(m_bUse0x0800Option ? true : false))
			return 3;
	} /* else if */

	/* check for system flags */
	m_TempetureStatus  = this->m_dtSystemInformation.bTemperatureOutOfRange ? "Temperature out of range" : "Temperature within range";
	m_DiagnosticStatus = this->m_dtSystemInformation.bDiagnosticsPending;

	/*
	* if a new port has become occupied we do the following:
	* 1) Stop tracking
	* 2) Activate Ports
	* 3) Start Tracking
	*/
	if (this->m_dtSystemInformation.bPortOccupied)
	{
		if (this->nStopTracking() &&
			nActivatePorts() &&
			this->nStartTracking())
		{
			m_TransformStatus = "Not Tracking";
			return 4;
		}/* if */

		/*
		* We don't want the tracking thread to track if
		* activating the ports failed!
		*/
		m_bStopTracking = true;
		m_bIsTracking = FALSE;
		return 5;
	} 

	for (auto it = m_PortID.begin(); it != m_PortID.end(); ++it)//for (int i = 0; i < NO_HANDLES; i++)
	{
		int i = (*it);
		if (this->m_dtHandleInformation[i].HandleInfo.bInitialized > 0 &&
			this->m_dtHandleInformation[i].szToolType[1] != '8')  // I don't what's the hell is '8', just leave it as it is.
		{
			/* only update the frame if the handle isn't disabled*/
			if (this->m_dtHandleInformation[i].Xfrms.ulFlags == TRANSFORM_VALID ||
				this->m_dtHandleInformation[i].Xfrms.ulFlags == TRANSFORM_MISSING)
			{
				//frame number
				m_szFrameNumber = this->m_dtHandleInformation[i].Xfrms.ulFrameNumber;
			}

			if (i == this->m_nRefHandle)
				sprintf(pszTemp, "R%02X", i);
			else
				sprintf(pszTemp, "%02X", i); 

			//check serial port number
			/* this->m_dtHandleInformation[i].szPhysicalPort == szPortNo */

			/* fill the table */
			if (this->m_dtHandleInformation[i].Xfrms.ulFlags == TRANSFORM_VALID)  // transform information is valid
			{
				m_TransformStatus = "Transform Valid";  //transform tracking mode

				//===== put into map ======//
				/*
				I have spent a whole day for this line. Be noted that the type of a structure must match, or you
				will get a lot of strange errors. For memcpy will cover certain areas.!!!!!!!
				*/
				memcpy(m_Transform_Map[i], &(this->m_dtHandleInformation[i].Xfrms), sizeof(TransformInformation));

				//====== this block get 3d position  ======//
				sprintf(pszTemp, "%.2f", this->m_dtHandleInformation[i].Xfrms.translation.x); //std::cout << pszTemp << std::endl;
				sprintf(pszTemp, "%.2f", this->m_dtHandleInformation[i].Xfrms.translation.y); //std::cout << pszTemp << std::endl;
				sprintf(pszTemp, "%.2f", this->m_dtHandleInformation[i].Xfrms.translation.z); //std::cout << pszTemp << std::endl;

				//========= use Euler angles display =========//
				if (!m_bUseEulerAngles)
				{
					sprintf(pszTemp, "%.4f", this->m_dtHandleInformation[i].Xfrms.rotation.q0);
					sprintf(pszTemp, "%.4f", this->m_dtHandleInformation[i].Xfrms.rotation.qx);
					sprintf(pszTemp, "%.4f", this->m_dtHandleInformation[i].Xfrms.rotation.qy);
					sprintf(pszTemp, "%.4f", this->m_dtHandleInformation[i].Xfrms.rotation.qz);
				}
				else    // use normal angles format
				{
					CvtQuatToEulerRotation(&this->m_dtHandleInformation[i].Xfrms.rotation, &dtEulerRot);
					sprintf(pszTemp, "%.4f", dtEulerRot.fYaw);
					sprintf(pszTemp, "%.4f", dtEulerRot.fPitch);
					sprintf(pszTemp, "%.4f", dtEulerRot.fRoll);
					sprintf(pszTemp, "");
				}

				//======= transorm error ========//
				sprintf(pszTemp, "%.4f", this->m_dtHandleInformation[i].Xfrms.fError);

				//if (this->m_dtHandleInformation[i].HandleInfo.bPartiallyOutOfVolume)
				//	std::cout << "Tool " << i << " Partially out of volume" << std::endl;
				//else if (this->m_dtHandleInformation[i].HandleInfo.bOutOfVolume)
				//	std::cout << "Tool "<< i <<" Out of volume" << std::endl;
				//else
				//	std::cout << "Tool " << i << " Within volume" << std::endl;
			}
			//============= transform invalid  ====================//
			else   
			{
				if (this->m_dtHandleInformation[i].Xfrms.ulFlags == TRANSFORM_MISSING)   // transforma is missing
				{
					m_TransformStatus = "Transform Missing";
				}
				else
				{
					m_TransformStatus = "Transform Disabled";
					if (this->m_dtHandleInformation[i].HandleInfo.bEnabled)
					{
						// if port has become DISABLED show it in the check box
						m_szManufID = "";
						m_bPortEnabled = FALSE;
						m_bPortInitialized = FALSE;
						m_szSerialNo = "";
						m_szToolRev = "";
						m_szToolType = "";
						m_szPartNumber = "";
					}
				}
			}
		}
	}

	return 0;
}