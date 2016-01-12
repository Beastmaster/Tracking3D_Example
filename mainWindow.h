/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/1/10

*/


#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include "ui_mainWindow.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <functional>
#include <algorithm>

#include <QMainWindow>
#include <QApplication>
#include <QFileDialog>
#include <QWheelEvent>
#include <QMessageBox>

#include "vtkNIFTIImageReader.h"

//qt + vtk include
#include "QVTKWidget.h"
//qt + vtk connect signal and slot
#include "vtkCommand.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkMarchingCubes.h"
#include "vtkSphereSource.h"
#include "vtkSTLReader.h"

#include "PolarisVicraConfiguration.h"
#include "ATC3DGConfiguration.h"
#include "ResliceView.h"
#include "vtkTracking3D.h"
#include "vtkTrackingMarkCapture.h"
#include "vtkTrackingRegistration.h"
#include "QtWrapvtkTracking3D.h"


namespace Ui {
	class MainWindow;
}



class MainWindow: public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void sys_Init();
	void createActions();

public slots:
	void on_ResliceAction(double,double,double);
	void on_Load_Image();
	void on_Load_Atlas();
	void on_Del_Atlas();
	void on_Sel_Tracker();
	void on_Config_Tracker();
	void on_Sel_Markers();
	void on_Cap_Btn();
	void on_CapDone_Btn();
	void on_StartTracking();
	void on_StopTracking();
	void on_OpacityMove(int);
	void on_ActionCalibrate();
	void on_ActionSetTool();
	void on_ActionLoadTarget();


private:
	Ui::MainWindow *ui;

	std::string m_ImageFileName;
	std::string m_AtlasFileName;
	std::string m_ToolModelFileName;
	std::string m_TargetFileName;

	vtkSmartPointer<vtkImageData> m_Image;
	vtkSmartPointer<vtkPolyData> m_ImageModel;
	vtkSmartPointer<vtkImageData> m_Atlas;
	vtkSmartPointer<vtkPolyData>  m_Tool;  //tracking tool model
	vtkSmartPointer<vtkPolyData>  m_Target;  //planed target model

	reslice_view_base* m_Sagittal_View;
	reslice_view_base* m_Axial_View;
	reslice_view_base* m_Coronal_View;
	QtWrapvtkTracking3D* m_3d_View;//vtkSmartPointer<QtWrapvtkTracking3D>  m_3d_View;
	ATC3DGConfiguration* m_TrackerATC3DG;
	PloarisVicraConfiguration* m_TrackerPolaris;


	vtkSmartPointer< vtkTrackingMarkCapture<TrackerBase> > m_Marker_Capture;

};






#endif
