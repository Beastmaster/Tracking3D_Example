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

	void GetNextMarker();
	void ClearMarkers();
	std::vector<double*> GetMarkerList();

private:
	TrackerType* m_Tracker;

	int m_ToolIndex;
	int m_ReferIndex;

	std::vector<QIN_Transform_Type*> m_ToolMarkers;
	std::vector<QIN_Transform_Type*> m_ReferMarkers;
	
	
};

template<typename TrackerType>
vtkStandardNewMacro(vtkTrackingMarkCapture<TrackerType>)

template<typename TrackerType>
vtkTrackingMarkCapture<TrackerType>::vtkTrackingMarkCapture()
{
	m_ToolIndex = 0;
	m_ReferIndex = 0;

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
	QIN_Transform_Type*  refer_trans = m_Tracker->GetTransform(m_ReferIndex);

	QIN_Transform_Type* tem_tool = new QIN_Transform_Type;
	QIN_Transform_Type* tem_refer = new QIN_Transform_Type;

	memcpy(tem_tool, tool_trans, sizeof(QIN_Transform_Type));
	memcpy(tem_refer, refer_trans, sizeof(QIN_Transform_Type));

	m_ToolMarkers.push_back(tem_tool);
	m_ReferMarkers.push_back(tem_refer);
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
	for (auto it = m_ToolMarkers.begin(); it != m_ToolMarkers.end(); ++it)
	{
		double* coor;
		coor = new double[3];
		coor[0] = (*it)->x;
		coor[1] = (*it)->y;
		coor[2] = (*it)->z;
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