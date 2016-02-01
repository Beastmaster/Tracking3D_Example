/*
Author: QIN Shuo
Date: 2015/01/20
Organization: RC-MIC (CUHK)

Description:
	Pivot Calibration lib

	Calibration the tool and the trakcer data

*/



#ifndef _PIVOT_CALIBRATION_H_
#define _PIVOT_CALIBRATION_H_

#include <vector>
#include <string>

#include "vtkSmartPointer.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h" 
#include "vtkMath.h"

class PivotCalibration : vtkObject
{
public:
	static PivotCalibration* New()
	{
		return new PivotCalibration;
	};
	vtkTypeMacro(PivotCalibration, vtkObject);

	// Clears all previously acquired tool transforms.
	// Call this before start adding transforms.
	void ClearToolToReferenceMatrices();

	// Add a tool transforms automatically by observing transform changes
	vtkGetMacro(RecordingState, bool);
	vtkSetMacro(RecordingState, bool);

	// Add a single tool transform manually
	void AddToolToReferenceMatrix(vtkMatrix4x4*);

	// Computes calibration results.
	// Returns with false on failure
	bool ComputePivotCalibration();

	// Computes calibration results.
	// Returns with false on failure
	bool ComputeSpinCalibration(bool snapRotation = false); //Note: The neede orientation protocol assumes that the shaft of the tool lies along the positive x-axis

	// Get calibration results
	void GetToolTipToToolTranslation(vtkMatrix4x4*);
	void GetToolTipToToolRotation(vtkMatrix4x4*);
	void GetToolTipToToolMatrix(vtkMatrix4x4*);
	vtkGetMacro(PivotRMSE, double);
	vtkGetMacro(SpinRMSE, double);

	// Returns human-readable description of the error occurred (non-empty if ComputePivotCalibration returns with failure)
	vtkGetMacro(ErrorText, std::string);

protected:
	PivotCalibration();
	~PivotCalibration();

	// Returns the orientation difference in degrees between two 4x4 homogeneous transformation matrix, in degrees.
	double GetOrientationDifferenceDeg(vtkMatrix4x4* aMatrix, vtkMatrix4x4* bMatrix);

	// Computes the maximum orientation difference in degrees between the first tool transformation
	// and all the others. Used for determining if there was enough variation in the input data.
	double GetMaximumToolOrientationDifferenceDeg();

private:

	// Calibration inputs
	double MinimumOrientationDifferenceDeg;
	std::vector< vtkSmartPointer<vtkMatrix4x4> > ToolToReferenceMatrices;
	bool RecordingState;

	// Calibration results
	vtkSmartPointer<vtkMatrix4x4> ToolTipToToolMatrix;
	double PivotRMSE;
	double SpinRMSE;
	std::string ErrorText;

};




#endif


