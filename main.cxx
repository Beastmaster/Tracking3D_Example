
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
	vtkSmartPointer<vtkConeSource> cone =
		vtkSmartPointer<vtkConeSource>::New();
	cone->SetHeight(80);
	cone->SetRadius(50);
	cone->Update();
	vtkSmartPointer<vtkCylinderSource> cylinder =
		vtkSmartPointer<vtkCylinderSource>::New();
	cylinder->SetHeight(80);
	cylinder->SetRadius(60);
	cylinder->Update();

	vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName("E:/QinShuoTShape.stl");
	reader->Update();


	auto track = vtkSmartPointer<vtkTracking3D>::New();


	track->AddPolySource(sphere->GetOutput());
	track->AddPolySource(cone->GetOutput());
	track->AddPolySource(cylinder->GetOutput());

	//auto win1 = vtkSmartPointer<vtkRenderWindow>::New();
	//track->SetWindow(win1);
	auto interactorx = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera >::New();
	track->SetInteractorStyle(style);
	track->SetInteractor(interactorx);
	track->InstallPipeline();

	track->m_tracker->ConfigureTracker();
	Sleep(20);

	track->m_tracker->StartTracking();

	Sleep(20);

	track->ConnectObjectTracker(0, 0);
	track->ConnectObjectTracker(1, 1);
	track->ConnectObjectTracker(2, 2);

	track->GetRenderWindow()->Render();
	track->GetInteractor()->Start();

}

