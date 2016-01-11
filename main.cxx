
#include "vtkTracking3D.h"
#include "QinStyle.h"


#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRendererCollection.h"
#include "vtkActorCollection.h"

#include "vtkSTLReader.h"


#include "vtkSphereSource.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"

//private head files
#include "vtkTrackingRegistration.h"
#include "vtkTrackingMarkCapture.h"

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
/*
Test Function for tracking device
*/
void TestTrackingDevice()
{
	vtkSmartPointer<vtkSphereSource> sphere =
		vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetCenter(0, 0, 0);
	sphere->SetRadius(50);
	sphere->Update();

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


	track->AddPolySource(boneExtractor->GetOutput());
	track->AddPolySource(sphere->GetOutput());

	auto interactorx = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera >::New();
	track->SetInteractorStyle(style);
	track->SetInteractor(interactorx);
	track->InstallPipeline();

	track->m_tracker->ConfigureTracker();
	track->ConnectObjectTracker(1, 0);
	Sleep(20);

	track->m_tracker->StartTracking();
	track->StartTracking();

	Sleep(20);

	track->GetRenderWindow()->Render();
	interactorx->Start();

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


	auto mark = vtkSmartPointer< vtkTrackingMarkCapture<ATC3DGConfiguration> >::New();
	
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
	reg1->SetSourcePoints(src_points);
	reg1->SetTargetPoints(dst_points);
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
	auto reg1 = vtkSmartPointer <vtkTrackingICPRegistration > ::New();
	auto reg2 = vtkSmartPointer<vtkTrackingLandMarkRegistration>::New();

	auto src_points = vtkSmartPointer<vtkPoints>::New();
	auto dst_points = vtkSmartPointer<vtkPoints>::New();

	float v1[4][3] = {
		{27.0123, -312.651, 175.357},
		{110.84, -313.097, 117.314 },
		{99.9009, -366.006, 130.82 },
		{19.4221, 288.541, -121.779}
	};

	float vv1[4][3] ={
		{116.265, 134.093, 90.2336},
		{90.6786, 150.426, 140.594},
		{147.852, 149.662, 146.22 },
		{105.495, 77.2705, 110.88 }
	};

	for (size_t i = 0; i < 4; i++)
	{
		src_points->InsertNextPoint(v1[i]);
	}
	for (size_t i = 0; i < 4; i++)
	{
		dst_points->InsertNextPoint(vv1[i]);
	}

	reg1->SetSourcePoints(src_points);
	reg1->SetTargetPoints(dst_points);
	reg1->GenerateTransform();
	auto res1 = reg1->GetTransformMatrix();

	reg2->SetSourcePoints(src_points);
	reg2->SetTargetPoints(dst_points);
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
	
	auto mark = vtkSmartPointer< vtkTrackingMarkCapture<ATC3DGConfiguration> >::New();
	
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

int main(int argc, char** argv)
{

	//TestMousePick();
	//TestTrackingDevice();
	//TestOrthogonalPlane();
	//TestTrackingMarkFunction();
	TestRegistration();
	return 0;
}

