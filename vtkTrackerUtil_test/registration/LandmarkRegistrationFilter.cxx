/**
Author: QIN Shuo
Date: 2017/05/11
*/

#include "LandmarkRegistrationFilter.h"

LandmarkRegistrationFilter::LandmarkRegistrationFilter(QObject *parent) :
AbstractFilter(parent)
{
    m_outputMatrix  = vtkMatrix4x4::New();
}

LandmarkRegistrationFilter::~LandmarkRegistrationFilter()
{
    m_outputMatrix->Delete();
}


void LandmarkRegistrationFilter::AppendPairedPoint(double *i_coord, double *w_coord)
{
    m_imageCoordinationList.append(i_coord);
    m_worldCoordinationList.append(w_coord);
}

void LandmarkRegistrationFilter::SetPairedPoint(int index, double *i_coord, double *w_coord)
{
    m_imageCoordinationList[index] = (i_coord);
    m_worldCoordinationList[index] = (w_coord);
}

void LandmarkRegistrationFilter::ClearAllPairedPoint()
{
    m_imageCoordinationList.clear();
    m_worldCoordinationList.clear();
}

double LandmarkRegistrationFilter::GetOutputError()
{
    return m_outputError;
}

vtkMatrix4x4 *LandmarkRegistrationFilter::GetOutputMatrix()
{
    return m_outputMatrix;
}

void LandmarkRegistrationFilter::Compute()
{
	//========= Add by qinshuo ===========//
    // Transform real-world points to patients' space
	std::vector<double*> src_pt;  // points in world coordinate is source points
	std::vector<double*> tgt_pt;  // points in patient coordinate is target points
	for (size_t i = 0; i < m_worldCoordinationList.size(); i++)
	{
		src_pt.push_back(m_worldCoordinationList[i]);
	}
	for (size_t i = 0; i < m_imageCoordinationList.size(); i++)
	{
		tgt_pt.push_back(m_imageCoordinationList[i]);
	}

	auto landmarkReg = vtkSmartPointer<vtkTrackingLandMarkRegistration>::New();
	landmarkReg->SetSourcePoints(src_pt);
	landmarkReg->SetTargetPoints(tgt_pt);
	landmarkReg->GenerateTransform();

    m_outputMatrix->DeepCopy(landmarkReg->GetTransformMatrix() );
    m_outputError = landmarkReg->EstimateRegistrationError();
}

int LandmarkRegistrationFilter::Update()
{
	emit Process(50);
	this->Compute();

	emit Process(100);
	emit Finished();
	return 0;
}


