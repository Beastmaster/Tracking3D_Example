/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date:

Description:
	Main test file
*/

#include "mainWindow.h"
#include "CalibrationWindow.h"
#include <QApplication>
#include <QMainWindow>


#include "vtkTracking3D.h"
#include "ConfigurationParse.h"

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRendererCollection.h"
#include "vtkActorCollection.h"

#include "vtkSTLReader.h"
#include "vtkXMLPolyDataReader.h"

#include "vtkSphereSource.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"

//private head files
#include "vtkTrackingRegistration.h"
#include "vtkTrackingMarkCapture.h"
#include "PolarisVicraConfiguration.h"

//test head files
#include "vtkOutlineFilter.h"
#include "vtkLookupTable.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkDICOMImageReader.h"
#include "vtkMarchingCubes.h"
#include "vtkStripper.h"
#include "vtkDataSetMapper.h"
#include "vtkCoordinate.h"
#include "vtkImagePlaneWidget.h"
#include "vtkImageData.h"
#include "vtkLineSource.h"
#include "vtkNIFTIImageReader.h"
#include "vtkImageMapper.h"
#include "vtkImageActor.h"
#include "vtkSliderRepresentation3D.h"
/*
Test Function for tracking device
*/
void TestTrackingDevice()
{
	auto sphere = vtkSmartPointer<vtkNIFTIImageReader>::New();
	//sphere->SetFileName("E:/test/QinShuoTShape.stl");
	sphere->SetFileName("E:/test/atlas.nii");
	sphere->Update();

	auto line_x = vtkSmartPointer<vtkLineSource>::New();
	line_x->SetPoint1(0,0,0);
	line_x->SetPoint1(50,0,0);
	line_x->Update();

	auto line_y = vtkSmartPointer<vtkLineSource>::New();
	line_y->SetPoint1(0, 0, 0);
	line_y->SetPoint1(0, 50, 0);
	line_y->Update();

	auto line_z = vtkSmartPointer<vtkLineSource>::New();
	line_z->SetPoint1(0, 0, 0);
	line_z->SetPoint1(0, 0, 50);
	line_z->Update();


	//vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
	//reader->SetFileName("E:/QinShuoTShape.stl");
	//reader->Update();
	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetDirectoryName("E:/test/with_BB");
	reader->Update();
	double ori[3];
	reader->GetOutput()->GetOrigin(ori);
	double space[3];
	reader->GetOutput()->GetSpacing(space);

	auto boneExtractor = vtkSmartPointer<vtkMarchingCubes>::New();
	boneExtractor->SetInputData(reader->GetOutput());
	boneExtractor->SetValue(0, 500);
	boneExtractor->Update();


	auto track = vtkSmartPointer<vtkTracking3D>::New();

	track->AddPolySource(line_x->GetOutput());
	track->AddPolySource(line_y->GetOutput());
	track->AddPolySource(line_z->GetOutput());

	//track->AddPolySource(sphere->GetOutput());

	auto tra = vtkSmartPointer<vtkMatrix4x4>::New();
	tra->SetElement(0, 0, 0.491933);
	tra->SetElement(0, 1, 0.159776);
	tra->SetElement(0, 2, 0.855847);
	tra->SetElement(0, 3, 46.9787);
	tra->SetElement(1, 0, -0.186244);
	tra->SetElement(1, 1, -0.940949);
	tra->SetElement(1, 2, 0.282715);
	tra->SetElement(1, 3, -46.9093);
	tra->SetElement(2, 0, 0.850479);
	tra->SetElement(2, 1, -0.298473);
	tra->SetElement(2, 2, -0.433126);
	tra->SetElement(2, 3, 47.4986);
	tra->SetElement(3, 0, 0);
	tra->SetElement(3, 1, 0);
	tra->SetElement(3, 2, 0);
	tra->SetElement(3, 3, 1);

	auto tra2= vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Invert(tra,tra2);

	//track->SetRegisterTransform(tra2);

	auto interactorx = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera >::New();
	track->SetInteractorStyle(style);
	track->SetInteractor(interactorx);
	track->InstallPipeline();

	track->AddPolySource(boneExtractor->GetOutput());

	//track->m_tracker->ConfigureTracker();
	track->ConnectObjectTracker(4, 0);
	Sleep(20);

	track->m_tracker->StartTracking();
	track->StartTracking();

	Sleep(20);

	track->GetRenderWindow()->Render();
	interactorx->Start();

	track->m_tracker->StopTracking();
}

