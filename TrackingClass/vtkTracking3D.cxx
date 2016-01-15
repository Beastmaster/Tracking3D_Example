/*
Author: QIN Shuo
Date: 2015/12/29
Organization: RC-MIC (CUHK)
Description:
	tracking 3d object
	add OrthogonalPlanes
*/


#include "vtkTracking3D.h"


vtkStandardNewMacro(vtkTracking3D); //New() function
vtkTracking3D::vtkTracking3D()
{
	m_interval = 100;
	m_CurrentRenderer = vtkSmartPointer<vtkRenderer>::New();
	m_CurrentRenderer->SetBackground(1,1,1);
	m_RendererCollection = vtkSmartPointer<vtkRendererCollection>::New();
	m_RenderWindow = vtkSmartPointer< vtkRenderWindow >::New();
	m_ActorCollection = vtkSmartPointer < vtkActorCollection >::New();
	//m_ActorCollection = m_CurrentRenderer->GetActors();
	m_PropCollection = vtkSmartPointer < vtkPropCollection > ::New();
	m_PropCollection = m_CurrentRenderer->GetViewProps();
	
	m_Interactor = vtkSmartPointer < vtkRenderWindowInteractor >::New();
	m_InteractorStyle = vtkSmartPointer < vtkInteractorStyleImage > ::New();
	m_KeyPressCallBack = vtkSmartPointer<vtkCallbackCommand> ::New();
	m_TimerCallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	m_MouseCallback = vtkSmartPointer<vtkCallbackCommand>::New();

	//
	m_Transform = vtkSmartPointer<vtkTransform>::New();
	m_Transform->PostMultiply(); //this is the key line

	//m_tracker = new TrackerType;
	m_tracker = NULL;

	m_PlaneX = vtkSmartPointer<vtkImagePlaneWidget>::New();
	m_PlaneY = vtkSmartPointer<vtkImagePlaneWidget>::New();
	m_PlaneZ = vtkSmartPointer<vtkImagePlaneWidget>::New();
}
vtkTracking3D::~vtkTracking3D()
{
	//Tracker is managed by it self
	//delete m_tracker;	//m_tracker = NULL; 
}

// Please be noted that:
//   m_ActorCollection is used to manage actors but no view function
//   if you want to remove an actor from view, you must remove from
//   renderer
//   All manipulation of the actors will take effect only from renderer
// The following functions implement the management of actor list
int vtkTracking3D::AddObject(vtkSmartPointer< vtkActor > act)
{
	m_ActorCollection->AddItem(act);
	m_CurrentRenderer->AddActor(act);
	m_CurrentRenderer->ResetCamera();
	return 0;
}

int vtkTracking3D::AddPolySource(vtkSmartPointer<vtkPolyData> poly)
{
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->ScalarVisibilityOff();
	mapper->SetInputData(poly);
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	this->AddObject(actor);
	return 1;
}

/*
Orthogonal View planes
*/
int vtkTracking3D::EnableOrthogonalPlanes() 
{ 
	m_PlaneX->On();
	m_PlaneY->On();
	m_PlaneZ->On();
	return 0; 
};
int vtkTracking3D::DisableOrthogonalPlanes() 
{ 
	m_PlaneX->Off();
	m_PlaneY->Off();
	m_PlaneZ->Off();
	return 0; 
};




/*
Description:
	Link index of actor and tracking tool
	Linkage of tool and actor is checked in callback, 
	be free to use it anywhere
Input:
	obj_index:   actor index
	too_index:   tool index

Return:
	0: success
	1: fail, actor index out of range
	2: fail, tool index out of range
Note: 
	Index count from 0
*/
int vtkTracking3D::ConnectObjectTracker(int obj_index, int tool_index)
{
	if (obj_index >= m_ActorCollection->GetNumberOfItems())
	{
		std::cout << "actor index out of range" << std::endl;
		return 1;
	}
	if (m_tracker->GetToolValidation(tool_index) != 0)
	{
		std::cout << "tool index out of range" << std::endl;
		return 2;
	}
	m_Obj_Tool_Map[obj_index] = tool_index;
	return 0;
}

/*
Description:
	Disconnect actor and tracking tool by index
Input:
	obj_index:   actor index
Return:
	0: success
	1: fail, actor index out of range
	2: Not connected
Note:
	Index count from 0
*/
int vtkTracking3D::DisConnectObjectTracker(int obj_index)
{
	if (obj_index >= m_ActorCollection->GetNumberOfItems())
	{
		std::cout << "actor index out of range" << std::endl;
		return 1;
	}
	auto it = m_Obj_Tool_Map.find(obj_index);
	if (it!=m_Obj_Tool_Map.end())
	{
		m_Obj_Tool_Map.erase(it);
		return 0;
	}
	else
	{
		std::cout << "no connection" << std::endl;
		return 2;
	}
}

