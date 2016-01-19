/*
Author: QIN Shuo
Date: 2015/12/29
Organization: RC-MIC (CUHK)
Description:
	registration function in tracking
*/


#include "vtkTrackingRegistration.h"

//============== Base Class ================//
// cannot instantiate abstract class
//vtkStandardNewMacro(vtkTrackingRegistrationBase);
vtkTrackingRegistrationBase::vtkTrackingRegistrationBase()
{
	src_Points = vtkSmartPointer<vtkPoints>::New();
	target_Points = vtkSmartPointer<vtkPoints>::New();
	transform_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
}
vtkTrackingRegistrationBase::~vtkTrackingRegistrationBase()
{
}

/*
Description:
	Set input points to be registered
*/
void vtkTrackingRegistrationBase::SetSourcePoints(vtkPoints* input)
{
	src_Points->DeepCopy(input);
	std::cout << "Number of src poins:" << src_Points->GetNumberOfPoints() << std::endl;

	for (int i = 0; i < input->GetNumberOfPoints(); i++)
	{
		double* newp;
		newp = new double[3];
		memcpy(newp, input->GetPoint(i), 3 * sizeof(double));
		m_src_Points.push_back(newp);
	}
}
void vtkTrackingRegistrationBase::SetSourcePoints(std::vector<double*> xx2)
{
	for (auto it = xx2.begin(); it != xx2.end(); ++it)
	{
		src_Points->InsertNextPoint((*it));
	}
	m_src_Points = xx2;
}

void vtkTrackingRegistrationBase::SetTargetPoints(vtkPoints* input)
{
	target_Points->DeepCopy(input);
	for (int i = 0; i < input->GetNumberOfPoints(); i++)
	{
		double* newp;
		newp = new double[3];
		memcpy(newp, input->GetPoint(i), 3 * sizeof(double));
		m_target_Points.push_back(newp);
	}
}
void vtkTrackingRegistrationBase::SetTargetPoints(std::vector<double*> xx2)
{
	for (auto it = xx2.begin(); it != xx2.end(); ++it)
	{
		target_Points->InsertNextPoint((*it));
	}
	m_target_Points = xx2;
}

/*
Output a 4x4 matrix
*/
vtkSmartPointer<vtkMatrix4x4> vtkTrackingRegistrationBase::GetTransformMatrix()
{
	return transform_matrix;
}




/** Compute landmark centroid **/
void vtkTrackingRegistrationBase::ComputeLandmarksCentroid()
{
	std::vector<double* >::iterator					 pointItr;
	double                                           landmarkVector[3];

	// initialize to zero
	landmarkVector[0] = 0.0;
	landmarkVector[1] = 0.0;
	landmarkVector[2] = 0.0;

	pointItr = m_src_Points.begin();
	while (pointItr != m_src_Points.end())
	{
		landmarkVector[0] += (*pointItr)[0];
		landmarkVector[1] += (*pointItr)[1];
		landmarkVector[2] += (*pointItr)[2];
		++pointItr;
	}

	for (unsigned int ic = 0; ic<3; ic++)
	{
		this->m_LandmarkCentroid[ic] = landmarkVector[ic] /	this->m_src_Points.size();
	}
}

/*

*/
void vtkTrackingRegistrationBase::ComputeLandmarkPrincipalAxes()
{

}

/*

*/
void vtkTrackingRegistrationBase::ComputeRMSDistanceLandmarksFromPrincipalAxes()
{
}

/*
Description:
	Compute the error between the source point and the target point
Return:
	double type error
*/
double vtkTrackingRegistrationBase::EstimateRegistrationError()
{
	double error = 0.0;
	auto trans = vtkSmartPointer<vtkTransform>::New();
	trans->PostMultiply();
	trans->SetMatrix(transform_matrix);

	auto src_iter = m_src_Points.begin();
	auto tgt_iter = m_target_Points.begin();

	double temp[3];

	while (src_iter!=m_src_Points.end())
	{
		trans->TransformNormal(*src_iter,temp);


		++src_iter;
		++tgt_iter;
	}

	//error = sqrt();
}












//===================== ICP Registration====================//

vtkStandardNewMacro(vtkTrackingICPRegistration)
vtkTrackingICPRegistration::vtkTrackingICPRegistration()
{
	m_icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
	max_steps = 50;
}
vtkTrackingICPRegistration::~vtkTrackingICPRegistration()
{}
void vtkTrackingICPRegistration::GenerateTransform()
{
	auto source_poly = vtkSmartPointer<vtkPolyData>::New();
	auto target_poly = vtkSmartPointer<vtkPolyData>::New();

	if (src_Points->GetNumberOfPoints()<1)
	{
		return;
	}
	if (target_Points->GetNumberOfPoints()<1)
	{
		return;
	}
	//convert point to polydata first
	source_poly->SetPoints(src_Points);
	target_poly->SetPoints(target_Points);

	//to accerlate calculation: convert to vertex
	auto source_vertex = vtkSmartPointer<vtkVertexGlyphFilter>::New();
	source_vertex->SetInputData(source_poly);
	source_vertex->Update();
	auto target_vertex = vtkSmartPointer<vtkVertexGlyphFilter>::New();
	target_vertex->SetInputData(target_poly);
	target_vertex->Update();

	m_icp->SetSource(source_vertex->GetOutput());
	m_icp->SetTarget(target_vertex->GetOutput());
	m_icp->SetMeanDistanceModeToRMS();
	m_icp->GetLandmarkTransform()->SetModeToRigidBody();
	m_icp->CheckMeanDistanceOn();
	m_icp->StartByMatchingCentroidsOn();
	m_icp->SetMaximumNumberOfIterations(max_steps);
	m_icp->SetMaximumMeanDistance(0.0000001);
	m_icp->SetMaximumNumberOfLandmarks(src_Points->GetNumberOfPoints());
	m_icp->GetLandmarkTransform()->SetModeToRigidBody();
	m_icp->Modified();
	m_icp->Update();

	transform_matrix = m_icp->GetMatrix();
}


//========================Land Mark Registration=========================//

vtkStandardNewMacro(vtkTrackingLandMarkRegistration)
vtkTrackingLandMarkRegistration::vtkTrackingLandMarkRegistration()
{
	m_landmarkTransform = vtkSmartPointer<vtkLandmarkTransform>::New();
}
vtkTrackingLandMarkRegistration::~vtkTrackingLandMarkRegistration()
{}
void vtkTrackingLandMarkRegistration::GenerateTransform()
{
	if (src_Points->GetNumberOfPoints()<1)
	{
		return;
	}
	if (target_Points->GetNumberOfPoints()<1)
	{
		return;
	}
	m_landmarkTransform->SetSourceLandmarks(src_Points);
	m_landmarkTransform->SetTargetLandmarks(target_Points);

	//m_landmarkTransform->SetModeToRigidBody(); //This mode is tested but not good
	m_landmarkTransform->SetModeToAffine(); // Please use this affine mode.
	m_landmarkTransform->Update(); //should this be here? YES

	transform_matrix = m_landmarkTransform->GetMatrix();
}