void TestMousePick()
{
	/*
	vtkSmartPointer<vtkSphereSource> sphere =
		vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetCenter(0, 0, 0);
	sphere->SetRadius(50);
	sphere->Update();
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

	vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName("E:/QinShuoTShape.stl");
	reader->Update();

	//create outline source
	auto outline = vtkSmartPointer<vtkOutlineFilter>::New();
	outline->SetInputData(reader->GetOutput());
	outline->Update();

	auto track = vtkSmartPointer<vtkTracking3D>::New();

	track->AddPolySource(reader->GetOutput());
	track->AddPolySource(outline->GetOutput());
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
}

void TestRegistration()
{
	auto reg1 = vtkSmartPointer <vtkTrackingICPRegistration > ::New();
	auto reg2 = vtkSmartPointer<vtkTrackingLandMarkRegistration>::New();

	auto src_points = vtkSmartPointer<vtkPoints>::New();
	auto dst_points = vtkSmartPointer<vtkPoints>::New();

	float v1[9][3] = {
		{ 0, 0, 0 },
		{ 1, 0, 0 },
		{ 1, 1, 0 },
		{ 1, 1, 1 },
		{ 0, 1, 0 },
		{ 0, 1, 1 },
		{ 0, 1, 1 },
		{ 0, 0, 1 },
		{ 1, 1, 1 } };

	float vv1[9][3] ={
	{ 0 + 10, 0 + 10, 0 + 10 },
	{ 1 + 10, 0 + 10, 0 + 10 },
	{ 1 + 10, 1 + 10, 0 + 10 },
	{ 1 + 10, 1 + 10, 1 + 10 },
	{ 0 + 10, 1 + 10, 0 + 10 },
	{ 0 + 10, 1 + 10, 1 + 10 },
	{ 0 + 10, 1 + 10, 1 + 10 },
	{ 0 + 10, 0 + 10, 1 + 10 },
	{ 1 + 10, 1 + 10, 1 + 10 }
	};

	for (size_t i = 0; i < 9; i++)
	{
		src_points->InsertNextPoint(v1[i]);
	}
	for (size_t i = 0; i < 9; i++)
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
	
	auto track = new PloarisVicraConfiguration ;
	
	auto mark = vtkSmartPointer< vtkTrackingMarkCapture<PloarisVicraConfiguration> >::New();
	
	mark->SetTracker(track);
	mark->SetToolIndex(1);
	mark->SetReferIndex(2);

	track->ConfigureTracker();
	track->StartTracking();
	
	while (getchar()!='q')
	{
		mark->GetNextMarker();
	}

	track->StopTracking();
}

void TestOrthogonalPlane()
{
	auto reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetDirectoryName("E:/test/ct_spine");
	reader->Update();

	auto boneExtractor = vtkMarchingCubes::New();
	boneExtractor->SetInputConnection(reader->GetOutputPort());
	boneExtractor->SetValue(0, 1000);
	boneExtractor->Update();
	vtkStripper *boneStripper = vtkStripper::New();
	boneStripper->SetInputData(boneExtractor->GetOutput());
	boneExtractor->Update();
	vtkPolyDataMapper *boneMapper = vtkPolyDataMapper::New();
	boneMapper->SetInputData(boneStripper->GetOutput());
	boneMapper->ScalarVisibilityOff();
	vtkActor *bone = vtkActor::New();
	bone->SetMapper(boneMapper);
	bone->GetProperty()->SetDiffuseColor(1, 1, .9412);


	auto bwLut = vtkSmartPointer<vtkLookupTable> ::New();
	bwLut->SetTableRange(0, 2000);
	bwLut->SetSaturationRange(0, 0);
	bwLut->SetHueRange(0, 0);
	bwLut->SetValueRange(0, 1);
	bwLut->Build(); //effective built

	auto hueLut = vtkSmartPointer<vtkLookupTable>::New();
	hueLut->SetTableRange(0, 2000);
	hueLut->SetHueRange(0, 1);
	hueLut->SetSaturationRange(1, 1);
	hueLut->SetValueRange(1, 1);
	hueLut->Build(); //effective built

	auto satLut = vtkSmartPointer<vtkLookupTable>::New();
	satLut->SetTableRange(0, 2000);
	satLut->SetHueRange(.6, .6);
	satLut->SetSaturationRange(0, 1);
	satLut->SetValueRange(1, 1);
	satLut->Build(); //effective built


	// sagittal
	auto saggitalSection =
		vtkSmartPointer<vtkImageDataGeometryFilter>::New();
	saggitalSection->SetExtent(32, 32, 0, 63, 0, 93);
	saggitalSection->SetInputConnection(reader->GetOutputPort());
	vtkPolyDataMapper *saggitalMapper = vtkPolyDataMapper::New();
	saggitalMapper->SetInputData(saggitalSection->GetOutput());
	saggitalMapper->ScalarVisibilityOn();
	saggitalMapper->SetScalarRange(0, 2000);
	saggitalMapper->SetLookupTable(bwLut);
	vtkActor *sagittal = vtkActor::New();
	sagittal->SetMapper(saggitalMapper);

	// axial
	auto axialSection =
		vtkSmartPointer<vtkImageDataGeometryFilter>::New();
	axialSection->SetExtent(0, 63, 0, 63, 46, 46);
	axialSection->SetInputData(reader->GetOutput());
	vtkPolyDataMapper *axialMapper = vtkPolyDataMapper::New();
	axialMapper->SetInputData(axialSection->GetOutput());
	axialMapper->ScalarVisibilityOn();
	axialMapper->SetScalarRange(0, 2000);
	axialMapper->SetLookupTable(hueLut);
	vtkActor *axial = vtkActor::New();
	axial->SetMapper(axialMapper);

	// coronal
	auto coronalSection =
		vtkSmartPointer<vtkImageDataGeometryFilter>::New();
	coronalSection->SetExtent(0, 63, 32, 32, 0, 92);
	coronalSection->SetInputConnection(reader->GetOutputPort());
	vtkPolyDataMapper *coronalMapper =
		vtkPolyDataMapper::New();
	coronalMapper->SetInputData(coronalSection->GetOutput());
	coronalMapper->ScalarVisibilityOn();
	coronalMapper->SetScalarRange(0, 2000);
	coronalMapper->SetLookupTable(satLut);
	vtkActor *coronal = vtkActor::New();
	coronal->SetMapper(coronalMapper);


	auto coor = vtkSmartPointer<vtkCoordinate>::New();
	coor->SetCoordinateSystemToNormalizedDisplay();


	auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(boneExtractor->GetOutput());
	auto b_actor = vtkSmartPointer<vtkActor>::New();
	b_actor->SetMapper(mapper);


	auto renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(0.1, 0.1, 0.1);
	//renderer->AddActor(b_actor);
	//renderer->AddActor(sagittal);
	//renderer->AddActor(axial);
	renderer->AddActor(coronal);

	auto rewin = vtkSmartPointer<vtkRenderWindow>::New();
	auto intact = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	intact->SetRenderWindow(rewin);
	rewin->AddRenderer(renderer);

	//auto planeWidget = vtkSmartPointer<vtkImagePlaneWidget>::New();
	//planeWidget->SetInteractor(intact);
	//planeWidget->SetOrigin(0, 1, 0);
	//planeWidget->UpdatePlacement();
	
	rewin->Render();
	//planeWidget->On();
	intact->Start();
}

int main(int argc, char** argv)
{
	TestOrthogonalPlane();

	return 0;
}

