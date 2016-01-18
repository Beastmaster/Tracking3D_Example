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
	//some ui parameters
	ui->opacity_Slider->setMaximum(100);
	ui->opacity_Slider->setMinimum(1);
	ui->opacity_Slider->setValue(100);

	//system parameters initialization
	sys_Init();

	//connect vtkTracking3D
	//connect(m_3d_View, SIGNAL(on_timer_signal_coor(double, double, double)), this, SLOT(on_ResliceAction(double,double,double)));
	connect(m_3d_View, SIGNAL(on_timer_signal_index(int, int, int)), this, SLOT(on_ResliceAction(int, int, int)));
	connect(m_3d_View, SIGNAL(on_timer_signal_coor(double,double,double)), this, SLOT(on_ResliceAction(double, double, double)));
	//connect
	connect(ui->load_Image_Btn,SIGNAL(clicked()),this,SLOT(on_Load_Image()));
	connect(ui->load_Atlas_Btn, SIGNAL(clicked()), this, SLOT(on_Load_Atlas()));
	connect(ui->del_Atlas_Btn, SIGNAL(clicked()),this,SLOT(on_Del_Atlas()));
	connect(ui->config_Tracker_Btn,SIGNAL(clicked()),this,SLOT(on_Config_Tracker()));
	connect(ui->sel_Tracker_Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_Sel_Tracker(int)));
	connect(ui->sel_Marker_Btn, SIGNAL(clicked()),this,SLOT(on_Sel_Markers()));
	connect(ui->cap_Marker_Btn, SIGNAL(clicked()), this, SLOT(on_Cap_Btn()));
	connect(ui->ok_Sel_Btn, SIGNAL(clicked()), this, SLOT(on_CapDone_Btn()));
	connect(ui->start_Tracking_Btn,SIGNAL(clicked()),this,SLOT(on_StartTracking()));
	connect(ui->stop_Tracking_Btn,SIGNAL(clicked()),this,SLOT(on_StopTracking()));
	connect(ui->opacity_Slider, SIGNAL(valueChanged(int)), this, SLOT(on_Opacity_Slider(int)));
	connect(ui->axial_slider, SIGNAL(valueChanged(int)), this, SLOT(on_Axial_Slider(int)));
	connect(ui->sagittal_slider, SIGNAL(valueChanged(int)), this, SLOT(on_Sagittal_Slider(int)));
	connect(ui->coronal_slider, SIGNAL(valueChanged(int)), this, SLOT(on_Coronal_Slider(int)));
	connect(ui->en_Plane_Check, SIGNAL(stateChanged(int)), this, SLOT(on_EnablePlane(int)));
	createActions();
}

MainWindow:: ~MainWindow()
{
	delete m_Sagittal_View;
	delete m_Axial_View;
	delete m_Coronal_View;

	if (m_TrackerPolaris != NULL)
	{
		delete m_TrackerPolaris;
	}
	if (m_TrackerATC3DG!=NULL)
	{
		delete m_TrackerATC3DG;
	}
}


void MainWindow::sys_Init()
{
	m_ImageFileName = "";
	m_AtlasFileName = "";
	m_ToolModelFileName = "";

	//init qvtkwidget 
	ui->axialWidget->GetRenderWindow()->Render();
	ui->sagittalWidget->GetRenderWindow()->Render();
	ui->coronalWidget->GetRenderWindow()->Render();
	ui->threeDWidget->GetRenderWindow()->Render();

	m_Sagittal_View = new reslice_view_base(ui->sagittalWidget->GetRenderWindow(),'s');
	m_Axial_View = new reslice_view_base(ui->axialWidget->GetRenderWindow(), 'a');
	m_Coronal_View = new reslice_view_base(ui->coronalWidget->GetRenderWindow(), 'c');
	m_3d_View = vtkSmartPointer<QtWrapvtkTracking3D>::New();
	m_3d_View->SetWindow(ui->threeDWidget->GetRenderWindow());
	m_3d_View->SetInteractor(ui->threeDWidget->GetRenderWindow()->GetInteractor());
	m_3d_View->InstallPipeline();

	m_TrackerATC3DG = NULL;
	m_TrackerPolaris = NULL;
	//by default, we use ATC3DG device for tracking
	m_TrackerATC3DG = new ATC3DGConfiguration;
	m_TrackerPolaris = new PloarisVicraConfiguration;
	m_3d_View->SetTracker(m_TrackerATC3DG); // default tracker is ATC3DG device

	m_Marker_Capture = vtkSmartPointer< vtkTrackingMarkCapture<TrackerBase> >::New();
	m_Marker_Capture->SetTracker(m_3d_View->m_tracker);
	m_Marker_Capture->SetToolIndex(0);

	////setup plane widget
	//m_PlaneX = vtkSmartPointer<vtkImagePlaneWidget>::New();
	//m_PlaneY = vtkSmartPointer<vtkImagePlaneWidget>::New();
	//m_PlaneZ = vtkSmartPointer<vtkImagePlaneWidget>::New();
	//m_PlaneX->SetInteractor(ui->threeDWidget->GetInteractor());
	//m_PlaneY->SetInteractor(ui->threeDWidget->GetInteractor());
	//m_PlaneZ->SetInteractor(ui->threeDWidget->GetInteractor());
}

