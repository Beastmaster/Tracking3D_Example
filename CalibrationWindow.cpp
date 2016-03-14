/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/2/22
Module:
	Tool Calibration module
*/




#include "CalibrationWindow.h"
#include "ui_CalibrationWindow.h"


// private include 
#include <vtkArrowSource.h>
#include <vtkPolyData.h>
#include "vtkAxesActor.h"
#include "vtkCylinderSource.h"


CalibrationWindow::CalibrationWindow(QWidget *parent) :
QMainWindow(parent), ui(new Ui::CalibrationWindow)
{
	ui->setupUi(this);
	sys_Init();

	connect(ui->config_Polaris_Btn,SIGNAL(clicked()),this,SLOT(On_Config_Polaris()));
	connect(ui->config_ATC_Btn, SIGNAL(clicked()), this, SLOT(On_Config_ATC()));
	connect(ui->polaris_Radio, SIGNAL(toggled(bool)), this, SLOT(Toggle_Polaris_Radio(bool)));
	connect(ui->ATC_Radio, SIGNAL(toggled(bool)), this, SLOT(Toggle_ATC_Radio(bool)));
	connect(ui->capture_Btn, SIGNAL(clicked()), this, SLOT(On_Capture()));
	connect(ui->cal1_Btn, SIGNAL(clicked()), this, SLOT(On_Calculate1()));
	connect(ui->cal2_Btn, SIGNAL(clicked()), this, SLOT(On_Calculate2()));
	connect(ui->move_Btn, SIGNAL(clicked()), this, SLOT(On_Move()));
	connect(ui->close_Btn, SIGNAL(clicked()), this, SLOT(On_Close()));
	// connect actions here	
	connect(ui->actionLoad_STL, SIGNAL(triggered()), this, SLOT(Act_LoadSTL()));
	connect(ui->actionLoad_nii, SIGNAL(triggered()), this, SLOT(Act_Load_nii()));
}

CalibrationWindow:: ~CalibrationWindow()
{
}


void CalibrationWindow::sys_Init()
{

	m_View = ui->view_Widget->GetRenderWindow();
	m_Interactor = m_View->GetInteractor();

	m_Renderer = vtkSmartPointer<vtkRenderer>::New();

	m_View->AddRenderer(m_Renderer);
	m_Renderer->SetBackground(1, 1, 1);

	m_Polaris = new PloarisVicraConfiguration;
	m_ATC = new ATC3DGConfiguration; 

	//configure tracker
	m_Tracking3D = vtkSmartPointer<QtWrapvtkTracking3D>::New();
	m_Tracking3D->AddToolIndex(0);
	m_Tracking3D->SetReferenceIndex(1);

	double mat[4][4];
	mat[0][0] = 0.0392916;
	mat[0][1] = 0.87901;
	mat[0][2] = -0.925426;
	mat[0][3] = -544.782;
	mat[1][0] = -0.173115;
	mat[1][1] = 0.69974;
	mat[1][2] = 0.791138;
	mat[1][3] = 898.569;
	mat[2][0] = 1.05681;
	mat[2][1] = 0.145884;
	mat[2][2] = -0.132635;
	mat[2][3] = -103.639;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	auto reg = vtkSmartPointer<vtkMatrix4x4>::New();
	std::cout << "Setting Registration transform:" << std::endl;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			reg->SetElement(i, j, mat[i][j]);
		}
	}
	//m_Tracking3D->SetRegisterTransform(reg);



	m_Tool_Transform = new QIN_Transform_Type;
	m_Tool2TipTransform = vtkSmartPointer<vtkTransform>::New();

	m_CalibrationHandle1 = vtkSmartPointer<PivotCalibration>::New();
	m_CalibrationHandle2 = vtkSmartPointer<PivotCalibration2>::New();


	vtkSmartPointer<vtkAxesActor> axes =
		vtkSmartPointer<vtkAxesActor>::New();
	axes->SetTotalLength(500.0,500.0,500.0);
	m_Renderer->AddActor(axes);
	m_Renderer->ResetCamera();

	m_View->Render();
}

/*
Description:
	Get the tool calibration matrix
Parameter:
	out: An allocated 
*/
void CalibrationWindow::GetCalibrationMatrix(vtkMatrix4x4* out)
{
	
}

void CalibrationWindow::On_Config_Polaris()
{
	m_Polaris->ConfigureTracker();
	m_Polaris->StartTracking();
}


void CalibrationWindow::On_Config_ATC()
{
	//m_ATC->Config2();
	m_ATC->ConfigureTracker();
	//m_ATC->StartTracking();
}

void CalibrationWindow::Toggle_Polaris_Radio(bool checked)
{
	if (checked)
	{
		m_Tracking3D->SetTracker(m_Polaris);
	}
}
void CalibrationWindow::Toggle_ATC_Radio(bool checked)
{
	if (checked)
	{
		m_Tracking3D->SetTracker(m_ATC);
	}
}


