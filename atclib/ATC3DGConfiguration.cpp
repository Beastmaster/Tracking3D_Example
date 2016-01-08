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
	m_Transform = NULL;
	
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

	if (m_Transform!=NULL)
	{
		delete m_Transform;
		m_Transform = NULL;
	}
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
	return 0;
}

/*
Description:

*/
int ATC3DGConfiguration::GetTrackingStatus()
{
	return 0;
}

/*
Description:

Return Value:
	NULL: No value returned
*/
QIN_Transform_Type* ATC3DGConfiguration::GetTransform(int index)
{
	if (GetTransformInformation()!=0)
	{
		return NULL;
	}
	if (index <0 || index>=m_Num_Sensor-1)  // index outof range
	{
		return NULL;
	}
	
	if (m_Transform == NULL)
	{
		m_Transform = new QIN_Transform_Type;
	}
	memset(m_Transform, 0, sizeof(QIN_Transform_Type));

	m_Transform->x = m_TransformInformation[index]->x;
	m_Transform->y = m_TransformInformation[index]->y;
	m_Transform->x = m_TransformInformation[index]->z;
	m_Transform->qx = m_TransformInformation[index]->a;
	m_Transform->qy = m_TransformInformation[index]->e;
	m_Transform->qz = m_TransformInformation[index]->r;
	m_Transform->q0 = 0;
	m_Transform->error = m_TransformInformation[index]->quality;

	return m_Transform;
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
		DATA_FORMAT_TYPE format = DOUBLE_POSITION_ANGLES_TIME_Q;
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
			std::cout <<
				sensorID << "  "<<
				m_TransformInformation[sensorID]->x << "  " <<
				m_TransformInformation[sensorID]->y << "  " <<
				m_TransformInformation[sensorID]->z << "  " <<
				m_TransformInformation[sensorID]->a << "  " <<
				m_TransformInformation[sensorID]->e << "  " <<
				m_TransformInformation[sensorID]->r << "  " <<
				m_TransformInformation[sensorID]->time << "  " <<
				m_TransformInformation[sensorID]->quality << "  " <<
				std::endl;
			//numberBytes = strlen(output);
		}
		else
		{
			delete m_TransformInformation[sensorID];
			m_TransformInformation[sensorID] = NULL;
			std::cout << "Sensor" <<sensorID <<" Invalid" << std::endl;
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