/*
Connect signal/slot of QActions
*/
void MainWindow::createActions()
{
	connect(ui->actionCalibrate_Tool, SIGNAL(triggered()), this, SLOT(on_ActionCalibrate()));
	connect(ui->actionSet_Tool, SIGNAL(triggered()), this, SLOT(on_ActionSetTool()));
	connect(ui->actionLoad_Target_Model, SIGNAL(triggered()), this, SLOT(on_ActionLoadTarget()));
}

/*
This function receive signal from vtkTracking3D class
Parameters are coordinate from the world coordinate.
*/
void MainWindow::on_ResliceAction(double x, double y, double z)
{
	int pt_ID = 0;
	pt_ID = m_Image->FindPoint(x,y,z);
	std::cout << "Point ID is: " << pt_ID << std::endl;

	int extent[6];
	m_Image->GetExtent(extent);

	int x_e = extent[1] - extent[0] +1;
	int y_e = extent[3] - extent[2] +1 ;
	int z_e = extent[5] - extent[4] +1 ;

	m_SliceZ = floor(pt_ID/(x_e*y_e));
	m_SliceY = floor(pt_ID%(x_e*y_e) /x_e );
	m_SliceX = pt_ID%x_e;
}
void MainWindow::on_ResliceAction(int x, int y, int z)
{
	m_SliceX = m_3d_View->m_SliceX;
	m_SliceY = m_3d_View->m_SliceY;
	m_SliceZ = m_3d_View->m_SliceZ;

	std::cout << "x coor:" << m_SliceX << std::endl;
	std::cout << "y coor:" << m_SliceY << std::endl;
	std::cout << "z coor:" << m_SliceZ << std::endl;

	ui->axial_slider->setSliderPosition(m_SliceZ);
	ui->sagittal_slider->setSliderPosition(m_SliceX);
	ui->coronal_slider->setSliderPosition(m_SliceY);
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

		//modify slider range
		int extent[6];
		m_SliceX = (extent[5] + extent[4]) / 2;
		m_SliceY = (extent[0] + extent[1]) / 2;
		m_SliceZ = (extent[2] + extent[3]) / 2;
		m_Image->GetExtent(extent);
		ui->axial_slider->setRange(extent[4], extent[5]);
		ui->axial_slider->setValue(m_SliceX);
		ui->sagittal_slider->setRange(extent[0], extent[1]);
		ui->sagittal_slider->setValue(m_SliceY);
		ui->coronal_slider->setRange(extent[2], extent[3]);
		ui->coronal_slider->setValue(m_SliceZ);

		m_Sagittal_View->Set_View_Img(m_Image);
		m_Axial_View->Set_View_Img(m_Image);
		m_Coronal_View->Set_View_Img(m_Image);

		m_Sagittal_View->RenderView();
		m_Axial_View->RenderView();
		m_Coronal_View->RenderView();

		m_3d_View->SetImage(m_Image);
		//plane widget
		//m_PlaneX->SetInputData(m_Image);
		//m_PlaneY->SetInputData(m_Image);
		//m_PlaneZ->SetInputData(m_Image);
		//m_PlaneX->SetPlaneOrientationToXAxes();
		//m_PlaneY->SetPlaneOrientationToYAxes();
		//m_PlaneZ->SetPlaneOrientationToZAxes();


		//extract 3d model
		auto marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
		marchingCubes->SetInputData(m_Image);
		marchingCubes->SetValue(0,500);
		marchingCubes->Update();
		m_3d_View->AddPolySource(marchingCubes->GetOutput());
		m_3d_View->SetColor(0,0.5,0.6,0.7);
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
void MainWindow::on_Sel_Tracker(int value)
{
	if (value == 1)
	{
		m_3d_View->m_tracker = m_TrackerPolaris;
		m_Marker_Capture->SetTracker(m_3d_View->m_tracker);
	}
}

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
	msgBox.setInformativeText("Put the Tool in the corresponding place and press \"Capture\" ");
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

	QMessageBox msgBox;
	msgBox.setText("Register Box");
	msgBox.setInformativeText("Do you want to apply this matrix?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Discard);
	msgBox.setDefaultButton(QMessageBox::Yes);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes)
	{
		m_3d_View->SetRegisterTransform(res2);
		//m_3d_View->SetLandMarks(temp_src,temp_dst);
	}
	else
	{
		m_Marker_Capture->ClearMarkers();
		m_3d_View->ClearMarkers();
	}
}

