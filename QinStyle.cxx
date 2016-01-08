
#include "QinStyle.h"


//---------------My_Style-------------//
vtkStandardNewMacro(My_Style);
My_Style::My_Style()
{
	rwi = this->Interactor;
	win = vtkSmartPointer<vtkRenderWindow>::New();
	win = rwi->GetRenderWindow();
	ren = vtkSmartPointer<vtkRenderer>::New();
	ren = win->GetRenderers()->GetFirstRenderer();

	actor_collection = vtkSmartPointer<vtkActorCollection>::New();
	actor_collection = ren->GetActors();
	actor = actor_collection->GetLastActor();

	position[3] = { 0 };
	position_r[3] = { 0 };
	rotate = 0;
	actor->GetPosition(position);
	actor->GetPosition(position_r);
};
My_Style::~My_Style()
{}

void My_Style::OnKeyPress()
{
	// Get the keypress
	std::string key = rwi->GetKeySym();

	// Output the key that was pressed
	std::cout << key << std::endl;

	// Handle an arrow key
	if (key == "Up")
	{
		rotate = rotate + 0.01;
	}
	if (key == "Down")
	{
		rotate = rotate - 0.01;
	}

	// move left
	if (key == "a")
	{
		position[2] = position[2] + 10;
	}
	// move down
	if (key == "s")
	{
		position[1] = position[1] - 10;
	}
	// move right
	if (key == "w")
	{
		position[1] = position[1] + 10;
	}
	//move up
	if (key == "d")
	{
		position[2] = position[2] - 10;
	}
	//reset 
	if (key == "r")
	{
		for (int i = 0; i < 3; i++)
		{
			position[i] = position_r[i];
		}
	}

	actor->SetPosition(position);
	actor->RotateX(rotate);
	win->Render();
	//forward
	vtkInteractorStyleImage::OnKeyPress();
};




//-------------Transform Style--------------//
vtkStandardNewMacro(Transform_Style);



//------------ Test Style -----------------//
vtkStandardNewMacro(TestStyle);

TestStyle::TestStyle()
{
	rwi = this->Interactor;
	win = vtkSmartPointer<vtkRenderWindow>::New();
	win = rwi->GetRenderWindow();
	ren = vtkSmartPointer<vtkRenderer>::New();
	ren = win->GetRenderers()->GetFirstRenderer();

	actor_collection = vtkSmartPointer<vtkActorCollection>::New();
	actor_collection = ren->GetActors();
	actor = actor_collection->GetLastActor();

	position[3] = { 0 };
	position_r[3] = { 0 };
	rotate = 0;
	actor->GetPosition(position);
	actor->GetPosition(position_r);
}
TestStyle::~TestStyle()
{}
