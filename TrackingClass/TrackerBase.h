
/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/01/05

Description:
	Base class of tracker device
	This base class define several virtual function
	And base data sturcture
*/

#ifndef _TRACKERBASE_H_
#define _TRACKERBASE_H_

#include <stdlib.h> //NULL

typedef struct
{
	// 3D position
	double   x;
	double   y;
	double   z;
	// QuatRotation
	double   q0;
	double   qx;
	double   qy;
	double   qz;

	double error;
} QIN_Transform_Type;

typedef  double** QIN_Matrix_Type;

class TrackerBase
{
public:
	TrackerBase()
	{
		m_Transform = new QIN_Transform_Type;
		m_ToolIndex = 0;
	};
	~TrackerBase() 
	{ 
		delete m_Transform; 
		m_Transform = NULL;
	};

	/*
	Start Tracking process
	*/
	virtual int StartTracking() = 0;
	
	/*
	Stop tracking and close the tracking system
	*/
	virtual int StopTracking() = 0;
	
	/*
	Configure tracker for initialization
	Get ready for tracking
	*/
	virtual int ConfigureTracker()     = 0;

	/*
	Different type of trackers define different tracking status
	*/
	virtual int GetTrackingStatus()    = 0;

	/*
	*/
	virtual int GetToolValidation(int) = 0;
	
	/*
	
	*/
	virtual int GetTransformValidation(int) = 0;

	/*
	Description:
		Get transform from device
	Parameters:
		index count from 0
	*/
	virtual QIN_Transform_Type* GetTransform( int index ) = 0;


protected:
	int m_ToolIndex;
	QIN_Transform_Type* m_Transform;
};






#endif