void TestMousePick()
{

	vtkSmartPointer<vtkSphereSource> sphere =
		vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetCenter(0, 0, 0);
	sphere->SetRadius(50);
	sphere->Update();

	

	/*
	vtkSmartPointer<vtkConeSource> cone =
		vtkSmartPointer<vtkConeSource>::New();
	cone->SetHeight(80);
	cone->SetRadius(50);
	cone->Update();
	vtkSmartPointer<vtkCylinderSource> cylinder =
		vtkSmartPointer<vtkCylinderSource>::New();
	cylinder->SetHeight(80);
	cylinder->SetRadius(60);
	cylinder->Update();*/

	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetDirectoryName("E:/test/with_BB");
	reader->Update();
	double ori[3];
	reader->GetOutput()->GetOrigin(ori);
	double space[3];
	reader->GetOutput()->GetSpacing(space);

	auto boneExtractor = vtkSmartPointer<vtkMarchingCubes>::New();
	boneExtractor->SetInputData(reader->GetOutput());
	boneExtractor->SetValue(0, 500);
	boneExtractor->Update();

	vtkPolyDataMapper *boneMapper = vtkPolyDataMapper::New();
	boneMapper->SetInputData(boneExtractor->GetOutput());

	double bound[6];
	boneExtractor->GetOutput()->GetBounds(bound);
	std::cout << bound[0] << " " << bound[1] << " " << bound[2] << " " << bound[3] << " " << bound[4] << " " << bound[5] << std::endl;

	auto boneActor = vtkSmartPointer<vtkActor>::New();
	boneActor->SetMapper(boneMapper);

	//create outline source
	auto outline = vtkSmartPointer<vtkOutlineFilter>::New();
	outline->SetInputData(reader->GetOutput());
	outline->Update();

	auto track = vtkSmartPointer<vtkTracking3D>::New();

	track->AddObject(boneActor);
	track->AddPolySource(sphere->GetOutput());
	track->SetOpacity(0,0.5);
	//track->AddPolySource(outline->GetOutput());
	//track->AddPolySource(sphere->GetOutput());
	//track->AddPolySource(cone->GetOutput());
	//track->AddPolySource(cylinder->GetOutput());

	//auto win1 = vtkSmartPointer<vtkRenderWindow>::New();
	//track->SetWindow(win1);
	auto interactorx = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera >::New();
	track->SetInteractorStyle(style);
	track->SetInteractor(interactorx);
	track->InstallPipeline();

	track->GetRenderWindow()->Render();
	track->GetInteractor()->Start();

	auto xx1 = track->GetMarkerList();

	auto tracker = new ATC3DGConfiguration;

	tracker->ConfigureTracker();
	tracker->StartTracking();


	auto mark = vtkSmartPointer< vtkTrackingMarkCapture>::New();
	
	mark->SetTracker(tracker);
	mark->SetToolIndex(0);
	//mark->SetReferIndex(2);

	//tracker->ConfigureTracker();
	//tracker->StartTracking();

	char a;
	std::cout << "start picking ... " << std::endl;
	a = getchar();
	while (a != 'q')
	{
		mark->GetNextMarker();
		std::cout << "done" << std::endl;
		a = getchar();
	}

	tracker->StopTracking();

	auto xx2 = mark->GetMarkerList();

	//registration
	auto reg1 = vtkSmartPointer <vtkTrackingICPRegistration > ::New();
	auto src_points = vtkSmartPointer<vtkPoints>::New();
	auto dst_points = vtkSmartPointer<vtkPoints>::New();

	std::cout << "source:" << std::endl;
	for (auto it = xx2.begin(); it != xx2.end(); ++it)
	{
		std::cout << (*it)[0] << "," << (*it)[1] << "," << (*it)[2] << std::endl;
		src_points->InsertNextPoint((*it));
	}
	std::cout << "dest:" << std::endl;
	for (auto it = xx1.begin(); it != xx1.end(); ++it)
	{
		std::cout << (*it)[0] << "," << (*it)[1] << "," << (*it)[2] << std::endl;
		dst_points->InsertNextPoint((*it));
	}
	//reg1->SetSourcePoints(src_points);
	//reg1->SetTargetPoints(dst_points);
	reg1->GenerateTransform();
	auto res1 = reg1->GetTransformMatrix();

	std::cout << "\nres1:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << res1->GetElement(i, j) << ",";
		std::cout << std::endl;
	}

}

