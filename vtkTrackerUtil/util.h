/*
Author: QIN Shuo
Date: 2017/5/11
Organization: RC-MIC (CUHK)
Description:
	utilities.
	need vtk functions
*/


#ifndef __UTIL_H__
#define __UTIL_H__


#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"
#include "TrackerBase.h"

/*
Description:
	Conversion method from matrix to double array
Input:
	matrix: destination matrix
	arr:    source double array (length: 4)
Note:
	This conversion is based on left-handed coordinate,
	So there will be some problems in right-handed coordinate
	The solution is revert the w/q0 direct(multiply by -1)
*/
inline void QuaternionToMatrix(vtkMatrix4x4* matrix, double* arr)
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
};

/*
Description:
Conversion method from  double array to matrix
Input:
matrix: source matrix  (length: 4)
arr:    destination array
*/
inline void MatrixToQuaternion(vtkMatrix4x4* src_matrix, double* arr)
{
	arr[0] = sqrt(1 + src_matrix->GetElement(0, 0) + src_matrix->GetElement(1, 1) + src_matrix->GetElement(2, 2)) / 2; // q0
	arr[1] = (src_matrix->GetElement(2, 1) - src_matrix->GetElement(1, 2)) / (4 * arr[0]);  // qx
	arr[2] = (src_matrix->GetElement(0, 2) - src_matrix->GetElement(2, 0)) / (4 * arr[0]);  // qy
	arr[3] = (src_matrix->GetElement(1, 0) - src_matrix->GetElement(0, 1)) / (4 * arr[0]);  // qz
};



/*
Description:
Convert raw transform to an matrix
Parameters:
index count from 0
*/
inline void TransformToMatrix(QIN_Transform_Type* trans_in, QIN_Matrix_Type trans_out)
{
	auto tem_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	// convert quaternion to matrix
	QuaternionToMatrix(tem_matrix, &(trans_in->q0));

	trans_out[3][0] = 0;
	trans_out[3][1] = 0;
	trans_out[3][2] = 0;
	trans_out[3][3] = 1;

	for (unsigned int i = 0; i<3; i++)
	{
		for (unsigned int j = 0; j<3; j++)
		{
			trans_out[i][j] = tem_matrix->GetElement(i, j);
			//outmatrix.SetElement(i, j, inmatrix.GetVnlMatrix().get(i, j));
		}
	}
	trans_out[0][3] = trans_in->x;
	trans_out[1][3] = trans_in->y;
	trans_out[2][3] = trans_in->z;
	trans_out[3][3] = 1;
};


/*
Input:
	trans_in: source
	outmatrix: destination
*/
inline void TransformToMatrix(QIN_Transform_Type* trans_in, vtkMatrix4x4* outmatrix)
{
	auto rot_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	// convert quaternion to matrix
	QuaternionToMatrix(rot_matrix, &(trans_in->q0));

	outmatrix->SetElement(3, 0, 0);
	outmatrix->SetElement(3, 1, 0);
	outmatrix->SetElement(3, 2, 0);
	outmatrix->SetElement(3, 3, 1);


	for (unsigned int i = 0; i<3; i++)
	{
		for (unsigned int j = 0; j<3; j++)
		{
			outmatrix->SetElement(i, j, rot_matrix->GetElement(i, j));
		}
	}
	outmatrix->SetElement(0, 3, trans_in->x);
	outmatrix->SetElement(1, 3, trans_in->y);
	outmatrix->SetElement(2, 3, trans_in->z);
	outmatrix->SetElement(3, 3, 1);
};


/*
Description:
	convert 2d array to vtkMatrix4x4
*/
inline void MatrixToVTKMatrix(QIN_Matrix_Type trans_in, vtkMatrix4x4* outmatrix)
{
	int length = 4;
	for (size_t i = 0; i < length; i++)
	{
		for (size_t j = 0; j < length; j++)
		{
			outmatrix->SetElement(i, j, trans_in[i][j]);
		}
	}
};










#endif
