/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/01/07 (Modified 2016/11/9)

Description:
	

*/


#include "vtkTrackingMarkCapture.h"



vtkTrackingMarkCapture::vtkTrackingMarkCapture()
{
	m_ToolMarkers.clear();
}

vtkTrackingMarkCapture::~vtkTrackingMarkCapture()
{
	m_ToolMarkers.clear();
}

/*
Description:
1. Set index of tool
2. Count from 0
*/
void vtkTrackingMarkCapture::SetToolID(int id)
{
	m_toolID = id;
}



/*
Description:
1. Set index of reference
2. Count from 0
*/
void vtkTrackingMarkCapture::SetRefID(int id)
{
	m_refID = id;
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
	auto mat = vtkSmartPointer<vtkMatrix4x4>::New();
	m_TrackerUtil->GetTransformMatrix(m_toolID, m_refID, mat);
	m_TrackerUtil->Beep(1);
	m_ToolMarkers.push_back(mat);
	return 0;
}

/*
Delete the last marker
*/
int vtkTrackingMarkCapture::DelLastMarker()
{
	m_ToolMarkers.pop_back();
	return 0;
}


/*
Description:
Clear all markers captured
*/
void vtkTrackingMarkCapture::ClearMarkers()
{
	m_ToolMarkers.clear();
}

/*
Get Number of markers holding on the vector
Return 0 if number of tool and reference are not equal
*/
int vtkTrackingMarkCapture::GetNumberOfMarkers()
{
	return m_ToolMarkers.size();
}


/*
Description:
Return a list of markers, by type std::vector
*/
std::vector<double*> vtkTrackingMarkCapture::GetMarkerList()
{
	std::vector<double* > ret;
	auto tool_iter = m_ToolMarkers.begin();
	for (; tool_iter != m_ToolMarkers.end(); ++tool_iter)
	{
		double* coor;
		coor = new double[3];
		auto tmp = *tool_iter;
		//// get raw matrix
		//auto raw_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
		//TransformToMatrix((*tool_iter), raw_matrix);
		//auto refer_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
		//TransformToMatrix((*refer_iter), refer_matrix);
		//
		//auto invert_refer = vtkSmartPointer<vtkMatrix4x4>::New();
		//invert_refer->Identity();
		//vtkMatrix4x4::Invert(refer_matrix, invert_refer);
		//
		//auto refer_raw = vtkSmartPointer<vtkMatrix4x4>::New();
		//vtkMatrix4x4::Multiply4x4(invert_refer, raw_matrix, refer_raw);
		//
		//auto calibrate_raw = vtkSmartPointer<vtkMatrix4x4>::New();
		//vtkMatrix4x4::Multiply4x4(refer_raw, m_CalibrationMatrix, calibrate_raw);
		////vtkMatrix4x4::Multiply4x4(raw_matrix, m_CalibrationMatrix, calibrate_raw);//test disable reference
		//
		//auto temp = vtkSmartPointer<vtkTransform>::New();
		//temp->SetMatrix(calibrate_raw);

		coor[0] = tmp->GetElement(0, 3);
		coor[1] = tmp->GetElement(1, 3);
		coor[2] = tmp->GetElement(2, 3);
		ret.push_back(coor);
	}
	return ret;
}




