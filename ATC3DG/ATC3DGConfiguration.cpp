/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/01/05

*/

#include "ATC3DGConfiguration.h"


/*
Description:
	m_ErrorCode is got from API
	m_Status indicate operating status of tracking system

Status:
	There are 5 status:
	||====================================
	1. "NULL Status" :        Default status
	2. "Initialized Status"   System initialized
	3. "System Ready"         System is set up and ready for next step
	4. "Sensors Ready"         Sensors is set up and ready for next step
	5. "Transmitters Ready"    Transmitter is ready for next step (ready for tracking)
	6. "Tracking"			   If all configurations are ready
	||======================================
	Order of setting up system/sensors/transmitters is not restricted,
	I restrict the order for easy usage purpose

*/
ATC3DGConfiguration::ATC3DGConfiguration()
{
	m_ErrorCode = 0;
	m_Status = "NULL Status";

	memset(&(m_SystemConfig), 0, sizeof(SYSTEM_CONFIGURATION));
	m_SensorConfig = NULL;
	m_TransmitterConfig = NULL;
	m_BoardConfig = NULL;

	m_TransformInformation.clear();
	//m_Transform = NULL; This transform is managed in base class
	
}
ATC3DGConfiguration::~ATC3DGConfiguration()
{
	if (m_SensorConfig!=NULL)
	{
		delete m_SensorConfig;
		m_SensorConfig = NULL;
	}
	if (m_TransmitterConfig!=NULL)
	{
		delete m_TransmitterConfig;
		m_TransmitterConfig = NULL;
	}
	if (m_BoardConfig!=NULL)
	{
		delete m_BoardConfig;
		m_BoardConfig = NULL;
	}
	if (m_TransformInformation.size() != 0)
	{
		for (auto it = m_TransformInformation.begin(); it != m_TransformInformation.end(); ++it)
		{
			if (it->second != NULL)
			{
				delete it->second;
				it->second = NULL;
			}
		}
	}
	m_TransformInformation.clear();
}


/*
Description:
	Configure and start tracker
Return Value:
	0: success
	1: SystemInitialization fail
	2: SystemSetup fail
	3: Sensor setup fail
	4: TransmitterSetup fail
*/
int ATC3DGConfiguration::ConfigureTracker()
{
	if (SystemInitialization()!=0)
	{
		return 1;
	}
	if (SystemSetup()!=0)
	{
		return 2;
	}
	if (SensorSetup()!=0)
	{
		return 3;
	}
	if (TransmitterSetup()!=0)
	{
		return 4;
	}
	m_Status = "Tracking";
	return 0;
}

/*
Description:
	Get the validation status of tool by index
Input:
	index: index of sensor to check, count from 0
Return:
	0: valid
	others: invalid
*/
int ATC3DGConfiguration::GetToolValidation(int sensorID) 
{ 
	unsigned int status = GetSensorStatus(sensorID);

	switch (status)
	{	
		case VALID_STATUS :
		{
			return 0;
		}
		case  GLOBAL_ERROR: 
		{
			return 1;
		}
		case  NOT_ATTACHED: 
		{
			return 2;
		}
		case  SATURATED:	  
		{
			return 3;
		}
		case  BAD_EEPROM:	 
		{
			return 4;
		}
		case  HARDWARE:	 
		{
			return 5;
		}
		case  NON_EXISTENT: 
		{
			return 6;
		}
		case  UNINITIALIZED:
		{
			return 7;
		}
		default:
		{ 
			return 8;
		}
	}

}

/*
Description:
	This function is for checking the validation of the transform
	information from a sensor. In EM tracking system, you can always
	get a valid transform, so we set the flag to 0 (success)

Return:
	0:     success
	other: fail
*/
int ATC3DGConfiguration::GetTransformValidation(int index)
{
	return GetToolValidation(index);
}


