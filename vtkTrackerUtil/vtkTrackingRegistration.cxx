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
	m_transform_matrix = vtkSmartPointer<vtkMatrix4x4>::New();

	m_Error = 0.0;
}
vtkTrackingRegistrationBase::~vtkTrackingRegistrationBase()
{
}

/*
Description:
	Set input points to be registered
	Typically the points from tracker
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
		//double* temp = new double[3];
		//memcpy(temp, *it, 3 * sizeof(double));
		//m_target_Points.push_back(temp);
	}
	m_target_Points = xx2;
}

/*
Output a 4x4 matrix
*/
vtkSmartPointer<vtkMatrix4x4> vtkTrackingRegistrationBase::GetTransformMatrix()
{
	return m_transform_matrix;
}

/* Output the estimated registration error from buffer  */
double vtkTrackingRegistrationBase::EstimateRegistrationError()
{
	return m_Error;
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
Tobe continued
*/
void vtkTrackingRegistrationBase::ComputeLandmarkPrincipalAxes()
{

}

/*
Tobe continued
*/
void vtkTrackingRegistrationBase::ComputeRMSDistanceLandmarksFromPrincipalAxes()
{
}

/*
Description:
	Start computing the error between the source point and the target point
	Compute RMSD of registration result
Main functions:
	1. Set/Get input markers
	2. Set Iterate Steps
	3. Get transform matrix
Return:
	None
*/
void vtkTrackingRegistrationBase::EstimatingRegistrationError()
{
	double error = 0.0;
	auto trans = vtkSmartPointer<vtkTransform>::New();
	trans->PostMultiply();
	trans->SetMatrix(m_transform_matrix);

	auto src_iter = m_src_Points.begin();
	auto tgt_iter = m_target_Points.begin();

	double temp[3];

	while ((src_iter != m_src_Points.end()) && (tgt_iter != m_target_Points.end()))
	{
		trans->TransformPoint(*src_iter,temp);
		error += ((*tgt_iter)[0] - temp[0])*((*tgt_iter)[0] - temp[0]); 
		error += ((*tgt_iter)[1] - temp[1])*((*tgt_iter)[1] - temp[1]);
		error += ((*tgt_iter)[2] - temp[2])*((*tgt_iter)[2] - temp[2]);

		++src_iter;
		++tgt_iter;
	}

	m_Error = sqrt(error/m_src_Points.size());
}












//===================== ICP Registration====================//

vtkStandardNewMacro(vtkTrackingICPRegistration)
vtkTrackingICPRegistration::vtkTrackingICPRegistration()
{
	m_icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
	m_pre_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	max_steps = 1000;
}
vtkTrackingICPRegistration::~vtkTrackingICPRegistration()
{}


void vtkTrackingICPRegistration::EstimatingRegistrationError()
{
	// transform source points
	auto source_poly = vtkSmartPointer<vtkPolyData>::New();
	source_poly->SetPoints(src_Points);
	auto source_vertex = vtkSmartPointer<vtkVertexGlyphFilter>::New();
#if VTK_MAJOR_VERSION <= 5
	source_vertex->SetInput(source_poly);
#else
	source_vertex->SetInputData(source_poly);
#endif
	source_vertex->Update();
	auto src_transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
	auto src_transform = vtkSmartPointer<vtkTransform>::New();
	src_transform->SetMatrix(m_post_matrix);
	src_transformFilter->SetTransform(src_transform);
	src_transformFilter->SetInputData(source_vertex->GetOutput());
	src_transformFilter->Update();
	auto tranformed_src_poly = src_transformFilter->GetOutput();

	m_Error = 0.0;
	// build a kDTree from target_points
	auto kDTree = vtkSmartPointer<vtkKdTree>::New();
	kDTree->BuildLocatorFromPoints(target_Points);
	for (int i = 0; i < tranformed_src_poly->GetNumberOfPoints(); i++)
	{
		double closestDist = 0.0;
		kDTree->FindClosestPoint(tranformed_src_poly->GetPoint(i), closestDist);
		m_Error += closestDist;
	}
	m_Error /= tranformed_src_poly->GetNumberOfPoints();
}

void vtkTrackingICPRegistration::GenerateTransform()
{
	auto source_poly = vtkSmartPointer<vtkPolyData>::New();
	auto target_poly = vtkSmartPointer<vtkPolyData>::New();

	if (src_Points->GetNumberOfPoints() < 1)
	{
		return;
	}
	if (target_Points->GetNumberOfPoints() < 1)
	{
		return;
	}

	//convert point to polydata first
	source_poly->SetPoints(src_Points);
	target_poly->SetPoints(target_Points);

	//to accerlate calculation: convert to vertex
	auto source_vertex = vtkSmartPointer<vtkVertexGlyphFilter>::New();
#if VTK_MAJOR_VERSION <= 5
	source_vertex->SetInput(source_poly);
#else
	source_vertex->SetInputData(source_poly);
#endif
	source_vertex->Update();
	auto target_vertex = vtkSmartPointer<vtkVertexGlyphFilter>::New();

#if VTK_MAJOR_VERSION <= 5
	target_vertex->SetInput(target_poly);
#else
	target_vertex->SetInputData(target_poly);
#endif
	target_vertex->Update();

	m_icp->SetSource(source_vertex->GetOutput());
	m_icp->SetTarget(target_vertex->GetOutput());
	m_icp->GetLandmarkTransform()->SetModeToRigidBody();
	//m_icp->CheckMeanDistanceOn();  ####################### This line must be disabled!!!!!!!!! #############
	m_icp->SetMaximumNumberOfIterations(max_steps);
	m_icp->Modified();
	m_icp->Update();

	m_post_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_post_matrix->DeepCopy(m_icp->GetMatrix());
	m_transform_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Multiply4x4(m_post_matrix, m_pre_matrix, m_transform_matrix);

	EstimatingRegistrationError(); //this line compute the registration error
}
void vtkTrackingICPRegistration::SetPreMultipliedMatrix(vtkMatrix4x4* mat)
{
	m_pre_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_pre_matrix->DeepCopy(mat);
}

/*
This function is deprecated..
No need to transform source points with coarse transform matrix
*/
int vtkTrackingICPRegistration::Pre_process()
{
	//src_Points

	// step1: transform with pre_matrix
	auto transform = vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(m_pre_matrix);
	auto trans_Filter = vtkSmartPointer<vtkTransformFilter>::New();
	trans_Filter->SetTransform(transform);
	auto temp_pointset = vtkSmartPointer<vtkPolyData>::New();
	temp_pointset->SetPoints(src_Points);
	trans_Filter->SetInputData(temp_pointset);
	trans_Filter->Update();
	src_Points = trans_Filter->GetOutput()->GetPoints();

	return 0;
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

	m_landmarkTransform->SetModeToRigidBody(); //This mode is tested but not good
	//m_landmarkTransform->SetModeToAffine(); // Please use this affine mode.
	m_landmarkTransform->Update(); //should this be here? YES

	m_transform_matrix = m_landmarkTransform->GetMatrix();

	EstimatingRegistrationError(); //this line compute the registration error
}






