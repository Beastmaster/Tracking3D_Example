/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2017/5/15
*/




#include "vtkTrackerUtil_test_Win.h"
#include "ui_vtkTrackerUtil_test_Win.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

#pragma region MarchingCubes

#pragma endregion MarchingCubes

#pragma region Main_Configuration_TrackerUtil
	m_trackerUtil = vtkSmartPointer<vtkTrackerUtil>::New();
	m_trackerUtil->SetTrackerType(&m_tracker);
	m_trackerUtil->SetCalibrationMat(0,m_ToolCalibrationMat);
#pragma endregion Main_Configuration_TrackerUtil


}

MainWindow:: ~MainWindow()
{

}

