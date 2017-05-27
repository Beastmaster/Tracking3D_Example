/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/1/10
*/




#include "mainWindow.h"
#include "ui_mainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
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
	connect(m_3d_View, SIGNAL(on_timer_signal_index(int, int, int)), this, SLOT(on_ResliceAction(int, int, int)), Qt::QueuedConnection);
	connect(m_3d_View, SIGNAL(on_timer_signal_coor(double, double, double)), this, SLOT(on_ResliceAction(double, double, double)), Qt::QueuedConnection);
	connect(m_3d_View, SIGNAL(qs_transform_valid(int)), this, SLOT(on_Change_Tracking_Status(int)));
	//connect(m_3d_View, SIGNAL(on_timer_signal_coor(double, double, double)), this, SLOT(on_ResliceActionMarker(double, double, double)));

	//connect
	connect(ui->load_Image_Btn, SIGNAL(clicked()), this, SLOT(on_Load_Image()), Qt::QueuedConnection);
	connect(ui->load_Atlas_Btn, SIGNAL(clicked()), this, SLOT(on_Load_Atlas()), Qt::QueuedConnection);
	connect(ui->del_Atlas_Btn, SIGNAL(clicked()), this, SLOT(on_Del_Atlas()));
	connect(ui->config_Tracker_Btn, SIGNAL(clicked()), this, SLOT(on_Config_Tracker()));
	connect(ui->sel_Tracker_Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_Sel_Tracker(int)));
	connect(ui->sel_Marker_Btn, SIGNAL(clicked()), this, SLOT(on_Sel_Markers()));
	connect(ui->valid_Marker_Btn, SIGNAL(clicked()), this, SLOT(on_Valid_Marker()));
	connect(ui->sel_Done_Btn, SIGNAL(clicked()), this, SLOT(on_Sel_MarkersDone()));
	connect(ui->cap_Marker_Btn, SIGNAL(clicked()), this, SLOT(on_Cap_Btn()));
	connect(ui->ok_Sel_Btn, SIGNAL(clicked()), this, SLOT(on_CapDone_Btn()));
	connect(ui->start_Tracking_Btn, SIGNAL(clicked()), this, SLOT(on_StartTracking()));
	connect(ui->stop_Tracking_Btn, SIGNAL(clicked()), this, SLOT(on_StopTracking()));
	connect(ui->opacity_Slider, SIGNAL(valueChanged(int)), this, SLOT(on_Opacity_Slider(int)));
	connect(ui->axial_slider, SIGNAL(valueChanged(int)), this, SLOT(on_Axial_Slider(int)));
	connect(ui->sagittal_slider, SIGNAL(valueChanged(int)), this, SLOT(on_Sagittal_Slider(int)));
	connect(ui->coronal_slider, SIGNAL(valueChanged(int)), this, SLOT(on_Coronal_Slider(int)));
	connect(ui->en_Plane_Check, SIGNAL(stateChanged(int)), this, SLOT(on_EnablePlane(int)));
	connect(ui->en_Skull_Check, SIGNAL(stateChanged(int)), this, SLOT(on_EnableSkull(int)));
	
	connect(ui->con_capture_Btn, SIGNAL(clicked()), this, SLOT(on_StartCapture()));
	connect(ui->con_capture_Done_Btn, SIGNAL(clicked()), this, SLOT(On_DoneCapture()));
	connect(ui->con_Register_Btn, SIGNAL(clicked()), this, SLOT(On_FineRegister()));

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
	if (m_TrackerATC3DG != NULL)
	{
		delete m_TrackerATC3DG;
	}
}