void TestRegistration()
{
	auto src_points = vtkSmartPointer<vtkPoints>::New();
	auto dst_points = vtkSmartPointer<vtkPoints>::New();

	std::vector<double*> src1;
	std::vector<double*> dst1;

	double v1[4][3] = {
		{27.0123, -312.651, 175.357},
		{110.84, -313.097, 117.314 },
		{99.9009, -366.006, 130.82 },
		{19.4221, 288.541, -121.779}
	};

	double vv1[4][3] ={
		{116.265, 134.093, 90.2336},
		{90.6786, 150.426, 140.594},
		{147.852, 149.662, 146.22 },
		{105.495, 77.2705, 110.88 }
	};

	for (size_t i = 0; i < 4; i++)
	{
		src1.push_back(v1[i]);
	}
	for (size_t i = 0; i < 4; i++)
	{
		//dst_points->InsertNextPoint(vv1[i]);
		dst1.push_back(vv1[i]);
	}

	auto reg1 = vtkSmartPointer <vtkTrackingICPRegistration > ::New();
	auto reg2 = vtkSmartPointer<vtkTrackingLandMarkRegistration>::New();

	//reg1->SetSourcePoints(src1);
	//reg1->SetTargetPoints(dst1);
	reg1->GenerateTransform();
	auto res1 = reg1->GetTransformMatrix();

	reg2->SetSourcePoints(src1);
	reg2->SetTargetPoints(dst1);
	reg2->GenerateTransform();
	auto res2 = reg2->GetTransformMatrix();

	std::cout << "\nres1:" << std::endl;
	for (int i = 0; i < 4 ; i++)
	{
		for (int j = 0; j < 4 ; j++)
			std::cout << res1->GetElement(i,j)<<",";
		std::cout << std::endl;
	}
	std::cout << "\nres2:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << res2->GetElement(i, j) << ",";
		std::cout << std::endl;
	}
}

void TestTrackingMarkFunction()
{
	auto track = new ATC3DGConfiguration;
	
	auto mark = vtkSmartPointer< vtkTrackingMarkCapture >::New();
	
	mark->SetTracker(track);
	mark->SetToolIndex(0);

	track->ConfigureTracker();
	track->StartTracking();
	
	char a;
	a = getchar();
	while (a!='q')
	{

		mark->GetNextMarker();
		a = getchar();
	}

	auto xx2 = mark->GetMarkerList();

	std::cout << "source:" << std::endl;
	for (auto it = xx2.begin(); it != xx2.end(); ++it)
	{
		std::cout << (*it)[0] << "," << (*it)[1] << "," << (*it)[2] << std::endl;
	}

	track->StopTracking();
}

void TestOrthogonalPlane()
{
	auto reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetDirectoryName("E:/test/with_BB");
	reader->Update();

	auto image = vtkSmartPointer<vtkImageData>::New();
	image = reader->GetOutput();

	int ext[6];
	image->GetExtent(ext);
	
	auto renderer = vtkSmartPointer<vtkRenderer>::New();
	auto renWin = vtkSmartPointer<vtkRenderWindow>::New();
	auto interact = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	auto styleImage = vtkSmartPointer<vtkInteractorStyleImage>::New();
	interact->SetInteractorStyle(styleImage);
	renWin->AddRenderer(renderer);
	renWin->SetInteractor(interact);

	auto planeX = vtkSmartPointer<vtkImagePlaneWidget>::New();
	auto planeY = vtkSmartPointer<vtkImagePlaneWidget>::New();
	auto planeZ = vtkSmartPointer<vtkImagePlaneWidget>::New();

	planeX->SetInteractor(interact);
	planeY->SetInteractor(interact);
	planeZ->SetInteractor(interact);

	planeX->GetPlaneProperty()->SetColor(1, 0, 0);
	planeX->GetPlaneProperty()->SetColor(1, 1, 0);
	planeX->GetPlaneProperty()->SetColor(1, 1, 1);

	planeX->SetInputData((vtkDataSet*)reader->GetOutput());
	planeY->SetInputData((vtkDataSet*)reader->GetOutput());
	planeZ->SetInputData((vtkDataSet*)reader->GetOutput());
	planeX->SetPlaneOrientationToXAxes();
	planeY->SetPlaneOrientationToYAxes();
	planeZ->SetPlaneOrientationToZAxes();

	planeX->On();
	planeY->On();
	planeZ->On();

	renderer->ResetCamera();
	renWin->Render();
	interact->Start();
}


