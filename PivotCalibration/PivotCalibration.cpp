/*
Author: QIN Shuo
Date: 2015/01/20
Organization: RC-MIC (CUHK)

Description:
	Pivot Calibration lib

Calibration the tool and the trakcer data

*/


#include "PivotCalibration.h"

// VNL includes
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_determinant.h"




static const double PARALLEL_ANGLE_THRESHOLD_DEGREES = 20.0;

PivotCalibration::PivotCalibration()
{
	this->ToolTipToToolMatrix = vtkMatrix4x4::New();
	this->MinimumOrientationDifferenceDeg = 15.0;
}

PivotCalibration::~PivotCalibration()
{
	this->ClearToolToReferenceMatrices();
	this->ToolTipToToolMatrix->Delete();
}

void PivotCalibration::AddToolToReferenceMatrix(vtkMatrix4x4* transformMatrix)
{
	this->ToolToReferenceMatrices.push_back(transformMatrix);
}



void PivotCalibration::ClearToolToReferenceMatrices()
{
	std::vector< vtkSmartPointer<vtkMatrix4x4> >::const_iterator it;
	std::vector< vtkSmartPointer<vtkMatrix4x4> >::const_iterator matricesEnd = this->ToolToReferenceMatrices.end();
	this->ToolToReferenceMatrices.clear();
}



double PivotCalibration::GetOrientationDifferenceDeg(vtkMatrix4x4* aMatrix, vtkMatrix4x4* bMatrix)
{
	vtkSmartPointer<vtkMatrix4x4> diffMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> invBmatrix = vtkSmartPointer<vtkMatrix4x4>::New();

	vtkMatrix4x4::Invert(bMatrix, invBmatrix);

	vtkMatrix4x4::Multiply4x4(aMatrix, invBmatrix, diffMatrix);

	vtkSmartPointer<vtkTransform> diffTransform = vtkSmartPointer<vtkTransform>::New();
	diffTransform->SetMatrix(diffMatrix);

	double angleDiff_rad = vtkMath::RadiansFromDegrees(diffTransform->GetOrientationWXYZ()[0]);

	double normalizedAngleDiff_rad = atan2(sin(angleDiff_rad), cos(angleDiff_rad)); // normalize angle to domain -pi, pi 

	return vtkMath::DegreesFromRadians(normalizedAngleDiff_rad);
}


double PivotCalibration::GetMaximumToolOrientationDifferenceDeg()
{
	// this will store the maximum difference in orientation between the first transform and all the other transforms
	double maximumOrientationDifferenceDeg = 0;

	std::vector<vtkSmartPointer<vtkMatrix4x4> >::const_iterator it;
	std::vector<vtkSmartPointer<vtkMatrix4x4> >::const_iterator matricesEnd = this->ToolToReferenceMatrices.end();
	unsigned int currentRow;
	vtkMatrix4x4* referenceOrientationMatrix = this->ToolToReferenceMatrices.front();
	for (currentRow = 0, it = this->ToolToReferenceMatrices.begin(); it != matricesEnd; it++, currentRow += 3)
	{
		double orientationDifferenceDeg = GetOrientationDifferenceDeg(referenceOrientationMatrix, (*it));
		if (maximumOrientationDifferenceDeg < orientationDifferenceDeg)
		{
			maximumOrientationDifferenceDeg = orientationDifferenceDeg;
		}
	}
	return maximumOrientationDifferenceDeg;
}

/*
Description:
	
*/
bool PivotCalibration::ComputePivotCalibration()
{
	if (this->ToolToReferenceMatrices.size() < 10)
	{
		this->ErrorText = "Not enough input transforms are available";
		return false;
	}

	if (this->GetMaximumToolOrientationDifferenceDeg() < this->MinimumOrientationDifferenceDeg)
	{
		this->ErrorText = "Not enough variation in the input transforms";
		return false;
	}

	unsigned int rows = 3 * this->ToolToReferenceMatrices.size();
	unsigned int columns = 6;


	vnl_matrix<double> A(rows, columns);

	vnl_matrix<double> minusI(3, 3, 0);
	minusI(0, 0) = -1;
	minusI(1, 1) = -1;
	minusI(2, 2) = -1;

	vnl_matrix<double> R(3, 3);
	vnl_vector<double> b(rows);
	vnl_vector<double> x(columns);
	vnl_vector<double> t(3);

	std::vector<vtkSmartPointer<vtkMatrix4x4> >::const_iterator it;
	std::vector<vtkSmartPointer<vtkMatrix4x4> >::const_iterator matricesEnd = this->ToolToReferenceMatrices.end();
	unsigned int currentRow;
	vtkMatrix4x4* referenceOrientationMatrix = this->ToolToReferenceMatrices.front();
	for (currentRow = 0, it = this->ToolToReferenceMatrices.begin(); it != matricesEnd; it++, currentRow += 3)
	{
		for (int i = 0; i < 3; i++)
		{
			t(i) = (*it)->GetElement(i, 3);
		}
		t *= -1;
		b.update(t, currentRow);

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				R(i, j) = (*it)->GetElement(i, j);
			}
		}
		A.update(R, currentRow, 0);
		A.update(minusI, currentRow, 3);
	}

	vnl_svd<double> svdA(A);
	svdA.zero_out_absolute(1e-1);
	x = svdA.solve(b);

	//set the RMSE
	this->PivotRMSE = (A * x - b).rms();

	//set the transformation
	this->ToolTipToToolMatrix->SetElement(0, 3, x[0]);
	this->ToolTipToToolMatrix->SetElement(1, 3, x[1]);
	this->ToolTipToToolMatrix->SetElement(2, 3, x[2]);

	this->ErrorText.empty();
	return true;
}

