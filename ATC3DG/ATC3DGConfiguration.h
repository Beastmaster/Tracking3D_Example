/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/01/05


Note:
	For QUATERNION: q0, q1, q2, and q3, q0 is the scaler component
*/

#ifndef _ATC3DGCONFIGURATION_H_
#define _ATC3DGCONFIGURATION_H_


#include "ATC3DG.h"
#include "../TrackingClass/TrackerBase.h"
#include <string>
#include <iostream>
#include <map>

#define EN_INFO_ATC 1		// enable information output


typedef DOUBLE_POSITION_MATRIX_RECORD		TestType;
class ATC3DGConfiguration : public TrackerBase
{
	typedef SYSTEM_CONFIGURATION        ATC3DGSysType;
	typedef SENSOR_CONFIGURATION        ATC3DGSensorType;
	typedef TRANSMITTER_CONFIGURATION   ATC3DGTransimitterType;
	typedef BOARD_CONFIGURATION		    ATC3DGBoardType;

	typedef DOUBLE_POSITION_ANGLES_RECORD      DefaultTransformType; //useless
	typedef DOUBLE_POSITION_QUATERNION_TIME_Q_RECORD  DefinedTransformType;

	

public:
	ATC3DGConfiguration();
	~ATC3DGConfiguration();

	virtual int StartTracking() { return 0; };
	virtual int StopTracking() { CloseSystem(); return 0; };
	virtual int GetTrackingStatus();
	virtual int ConfigureTracker();
	virtual int GetToolValidation(int);
	virtual int GetTransformValidation(int);
	virtual QIN_Transform_Type* GetTransform(int index);

	int Config2()
	{
		InitializeBIRDSystem();

		BOOL metric = true;
		m_ErrorCode = SetSystemParameter(METRIC, &(metric), sizeof(metric)); //Get SystemInformation
		m_ErrorCode = GetBIRDSystemConfiguration(&(m_SystemConfig)); //Get SystemInformation

		m_Num_Sensor = m_SystemConfig.numberSensors;

		if (m_SensorConfig != NULL)
		{
			delete m_SensorConfig;
		}
		m_SensorConfig = new ATC3DGSensorType[m_Num_Sensor];
		for (int i = 0; i<m_Num_Sensor; i++)
		{
			//Setup sensor data format
			//See also: GetTransformInformation() / ATC3DG.h
			//Output Data format is strictly related to this line.
			DATA_FORMAT_TYPE format = DOUBLE_POSITION_MATRIX;
			m_ErrorCode = SetSensorParameter(i, DATA_FORMAT, &format, sizeof(format));

			m_ErrorCode = GetSensorConfiguration(i, m_SensorConfig + i);
		}
		m_Num_Transmitter = m_SystemConfig.numberTransmitters;

		if (m_TransmitterConfig != NULL)
		{
			delete m_TransmitterConfig;
		}

		m_TransmitterConfig = new ATC3DGTransimitterType[m_Num_Transmitter];
		for (int i = 0; i<m_Num_Transmitter; i++)
		{
			m_ErrorCode = GetTransmitterConfiguration(i, m_TransmitterConfig + i);
			if (m_ErrorCode != BIRD_ERROR_SUCCESS)
			{
				ErrorHandler();
				return 1;
			}
		}
		for (short id = 0; id<m_Num_Transmitter; id++)
		{
			if ((m_TransmitterConfig + id)->attached) // check attachment
			{
				// Transmitter selection is a system function.
				// Using the SELECT_TRANSMITTER parameter we send the id of the
				// transmitter that we want to run with the SetSystemParameter() call
				m_ErrorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id)); //note: id is a short type
				break;
			}
		}
		m_Status = "Tracking";
	}
	void  GetTransform(TestType* temp)
	{
		m_ErrorCode = GetAsynchronousRecord(0, temp , sizeof(TestType));
	};
	//
	int PreInitialization(); // this function is controversial, not recommanded
	int SystemInitialization();
	int SystemSetup();
	int SensorSetup();
	int TransmitterSetup();
	int GetTransformInformation();
	int CloseSystem();
	int ErrorHandler();

private:
	int m_ErrorCode;
	std::string m_Status; //  "NULL Status" :        Default status
						  //  "Initialized Status"   System initialized
						  //  "System Ready"         System is set up and ready for next step
						  //  "Sensors Ready"         Sensors is set up and ready for next step
						  //  "Transmitters Ready"    Transmitter is ready for next step (ready for tracking)

	SYSTEM_CONFIGURATION			m_SystemConfig;
	SENSOR_CONFIGURATION*			m_SensorConfig;
	TRANSMITTER_CONFIGURATION*		m_TransmitterConfig;
	BOARD_CONFIGURATION*		    m_BoardConfig;

	int m_Num_Sensor;
	int m_Num_Transmitter;
	std::map< int, DefinedTransformType* >  m_TransformInformation;
	//QIN_Transform_Type*                  m_Transform;  // this object has been defined in TrackerBase class
};





#endif