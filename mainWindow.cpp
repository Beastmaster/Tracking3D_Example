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
	connect(ui->config_Tracker_Btn,SIGNAL(clicked()),this,SLOT(on_Config_Tracker()));
	connect(ui->sel_Marker_Btn, SIGNAL(clicked()),this,SLOT(on_Sel_Markers()));
	connect(ui->cap_Marker_Btn, SIGNAL(clicked()), this, SLOT(on_Cap_Btn()));
	connect(ui->ok_Sel_Btn, SIGNAL(clicked()), this, SLOT(on_CapDone_Btn()));
	connect(ui->start_Tracking_Btn,SIGNAL(clicked()),this,SLOT(on_StartTracking()));
	connect(ui->stop_Tracking_Btn,SIGNAL(clicked()),this,SLOT(on_StopTracking()));
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
	m_3d_View = vtkSmartPointer<QtWrapvtkTracking3D>::New();//vtkSmartPointer<QtWrapvtkTracking3D>::New();
	m_3d_View->SetWindow(ui->threeDWidget->GetRenderWindow());
	m_3d_View->SetInteractor(ui->threeDWidget->GetRenderWindow()->GetInteractor());
	m_3d_View->InstallPipeline();

	m_Marker_Capture = vtkSmartPointer< vtkTrackingMarkCapture<ATC3DGConfiguration> >::New();
	m_Marker_Capture->SetTracker(m_3d_View->m_tracker);
	m_Marker_Capture->SetToolIndex(0);
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

/*
No use by now
*/
void MainWindow::on_Sel_Tracker()
{}

void MainWindow::on_Config_Tracker()
{
	if (m_3d_View->m_tracker->ConfigureTracker()!=0)
	{
		std::cout << "Tracker configuration fail" << std::endl;
	}
	if (m_3d_View->m_tracker->StartTracking()!=0)
	{
		std::cout << "Start Tracker fail" << std::endl;
	}
	
}

/*
Begin selecting marker
*/
void MainWindow::on_Sel_Markers()
{
	QMessageBox msgBox;
	msgBox.setText("Select Marker.");
	//msgBox.setInformativeText("Press \"a\" to accept the marker.\nPress \"b\" to abort. \nPress \"q\" to finish.");
	msgBox.setInformativeText("Press \"a\" to accept the marker.\nPress \"b\" to abort. \nPress \"q\" to finish.");
	msgBox.setStandardButtons(QMessageBox::Ok);
	int ret = msgBox.exec();

	// enable mouse pick first
	m_3d_View->EnablePick(); //press a to accept
}
/*
Capture the point and the device position
*/
void MainWindow::on_Cap_Btn()
{
	// valid marker you select in the 3d view
	m_3d_View->ValidMarker();

	// marker capture get marker;
	m_Marker_Capture->GetNextMarker();
}
/*
Finish capture and run registration
*/
void MainWindow::on_CapDone_Btn()
{
	//disconnect pick callback
	m_3d_View->DisablePick();

	if (m_3d_View->GetMarkerList().size() != m_Marker_Capture->GetMarkerList().size())
	{
		std::cout << "Markers invalid" << std::endl;
		m_3d_View->ClearMarkers();
		m_Marker_Capture->ClearMarkers();
	}

	// start registration here
	auto temp_src = m_Marker_Capture->GetMarkerList();
	auto temp_dst = m_3d_View->GetMarkerList();

	auto reg = vtkSmartPointer<vtkTrackingLandMarkRegistration>::New();
	reg->SetSourcePoints(temp_src);
	reg->SetTargetPoints(temp_dst);
	reg->GenerateTransform();
	auto res2 = reg->GetTransformMatrix();
	std::cout << "Result" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << res2->GetElement(i, j) << ",";
		std::cout << std::endl;
	}

	m_3d_View->SetRegisterTransform(res2);
}

void MainWindow::on_StartTracking()
{
	//connect tracking object
	auto sphere = vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetRadius(50);
	sphere->Update();

	m_3d_View->AddPolySource(sphere->GetOutput());
	m_3d_View->ConnectObjectTracker(1,0);
	m_3d_View->StartTracking();
}

void MainWindow::on_StopTracking()
{
	m_3d_View->StopTracking();
}

void MainWindow::on_OpacityMove(int op)
{
	m_3d_View->SetOpacity( 0 , op/1000);
}


