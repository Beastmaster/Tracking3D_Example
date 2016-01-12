/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/1/12

Description:
	Wrap vtkTracking3D class to qt
	Main purpose is share interactor timer
*/

#ifndef _QTWRAPVTKTRACKING3D_H_
#define _QTWRAPVTKTRACKING3D_H_


#include <QApplication>
//qt + vtk include
#include "QVTKWidget.h"
#include "vtkCommand.h"
#include "vtkEventQtSlotConnect.h"

#include "vtkTracking3D.h"


/*
Description:
	This class wrap vtkTracking3D class
	Export timer invoke in vtkTracking3D event
Purpose:
	Send reslice information
*/
class QtWrapvtkTracking3D :public QObject, public vtkTracking3D
{
	Q_OBJECT
public:
	static QtWrapvtkTracking3D* New()
	{
		VTK_STANDARD_NEW_BODY(QtWrapvtkTracking3D);
	};
	vtkTypeMacro(QtWrapvtkTracking3D, vtkTracking3D);

	//explicit QtWrapvtkTracking3D(QWidget *parent = 0);
	QtWrapvtkTracking3D()
	{
		m_TimerConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
		m_TimerConnect->Connect(this->GetInteractor(),
			vtkCommand::TimerEvent, this, SLOT(on_timer_callback(vtkObject*)));
	};
	~QtWrapvtkTracking3D(){};

public slots:
	void on_timer_callback(vtkObject*)
	{
		emit on_timer_signal(this->m_index_tobe_set[0], this->m_index_tobe_set[1], this->m_index_tobe_set[2]);
	};
signals:
	void on_timer_signal(int index_x,int index_y, int index_z);
	
private:
	vtkSmartPointer<vtkEventQtSlotConnect> m_TimerConnect;
};





#endif