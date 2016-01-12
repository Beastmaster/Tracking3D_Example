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
	{																						   //   I have tested that, if you want to pass an
		m_MouseClickConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();				   //   VTK User defined event to other object, for 
		m_MouseClickConnect->Connect( this,QIN_S_VTK_EVENT,//vtkCommand::LeftButtonPressEvent, //   example Qt slot, you should define an unique 
			this, SLOT(on_emit_callback(vtkObject*)));										   //   EVENT ID like QIN_S_VTK_EVENT, and then
	};																						   //   connect in the way like left code block.
	~QtWrapvtkTracking3D(){};

public slots:
	void on_emit_callback(vtkObject*)
	{
		emit on_timer_signal_index(this->m_index_tobe_set[0], this->m_index_tobe_set[1], this->m_index_tobe_set[2]);
		emit on_timer_signal_coor(m_marker_tobe_set[0], m_marker_tobe_set[1], m_marker_tobe_set[2]);
	};
signals:
	void on_timer_signal_index(int index_x,int index_y, int index_z);
	void on_timer_signal_coor(double x, double y, double z);
	
private:
	vtkSmartPointer<vtkEventQtSlotConnect> m_TimerConnect;
	vtkSmartPointer<vtkEventQtSlotConnect> m_MouseClickConnect;
};





#endif