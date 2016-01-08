#ifndef __QINSTYLE__H_
#define __QINSTYLE__H_

#include "vtkInteractorStyleImage.h"

#include "vtkSmartPointer.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRendererCollection.h"
#include "vtkActorCollection.h"

#include <vector>
#include <string>
#include <iostream>




class My_Style : public vtkInteractorStyleImage
{
public:
	static My_Style* New();
	vtkTypeMacro(My_Style, vtkInteractorStyleImage);

	virtual void OnKeyPress();
protected:
	My_Style();
	~My_Style();
private:
	vtkRenderWindowInteractor* rwi;
	vtkSmartPointer<vtkRenderWindow> win;
	vtkSmartPointer<vtkRenderer> ren;

	vtkSmartPointer< vtkActorCollection > actor_collection;
	vtkSmartPointer< vtkActor > actor;

	double position[3];
	double position_r[3];
	double rotate;
};





class Transform_Style : public vtkInteractorStyleImage
{
public:
	static Transform_Style* New();
	vtkTypeMacro(Transform_Style, vtkInteractorStyleImage);

	virtual void OnKeyPress(){};

private:
	vtkRenderWindowInteractor* rwi;
	vtkSmartPointer<vtkRenderWindow> win;
	vtkSmartPointer<vtkRenderer> ren;

	vtkSmartPointer< vtkActorCollection > actor_collection;
	vtkSmartPointer< vtkActor > actor;

	double position[3];
	double position_r[3];
	double rotate;
};





class TestStyle : public vtkInteractorStyleImage
{
public:
	static TestStyle* New();
	vtkTypeMacro(TestStyle, vtkInteractorStyleImage);

	//virtual void OnKeyPress();
protected:
	TestStyle();
	~TestStyle();
private:
	vtkRenderWindowInteractor* rwi;
	vtkSmartPointer<vtkRenderWindow> win;
	vtkSmartPointer<vtkRenderer> ren;

	vtkSmartPointer< vtkActorCollection > actor_collection;
	vtkSmartPointer< vtkActor > actor;

	double position[3];
	double position_r[3];
	double rotate;
};







#endif