//get vtkactor pointer from a vtkActorCollection 
vtkActor* vtkTracking3D::GetActorPointer(vtkPropCollection* collection, int index)
{
	if (collection->GetNumberOfItems()>0 && collection->GetNumberOfItems() >= index)
	{
		return static_cast<vtkActor*>(collection->GetItemAsObject(index));
	}
	else
		return NULL;
}

/*
Description:
	Put the selected marker in the world coordinate to marker list
Return:
	0: success
	1: fail
*/
int vtkTracking3D::ValidMarker()
{
	double* mar;
	mar = new double[3];
	memcpy(mar,m_marker_tobe_set,3*sizeof(double));
	m_marker_list.push_back(mar);

	std::cout << "The "<<m_marker_list.size() << " marker is set" << std::endl;
	return 0;
}

int vtkTracking3D::ClearMarkers()
{
	for (auto it = m_marker_list.begin(); it != m_marker_list.end(); ++it)
	{
		delete *it;
	}
	m_marker_list.clear();
	return 0;
}

std::vector<double*> vtkTracking3D::GetMarkerList()
{
	return this->m_marker_list;
}


/*
Description:
	Set the outline of a specified actor
Input:
	index: index of the actor
	en:    enablitiy of outline
		true:  enable
		false: disable
*/
int vtkTracking3D::SetEnableOutline(int index, bool en)
{
	return 0;
}


/*
Description:
	Set Input image data
	This image is badly need to calculate the reslice 
	index in 3d image.
Input:
	type: vtkImageData*

*/
int vtkTracking3D::SetImage(vtkSmartPointer<vtkImageData> in)
{
	m_Image = vtkSmartPointer<vtkImageData>::New();
	m_Image = in;

	m_PlaneX->SetInputData(m_Image);
	m_PlaneY->SetInputData(m_Image);
	m_PlaneZ->SetInputData(m_Image);
	m_PlaneX->SetPlaneOrientationToXAxes();
	m_PlaneY->SetPlaneOrientationToYAxes();
	m_PlaneZ->SetPlaneOrientationToZAxes();

	return 0;
}


/*
Description:
Set the opacity of a specified actor
index count from 0
*/
int vtkTracking3D::SetOpacity(int index, float opacity)
{
	if (m_ActorCollection->GetNumberOfItems() > 0 && m_ActorCollection->GetNumberOfItems() >= index)
	{
		//static_cast<vtkActor*>(m_ActorCollection->GetItemAsObject(index))->GetProperty()->SetOpacity(opacity);
		GetActorPointer(m_ActorCollection, index)->GetProperty()->SetOpacity(opacity);
		return 0;
	}
	else
		return 1;
}

/*
Description:
	Set color function
*/
int vtkTracking3D::SetColor(int index, double r, double g, double b)
{
	if (m_ActorCollection->GetNumberOfItems() > 0 && m_ActorCollection->GetNumberOfItems() >= index)
	{
		//static_cast<vtkActor*>(m_ActorCollection->GetItemAsObject(index))->GetProperty()->SetOpacity(opacity);
		GetActorPointer(m_ActorCollection, index)->GetProperty()->SetColor(r,g,b);	
		return 0;
	}
	else
		return 1;
}
int vtkTracking3D::SetColor(int index, double* rgb)
{
	if (m_ActorCollection->GetNumberOfItems() > 0 && m_ActorCollection->GetNumberOfItems() >= index)
	{
		//static_cast<vtkActor*>(m_ActorCollection->GetItemAsObject(index))->GetProperty()->SetOpacity(opacity);
		GetActorPointer(m_ActorCollection, index)->GetProperty()->SetColor(rgb);
		return 0;
	}
	else
		return 1;
}

/*
Description:
	Private function
	Apply certain transform to moving actor
*/
int vtkTracking3D::SetTransform(int index, QIN_Transform_Type* trans)
{

	if (m_ActorCollection->GetNumberOfItems() > 0 && m_ActorCollection->GetNumberOfItems() > index)
	{
		// construct transform
		double* temp = m_Transform->TransformPoint(trans->x, trans->y, trans->z);
		//double* temp = m_LandMarkTransform->TransformVector(trans->x, trans->y, trans->z);
		
		// put out
		m_marker_tobe_set[0] = temp[0];
		m_marker_tobe_set[1] = temp[1];
		m_marker_tobe_set[2] = temp[2];

		//move actor here
		GetActorPointer(m_ActorCollection, index)->SetPosition(temp[0], temp[1], temp[2]);
		GetActorPointer(m_ActorCollection, index)->SetOrientation(trans->qx, trans->qy, trans->qz);

		//this emit a signal to connect qt signal, to reslice 2D views
		this->InvokeEvent(QIN_S_VTK_EVENT, this);
		return 0;
	}
	else
		return 1;

}

