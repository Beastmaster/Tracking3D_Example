/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/01/08

Description:
	Get markers 

Note:
	Please be noted that, save your file regularly is a very
	good manner. Git must be a powerful tool. Every commit 
	you make may save large amount of time. My vs crashed 
	this file onece to be a pile of useless code.
	
	And, be careful with template
*/




#ifndef _VTKTRACKINGMARKCAPTURE_H_
#define _VTKTRACKINGMARKCAPTURE_H_

#include "TrackerBase.h"
#include "vtkTracking3D.h"

#include "vtkSmartPointer.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include <vector>
#include <iostream>

/*
This Class is inhert from vtkObject,
So it is recommanded to use vtkSmartPointer to manage
instead of managing by hand.

There seems no need to use templete here because
there is no tracker type specified functions to be called.

Please be noted that, this class do not configure any tracker,
you should pass a configured tracker in. Because this class 
just call some get data functions.

SetCalibrationMatrix() function is badly needed
*/
template<typename TrackerType>
class vtkTrackingMarkCapture : public vtkObject
{
public:
	static vtkTrackingMarkCapture<TrackerType>* New();// cannot instantiate abstract class
	vtkTypeMacro(vtkTrackingMarkCapture<TrackerType>, vtkObject);
	vtkTrackingMarkCapture();
	~vtkTrackingMarkCapture();

	void SetTracker(TrackerType* in) { m_Tracker = in; };
	void SetToolIndex(int);
	void SetReferIndex(int);
	void SetCalibrationMatrix(vtkMatrix4x4* in) { m_CalibrationMatrix = in; };

	void GetNextMarker();
	void DelLastMarker();
	void ClearMarkers();
	std::vector<double*> GetMarkerList();

private:
	TrackerType* m_Tracker;

	int m_ToolIndex;
	int m_ReferIndex;

	vtkSmartPointer<vtkMatrix4x4>	 m_CalibrationMatrix;

	std::vector<QIN_Transform_Type*> m_ToolMarkers;
	std::vector<QIN_Transform_Type*> m_ReferMarkers;
	
	
};

template<typename TrackerType>
//vtkStandardNewMacro(vtkTrackingMarkCapture<TrackerType>)
vtkTrackingMarkCapture<TrackerType>* vtkTrackingMarkCapture<TrackerType>::New()
{
	return new  vtkTrackingMarkCapture<TrackerType>;
}

template<typename TrackerType>
vtkTrackingMarkCapture<TrackerType>::vtkTrackingMarkCapture()
{
	m_CalibrationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_CalibrationMatrix->Identity();

	m_ToolIndex = 0;
	m_ReferIndex = 1;

	m_ToolMarkers.clear();
	m_ReferMarkers.clear();
}
template<typename TrackerType>
vtkTrackingMarkCapture<TrackerType>::~vtkTrackingMarkCapture()
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
*/
template<typename TrackerType>
void vtkTrackingMarkCapture<TrackerType>::GetNextMarker()
{
	if (m_Tracker->GetTrackingStatus() != 0)
	{
		std::cout << "Not Tracking" << std::endl;
		return;
	}

	QIN_Transform_Type*  tool_trans = m_Tracker->GetTransform(m_ToolIndex);
	QIN_Transform_Type* tem_tool = new QIN_Transform_Type;
	memcpy(tem_tool, tool_trans, sizeof(QIN_Transform_Type));
	m_ToolMarkers.push_back(tem_tool);

	QIN_Transform_Type*  refer_trans = m_Tracker->GetTransform(m_ReferIndex);
	QIN_Transform_Type* tem_refer = new QIN_Transform_Type;
	memcpy(tem_refer, refer_trans, sizeof(QIN_Transform_Type));	
	m_ReferMarkers.push_back(tem_refer);
}

/*
Delete the last marker
*/
template<typename TrackerType>
void vtkTrackingMarkCapture<TrackerType>::DelLastMarker()
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

}


/*
Description:
	Clear all markers captured
*/
template<typename TrackerType>
void vtkTrackingMarkCapture<TrackerType>::ClearMarkers()
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
template<typename TrackerType>
std::vector<double*> vtkTrackingMarkCapture<TrackerType>::GetMarkerList()
{
	std::vector<double* > ret;
	auto refer_iter = m_ReferMarkers.begin();
	auto tool_iter = m_ToolMarkers.begin();
	for (; (tool_iter != m_ToolMarkers.end())&&(refer_iter!=m_ReferMarkers.end()); ++refer_iter,++tool_iter)
	{
		double* coor;
		coor = new double[3];

		// get raw matrix
		auto raw_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
		PivotCalibration2::TransformToMatrix((*tool_iter), raw_matrix);
		auto refer_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
		PivotCalibration2::TransformToMatrix((*refer_iter),refer_matrix);

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
template<typename TrackerType>
void vtkTrackingMarkCapture<TrackerType>::SetToolIndex(int index)
{
	m_ToolIndex = index;
}

/*
Description:
1. Set index of reference
2. Count from 0
*/
template<typename TrackerType>
void vtkTrackingMarkCapture<TrackerType>::SetReferIndex(int index)
{
	m_ReferIndex = index;
}


#endif