/*
Author: QIN Shuo
Date: 2015/01/20
Organization: RC-MIC (CUHK)
Description:
	Pivot Calibration lib
	Calibration the tool and the trakcer data

This file refer to IGSTK
*/


#include "PivotCalibration2.h"

// VNL includes
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_determinant.h"




static const double PARALLEL_ANGLE_THRESHOLD_DEGREES = 20.0;

PivotCalibration2::PivotCalibration2()
{
	this->ToolTipToToolMatrix = vtkMatrix4x4::New();
	this->MinimumOrientationDifferenceDeg = 15.0;
}

PivotCalibration2::~PivotCalibration2()
{
	this->ClearToolToReferenceMatrices();
	this->ToolTipToToolMatrix->Delete();
}

void PivotCalibration2::AddToolToReferenceMatrix(vtkMatrix4x4* transformMatrix)
{
	this->ToolToReferenceMatrices.push_back(transformMatrix);
}



void PivotCalibration2::ClearToolToReferenceMatrices()
{
	std::vector< vtkSmartPointer<vtkMatrix4x4> >::const_iterator it;
	std::vector< vtkSmartPointer<vtkMatrix4x4> >::const_iterator matricesEnd = this->ToolToReferenceMatrices.end();
	this->ToolToReferenceMatrices.clear();
}



double PivotCalibration2::GetOrientationDifferenceDeg(vtkMatrix4x4* aMatrix, vtkMatrix4x4* bMatrix)
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


double PivotCalibration2::GetMaximumToolOrientationDifferenceDeg()
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
bool PivotCalibration2::ComputePivotCalibration()
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