/*
Description:


Return:
	0: "Tracking"
	1. "NULL Status" :
	2. "Initialized Status"
	3. "System Ready"
	4. "Sensors Ready"
	5. "Transmitters Ready"
	6. Unknow state
*/
int ATC3DGConfiguration::GetTrackingStatus()
{
	if (m_Status == "Tracking")
	{
		//if the status is tracking, then check device status
		//1. Check Transmitter status
		return 0;
	}
	//If the global status is not Tracking,
	//There is no need to check device
	if (m_Status == "NULL Status")
	{
		return 1;
	}
	if (m_Status == "Initialized Status" )
	{
		return 2;
	}
	if (m_Status == "System Ready")
	{
		return 3;
	}
	if (m_Status == "Sensors Ready")
	{
		return 4;
	}
	if (m_Status == "Transmitters Ready")
	{
		return 5;
	}
	else
	{
		return 6;
	}
}



/*
Description:

Return Value:
	NULL: No value returned
*/
QIN_Transform_Type* ATC3DGConfiguration::GetTransform(int index)
{
	if (m_Transform == NULL)
	{
		m_Transform = new QIN_Transform_Type;
		m_Transform->q0 = 1;
	}

	if (GetTransformInformation()!=0)
	{
		m_Transform->x = 0;
		m_Transform->y = 0;
		m_Transform->z = 0;
		m_Transform->qx = 0;
		m_Transform->qy = 0;
		m_Transform->qz = 0;
		m_Transform->q0 = 1;
		m_Transform->error = 0;
		return m_Transform;
	}
	if (index <0 || index>=m_Num_Sensor-1)  // index outof range
	{
		m_Transform->x = 0;
		m_Transform->y = 0;
		m_Transform->z = 0;
		m_Transform->qx = 0;
		m_Transform->qy = 0;
		m_Transform->qz = 0;
		m_Transform->q0 = 1;
		m_Transform->error = 0;
		return m_Transform;
	}
	
	memset(m_Transform, 0, sizeof(QIN_Transform_Type));
	m_Transform->q0 = 1;

	if (m_TransformInformation[index]!=NULL)
	{
		m_Transform->x = m_TransformInformation[index]->x;
		m_Transform->y = m_TransformInformation[index]->y;
		m_Transform->z = m_TransformInformation[index]->z;
		m_Transform->qx = m_TransformInformation[index]->q[1];//[1];
		m_Transform->qy = m_TransformInformation[index]->q[2];//[2];
		m_Transform->qz = m_TransformInformation[index]->q[3];//[3];
		m_Transform->q0 = - m_TransformInformation[index]->q[0];//[0];
		m_Transform->error = m_TransformInformation[index]->quality;
	}
	return m_Transform;
}

/*
Description:
	Another function to get transform
Input: 
	index: index of sensors, count from 0
	output:	output transform matrix

Return:
	0:	success
	others:	fail
*/
int ATC3DGConfiguration::GetTransform(int index, double** output)
{
	int sz = sizeof(double[4][4]);
	for (size_t i = 0; i < 4; i++)
	{
		output[i][i] = 1;
	}
	if (GetTransformInformation() != 0)
	{
		return 1;
	}

	memset(m_Transform, 0, sizeof(QIN_Transform_Type));
	if (m_TransformInformation[index] != NULL)
	{
		m_Transform->x = m_TransformInformation[index]->x;
		m_Transform->y = m_TransformInformation[index]->y;
		m_Transform->z = m_TransformInformation[index]->z;
		m_Transform->qx = m_TransformInformation[index]->q[1];//[1];
		m_Transform->qy = m_TransformInformation[index]->q[2];//[2];
		m_Transform->qz = m_TransformInformation[index]->q[3];//[3];
		m_Transform->q0 = -m_TransformInformation[index]->q[0];//[0];
		m_Transform->error = m_TransformInformation[index]->quality;
	}
	else
	{
		m_Transform->q0 = 1;
	}

	// Convert quaternion to matrix
	double sqw = m_Transform->q0*m_Transform->q0;
	double sqx = m_Transform->qx*m_Transform->qx;
	double sqy = m_Transform->qy*m_Transform->qy;
	double sqz = m_Transform->qz*m_Transform->qz;
	double invs = 1 / (sqx + sqy + sqz + sqw);
	output[0][0] = (sqx - sqy - sqz + sqw)*invs; // since sqw + sqx + sqy + sqz =1/invs*invs
	output[1][1] = (-sqx + sqy - sqz + sqw)*invs;
	output[2][2] = (-sqx - sqy + sqz + sqw)*invs;
	double tmp1 = m_Transform->qx*m_Transform->qy;
	double tmp2 = m_Transform->qz*m_Transform->q0;
	output[1][0] = 2.0 * (tmp1 + tmp2)*invs;
	output[0][1] = 2.0 * (tmp1 - tmp2)*invs;
	tmp1 = m_Transform->qx*m_Transform->qz;
	tmp2 = m_Transform->qy*m_Transform->q0;
	output[2][0] = 2.0 * (tmp1 - tmp2)*invs;
	output[0][2] = 2.0 * (tmp1 + tmp2)*invs;
	tmp1 = m_Transform->qy*m_Transform->qz;
	tmp2 = m_Transform->qx*m_Transform->q0;
	output[2][1] = 2.0 * (tmp1 + tmp2)*invs;
	output[1][2] = 2.0 * (tmp1 - tmp2)*invs;
	// put translation to matrix
	output[0][3] = m_Transform->x;
	output[1][3] = m_Transform->y;
	output[2][3] = m_Transform->z;

	return 0;
}


