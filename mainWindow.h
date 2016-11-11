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
#include "vtkXMLPolyDataReader.h"

#include "PolarisVicraConfiguration.h"
#include "ATC3DGConfiguration.h"
#include "vtkTrackingResliceView.h"
#include "vtkTracking3D.h"
#include "vtkTrackingMarkCapture.h"
#include "vtkTrackingRegistration.h"
#include "QtWrapvtkTracking3D.h"
#include "CalibrationWindow.h"


namespace Ui {
	class MainWindow;
}



class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void sys_Init();
	void createActions();

	public slots:
	void on_ResliceAction(double, double, double);
	void on_ResliceAction(int x, int y, int z);
	void on_ResliceActionMarker(double, double, double);
	void on_Load_Image();
	void on_Load_Atlas();
	void on_Del_Atlas();
	void on_Sel_Tracker(int);
	void on_Config_Tracker();
	void on_Sel_Markers();
	void on_Valid_Marker();
	void on_Sel_MarkersDone();
	void on_Cap_Btn();
	void on_CapDone_Btn();
	void on_StartTracking();
	void on_StopTracking();
	void on_ActionCalibrate();
	void on_ActionSetTool();
	void on_ActionLoadTarget();
	void on_Sagittal_Slider(int);
	void on_Axial_Slider(int);
	void on_Coronal_Slider(int);
	void on_Opacity_Slider(int);
	void on_EnablePlane(int);
	void on_EnableSkull(int);
	void on_Change_Tracking_Status(int);
	//
	void on_StartCapture();
	void On_DoneCapture();
	void On_FineRegister();
	void On_Capture_squence();


private:
	Ui::MainWindow *ui;
	QTimer* m_captureTimer;

	std::string m_ImageFileName;
	std::string m_AtlasFileName;
	std::string m_ToolModelFileName;
	std::string m_TargetFileName;

	double m_StripValue;

	vtkSmartPointer<vtkImageData> m_Image;
	vtkSmartPointer<vtkPolyData> m_ImageModel;
	vtkSmartPointer<vtkImageData> m_Atlas;
	vtkSmartPointer<vtkPolyData>  m_Tool;  //tracking tool model
	vtkSmartPointer<vtkPolyData>  m_Target;  //planed target model

	reslice_view_base* m_Sagittal_View;
	reslice_view_base* m_Axial_View;
	reslice_view_base* m_Coronal_View;
	vtkSmartPointer<QtWrapvtkTracking3D>  m_3d_View;
	ATC3DGConfiguration* m_TrackerATC3DG;
	PloarisVicraConfiguration* m_TrackerPolaris;
	vtkSmartPointer<vtkMatrix4x4>		m_ToolTipCalibration_Matrix;
	CalibrationWindow*					m_Calibration_Win;

	vtkSmartPointer< vtkTrackingMarkCapture> m_Marker_Capture;

	int m_SliceX;  //---> Sagittal
	int m_SliceY;  //---> Coronal
	int m_SliceZ;  //---> Axial

};






#endif
