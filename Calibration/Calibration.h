/*
Author: QIN Shuo
Date: 2015/01/20
Organization: RC-MIC (CUHK)

Description:
	Calibration lib

	Calibration the tool and the trakcer data

*/



#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

#include "vtkSmartPointer.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"

 
class Calibration : vtkObject
{
public:
	static Calibration* New()
	{
		return new Calibration;
	};
	vtkTypeMacro(Calibration , vtkObject);



private:
	Calibration();
	~Calibration();



};




#endif


