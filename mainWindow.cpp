/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/1/10
*/




#include "mainWindow.h"
#include "ui_mainWindow.h"

MainWindow::MainWindow(QWidget *parent):
QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	//system parameters initialization
	sys_Init();

	//connect
	connect(ui->load_Image_Btn,SIGNAL(clicked()),this,SLOT(on_Load_Image()));
	connect(ui->load_Atlas_Btn, SIGNAL(clicked()), this, SLOT(on_Load_Atlas()));
	connect(ui->sel_Marker_Btn, SIGNAL(clicked()),this,SLOT(on_Sel_Markers()));
	connect(ui->cap_Marker_Btn, SIGNAL(clicked()), this, SLOT(on_Cap_Btn()));
	connect(ui->ok_Sel_Btn, SIGNAL(clicked()), this, SLOT(on_CapDone_Btn()));
}

MainWindow:: ~MainWindow()
{
}


void MainWindow::sys_Init()
{
	//init qvtkwidget 
	ui->axialWidget->GetRenderWindow()->Render();
	ui->sagittalWidget->GetRenderWindow()->Render();
	ui->coronalWidget->GetRenderWindow()->Render();
	ui->threeDWidget->GetRenderWindow()->Render();

	m_Sagittal_View = new reslice_view_base(ui->sagittalWidget->GetRenderWindow(),'s');
	m_Axial_View = new reslice_view_base(ui->axialWidget->GetRenderWindow(), 'a');
	m_Coronal_View = new reslice_view_base(ui->coronalWidget->GetRenderWindow(), 'c');
	m_3d_View = vtkSmartPointer<vtkTracking3D>::New();
	m_3d_View->SetWindow(ui->threeDWidget->GetRenderWindow());
	m_3d_View->SetInteractor(ui->threeDWidget->GetRenderWindow()->GetInteractor());
	m_3d_View->InstallPipeline();
}

void MainWindow::on_Load_Image()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "E:/", tr("Image Files (*.nii)"));
	if (fileName.isEmpty())
	{
		return;
	}
	else
	{
		m_ImageFileName = fileName.toStdString();
		auto reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
		reader->SetFileName(m_ImageFileName.c_str());
		reader->Update();
		m_Image = reader->GetOutput();

		m_Sagittal_View->Set_View_Img(m_Image);
		m_Axial_View->Set_View_Img(m_Image);
		m_Coronal_View->Set_View_Img(m_Image);

		m_Sagittal_View->RenderView();
		m_Axial_View->RenderView();
		m_Coronal_View->RenderView();

		//extract 3d model
		auto marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
		marchingCubes->SetInputData(m_Image);
		marchingCubes->SetValue(0,500);
		marchingCubes->Update();
		m_3d_View->AddPolySource(marchingCubes->GetOutput());
		m_3d_View->RefreshView();
		m_3d_View->GetRenderWindow()->Render();
	}
}

void MainWindow::on_Load_Atlas()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "E:/", tr("Image Files (*.nii)"));
	if (fileName.isEmpty())
	{
		return;
	}
	else
	{
		m_AtlasFileName = fileName.toStdString();
		auto reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
		reader->SetFileName(m_AtlasFileName.c_str());
		reader->Update();
		m_Atlas = reader->GetOutput();

		m_Sagittal_View->Set_Mask_Img(m_Atlas);
		m_Axial_View->Set_Mask_Img(m_Atlas);
		m_Coronal_View->Set_Mask_Img(m_Atlas);

		m_Sagittal_View->RenderView();
		m_Axial_View->RenderView();
		m_Coronal_View->RenderView();
	}

}

void MainWindow::on_Del_Atlas()
{
	m_Sagittal_View->RemoveMask();
	m_Axial_View->RemoveMask();
	m_Coronal_View->RemoveMask();

	m_Sagittal_View->RenderView();
	m_Axial_View->RenderView();
	m_Coronal_View->RenderView();
}

void MainWindow::on_Sel_Tracker()
{}

void MainWindow::on_Config_Tracker()
{}

/*
Begin selecting marker
*/
void MainWindow::on_Sel_Markers()
{
	QMessageBox msgBox;
	msgBox.setText("Select Marker.");
	msgBox.setInformativeText("Press \"a\" to accept the marker.\nPress \"b\" to abort. \nPress \"q\" to finish.");
	msgBox.setStandardButtons(QMessageBox::Ok);
	int ret = msgBox.exec();

}
/*
Capture the point and the device position
*/
void MainWindow::on_Cap_Btn()
{

}
/*
Finish capture and run registration
*/
void MainWindow::on_CapDone_Btn()
{

}

void MainWindow::on_StartTracking()
{}

void MainWindow::on_StopTracking()
{}

void MainWindow::on_OpacityMove(int)
{}


