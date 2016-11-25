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

#include <QApplication>
//qt + vtk include
#include "QVTKWidget.h"
#include <QTimer>

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


class vtkTrackingMarkCapture :public QObject, public vtkObject
{	
	Q_OBJECT
public:
	typedef TrackerBase TrackerType;
	static vtkTrackingMarkCapture* New()// cannot instantiate abstract class
	{
		VTK_STANDARD_NEW_BODY(vtkTrackingMarkCapture);
	};
	vtkTypeMacro(vtkTrackingMarkCapture, vtkObject);

	vtkTrackingMarkCapture();
	~vtkTrackingMarkCapture();

	void SetTracker(TrackerType* in) { m_Tracker = in; };
	void SetToolIndex(int);
	void SetReferIndex(int);
	void SetCalibrationMatrix(vtkMatrix4x4* in) { m_CalibrationMatrix = in; };

	int GetNextMarker();
	int DelLastMarker();
	void ClearMarkers();
	int GetNumberOfMarkers();
	std::vector<double*> GetMarkerList();


signals:
	void qs_transform_valid(int);// if transform is invalid, this signal will emitted

private:
	TrackerType* m_Tracker;

	int m_ToolIndex;
	int m_ReferIndex;

	vtkSmartPointer<vtkMatrix4x4>	 m_CalibrationMatrix;

	std::vector<QIN_Transform_Type*> m_ToolMarkers;
	std::vector<QIN_Transform_Type*> m_ReferMarkers;
	
	
};


#endif