/*
Description:
	This function is controversial, not recommanded

*/
int ATC3DGConfiguration::PreInitialization()
{
	if (m_ErrorCode != BIRD_ERROR_SUCCESS)
	{
		ErrorHandler();
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
Description:
	Initialize the tracking system

Return Value:
	0: Success
	1: Fail

Tip:
	If return value is fail, you should check 
	m_ErrorCode for details.
*/
int ATC3DGConfiguration::SystemInitialization()
{
	m_ErrorCode = InitializeBIRDSystem();
	
	if (m_ErrorCode != BIRD_ERROR_SUCCESS)
	{
		ErrorHandler();
		return 1;
	}
	else
	{
		m_Status = "Initialized Status";
		return 0;
	}
}

/*
Description:
	0: Success
	1: Fail
*/
int ATC3DGConfiguration::SystemSetup()
{
	BOOL metric = true;
	m_ErrorCode = SetSystemParameter(METRIC, &(metric), sizeof(metric)); //Get SystemInformation
	if (m_ErrorCode != BIRD_ERROR_SUCCESS)
	{
		ErrorHandler();
		return 1;
	}

	m_ErrorCode = GetBIRDSystemConfiguration(&(m_SystemConfig)); //Get SystemInformation
	if (m_ErrorCode != BIRD_ERROR_SUCCESS)
	{
		ErrorHandler();
		return 1;
	}
	else
	{
		m_Status = "System Ready";
		return 0;
	}
}

/*
Description:
	Initialize Sensors, return success only if ALL
	the sensor can get configuration information
Return Value:
	0: Success
	1: Fail
*/
int ATC3DGConfiguration::SensorSetup()
{
	m_Num_Sensor = m_SystemConfig.numberSensors;

	if (m_SensorConfig!= NULL)
	{
		delete m_SensorConfig;
	}
	m_SensorConfig = new ATC3DGSensorType[m_Num_Sensor];
	for (int i = 0; i<m_Num_Sensor; i++)
	{
		//Setup sensor data format
		//See also: GetTransformInformation() / ATC3DG.h
		//Output Data format is strictly related to this line.
		DATA_FORMAT_TYPE format = DOUBLE_POSITION_QUATERNION_TIME_Q;
		m_ErrorCode = SetSensorParameter(i,DATA_FORMAT,&format,sizeof(format));

		m_ErrorCode = GetSensorConfiguration(i, m_SensorConfig + i);
		if (m_ErrorCode != BIRD_ERROR_SUCCESS)
		{
			ErrorHandler();
			return 1;
		}
	}

	if (m_ErrorCode != BIRD_ERROR_SUCCESS)
	{
		ErrorHandler();
		return 1;
	}
	else
	{
		m_Status = "Sensors Ready";
		return 0;
	}
}

/*
Description:
	Initialize Sensors, return success only if ALL
	the Transmitter can get configuration information
Return Value:
	0: Success
	1: Fail
*/
int ATC3DGConfiguration::TransmitterSetup()
{
	m_Num_Transmitter = m_SystemConfig.numberTransmitters;

	if (m_TransmitterConfig!=NULL)
	{
		delete m_TransmitterConfig;
	}

	m_TransmitterConfig = new ATC3DGTransimitterType[m_Num_Transmitter];
	for (int i = 0; i<m_Num_Transmitter; i++)
	{
		m_ErrorCode = GetTransmitterConfiguration(i, m_TransmitterConfig+i);
		if (m_ErrorCode != BIRD_ERROR_SUCCESS)
		{
			ErrorHandler();
			return 1;
		}
	}


	if (m_ErrorCode != BIRD_ERROR_SUCCESS)
	{
		ErrorHandler();
		return 1;
	}


	//===========
	//Search for the first attached transmitter and turn it on
	//===========
	for (short id = 0; id<m_Num_Transmitter; id++)
	{
		if ((m_TransmitterConfig + id)->attached) // check attachment
		{
			// Transmitter selection is a system function.
			// Using the SELECT_TRANSMITTER parameter we send the id of the
			// transmitter that we want to run with the SetSystemParameter() call
			m_ErrorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id)); //note: id is a short type
			if (m_ErrorCode != BIRD_ERROR_SUCCESS)
			{
				ErrorHandler();
				return 1;
			}
			break;
		}
	}


	m_Status = "Transmitters Ready";
	return 0;

}

