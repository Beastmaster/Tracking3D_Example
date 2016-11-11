/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/01/07 (Modified 2016/11/9)

Description:
	

*/


#include "vtkTrackingMarkCapture.h"



vtkTrackingMarkCapture::vtkTrackingMarkCapture()
{
	m_CalibrationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_CalibrationMatrix->Identity();

	m_ToolIndex = 0;
	m_ReferIndex = 1;

	m_ToolMarkers.clear();
	m_ReferMarkers.clear();
}

vtkTrackingMarkCapture::~vtkTrackingMarkCapture()
{
	for (auto it = m_ToolMarkers.begin(); it != m_ToolMarkers.end(); ++it)
	{
		delete *it;
		*it = NULL;
	}
	m_ToolMarkers.clear();
	for (auto it = m_ReferMarkers.begin(); it != m_ReferMarkers.end(); ++it)
	{
		delete *it;
		*it = NULL;
	}
	m_ReferMarkers.clear();
}


/*
Description:
When all configurations are done
Call this function to append the marker to the
bufer list
return:
0: success
1: transform invalid
*/
int vtkTrackingMarkCapture::GetNextMarker()
{
	if (m_Tracker->GetTrackingStatus() != 0)
	{
		emit qs_transform_valid(0);
		std::cout << "Not Tracking" << std::endl;
		return 1;
	}

	std::cout << "Selecting " << m_ToolMarkers.size() + 1 << " th marker" << std::endl;
	QIN_Transform_Type* tool_trans = m_Tracker->GetTransform(m_ToolIndex);
	QIN_Transform_Type*  refer_trans = m_Tracker->GetTransform(m_ReferIndex);

	if ((tool_trans == NULL) && (refer_trans == NULL))
	{
		emit qs_transform_valid(0);
		return 1;
	}

	QIN_Transform_Type* tem_tool = new QIN_Transform_Type;
	memcpy(tem_tool, tool_trans, sizeof(QIN_Transform_Type));
	m_ToolMarkers.push_back(tem_tool);

	QIN_Transform_Type* tem_refer = new QIN_Transform_Type;
	memcpy(tem_refer, refer_trans, sizeof(QIN_Transform_Type));
	m_ReferMarkers.push_back(tem_refer);
	emit qs_transform_valid(1);
	return 0;
}

/*
Delete the last marker
*/
int vtkTrackingMarkCapture::DelLastMarker()
{
	if (m_ToolMarkers.size()>0)
	{
		delete m_ToolMarkers.back();
		m_ToolMarkers.pop_back();
	}
	if (m_ReferMarkers.size()>0)
	{
		delete m_ReferMarkers.back();
		m_ReferMarkers.pop_back();
	}
	return 0;
}


/*
Description:
Clear all markers captured
*/
void vtkTrackingMarkCapture::ClearMarkers()
{
	for (auto it = m_ToolMarkers.begin(); it != m_ToolMarkers.end(); ++it)
	{
		delete *it;
		*it = NULL;
	}
	m_ToolMarkers.clear();
	for (auto it = m_ReferMarkers.begin(); it != m_ReferMarkers.end(); ++it)
	{
		delete *it;
		*it = NULL;
	}
	m_ReferMarkers.clear();
}

/*
Description:
Return a list of markers, by type std::vector
*/
std::vector<double*> vtkTrackingMarkCapture::GetMarkerList()
{
	std::vector<double* > ret;
	auto refer_iter = m_ReferMarkers.begin();
	auto tool_iter = m_ToolMarkers.begin();
	for (; (tool_iter != m_ToolMarkers.end()) && (refer_iter != m_ReferMarkers.end()); ++refer_iter, ++tool_iter)
	{
		double* coor;
		coor = new double[3];

		// get raw matrix
		auto raw_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
		PivotCalibration2::TransformToMatrix((*tool_iter), raw_matrix);
		auto refer_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
		PivotCalibration2::TransformToMatrix((*refer_iter), refer_matrix);

		auto invert_refer = vtkSmartPointer<vtkMatrix4x4>::New();
		invert_refer->Identity();
		vtkMatrix4x4::Invert(refer_matrix, invert_refer);

		auto refer_raw = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Multiply4x4(invert_refer, raw_matrix, refer_raw);

		auto calibrate_raw = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Multiply4x4(refer_raw, m_CalibrationMatrix, calibrate_raw);
		//vtkMatrix4x4::Multiply4x4(raw_matrix, m_CalibrationMatrix, calibrate_raw);//test disable reference

		auto temp = vtkSmartPointer<vtkTransform>::New();
		temp->SetMatrix(calibrate_raw);


		coor[0] = temp->GetPosition()[0];
		coor[1] = temp->GetPosition()[1];
		coor[2] = temp->GetPosition()[2];
		ret.push_back(coor);
	}
	return ret;
}


/*
Description:
1. Set index of tool
2. Count from 0
*/
void vtkTrackingMarkCapture::SetToolIndex(int index)
{
	m_ToolIndex = index;
}

/*
Description:
1. Set index of reference
2. Count from 0
*/
void vtkTrackingMarkCapture::SetReferIndex(int index)
{
	m_ReferIndex = index;
}



