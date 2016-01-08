/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/01/07

Description:


*/


#ifndef _VTKTRACKINGMARKCAPTURE_HXX_
#define _VTKTRACKINGMARKCAPTURE_HXX_

#include "vtkTrackingMarkCapture.h"

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
	}
	for (auto it = m_ReferMarkers.begin(); it != m_ReferMarkers.end(); ++it)
	{
		delete *it;
	}
}


/*

*/
template<typename TrackerType>
void vtkTrackingMarkCapture<TrackerType>::GetNextMarker()
{
	if (m_Tracker->GetTrackingStatus()!=0)
	{
		std::cout << "Not Tracking" << std::endl;
		return;
	}


	

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