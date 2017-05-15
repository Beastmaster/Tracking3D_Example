/*
Author: QIN Shuo
Date: 2017/5/11
Organization: RC-MIC (CUHK)
Description:
	
*/

#ifndef __VTKTRACKERUTIL_H__
#define __VTKTRACKERUTIL_H__


#include "vtkSmartPointer.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkMatrix4x4.h"

#include <vector>
#include <map>
#include <functional>
#include "TrackerBase.h"
#include "util.h"


class vtkTrackerUtil: public vtkObject
{
public:
	static vtkTrackerUtil* New();
	vtkTypeMacro(vtkTrackerUtil, vtkObject);

	int ConfigureTracker();
	int StartTracking();
	int StopTracking();
	void Beep(int);

	void SetTrackerType(TrackerBase*);   ///
	void SetReferenceMat(vtkMatrix4x4*);
	void SetCalibrationMat(vtkMatrix4x4*);///
	void SetRegistrationMat(vtkMatrix4x4*);///

	int GetRawTransformMatrix(int,double**);
	int GetRawTransformMatrix(int,vtkMatrix4x4* );
	int GetTransformMatrix(int toolID, int refID, vtkMatrix4x4* ); ///
	vtkSmartPointer<vtkMatrix4x4> GetRegistrationMat();
	TrackerBase* GetTracker();
	

protected:
	vtkTrackerUtil();
	~vtkTrackerUtil();

private:
	inline vtkSmartPointer<vtkMatrix4x4> 
		CoordinateConversion(vtkMatrix4x4* raw_mat,
								vtkMatrix4x4* reference_mat,
								vtkMatrix4x4* calibration_mat,
								vtkMatrix4x4* registration_mat);

	TrackerBase* m_Tracker;
	// Registration Transform and matrix
	vtkSmartPointer<vtkMatrix4x4> m_RegistrationMatrix;
	// Tooltip to tool calibration matrix
	vtkSmartPointer<vtkMatrix4x4> m_CalibrationMatrix;
	// Recommand to dynamic set reference 
	vtkSmartPointer<vtkMatrix4x4> m_ReferenceMatrix;
};






#endif