/*
Description:
	Collect data from all birds
	The default data format is DOUBLE_POSITION_ANGLES. We can use this
	format without first setting it.

	The angle is represented in Euler angle
	Note: vtkTransform angle represent in degree
Return Value:
	0: Success
	1: Fail
*/
int ATC3DGConfiguration::GetTransformInformation()
{
	for (int sensorID = 0; sensorID<m_Num_Sensor; sensorID++)
	{
		m_TransformInformation[sensorID] = new DefinedTransformType;
		// sensor attached so get record
		//m_ErrorCode = GetAsynchronousRecord(sensorID, m_TransormInformation, sizeof(DefaultTransformType));
		m_ErrorCode = GetAsynchronousRecord(sensorID, m_TransformInformation[sensorID], sizeof(DefinedTransformType));

		if (m_ErrorCode != BIRD_ERROR_SUCCESS) 
		{ 
			ErrorHandler();
			return 1;
		}

		// get the status of the last data record
		// only report the data if everything is okay
		unsigned int status = GetSensorStatus(sensorID);
		
		if (status == VALID_STATUS)
		{
#if EN_INFO_ATC
			std::cout << "Sensor"<<
				sensorID << " : \n"<<
				m_TransformInformation[sensorID]->x << "  " <<
				m_TransformInformation[sensorID]->y << "  " <<
				m_TransformInformation[sensorID]->z << "  " <<
				m_TransformInformation[sensorID]->q[0] << "  " <<
				m_TransformInformation[sensorID]->q[1] << "  " <<
				m_TransformInformation[sensorID]->q[2] << "  " <<
				m_TransformInformation[sensorID]->q[3] << "  " <<
				m_TransformInformation[sensorID]->time << "  " <<
				m_TransformInformation[sensorID]->quality << "  " <<
				std::endl;
			//numberBytes = strlen(output);
#endif
		}
		else
		{
#if EN_INFO_ATC
			delete m_TransformInformation[sensorID];
			m_TransformInformation[sensorID] = NULL;
			std::cout << "Sensor" <<sensorID <<" Invalid" << std::endl;
#endif
		}
	}


	if (m_ErrorCode != BIRD_ERROR_SUCCESS)
	{
		ErrorHandler();
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
Description:
	Close system in the end

Return Value:
	0: Success
	1: Fail
*/
int ATC3DGConfiguration::CloseSystem()
{
	m_ErrorCode = CloseBIRDSystem();
	if (m_ErrorCode != BIRD_ERROR_SUCCESS)
	{
		ErrorHandler();
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
Description:
	Post Error message
*/
int ATC3DGConfiguration::ErrorHandler()
{
	char			buffer[1024];
	char			*pBuffer = &buffer[0];
	int				numberBytes;

	while (m_ErrorCode != BIRD_ERROR_SUCCESS)
	{
		m_ErrorCode = GetErrorText(m_ErrorCode, pBuffer, sizeof(buffer), SIMPLE_MESSAGE);
		numberBytes = strlen(buffer);
		buffer[numberBytes] = '\n';		// append a newline to buffer
		printf("%s", buffer);
	}
	return 0;
}