void TestView()
{
	auto reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
	reader->SetFileName("E:/test/a1.nii");
	reader->Update();
	auto reslice = vtkSmartPointer<vtkImageReslice>::New();
	reslice->SetInputData(reader->GetOutput());
	double axialX[3] = { -1, 0, 0 };
	double axialY[3] = { 0, -1, 0 };
	double axialZ[3] = { 0, 0, -1 };
	reslice->SetResliceAxesDirectionCosines(axialX, axialY, axialZ);
	reslice->SetResliceAxesOrigin(20, 30, 40);
	reslice->SetInterpolationModeToLinear();
	reslice->Update();


	vtkSmartPointer<vtkWindowLevelLookupTable> windowLevelLookupTable =
		vtkSmartPointer<vtkWindowLevelLookupTable>::New();
	float window = 1000;
	float level = 500;
	int nColours = 30;
	windowLevelLookupTable->SetWindow(window);
	windowLevelLookupTable->SetLevel(level);
	windowLevelLookupTable->SetNumberOfTableValues(nColours);

	auto actor = vtkSmartPointer<vtkImageActor >::New();
	auto mapper = vtkImageMapToWindowLevelColors::New();
	mapper->SetWindow(window);
	mapper->SetLevel(level);
	mapper->SetInputData(reslice->GetOutput());

	actor->SetInputData(mapper->GetOutput());

	auto renderer = vtkSmartPointer<vtkRenderer>::New();
	auto win = vtkSmartPointer<vtkRenderWindow>::New();
	win->AddRenderer(renderer);

	auto intact = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	intact->SetRenderWindow(win);

	win->Render();
	intact->Start();



}

void TestSliderWidget()
{
	// A sphere
	vtkSmartPointer<vtkSphereSource> sphereSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(4.0);
	sphereSource->SetPhiResolution(4);
	sphereSource->SetThetaResolution(8);

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetInterpolationToFlat();

	// A renderer and render window
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	// An interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// Add the actors to the scene
	renderer->AddActor(actor);

	// Render an image (lights and cameras are created automatically)
	renderWindow->Render();

	vtkSmartPointer<vtkSliderRepresentation2D> sliderRep =
		vtkSmartPointer<vtkSliderRepresentation2D>::New();
	sliderRep->SetMinimumValue(3.0);
	sliderRep->SetMaximumValue(50.0);
	sliderRep->SetValue(sphereSource->GetThetaResolution());
	sliderRep->SetTitleText("Sphere Resolution");
	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToWorld();
	sliderRep->GetPoint1Coordinate()->SetValue(-4, 6, 0);
	sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToWorld();
	sliderRep->GetPoint2Coordinate()->SetValue(4, 6, 0);
	sliderRep->SetSliderLength(0.075);
	sliderRep->SetSliderWidth(0.05);
	sliderRep->SetEndCapLength(0.05);

	vtkSmartPointer<vtkSliderWidget> sliderWidget =
		vtkSmartPointer<vtkSliderWidget>::New();
	sliderWidget->SetInteractor(renderWindowInteractor);
	sliderWidget->SetRepresentation(sliderRep);
	sliderWidget->SetAnimationModeToAnimate();
	sliderWidget->EnabledOn();

	//vtkSmartPointer<vtkSliderCallback> callback =
	//	vtkSmartPointer<vtkSliderCallback>::New();
	//callback->SphereSource = sphereSource;

	//sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);

	renderWindowInteractor->Initialize();
	renderWindow->Render();

	renderWindowInteractor->Start();

}

void TestMesh()
{
	auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
	reader->SetFileName("E:/test/Appimplant.msh");
	reader->Update();
	auto xx = reader->GetOutput();
}

void TestPolaris()
{
	auto polaris_tracker = new PloarisVicraConfiguration;
}

void TestTrack3D()
{
	auto track = vtkTracking3D::New();
	auto interact = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	auto style = vtkSmartPointer<vtkInteractorStyleImage>::New();
	interact->SetInteractorStyle(style);
	auto win = vtkSmartPointer<vtkRenderWindow>::New();
	auto render = vtkSmartPointer<vtkRenderer>::New();
	//win->AddRenderer(render);
	interact->SetRenderWindow(win);
	interact->Initialize();

	track->SetInteractor(interact);

	track->StartTracking();

	//interact->Start();
}

void TestConfigParser()
{
	std::string file_name = "E:/WorkPlace/Cpp/vtk_solutions/tracking/SoftWareConfigure.xml";
	ConfigurationParse* parser = new ConfigurationParse();

	parser->SetFileName(file_name);
	parser->ReadFile();

	parser->UpdateFile();
}

QCoreApplication* createApplication(int &argc, char *argv[])
{
	for (int i = 1; i < argc; ++i)
		if (!qstrcmp(argv[i], "-no-gui"))
			return new QCoreApplication(argc, argv);
	return new QApplication(argc, argv);
}

int main(int argc, char** argv)
{
	//TestConfigParser();
	//TestMesh();
	//TestMousePick();
	//TestTrackingDevice();
	//TestOrthogonalPlane();
	//TestTrackingMarkFunction();
	//TestRegistration();
	//TestTrack3D();
	//TestSliderWidget();


	QScopedPointer<QCoreApplication> app(createApplication(argc, argv));

	if (qobject_cast<QApplication *>(app.data())) {
		// start GUI version...
		MainWindow* main_window;
		main_window = new MainWindow;
		main_window->show();
	}
	else {
		// start non-GUI version...
	}

	return app->exec();

}