void MainWindow::sys_Init()
{
	m_Calibration_Win = new CalibrationWindow();

	m_ImageFileName = "";
	m_AtlasFileName = "";
	m_ToolModelFileName = "toolz.stl";
	m_StripValue = 100;

	//init qvtkwidget 
	ui->axialWidget->GetRenderWindow()->Render();
	ui->sagittalWidget->GetRenderWindow()->Render();
	ui->coronalWidget->GetRenderWindow()->Render();
	ui->threeDWidget->GetRenderWindow()->Render();


	m_Sagittal_View = new reslice_view_base(ui->sagittalWidget->GetRenderWindow(), 's');
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
	m_3d_View->AddEnabledChannels(0);
	//m_3d_View->AddEnabledChannels(1);

	m_Marker_Capture = vtkSmartPointer< vtkTrackingMarkCapture>::New();
	m_Marker_Capture->SetTrackerUtil(m_3d_View->m_trackerUtil);
	m_Marker_Capture->SetRefID(m_3d_View->GetRefID());
	m_Marker_Capture->SetToolID(m_3d_View->GetToolID());

	//apply calibration matrix
	m_ToolTipCalibration_Matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	double mat[4][4];
	mat[0][0] = 1.0;
	mat[0][1] = 0.0;
	mat[0][2] = 0.0;
	mat[0][3] = -18.14;//137.743
	mat[1][0] = 0.0;
	mat[1][1] = 1.0;
	mat[1][2] = 0.0;
	mat[1][3] = 1.55;
	mat[2][0] = 0.0;
	mat[2][1] = 0.0;
	mat[2][2] = 1.0;
	mat[2][3] = -159.68;//25.9801
	mat[3][0] = 0.0;
	mat[3][1] = 0.0;
	mat[3][2] = 0.0;
	mat[3][3] = 1.0;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			m_ToolTipCalibration_Matrix->SetElement(i, j, mat[i][j]);
			std::cout << m_ToolTipCalibration_Matrix->GetElement(i, j) << " , ";
		}
		std::cout << std::endl;
	}

	m_3d_View->SetToolTipCalibrationMatrix(m_ToolTipCalibration_Matrix);

}

/*
Connect signal/slot of QActions
*/
void MainWindow::createActions()
{
	connect(ui->actionCalibrate_Tool, SIGNAL(triggered()), this, SLOT(on_ActionCalibrate()));
	connect(ui->actionSet_Tool, SIGNAL(triggered()), this, SLOT(on_ActionSetTool()));
	connect(ui->actionLoad_Target_Model, SIGNAL(triggered()), this, SLOT(on_ActionLoadTarget()));
	connect(ui->actionCalibrate_Tool, SIGNAL(triggered()), this, SLOT(on_ActionCalibrate()));
}

/*
This function receive signal from vtkTracking3D class
Parameters are coordinate from the world coordinate.
*/
void MainWindow::on_ResliceAction(double x, double y, double z)
{
	if (m_Image == NULL)
		return;

	double* spacing = m_Image->GetSpacing();
	double* origin = m_Image->GetOrigin();
	
	m_SliceX = floor((x - origin[0]) / spacing[0]);
	m_SliceY = floor((y - origin[1]) / spacing[1]);
	m_SliceZ = floor((z - origin[2]) / spacing[2]);
}
void MainWindow::on_ResliceActionMarker(double  x, double y, double z)
{
	on_ResliceAction(x, y, z);

	m_3d_View->GetActorPointer(1)->SetPosition(x,y,z);
	m_3d_View->RefreshView();
}

void MainWindow::on_ResliceAction(int x, int y, int z)
{
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

		//extract 3d model
		auto marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
		marchingCubes->SetInputData(m_Image);
		marchingCubes->SetValue(0, m_StripValue);
		marchingCubes->Update();
		m_ImageModel = marchingCubes->GetOutput();
		m_3d_View->AddPolySource(m_ImageModel);
		m_3d_View->SetColor(0, 0.5, 0.6, 0.7);
		m_3d_View->ResetView();
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
		m_3d_View->m_trackerUtil->SetTrackerType( m_TrackerPolaris );
		m_Marker_Capture->SetTrackerUtil(m_3d_View->m_trackerUtil);
	}
}

void MainWindow::on_Config_Tracker()
{
	if (m_3d_View->m_trackerUtil->ConfigureTracker() != 0)
	{
		std::cout << "Tracker configuration fail" << std::endl;
		QMessageBox msgBox;
		msgBox.setText("Configuration Failed, Please check system.");
		msgBox.exec();
		return;
	}
	if (m_3d_View->m_trackerUtil->StartTracking() != 0)
	{
		std::cout << "Start Tracker fail" << std::endl;
		QMessageBox msgBox;
		msgBox.setText("Configuration Failed, Please check system.");
		msgBox.exec();
		return;
	}
	QMessageBox msgBox;
	msgBox.setText("System configuration success!");
	msgBox.exec();
	m_3d_View->StartCheckingTimer();
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

	auto ball = vtkSmartPointer<vtkSphereSource>::New();
	ball->SetRadius(5.0);
	ball->SetPhiResolution(50);
	ball->SetThetaResolution(50);
	ball->Update();
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(ball->GetOutput());
	auto ball_actor = vtkSmartPointer<vtkActor>::New();
	ball_actor->SetMapper(mapper);
	ball_actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
	m_3d_View->AddObject(ball_actor);
	m_3d_View->RefreshView();

	disconnect(m_3d_View, SIGNAL(on_timer_signal_coor(double, double, double)), this, SLOT(on_ResliceAction(double, double, double)));
	connect(m_3d_View, SIGNAL(on_timer_signal_coor(double, double, double)), this, SLOT(on_ResliceActionMarker(double, double, double)));

	// enable mouse pick first
	m_3d_View->EnablePick(); //press a to accept
}
void MainWindow::on_Valid_Marker()
{
	// valid marker you select in the 3d view
	m_3d_View->ValidMarker();

	int tt = m_3d_View->GetMarkerList().size();

	std::string log = std::to_string(tt).append(" th marker selected");
	ui->log_Label->setText(log.c_str());
}
void MainWindow::on_Sel_MarkersDone()
{
	disconnect(m_3d_View, SIGNAL(on_timer_signal_coor(double, double, double)), this, SLOT(on_ResliceAction(double, double, double)));
	connect(m_3d_View, SIGNAL(on_timer_signal_coor(double, double, double)), this, SLOT(on_ResliceActionMarker(double, double, double)));


	//write to config file
	// m_3d_View->GetMarkerList()

	// disable pick
	m_3d_View->PopObject();
	m_3d_View->DisablePick();
	m_3d_View->RefreshView();
}

