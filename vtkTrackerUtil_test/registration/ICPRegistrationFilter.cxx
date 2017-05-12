/**
Author: QIN Shuo
Date: 2017/05/11
*/


#include "ICPRegistrationFilter.h"

ICPRegistrationFilter::ICPRegistrationFilter(QObject *parent) :
AbstractFilter(parent)
{
    m_outputMatrix = vtkMatrix4x4::New();
}

ICPRegistrationFilter::~ICPRegistrationFilter()
{
    m_outputMatrix->Delete();
}

void ICPRegistrationFilter::SetInitialMatrix(vtkMatrix4x4 *matrix)
{
    m_initialMatrix = matrix;
}

void ICPRegistrationFilter::SetInputPolyData(vtkPolyData *polydata)
{
    m_polydata = polydata;
}

void ICPRegistrationFilter::AppendWorldCoordiantion(double* w_coord)
{
    m_worldCoordinationList.append(w_coord);
}

void ICPRegistrationFilter::ClearAllPoints()
{
    m_worldCoordinationList.clear();
}

double ICPRegistrationFilter::GetOutputError()
{
    return m_outputError;
}

vtkMatrix4x4 *ICPRegistrationFilter::GetOutputMatrix()
{
    return m_outputMatrix;
}

/*
Procedure:
source  -- step1  -->   coarse transformed points   -- step2 ->  fine transformed points
        coarse matrix                                fine matrix  

How to get coarse transformed points: use vtkTrackerUtil, set coarse matrix to vtkTrackerUtil and get points
*/
void ICPRegistrationFilter::Compute()
{
    //Add ICP Registration function here
    //with use of m_initialMatrix and polydata

	auto model_points = m_polydata->GetPoints();
	std::vector<double*> landmarklist;
	for (size_t i = 0; i < m_worldCoordinationList.size(); i++)
	{
		landmarklist.push_back(m_worldCoordinationList[i]);
	}
	auto icp_reg = vtkSmartPointer<vtkTrackingICPRegistration>::New();

	icp_reg->SetTargetPoints(model_points);
	icp_reg->SetSourcePoints(landmarklist);
	auto mat = vtkSmartPointer<vtkMatrix4x4>::New();
	mat->DeepCopy(m_initialMatrix);

	icp_reg->SetPreMultipliedMatrix(mat);
	icp_reg->GenerateTransform();

    m_outputMatrix->DeepCopy(icp_reg->GetTransformMatrix());
	m_outputError = icp_reg->EstimateRegistrationError();
}

int ICPRegistrationFilter::Update()
{
	emit Process(50);
	this->Compute();

	emit Process(100);
	emit Finished();
	return 0;
}

