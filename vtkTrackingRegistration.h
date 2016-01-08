/*
Author: QIN Shuo
Date: 2015/12/29
Organization: RC-MIC (CUHK)
Description:
	registration function in tracking
*/




#ifndef __VTKTRACKINGREGISTRATION_H__
#define __VTKTRACKINGREGISTRATION_H__

#include "vtkSmartPointer.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkVertexGlyphFilter.h"

#include "vtkPoints.h"
#include "vtkMatrix4x4.h"

#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>



/*
User Manual:				   
	1. Set Source points	   
	2. Set Destination points   
	3. Run registration 	    GenerateTransform()
	4. Get transform matrix	    
*/
class vtkTrackingRegistrationBase : public vtkObject
{
public:
	//static vtkTrackingRegistrationBase* New();// cannot instantiate abstract class
	vtkTypeMacro(vtkTrackingRegistrationBase, vtkObject);

	void SetSourcePoints(vtkPoints*);
	void SetTargetPoints(vtkPoints*);
	virtual void GenerateTransform()=0;
	vtkSmartPointer<vtkMatrix4x4> GetTransformMatrix();

protected:
	vtkTrackingRegistrationBase();
	~vtkTrackingRegistrationBase();


	vtkSmartPointer<vtkPoints> src_Points;
	vtkSmartPointer<vtkPoints> target_Points;
	vtkSmartPointer<vtkMatrix4x4> transform_matrix;
};



/*
ICP registration method
*/
class vtkTrackingICPRegistration : public vtkTrackingRegistrationBase
{
public:
	static vtkTrackingICPRegistration* New();
	vtkTypeMacro(vtkTrackingRegistrationBase, vtkTrackingRegistrationBase);

	virtual void GenerateTransform();
	void SetMaxIterateSteps(int step){ max_steps = step; };
private:
	vtkTrackingICPRegistration();
	~vtkTrackingICPRegistration();

protected:
	int max_steps;
	vtkSmartPointer<vtkIterativeClosestPointTransform> m_icp;
};


/*
Land mark registration method
*/
class vtkTrackingLandMarkRegistration : public vtkTrackingRegistrationBase
{
public:
	static vtkTrackingLandMarkRegistration* New();
	vtkTypeMacro(vtkTrackingLandMarkRegistration, vtkTrackingRegistrationBase);

	virtual void GenerateTransform();
private:
	vtkTrackingLandMarkRegistration();
	~vtkTrackingLandMarkRegistration();

protected:
	vtkSmartPointer<vtkLandmarkTransform> m_landmarkTransform;
};





#endif