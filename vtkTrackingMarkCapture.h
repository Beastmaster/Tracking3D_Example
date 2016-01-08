/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/01/07

Description:


*/




#ifndef _VTKTRACKINGMARKCAPTURE_H_
#define _VTKTRACKINGMARKCAPTURE_H_

#include "TrackerBase.h"

#include "vtkSmartPointer.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include <vector>



template<typename TrackerType>
class vtkTrackingMarkCapture : public vtkObject
{
public:
	static vtkTrackingMarkCapture<TrackerType>* New();// cannot instantiate abstract class
	vtkTypeMacro(vtkTrackingMarkCapture<TrackerType>, vtkObject);
	vtkTrackingMarkCapture();
	~vtkTrackingMarkCapture();

	void SetTracker(TrackerType* in) { m_Tracker = in; };
	void GetNextMarker(){};

private:
	TrackerType* m_Tracker;

	std::vector<QIN_Transform_Type*> m_ToolMarkers;
	std::vector<QIN_Transform_Type*> m_ReferMarkers;

};

template<typename TrackerType>
vtkStandardNewMacro(vtkTrackingMarkCapture<TrackerType>)

template<typename TrackerType>
vtkTrackingMarkCapture<TrackerType>::vtkTrackingMarkCapture()
{
}
template<typename TrackerType>
vtkTrackingMarkCapture<TrackerType>::~vtkTrackingMarkCapture()
{
}







#endif