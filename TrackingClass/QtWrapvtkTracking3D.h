/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/1/12

Description:
	Wrap vtkTracking3D class to qt
	Main purpose is share interactor timer

	This class connect vtkTracking3D event
	to Qt slot by an unique EVENT_ID
*/

#ifndef _QTWRAPVTKTRACKING3D_H_
#define _QTWRAPVTKTRACKING3D_H_


#include <QApplication>
//qt + vtk include
#include "QVTKWidget.h"
#include <QTimer>

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

	void StartTracking2()
	{
		m_Timer->start(m_interval);
		connect(m_Timer, SIGNAL(timeout()), this, SLOT(on_Timer()));
	};

	QtWrapvtkTracking3D()
	{						
		m_Timer = new QTimer;
		
		//   I have tested that, if you want to pass an
		m_MouseClickConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();				   //   VTK User defined event to other object, for 
		m_MouseClickConnect->Connect(this, QIN_S_VTK_EVENT,//vtkCommand::LeftButtonPressEvent, //   example Qt slot, you should define an unique 
			this, SLOT(on_emit_callback(vtkObject*)));										   //   EVENT ID like QIN_S_VTK_EVENT(This ID is 
	};																						   //   defined in vtkTracking3D.h), and then
																							   //   connect in the way like left code block.
	~QtWrapvtkTracking3D()
	{
		disconnect(m_Timer, SIGNAL(timeout()), this, SLOT(on_Timer()));
		delete m_Timer;
	};

public slots:
	void on_emit_callback(vtkObject*)
	{
		emit on_timer_signal_index(this->m_SliceX, this->m_SliceY, this->m_SliceZ);
		emit on_timer_signal_coor(m_marker_tobe_set[0], m_marker_tobe_set[1], m_marker_tobe_set[2]);
	};
	void on_Timer()
	{
		for (auto it = this->m_Obj_Tool_Map.begin(); it != this->m_Obj_Tool_Map.end(); ++it)
		{
			//QIN_Transform_Type trans;
			QIN_Transform_Type* temp;
			//memset(&trans, 0, sizeof(QIN_Transform_Type));
			temp = this->m_tracker->GetTransform(it->second);
			if (temp != NULL)
			{
				// construct transform
				double* coor = this->GetRegisterTransform()->TransformPoint(temp->x, temp->y, temp->z);
				//double* temp = m_LandMarkTransform->TransformVector(trans->x, trans->y, trans->z);

				// put out
				this->m_marker_tobe_set[0] = coor[0];
				this->m_marker_tobe_set[1] = coor[1];
				this->m_marker_tobe_set[2] = coor[2];
				//this emit a signal to connect qt signal, to reslice 2D views
				std::cout << it->first << ":" << temp->x << " " << temp->y << " " << temp->z << std::endl;
			}
			else
			{
				std::cout << "transform invalid" << std::endl;
			}
		}
		emit on_timer_signal_index(this->m_SliceX, this->m_SliceY, this->m_SliceZ);
		emit on_timer_signal_coor(m_marker_tobe_set[0], m_marker_tobe_set[1], m_marker_tobe_set[2]);
	}

signals:
	void on_timer_signal_index(int index_x,int index_y, int index_z);
	void on_timer_signal_coor(double x, double y, double z);
	
private:
	vtkSmartPointer<vtkEventQtSlotConnect> m_TimerConnect;
	vtkSmartPointer<vtkEventQtSlotConnect> m_MouseClickConnect;


	QTimer* m_Timer;
};





#endif