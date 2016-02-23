/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/2/22
Module:
	Tool Calibration module
*/




#include "CalibrationWindow.h"
#include "ui_CalibrationWindow.h"

CalibrationWindow::CalibrationWindow(QWidget *parent) :
QMainWindow(parent), ui(new Ui::CalibrationWindow)
{
	ui->setupUi(this);
	sys_Init();
	
	connect(ui->config_Polaris_Btn,SIGNAL(clicked()),this,SLOT(On_Config_Polaris()));
	connect(ui->config_ATC_Btn, SIGNAL(clicked()), this, SLOT(On_Config_ATC()));
	connect(ui->capture_Btn, SIGNAL(clicked()), this, SLOT(On_Capture()));
	connect(ui->cal1_Btn, SIGNAL(clicked()), this, SLOT(On_Calculate1()));
	connect(ui->cal2_Btn, SIGNAL(clicked()), this, SLOT(On_Calculate2()));
}

CalibrationWindow:: ~CalibrationWindow()
{
}


void CalibrationWindow::sys_Init()
{
	m_Polaris = new PloarisVicraConfiguration;
	m_ATC = new ATC3DGConfiguration; 

	m_Tool_Transform = new QIN_Transform_Type;
	m_Tool2TipTransform = vtkSmartPointer<vtkTransform>::New();

	m_CalibrationHandle1 = vtkSmartPointer<PivotCalibration>::New();
	m_CalibrationHandle2 = vtkSmartPointer<PivotCalibration2>::New();

}

void CalibrationWindow::On_Config_Polaris()
{
	m_Polaris->ConfigureTracker();
	m_Polaris->StartTracking();


}


void CalibrationWindow::On_Config_ATC()
{
	m_ATC->ConfigureTracker();
	m_ATC->StartTracking();


}


void CalibrationWindow::On_Capture()
{
	// Capture and convert to vtkMatrix4X4
	auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();


	//=== Pivot Calibration method1, from PLUS_Lib ===//
	m_CalibrationHandle1->InsertNextCalibrationPoint(matrix);

	//=== Pivot Calibration method2, from IGSTK ===//
	m_CalibrationHandle2->AddToolToReferenceMatrix(matrix);

}


void CalibrationWindow::On_Calculate1()
{
	// run calibration process
	m_CalibrationHandle1->DoPivotCalibration();


	// print information
	std::string message = m_CalibrationHandle1->GetPivotPointToMarkerTranslationString();
	std::cout << "Calibration method 1 result: " << std::endl;
	std::cout << message << std::endl;

}


void CalibrationWindow::On_Calculate2()
{
	// run calibration process
	m_CalibrationHandle2->ComputePivotCalibration();

	// print information
	auto  mess = vtkSmartPointer<vtkMatrix4x4>::New();
	m_CalibrationHandle2->GetToolTipToToolMatrix(mess);
	std::cout << "Calibration method 2 result: " << std::endl;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			std::cout << mess[i][j] << " , ";
		}
		std::cout<<std::endl;
	}
}