/*
Description:
	Setup registration matrix, transforming abslote coordinate
	This function is badly need.
*/
int vtkTracking3D::SetRegisterTransform(vtkMatrix4x4* in)
{
	m_Transform->SetMatrix(in);
	m_Transform->PreMultiply();
	return 0;
}/*


/*
Description:
	Remove actor by index
Input:
	Index count from 0
*/
int vtkTracking3D::RemoveObject(int index)
{
	m_CurrentRenderer->RemoveActor(GetActorPointer(m_ActorCollection,index));
	m_ActorCollection->RemoveItem(index);
	return 0;
}
int vtkTracking3D::RemoveObject(vtkActor* obj)
{
	m_ActorCollection->RemoveItem(obj);
	m_CurrentRenderer->RemoveActor(obj);
	return 0;
}
void vtkTracking3D::SetInteractor(vtkSmartPointer<vtkRenderWindowInteractor> inct)
{
	this->m_Interactor = inct;
}
//Note: when you assign a new renderwindow, the interactor
//      and actor collection or other property will be reset
//      to the new window
void vtkTracking3D::SetWindow(vtkSmartPointer<vtkRenderWindow> win)
{
	m_RenderWindow = win;
}

vtkRenderer* vtkTracking3D::GetDefaultRenderer()
{
	return m_CurrentRenderer;
}

vtkSmartPointer<vtkRenderWindow> vtkTracking3D::GetRenderWindow()
{
	return m_RenderWindow;
}
vtkSmartPointer<vtkRenderWindowInteractor> vtkTracking3D::GetInteractor()
{
	return m_Interactor;
}
void vtkTracking3D::SetInteractorStyle( vtkInteractorStyle* style)
{
	m_InteractorStyle = style;
	m_Interactor->SetInteractorStyle(style);
}
/*
Description:
	Setup tracker device, check NULL point first
Note:
	In class constructor, the m_tracker is set to NULL
	to ensure safety.
*/
void vtkTracking3D::SetTracker(TrackerType* in) 
{ 
	if (m_tracker != NULL)
	{
		delete m_tracker;
	}
	m_tracker = in; 
};




/*
Description:
	These overload functions' function is get the reslice index
	in picked point of 3d view.
	Mainly for timer callback return. (connect qt)
Note:
	If you pass an pointer in, you must allocate memory!
Parameter:
	int[3]
*/
void vtkTracking3D::GetResliceIndex(int* in)
{
	in[0] = m_SliceX;
	in[1] = m_SliceY;
	in[2] = m_SliceZ;
}


// install / reinstall the visiualizaiton pipeline here
// must be done if any of the visi-pipline is modified
int vtkTracking3D::InstallPipeline()
{
	m_RenderWindow->AddRenderer(m_CurrentRenderer);
	m_RenderWindow->SetInteractor(m_Interactor);
	//m_Interactor->SetInteractorStyle(m_InteractorStyle);
	m_Interactor->Initialize();  //this line is very important to start timer

	//register callbacks
	m_KeyPressCallBack->SetClientData(this);
	m_KeyPressCallBack->SetCallback(KeypressCallbackFunction);
	m_TimerCallBack->SetClientData(this);
	m_TimerCallBack->SetCallback(TimerCallbackFunction);
	m_MouseCallback->SetClientData(this);
	m_MouseCallback->SetCallback(MouseclickCallbackFunction);
	m_Interactor->AddObserver(vtkCommand::KeyPressEvent, m_KeyPressCallBack);
	m_Interactor->AddObserver(vtkCommand::TimerEvent, m_TimerCallBack);

	m_PlaneX->SetInteractor(m_Interactor);
	m_PlaneY->SetInteractor(m_Interactor);
	m_PlaneZ->SetInteractor(m_Interactor);

	m_CurrentRenderer->ResetCamera();

	return 1;
}
/*
Description: 
	This function refresh the view
*/
void vtkTracking3D::RefreshView()
{
	m_CurrentRenderer->ResetCamera();
	m_RenderWindow->Render();
}