void MainWindow::on_StartTracking()
{
	if (m_ToolModelFileName.empty())
	{
		auto sphere = vtkSmartPointer<vtkSphereSource>::New();
		sphere->SetRadius(10);
		sphere->Update();
		m_Tool = sphere->GetOutput();
	}
	//connect tracking object
	m_3d_View->AddPolySource(m_Tool);
	m_3d_View->ConnectObjectTracker(1,0);

	//add target to view
	if (!m_TargetFileName.empty())
	{
		auto reader_ = vtkSmartPointer<vtkSTLReader>::New();
		reader_->SetFileName(m_TargetFileName.c_str());
		reader_->Update();
		m_Target = reader_->GetOutput();
		m_3d_View->AddPolySource(m_Target);
	}

	m_3d_View->StartTracking();
}

void MainWindow::on_StopTracking()
{
	m_3d_View->StopTracking();
}


void MainWindow::on_ActionCalibrate()
{

}

/*
Set Tracking Tool Model
*/
void MainWindow::on_ActionSetTool()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "E:/", tr("Image Files (*.stl)"));
	if (fileName.isEmpty())
	{
		return;
	}
	m_ToolModelFileName = fileName.toStdString();
	
	auto reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName(m_ToolModelFileName.c_str());
	reader->Update();
	m_Tool = reader->GetOutput();
}

/*
Load target
*/
void MainWindow::on_ActionLoadTarget()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "E:/", tr("Image Files (*.msh)"));
	if (fileName.isEmpty())
	{
		return;
	}
	m_TargetFileName = fileName.toStdString();
}

void MainWindow::on_Sagittal_Slider(int po)
{
	m_SliceX = po;
	//m_PlaneX->SetSliceIndex(m_SliceX);
	m_3d_View->m_PlaneX->SetSliceIndex(po);

	ui->threeDWidget->GetRenderWindow()->Render();
	m_Sagittal_View->SetSlice(po);
}
void MainWindow::on_Axial_Slider(int po)
{
	m_SliceZ = po;
	//m_PlaneZ->SetSliceIndex(m_SliceZ);
	m_3d_View->m_PlaneZ->SetSliceIndex(po);

	ui->threeDWidget->GetRenderWindow()->Render();
	m_Axial_View->SetSlice(po);
}
void MainWindow::on_Coronal_Slider(int po)
{
	m_SliceY = po;
	//m_PlaneY->SetSliceIndex(m_SliceY);
	m_3d_View->m_PlaneY->SetSliceIndex(po);

	ui->threeDWidget->GetRenderWindow()->Render();
	m_Coronal_View->SetSlice(po);
}

void MainWindow::on_Opacity_Slider(int value)
{
	float op = value;
	m_3d_View->SetOpacity(0,op/100);
}


void MainWindow::on_EnablePlane(int state)
{
	if (state == Qt::Checked)
	{
		m_3d_View->m_PlaneX->On();
		m_3d_View->m_PlaneY->On();
		m_3d_View->m_PlaneZ->On();

		//m_PlaneX->On();
		//m_PlaneY->On();
		//m_PlaneZ->On();
	}
	else
	{
		m_3d_View->m_PlaneX->Off();
		m_3d_View->m_PlaneY->Off();
		m_3d_View->m_PlaneZ->Off();

		//m_PlaneX->Off();
		//m_PlaneY->Off();
		//m_PlaneZ->Off();
	}

}
