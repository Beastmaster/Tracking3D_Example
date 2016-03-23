/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/2/22
Module:
	Tool Calibration module
*/




#include "CalibrationWindow.h"
#include "ui_CalibrationWindow.h"

#include <QDateTime>
#include <QTextStream>
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

	connect(ui->config_Polaris_Btn, SIGNAL(clicked()), this, SLOT(On_Config_Polaris()), Qt::QueuedConnection);
	connect(ui->config_ATC_Btn, SIGNAL(clicked()), this, SLOT(On_Config_ATC()));
	connect(ui->polaris_Radio, SIGNAL(toggled(bool)), this, SLOT(Toggle_Polaris_Radio(bool)));
	connect(ui->ATC_Radio, SIGNAL(toggled(bool)), this, SLOT(Toggle_ATC_Radio(bool)));
	connect(ui->capture_Btn, SIGNAL(clicked()), this, SLOT(On_Capture()));
	connect(ui->record_Btn, SIGNAL(clicked()), this, SLOT(On_Record()));
	connect(ui->cal1_Btn, SIGNAL(clicked()), this, SLOT(On_Calculate1()));
	connect(ui->cal2_Btn, SIGNAL(clicked()), this, SLOT(On_Calculate2()));
	connect(ui->move_Btn, SIGNAL(clicked()), this, SLOT(On_Move()));
	connect(ui->mov3d_Btn, SIGNAL(clicked()), this, SLOT(On_Move3d()));
	connect(ui->close_Btn, SIGNAL(clicked()), this, SLOT(On_Close()));
	connect(ui->save_Btn, SIGNAL(clicked()), this, SLOT(On_Save()));


	// connect actions here	
	connect(ui->actionLoad_STL, SIGNAL(triggered()), this, SLOT(Act_LoadSTL()));
	connect(ui->actionLoad_nii, SIGNAL(triggered()), this, SLOT(Act_Load_nii()));
}

CalibrationWindow:: ~CalibrationWindow()
{
}


void CalibrationWindow::sys_Init()
{
	m_Timer = new QTimer();

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

	m_Tool_Transform = new QIN_Transform_Type;
	m_Tool2TipTransform = vtkSmartPointer<vtkTransform>::New();

	m_CalibrationHandle1 = vtkSmartPointer<PivotCalibration>::New();
	m_CalibrationHandle2 = vtkSmartPointer<PivotCalibration2>::New();


	vtkSmartPointer<vtkAxesActor> axes =
		vtkSmartPointer<vtkAxesActor>::New();
	axes->SetTotalLength(500.0,500.0,500.0);
	m_Renderer->AddActor(axes);
	m_Renderer->ResetCamera();


	//load default tool
	auto reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName("E:/test/QinShuoTShapeM.stl");
	reader->Update();

	auto image = vtkSmartPointer<vtkPolyData>::New();
	image = reader->GetOutput();
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(image);
	m_Actor = vtkSmartPointer<vtkActor>::New();
	m_Actor->SetMapper(mapper);
	m_Renderer->AddActor(m_Actor);

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

	m_Tool_Transform = m_Tracking3D->GetTransform(0);
	
	QString pos;
	pos.append(QString::number(m_Tool_Transform->x));
	pos.append(",");
	pos.append(QString::number(m_Tool_Transform->y));
	pos.append(",");
	pos.append(QString::number(m_Tool_Transform->z));
	pos.append(";");
	m_calib_log.append(pos);

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

/*
Record position when moving
*/
void CalibrationWindow::On_Record()
{
	auto temp = m_Tracking3D->GetRegisteredTransform();

	m_Actor->SetUserTransform(temp);
	m_View->Render();

	QString pos;
	pos.append(QString::number(temp->GetPosition()[0]));
	pos.append(",");
	pos.append(QString::number(temp->GetPosition()[1]));
	pos.append(",");
	pos.append(QString::number(temp->GetPosition()[2]));
	pos.append(";");
	m_pos_log.append(pos);

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

void CalibrationWindow::On_Move3d()
{
	m_Timer->start(50);
	connect(m_Timer, SIGNAL(timeout()), this, SLOT(On_Timer()));
	m_Tracking3D->StartTracking2();
}
void CalibrationWindow::On_Move()
{
	m_Timer->start(50);
	connect(m_Timer, SIGNAL(timeout()), this, SLOT(On_Timer1()));
}

void CalibrationWindow::On_Close()
{
	m_Timer->stop();
	connect(m_Timer, SIGNAL(timeout()), this, SLOT(On_Timer1()));
	disconnect(m_Timer, SIGNAL(timeout()), this, SLOT(On_Timer1()));
	m_Tracking3D->StopTracking2();
	m_Polaris->StopTracking();
	m_ATC->StopTracking();
}


void CalibrationWindow::On_Save()
{
	//write to file //m_pos_log
	QString time_stamp = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
	QString file_name = time_stamp.append("calib");
	file_name.append(".txt");

	QFile file(file_name);
	// Trying to open in WriteOnly and Text mode
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox msgBox;
		msgBox.setText("Cannot Write file");
		msgBox.exec();
		return;
	}
	QTextStream out(&file);

	out << "Calibration Log:" << endl;
	for (size_t i = 0; i < m_pos_log.size(); i++)
	{
		out << m_calib_log.at(i) << endl;
	}

	out << "\n\nCalibration Log:" << endl;
	for (size_t i = 0; i < m_pos_log.size(); i++)
	{
		out << m_pos_log.at(i) << endl;
	}
	file.flush();
	file.close();

}

/*
Move actors with vtkTracking3D
*/
void CalibrationWindow::On_Timer()
{
	auto xx = vtkSmartPointer<vtkMatrix4x4>::New();
	xx = m_Tracking3D->GetRegisteredTransformMatrix();
	
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			std::cout<<xx->GetElement(i,j)<<",";
		}
		std::cout << std::endl;
	}

	//auto temp = m_Tracking3D->GetRegisteredTransform();
	auto temp = vtkSmartPointer<vtkTransform>::New();
	temp->SetMatrix(xx);

	m_Actor->SetUserTransform(temp);
	m_View->Render();
}

/*
Move using raw data
*/
void CalibrationWindow::On_Timer1()
{
	NEW2DARR(double, matrix);

	if (ui->ATC_Radio->isChecked())
	{
		m_ATC->GetTransform(0, matrix);
	}
	else
	{
		m_Polaris->GetTransform(0,matrix);
	}
	
	// Capture and convert to vtkMatrix4X4
	auto matrixx = vtkSmartPointer<vtkMatrix4x4>::New();
	for (size_t i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4;j++)
			matrixx->SetElement(i,j,matrix[i][j]);
	}

	m_Actor->SetUserMatrix(matrixx);
	//m_Actor->SetUserMatrix(m_Tracking3D->GetRegisteredTransformMatrix());
	
	m_View->Render();

	DEL2DARR(double,matrix);
}