void CalibrationWindow::On_Capture()
{
	// Capture and convert to vtkMatrix4X4
	auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();

	std::cout << "Tool 0: " << std::endl;
	m_Tool_Transform = m_Tracking3D->GetTransform(0);
	//m_Tool_Transform = m_ATC->GetTransform(0);
	
	m_Tool_Transform->PrintSelf();
	
	if (m_Tool_Transform->x!=0.0)
	{
		PivotCalibration2::TransformToMatrix(m_Tool_Transform, matrix);
		//=== Pivot Calibration method1, from PLUS_Lib ===//
		m_CalibrationHandle1->InsertNextCalibrationPoint(matrix);
		//=== Pivot Calibration method2, from IGSTK ===//
		m_CalibrationHandle2->AddToolToReferenceMatrix(matrix);
	}

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
	m_Tool2TipMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_CalibrationHandle2->GetToolTipToToolMatrix(m_Tool2TipMatrix);
	std::cout << "Calibration method 2 result: " << std::endl;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			std::cout << m_Tool2TipMatrix->GetElement(i, j) << " , ";
		}
		std::cout<<std::endl;
	}

	m_Tool2TipTransform = vtkSmartPointer<vtkTransform>::New();
	m_Tool2TipTransform->SetMatrix(m_Tool2TipMatrix);
	m_Tool2TipTransform->Update();

	m_Actor->SetUserTransform(m_Tool2TipTransform);
	m_View->Render();
	m_CalibrationHandle2->ClearToolToReferenceMatrices();

	m_Tracking3D->SetToolTipCalibrationMatrix(m_Tool2TipMatrix);
}



void CalibrationWindow::Act_LoadSTL()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "E:/", tr("Image Files (*.stl)"));
	if (fileName.isEmpty())
	{
		return;
	}
	auto reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName(fileName.toStdString().c_str());
	reader->Update();
	
	auto image = vtkSmartPointer<vtkPolyData>::New();
	image = reader->GetOutput();
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(image);
	m_Actor = vtkSmartPointer<vtkActor>::New();
	m_Actor->SetMapper(mapper);
	//vtkSmartPointer<vtkAxesActor> axes =
	//	vtkSmartPointer<vtkAxesActor>::New();
	//axes->SetTotalLength(200.0, 200.0, 200.0);
	//
	//m_Actor = axes;
	m_Renderer->AddActor(m_Actor);
	m_Renderer->ResetCamera();
	m_View->Render();
}

void CalibrationWindow::Act_CreateDefault()
{
	auto arrow = vtkSmartPointer<vtkCylinderSource>::New();
	arrow->SetHeight(300);
	arrow->SetRadius(50);
	//arrow->SetCenter(0,100,0);
	arrow->Update();
	auto image = vtkSmartPointer<vtkPolyData>::New();
	image = arrow->GetOutput();
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(image);
	auto axes = vtkSmartPointer<vtkAxesActor>::New();
	axes->SetTotalLength(300, 300, 300);
	//m_Actor2 = axes;
	m_Renderer->AddActor(m_Actor2);
	m_Renderer->ResetCamera();
	m_View->Render();
}

void CalibrationWindow::Act_Load_nii()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "E:/", tr("Image Files (*.nii)"));
	if (fileName.isEmpty())
	{
		return;
	}
	
	auto reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
	reader->SetFileName(fileName.toStdString().c_str());
	reader->Update();

	//extract 3d model
	auto marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
	marchingCubes->SetInputData(reader->GetOutput());
	marchingCubes->SetValue(0, 500);
	marchingCubes->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(marchingCubes->GetOutput());

	m_Actor2 = vtkSmartPointer<vtkActor>::New();
	m_Actor2->SetMapper(mapper);
	m_Renderer->AddActor(m_Actor2);
	m_Renderer->ResetCamera();
	m_View->Render();
}

void CalibrationWindow::On_Move()
{
	//connect(m_Tracking3D, SIGNAL(on_timer_signal_transform(vtkMatrix4x4*)), this, SLOT(On_Timer(vtkMatrix4x4*)));
	//m_Tracking3D->StartTracking2();
	
	m_Timer = new QTimer();
	m_Timer->start(50);
	connect(m_Timer, SIGNAL(timeout()), this, SLOT(On_Timer1()));
	
}
void CalibrationWindow::On_Close()
{
	m_Tracking3D->StopTracking2();
	m_Polaris->StopTracking();
	m_ATC->StopTracking();
}


void CalibrationWindow::On_Timer(vtkMatrix4x4* matrix)
{
	m_Actor->SetUserMatrix(matrix);
	m_View->Render();
}
void CalibrationWindow::On_Timer1()
{
	NEW2DARR(double, matrix);

	m_ATC->GetTransform(0,matrix);
	// Capture and convert to vtkMatrix4X4
	auto matrixx = vtkSmartPointer<vtkMatrix4x4>::New();
	for (size_t i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4;j++)
			matrixx->SetElement(i,j,matrix[i][j]);
	}

	m_Actor->SetUserMatrix(matrixx);
	m_View->Render();

	DEL2DARR(double,matrix);
}

/*

	// convert raw position and rotation information from sensor
	m_Tool_Transform = m_Polaris->GetTransform(0);
	auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	PivotCalibration2::TransformToMatrix(m_Tool_Transform, matrix);
	// calibrate the tool tip
	auto calibrated = vtkSmartPointer<vtkTransform>::New();
	calibrated->SetMatrix(matrix);
	calibrated->Concatenate(m_Tool2TipMatrix);
	// transform with registration transform
	auto registered = vtkSmartPointer<vtkTransform>::New();
	registered->SetMatrix(reg);
	registered->Concatenate(calibrated);

	m_Actor->SetUserTransform(registered);
	//m_Actor->SetPosition(temp);
	//m_Actor->SetOrientation(orientation);

*/