bool PivotCalibration2::ComputeSpinCalibration(bool snapRotation)
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

	// Setup our system to find the axis of rotation
	unsigned int rows = 3, columns = 3;

	vnl_matrix<double> A(rows, columns, 0);

	vnl_matrix<double> I(3, 3, 0);
	I.set_identity();

	vnl_matrix<double> RI(rows, columns);


	// this will store the maximum difference in orientation between the first transform and all the other transforms
	double maximumOrientationDifferenceDeg = 0;

	std::vector< vtkSmartPointer<vtkMatrix4x4> >::const_iterator previt = this->ToolToReferenceMatrices.end();
	for (std::vector< vtkSmartPointer<vtkMatrix4x4> >::const_iterator it = this->ToolToReferenceMatrices.begin(); it != this->ToolToReferenceMatrices.end(); it++)
	{
		if (previt == this->ToolToReferenceMatrices.end())
		{
			previt = it;
			continue; // No comparison to make for the first matrix
		}

		vtkSmartPointer< vtkMatrix4x4 > itinverse = vtkSmartPointer< vtkMatrix4x4 >::New();
		vtkMatrix4x4::Invert((*it), itinverse);

		vtkSmartPointer< vtkMatrix4x4 > instRotation = vtkSmartPointer< vtkMatrix4x4 >::New();
		vtkMatrix4x4::Multiply4x4(itinverse, (*previt), instRotation);

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				RI(i, j) = instRotation->GetElement(i, j);
			}
		}

		RI = RI - I;
		A = A + RI.transpose() * RI;

		previt = it;
	}

	// Note: If the needle orientation protocol changes, only the definitions of shaftAxis and secondaryAxes need to be changed
	// Define the shaft axis and the secondary shaft axis
	// Current needle orientation protocol dictates: shaft axis -z, orthogonal axis +x
	// If StylusX is parallel to ShaftAxis then: shaft axis -z, orthogonal axis +y
	vnl_vector<double> shaftAxis_Shaft(columns, 0); shaftAxis_Shaft(0) = 0; shaftAxis_Shaft(1) = 0; shaftAxis_Shaft(2) = -1;
	vnl_vector<double> orthogonalAxis_Shaft(columns, 0); orthogonalAxis_Shaft(0) = 1; orthogonalAxis_Shaft(1) = 0; orthogonalAxis_Shaft(2) = 0;
	vnl_vector<double> backupAxis_Shaft(columns, 0); backupAxis_Shaft(0) = 0; backupAxis_Shaft(1) = 1; backupAxis_Shaft(2) = 0;

	// Find the eigenvector associated with the smallest eigenvalue
	// This is the best axis of rotation over all instantaneous rotations
	vnl_matrix<double> eigenvectors(columns, columns, 0);
	vnl_vector<double> eigenvalues(columns, 0);
	vnl_symmetric_eigensystem_compute(A, eigenvectors, eigenvalues);
	// Note: eigenvectors are ordered in increasing eigenvalue ( 0 = smallest, end = biggest )
	vnl_vector<double> shaftAxis_ToolTip(columns, 0);
	shaftAxis_ToolTip(0) = eigenvectors(0, 0);
	shaftAxis_ToolTip(1) = eigenvectors(1, 0);
	shaftAxis_ToolTip(2) = eigenvectors(2, 0);
	shaftAxis_ToolTip.normalize();

	// Snap the direction vector to be exactly aligned with one of the coordinate axes
	// This is if the sensor is known to be parallel to one of the axis, just not which one
	if (snapRotation)
	{
		int closestCoordinateAxis = element_product(shaftAxis_ToolTip, shaftAxis_ToolTip).arg_max();
		shaftAxis_ToolTip.fill(0);
		shaftAxis_ToolTip.put(closestCoordinateAxis, 1); // Doesn't matter the direction, will be sorted out in the next step
	}

	// Make sure it is in the correct direction (opposite the StylusTipToStylus translation)
	vnl_vector<double> toolTipToToolTranslation(3);
	toolTipToToolTranslation(0) = this->ToolTipToToolMatrix->GetElement(0, 3);
	toolTipToToolTranslation(1) = this->ToolTipToToolMatrix->GetElement(1, 3);
	toolTipToToolTranslation(2) = this->ToolTipToToolMatrix->GetElement(2, 3);
	if (dot_product(shaftAxis_ToolTip, toolTipToToolTranslation) > 0)
	{
		shaftAxis_ToolTip = shaftAxis_ToolTip * (-1);
	}

	//set the RMSE
	this->SpinRMSE = (A * shaftAxis_ToolTip).rms();


	// If the secondary axis 1 is parallel to the shaft axis in the tooltip frame, then use secondary axis 2
	vnl_vector<double> orthogonalAxis_ToolTip;
	double angle = acos(dot_product(shaftAxis_ToolTip, orthogonalAxis_Shaft));
	// Force angle to be between -pi/2 and +pi/2
	if (angle > vtkMath::Pi() / 2)
	{
		angle -= vtkMath::Pi();
	}
	if (angle < -vtkMath::Pi() / 2)
	{
		angle += vtkMath::Pi();
	}
	if (fabs(angle) > vtkMath::RadiansFromDegrees(PARALLEL_ANGLE_THRESHOLD_DEGREES)) // If shaft axis and orthogonal axis are not parallel
	{
		orthogonalAxis_ToolTip = orthogonalAxis_Shaft;
	}
	else
	{
		orthogonalAxis_ToolTip = backupAxis_Shaft;
	}

	// Do the registration find the appropriate rotation
	orthogonalAxis_ToolTip = orthogonalAxis_ToolTip - dot_product(orthogonalAxis_ToolTip, shaftAxis_ToolTip) * shaftAxis_ToolTip;
	orthogonalAxis_ToolTip.normalize();

	// Register X,Y,O points in the two coordinate frames (only spherical registration - since pure rotation)
	vnl_matrix<double> ToolTipPoints(3, 3, 0.0);
	vnl_matrix<double> ShaftPoints(3, 3, 0.0);

	ToolTipPoints.put(0, 0, shaftAxis_ToolTip(0));
	ToolTipPoints.put(0, 1, shaftAxis_ToolTip(1));
	ToolTipPoints.put(0, 2, shaftAxis_ToolTip(2));
	ToolTipPoints.put(1, 0, orthogonalAxis_ToolTip(0));
	ToolTipPoints.put(1, 1, orthogonalAxis_ToolTip(1));
	ToolTipPoints.put(1, 2, orthogonalAxis_ToolTip(2));
	ToolTipPoints.put(2, 0, 0);
	ToolTipPoints.put(2, 1, 0);
	ToolTipPoints.put(2, 2, 0);

	ShaftPoints.put(0, 0, shaftAxis_Shaft(0));
	ShaftPoints.put(0, 1, shaftAxis_Shaft(1));
	ShaftPoints.put(0, 2, shaftAxis_Shaft(2));
	ShaftPoints.put(1, 0, orthogonalAxis_Shaft(0));
	ShaftPoints.put(1, 1, orthogonalAxis_Shaft(1));
	ShaftPoints.put(1, 2, orthogonalAxis_Shaft(2));
	ShaftPoints.put(2, 0, 0);
	ShaftPoints.put(2, 1, 0);
	ShaftPoints.put(2, 2, 0);

	vnl_svd<double> ShaftToToolTipRegistrator(ShaftPoints.transpose() * ToolTipPoints);
	vnl_matrix<double> V = ShaftToToolTipRegistrator.V();
	vnl_matrix<double> U = ShaftToToolTipRegistrator.U();
	vnl_matrix<double> Rotation = V * U.transpose();

	// Make sure the determinant is positve (i.e. +1)
	double determinant = vnl_determinant(Rotation);
	if (determinant < 0)
	{
		// Switch the sign of the third column of V if the determinant is not +1
		// This is the recommended approach from Huang et al. 1987
		V.put(0, 2, -V.get(0, 2));
		V.put(1, 2, -V.get(1, 2));
		V.put(2, 2, -V.get(2, 2));
		Rotation = V * U.transpose();
	}

	// Set the elements of the output matrix
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->ToolTipToToolMatrix->SetElement(i, j, Rotation[i][j]);
		}
	}

	this->ErrorText.empty();
	return true;
}
//---------------------------------------------------------------------------
void PivotCalibration2::GetToolTipToToolTranslation(vtkMatrix4x4* translationMatrix)
{
	translationMatrix->Identity();

	translationMatrix->SetElement(0, 3, this->ToolTipToToolMatrix->GetElement(0, 3));
	translationMatrix->SetElement(1, 3, this->ToolTipToToolMatrix->GetElement(1, 3));
	translationMatrix->SetElement(2, 3, this->ToolTipToToolMatrix->GetElement(2, 3));
}