/*
Capture the point and the device position
*/
void MainWindow::on_Cap_Btn()
{
	// marker capture get marker;
	m_Marker_Capture->GetNextMarker();
	ui->log_Label->setText("Capturing " + QString::number(m_Marker_Capture->GetNumberOfMarkers()) + " th point..");
}


/*
Finish capture and run registration
*/
void MainWindow::on_CapDone_Btn()
{
	//disconnect pick callback
	m_3d_View->DisablePick();

	std::vector<double*> temp_dst;
	if (m_3d_View->GetMarkerList().size() == 0)
	{
		double a1[] = { 176.286, 195.733, 90.0183 };
		double a2[] = { 83.6962, 197.693, 92.5363 };
		double a3[] = { 125.957, 136.392, 167.287 };
		double a4[] = { 64.2963, 142.04, 133.597 };

		temp_dst.push_back(a1);
		temp_dst.push_back(a2);
		temp_dst.push_back(a3);
		temp_dst.push_back(a4);
	}
	else
	{
		temp_dst = m_3d_View->GetMarkerList();
	}
	//if (m_3d_View->GetMarkerList().size() != m_Marker_Capture->GetMarkerList().size())
	//{
	//	std::cout << "Markers invalid" << std::endl;
	//	m_3d_View->ClearMarkers();
	//	m_Marker_Capture->ClearMarkers();
	//}

	// start registration here
	auto temp_src = m_Marker_Capture->GetMarkerList();

	auto reg = vtkSmartPointer<vtkTrackingLandMarkRegistration>::New();
	reg->SetSourcePoints(temp_src);
	reg->SetTargetPoints(temp_dst);
	reg->GenerateTransform();
	auto res2 = reg->GetTransformMatrix();
	std::cout << "Result" << std::endl;
	std::cout << "Error is: " << reg->EstimateRegistrationError() << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << res2->GetElement(i, j) << ",";
		std::cout << std::endl;
	}

	// a messagebox for user to accecpt or discard the error
	QMessageBox msgBox;
	msgBox.setWindowTitle("Register Box");
	QString msg = "Accept the registration error:\n   ";
	msg = msg + QString::number(reg->EstimateRegistrationError());
	msgBox.setInformativeText(msg);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Discard);
	msgBox.setDefaultButton(QMessageBox::Yes);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes)
	{
		m_3d_View->SetRegisterTransform(res2);
		//m_3d_View->SetLandMarks(temp_src,temp_dst);
	}
	m_Marker_Capture->ClearMarkers();
	m_3d_View->ClearMarkers();
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
	else
	{
		auto reader = vtkSmartPointer<vtkSTLReader>::New();
		reader->SetFileName(m_ToolModelFileName.c_str());
		reader->Update();
		m_Tool = reader->GetOutput();
	}

	//connect tracking object
	m_3d_View->AddPolySource(m_Tool);
	std::cout << "Number of actors: " << m_3d_View->GetNumberOfActors() << std::endl;
	m_3d_View->ConnectObjectTracker(1, 0);
	m_3d_View->SetReferenceIndex(1);

	//add target to view
	if (!m_TargetFileName.empty())
	{
		auto reader_ = vtkSmartPointer<vtkSTLReader>::New();
		reader_->SetFileName(m_TargetFileName.c_str());
		reader_->Update();
		m_Target = reader_->GetOutput();
		m_3d_View->AddPolySource(m_Target);
	}

	m_3d_View->StartTrackingQt();
}

void MainWindow::on_StopTracking()
{
	m_3d_View->StopTrackingQt();
	m_3d_View->m_trackerUtil->StopTracking();
}

