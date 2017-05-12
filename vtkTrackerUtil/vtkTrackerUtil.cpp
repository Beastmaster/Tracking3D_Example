/*
Author: QIN Shuo
Date: 2017/5/11
Organization: RC-MIC (CUHK)
Description:
	tracking 3d object
	add OrthogonalPlanes
*/



#include "vtkTrackerUtil.h"
#include "PolarisVicraConfiguration.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkTrackerUtil);
vtkTrackerUtil::vtkTrackerUtil()
{
	// Registration Transform and matrix
	m_RegistrationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_RegistrationMatrix->Identity();
	// Tooltip to tool calibration matrix
	m_CalibrationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_CalibrationMatrix->Identity();
	//
	m_ReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_ReferenceMatrix->Identity();
}

vtkTrackerUtil::~vtkTrackerUtil()
{
}

void vtkTrackerUtil::SetTrackerType(TrackerBase * tt)
{
	m_Tracker = tt;
}

int vtkTrackerUtil::ConfigureTracker()
{
	return m_Tracker->ConfigureTracker();
}

int vtkTrackerUtil::StartTracking()
{
	return m_Tracker->StartTracking();
}

int vtkTrackerUtil::StopTracking()
{
	return m_Tracker->StopTracking();
}

void vtkTrackerUtil::Beep(int n)
{
	m_Tracker->Beep(n);
}

/*
Get Raw transform from tracker
*/
int vtkTrackerUtil::GetRawTransformMatrix(int id, double ** out)
{
	return m_Tracker->GetTransform(id,out);
}
int vtkTrackerUtil::GetRawTransformMatrix(int id, vtkMatrix4x4*  out)
{
	NEW2DARR(double,mat);
	int ecode = m_Tracker->GetTransform(id, mat);
	MatrixToVTKMatrix(mat, out);
	DEL2DARR(double,mat);
	return ecode;
}

/*
Get transformed matrix
Return:
	0: Success
	1: Not tracking
	2: Tool index outof range
	3: Transform invalid
	4: Tool Partialy out of volume
	5: Tool outof volume
	6: Index invalid
*/
int vtkTrackerUtil::GetTransformMatrix(int toolID, int refID,vtkMatrix4x4* out)
{
	int ecode1 = 0;
	int ecode2 = 0;
	NEW2DARR(double, mat);
	ecode1 = m_Tracker->GetTransform(toolID,mat);
	auto raw_mat = vtkSmartPointer<vtkMatrix4x4>::New();
	MatrixToVTKMatrix(mat, raw_mat);

	ecode2 = m_Tracker->GetTransform(refID, mat);
	MatrixToVTKMatrix(mat, m_ReferenceMatrix);

	out->DeepCopy(this->CoordinateConversion(raw_mat,m_ReferenceMatrix,m_CalibrationMatrix,m_RegistrationMatrix));
	DEL2DARR(double,mat);

	return 10 * ecode1 + ecode2;
}


vtkSmartPointer<vtkMatrix4x4> vtkTrackerUtil::GetRegistrationMat()
{
	return m_RegistrationMatrix;
}



TrackerBase* vtkTrackerUtil::GetTracker()
{
	return this->m_Tracker;
}

/*
Recommand to use dynamic assign
*/
void vtkTrackerUtil::SetReferenceMat(vtkMatrix4x4 * in)
{
	m_ReferenceMatrix->DeepCopy(in);
}

void vtkTrackerUtil::SetCalibrationMat(vtkMatrix4x4 * in)
{
	m_CalibrationMatrix->DeepCopy(in);
}

void vtkTrackerUtil::SetRegistrationMat(vtkMatrix4x4 * in)
{
	m_RegistrationMatrix->DeepCopy(in);
}



/*
Input:
	raw_mat:		  raw transform matrix from tracker
	reference_mat:	  raw reference transform matrix from tracker
	calibration_mat:  calibration matrix for tool
	registration_mat: registration matrix
*/
vtkSmartPointer< vtkMatrix4x4 > vtkTrackerUtil::CoordinateConversion(vtkMatrix4x4 * raw_mat, 
										vtkMatrix4x4 * reference_mat, 
										vtkMatrix4x4 * calibration_mat, 
										vtkMatrix4x4 * registration_mat)
{
	auto invert_refer = vtkSmartPointer<vtkMatrix4x4>::New(); // invert reference coordinate in world coordinate system
	invert_refer->Identity();
	vtkMatrix4x4::Invert(reference_mat, invert_refer);
	
	auto tool_refer_mat = vtkSmartPointer<vtkMatrix4x4>::New(); // tool coordinate in reference coordinate system
	vtkMatrix4x4::Multiply4x4(invert_refer, raw_mat, tool_refer_mat);

	auto tool_calibrated_mat = vtkSmartPointer<vtkMatrix4x4>::New(); // calibrated coordinate inreference coordinate system
	vtkMatrix4x4::Multiply4x4(tool_refer_mat, calibration_mat , tool_calibrated_mat);

	// transform to registered coordinate system
	auto tool_registered_mat = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Multiply4x4(registration_mat, tool_calibrated_mat, tool_registered_mat);

	return tool_registered_mat;
}


