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
#include "vtkDataArray.h"
#include "vtkPoints.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>

#include <vtkKdTree.h>

#include <stdio.h>
#include <string.h>

/*
User Manual:				   
	1. Set Source points	   
	2. Set Destination points   
	3. Run registration 	    GenerateTransform()
	4. Get transform matrix	    


Note:
	You are suggested to use LandMarkRegistration method
	For ICP method require a lot of points and may return
	an invalid value.
*/
class vtkTrackingRegistrationBase : public vtkObject
{
public:
	//static vtkTrackingRegistrationBase* New();// cannot instantiate abstract class
	vtkTypeMacro(vtkTrackingRegistrationBase, vtkObject);

	void SetSourcePoints(vtkPoints*);
	void SetSourcePoints(std::vector<double*>);
	void SetTargetPoints(vtkPoints*);
	void SetTargetPoints(std::vector<double*>);
	virtual void GenerateTransform()=0;
	vtkSmartPointer<vtkMatrix4x4> GetTransformMatrix();
	double EstimateRegistrationError();
protected:
	vtkTrackingRegistrationBase();
	~vtkTrackingRegistrationBase();


	//private functions to calculate error
	void ComputeLandmarksCentroid();
	void ComputeLandmarkPrincipalAxes();
	void ComputeRMSDistanceLandmarksFromPrincipalAxes();
	
	virtual void EstimatingRegistrationError();
	double m_LandmarkCentroid[3];
	double m_Error;

	vtkSmartPointer<vtkPoints> src_Points;
	std::vector<double*> m_src_Points;
	vtkSmartPointer<vtkPoints> target_Points;
	std::vector<double*> m_target_Points;
	vtkSmartPointer<vtkMatrix4x4> m_transform_matrix;
	
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
	void SetPreMultipliedMatrix(vtkMatrix4x4*);  // set pre multiplied matrix, transform input source point first
	void SetMaxIterateSteps(int step){ max_steps = step; };
private:
	vtkTrackingICPRegistration();
	~vtkTrackingICPRegistration();
	virtual void EstimatingRegistrationError();
protected:
	int max_steps;
	vtkSmartPointer<vtkMatrix4x4> m_pre_matrix;
	vtkSmartPointer<vtkMatrix4x4> m_post_matrix;
	vtkSmartPointer<vtkIterativeClosestPointTransform> m_icp;

	int Pre_process();
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