/*
Description:
	These 2 function are used to enable mouse picker to get
	the coordinate of the point in 3d view.
	Default is disabled.

*/
void vtkTracking3D::EnablePick()
{
	m_Interactor->AddObserver(vtkCommand::LeftButtonPressEvent, m_MouseCallback);
}
void vtkTracking3D::DisablePick()
{
	m_Interactor->RemoveObserver(m_MouseCallback);
}

/*
Description:
	Tracking start working, means that you have already done registration
	This function is different from tracker device start tracking function,
	tracker start tracking means tracker begin working, it can get points.
*/
void vtkTracking3D::StartTracking()
{
	m_TimerID = m_Interactor->CreateRepeatingTimer(m_interval);
}

/*
Description:
	When this function is called, the vtk timer is stopped, 
	while the tracking is still in tracking mode,
	Just call StartTracking function to re-run the timer
*/
void vtkTracking3D::StopTracking()
{
	m_Interactor->DestroyTimer(m_TimerID);
}

void KeypressCallbackFunction(vtkObject* caller,long unsigned int eventId,void* clientData,void* callData)
{
	vtkSmartPointer<vtkTracking3D> tracking = static_cast<vtkTracking3D*> (clientData);
	
	vtkRenderWindowInteractor *iren =
		static_cast<vtkRenderWindowInteractor*>(caller);

	std::string key = iren->GetKeySym();
	
	/*
	//add actor
	if (key == "a")
	{
		tracking->ValidMarker();
	}
	//remove actor
	else if (key == "b")
	{
		tracking->StopTracking();
	}
	//change color
	else if (key == "c")
	{
		tracking->SetColor(1,0.5,0.6,0.8);
	}
	//transform actor
	else if (key == "d")
	{

	}
	//chagne opacity
	else if (key == "x")
	{
		tracking->SetOpacity(1,0.3);
	}
	tracking->GetRenderWindow()->Render();
	*/
}

/*
Description:
	This function contain some key technologies
	Show more attentions
*/
static void MouseclickCallbackFunction(
	vtkObject* caller,
	long unsigned int eventId,
	void* clientData,
	void* callData
	)
{
	vtkSmartPointer<vtkTracking3D> tracking = static_cast<vtkTracking3D*> (clientData);

	// Get the screen/window pixel coordinate
	int* clickPos = tracking->GetInteractor()->GetEventPosition();

	// Pick from this location
	vtkSmartPointer<vtkPropPicker> picker = vtkSmartPointer<vtkPropPicker>::New();
	picker->Pick(clickPos[0], clickPos[1],0, tracking->GetDefaultRenderer());

	double* pos = picker->GetPickPosition();

	std::cout << "Position is: " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
	
	tracking->m_marker_tobe_set[0] = pos[0];
	tracking->m_marker_tobe_set[1] = pos[1];
	tracking->m_marker_tobe_set[2] = pos[2];

	tracking->m_PlaneX->SetSlicePosition(pos[0]);
	tracking->m_PlaneY->SetSlicePosition(pos[1]);
	tracking->m_PlaneZ->SetSlicePosition(pos[2]);

	tracking->m_SliceZ = tracking->m_PlaneZ->GetSliceIndex();
	tracking->m_SliceY = tracking->m_PlaneY->GetSliceIndex();
	tracking->m_SliceX = tracking->m_PlaneX->GetSliceIndex();

	//this emit a signal to connect qt signal
	tracking->InvokeEvent(QIN_S_VTK_EVENT, tracking);
}


/*
Description:
	Timer callback
	When timer triggered, then new tool position is assigned.
	And you will see tool moving with your tracking device.
*/
void TimerCallbackFunction(
	vtkObject* caller,
	long unsigned int eventId,
	void* clientData,
	void* callData
	)
{
	vtkSmartPointer<vtkTracking3D> tracking = static_cast<vtkTracking3D*> (clientData);
	
	for (auto it = tracking->m_Obj_Tool_Map.begin(); it != tracking->m_Obj_Tool_Map.end(); ++it)
	{
		//QIN_Transform_Type trans;
		QIN_Transform_Type* temp;
		//memset(&trans, 0, sizeof(QIN_Transform_Type));
		temp = tracking->m_tracker->GetTransform(it->second);
		if (temp != NULL)
		{
			//memcpy(&trans, temp, sizeof(QIN_Transform_Type));
			tracking->SetTransform(it->first, temp);
			std::cout <<it->first<<":"<< temp->x<< " " << temp->y<< " " << temp->z<< std::endl;
		}
		else
		{
			std::cout << "transform invalid" << std::endl;
		}
	}

	/*						   
	Calculate index here!!!!!  
	Not implemented			   
							   
	Manipulate m_index_tobe_set
	*/

	tracking->GetRenderWindow()->Render();
}