//---------------------------------------------------------------------------
void PivotCalibration2::GetToolTipToToolRotation(vtkMatrix4x4* rotationMatrix)
{
	rotationMatrix->Identity();

	rotationMatrix->SetElement(0, 0, this->ToolTipToToolMatrix->GetElement(0, 0));
	rotationMatrix->SetElement(0, 1, this->ToolTipToToolMatrix->GetElement(0, 1));
	rotationMatrix->SetElement(0, 2, this->ToolTipToToolMatrix->GetElement(0, 2));
	rotationMatrix->SetElement(1, 0, this->ToolTipToToolMatrix->GetElement(1, 0));
	rotationMatrix->SetElement(1, 1, this->ToolTipToToolMatrix->GetElement(1, 1));
	rotationMatrix->SetElement(1, 2, this->ToolTipToToolMatrix->GetElement(1, 2));
	rotationMatrix->SetElement(2, 0, this->ToolTipToToolMatrix->GetElement(2, 0));
	rotationMatrix->SetElement(2, 1, this->ToolTipToToolMatrix->GetElement(2, 1));
	rotationMatrix->SetElement(2, 2, this->ToolTipToToolMatrix->GetElement(2, 2));
}

//---------------------------------------------------------------------------
void PivotCalibration2::GetToolTipToToolMatrix(vtkMatrix4x4* matrix)
{
	matrix->DeepCopy(this->ToolTipToToolMatrix);
}




/*
Description:
Conversion method from matrix to double array
Input:
matrix: destination matrix
arr:    source double array (length: 4)
*/
void PivotCalibration2::QuaternionToMatrix(vtkMatrix4x4* matrix, double* arr)
{
	double q0 = arr[0];
	double qx = arr[1];
	double qy = arr[2];
	double qz = arr[3];


	// Determine some calculations done more than once.

	double fQ0Q0 = q0 * q0;
	double fQxQx = qx * qx;
	double fQyQy = qy * qy;
	double fQzQz = qz * qz;
	double fQ0Qx = q0 * qx;
	double fQ0Qy = q0 * qy;
	double fQ0Qz = q0 * qz;
	double fQxQy = qx * qy;
	double fQxQz = qx * qz;
	double fQyQz = qy * qz;

	// Determine the rotation matrix elements.

	double Qmatrix[3][3];
	Qmatrix[0][0] = fQ0Q0 + fQxQx - fQyQy - fQzQz;
	Qmatrix[0][1] = 2.0 * (-fQ0Qz + fQxQy);
	Qmatrix[0][2] = 2.0 * (fQ0Qy + fQxQz);
	Qmatrix[1][0] = 2.0 * (fQ0Qz + fQxQy);
	Qmatrix[1][1] = fQ0Q0 - fQxQx + fQyQy - fQzQz;
	Qmatrix[1][2] = 2.0 * (-fQ0Qx + fQyQz);
	Qmatrix[2][0] = 2.0 * (-fQ0Qy + fQxQz);
	Qmatrix[2][1] = 2.0 * (fQ0Qx + fQyQz);
	Qmatrix[2][2] = fQ0Q0 - fQxQx - fQyQy + fQzQz;

	// put to matrix
	matrix->Identity();
	int length = 3;
	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < length; j++)
		{
			matrix->SetElement(i, j, Qmatrix[i][j]);
		}
	}
}

/*
Description:
Conversion method from  double array to matrix
Input:
matrix: source matrix  (length: 4)
arr:    destination array
*/
void PivotCalibration2::MatrixToQuaternion(vtkMatrix4x4*, double*)
{

}