/*
Run Calibration process and get output the calibration transform
*/

void MainWindow::on_ActionCalibrate()
{
	m_Calibration_Win->setWindowModality(Qt::WindowModal);
	m_Calibration_Win->show();
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
	m_3d_View->m_PlaneX->SetSliceIndex(po);
	m_Sagittal_View->SetSlice(po);
	m_3d_View->RefreshView();
}
void MainWindow::on_Axial_Slider(int po)
{
	m_SliceZ = po;
	m_3d_View->m_PlaneZ->SetSliceIndex(po);
	m_Axial_View->SetSlice(po);
	m_3d_View->RefreshView();
}
void MainWindow::on_Coronal_Slider(int po)
{
	m_SliceY = po;
	m_3d_View->m_PlaneY->SetSliceIndex(po);
	m_Coronal_View->SetSlice(po);
	m_3d_View->RefreshView();
}

void MainWindow::on_Opacity_Slider(int value)
{
	float op = value;
	m_3d_View->SetOpacity(0, op / 100);
}


void MainWindow::on_EnablePlane(int state)
{
	if (state == Qt::Checked)
	{
		m_3d_View->m_PlaneX->On();
		m_3d_View->m_PlaneY->On();
		m_3d_View->m_PlaneZ->On();
	}
	else
	{
		m_3d_View->m_PlaneX->Off();
		m_3d_View->m_PlaneY->Off();
		m_3d_View->m_PlaneZ->Off();
	}

}
void MainWindow::on_EnableSkull(int state)
{
	if (state == Qt::Checked)
	{
		m_3d_View->SetOpacity(0, 1.0);
		m_3d_View->RefreshView();
	}
	else
	{
		m_3d_View->SetOpacity(0, 0.0);
		m_3d_View->RefreshView();
	}

}

void MainWindow::on_Change_Tracking_Status(int status)
{
	if (status == 1)
	{
		ui->label_status->setText(("<font color='red'>Tracking</font>"));
	}
	else
	{
		m_3d_View->m_trackerUtil->Beep(1);
		ui->label_status->setText("Lost!!");
	}
}

void MainWindow::on_StartCapture()
{
	std::cout << "Continue capture... Triggered" << std::endl;

	if (ui->sel_Tracker_Combo->currentIndex() == 0)
	{
		if (m_TrackerATC3DG->GetTrackingStatus() != 0)
		{
			std::cout << "Tracker not working" << std::endl;
			return;
		}
	}
	if (ui->sel_Tracker_Combo->currentIndex() == 1)
	{
		if (m_TrackerPolaris->GetTrackingStatus() != 0)
		{
			std::cout << "Tracker not working" << std::endl;
			return;
		}
	}

	m_captureTimer = new QTimer;
	m_captureTimer->start(100);    // 100 millisecon interval

	//connect
	m_Marker_Capture->ClearMarkers();
	connect(m_captureTimer, SIGNAL(timeout()), this, SLOT(On_Capture_squence()));
}
void MainWindow::On_DoneCapture()
{
	ui->log_Label->setText("Continue capture done...");
	//stop timer
	m_captureTimer->stop();
	disconnect(m_captureTimer, SIGNAL(timeout()), this, SLOT(On_Capture_squence()));
	if (m_captureTimer != NULL)
	{
		delete m_captureTimer;
	}
	
}
void MainWindow::On_Capture_squence()
{
	m_Marker_Capture->GetNextMarker();
	ui->log_Label->setText("Capturing " + QString::number(m_Marker_Capture->GetNumberOfMarkers()) + " th point..");
}
void MainWindow::On_FineRegister()
{
	std::cout << "Register... Triggered" << std::endl;

	auto reg = vtkSmartPointer<vtkTrackingICPRegistration>::New();
	reg->SetTargetPoints(m_ImageModel->GetPoints());
	reg->SetSourcePoints(m_Marker_Capture->GetMarkerList());
	auto mat = m_3d_View->m_trackerUtil->GetRegistrationMat();

	reg->SetPreMultipliedMatrix(mat);
	reg->GenerateTransform();
	auto res2 = reg->GetTransformMatrix();

	// a messagebox for user to accecpt or discard the error
	QMessageBox msgBox;
	msgBox.setWindowTitle("Register Box");
	QString msg = "Accept the registration error:\n   ";
	msg = msg + QString::number(reg->EstimateRegistrationError());
	msgBox.setInformativeText(msg);msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Discard);msgBox.setDefaultButton(QMessageBox::Yes);
	
	if (msgBox.exec()== QMessageBox::Yes)
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