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
	m_interval = 100; //ms
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

	// reference frame transform
	m_ToolId = 0;
	m_RefID = 1;
	m_RefTransform = vtkSmartPointer<vtkTransform>::New();
	m_RefTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

	// Raw Transform from tracker device
	m_RawTransform = vtkSmartPointer<vtkTransform>::New();
	m_RawTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	
	// Registration matrix/transform
	m_RegisterTransform = vtkSmartPointer<vtkTransform>::New();;
	m_RegisterTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();;

	// Final transform after calibration and registration
	m_FinTransform = vtkSmartPointer<vtkTransform>::New();
	m_FinTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	//m_FinTransform->PostMultiply(); //this is the key line

	// Tool Calibration Matrix
	m_ToolTipCalibrationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

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
	m_CurrentRenderer->AddActor(act);
	m_ActorCollection = m_CurrentRenderer->GetActors();
	return 0;
}

int vtkTracking3D::AddPolySource(vtkSmartPointer<vtkPolyData> poly)
{
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->ScalarVisibilityOff();
#if VTK_MAJOR_VERSION <= 5	mapper->SetInput(poly);
#else
	mapper->SetInputData(poly);
#endif
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
	obj_index:    actor index
	tool_index:   tracking tool index

Return:
	0: success
	1: fail, actor index out of range
	2: fail, tool index out of range
Note: 
	Index count from 0
*/
int vtkTracking3D::ConnectObjectTracker(int obj_index, int tool_index)
{
	int return_value = 0;
	if (obj_index >= m_ActorCollection->GetNumberOfItems())
	{
		std::cout << "actor index out of range" << std::endl;
		return_value =  1;
	}
	if (m_tracker->GetToolValidation(tool_index) != 0)
	{
		std::cout << "tool index out of range" << std::endl;
		return_value =  2;
	}
	m_Obj_Tool_Map[obj_index] = tool_index;
	return return_value;
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

int vtkTracking3D::GetNumberOfActors()
{
	return m_CurrentRenderer->GetActors()->GetNumberOfItems();
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

vtkActor* vtkTracking3D::GetActorPointer(int index)
{
	if (m_ActorCollection->GetNumberOfItems() > 0 && m_ActorCollection->GetNumberOfItems() >= index)
		return GetActorPointer(m_ActorCollection, index);
	else
		return vtkSmartPointer<vtkActor>::New();
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
int vtkTracking3D::ValidMarker(double* out)
{
	double* mar;
	mar = new double[3];
	memcpy(mar, m_marker_tobe_set, 3 * sizeof(double));
	memcpy(mar, out, 3 * sizeof(double));
	m_marker_list.push_back(mar);

	std::cout << "The " << m_marker_list.size() << " marker is set" << std::endl;
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

/*
1: success
2: fail
*/
int vtkTracking3D::SetMarkerList(std::vector<double*> in)
{
	m_marker_list = in;
	return 1;
}

std::vector<double*> vtkTracking3D::GetMarkerList()
{
	std::cout << "Marker list: " << std::endl;
	for (size_t i = 0; i < m_marker_list.size(); i++)
	{
		std::cout << m_marker_list[i][0] << "," << m_marker_list[i][1] << "," << m_marker_list[i][2] << std::endl;
	}
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

#if VTK_MAJOR_VERSION <= 5	m_PlaneX->SetInput(m_Image);
	m_PlaneY->SetInput(m_Image);
	m_PlaneZ->SetInput(m_Image);
#else
	m_PlaneX->SetInputData(m_Image);
	m_PlaneY->SetInputData(m_Image);
	m_PlaneZ->SetInputData(m_Image);
#endif
	m_PlaneX->SetPlaneOrientationToXAxes();
	m_PlaneY->SetPlaneOrientationToYAxes();
	m_PlaneZ->SetPlaneOrientationToZAxes();

	return 0;
}


/*
Description:
	Set the background color
*/
int  vtkTracking3D::SetBackGround(double r, double g, double b)
{
	m_CurrentRenderer->SetBackground(r,g,b);
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
	m_ActorCollection = m_CurrentRenderer->GetActors();
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
	m_ActorCollection = m_CurrentRenderer->GetActors();
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

Input:
	index:	index of actors, count from 0
*/
int vtkTracking3D::SetTransform(int index, QIN_Transform_Type* trans)
{
	if (trans == NULL) // null ptr
		return 2;

	// convert trans to vtkTransform and vtkMatrix first
	PivotCalibration2::TransformToMatrix(trans,m_RawTransformMatrix);
	m_RawTransform->Identity();
	m_RawTransform->SetMatrix(m_RawTransformMatrix);
	

#pragma region Core_ALGORITHM
	auto invert_refer = vtkSmartPointer<vtkMatrix4x4>::New();
	invert_refer->Identity();
	vtkMatrix4x4::Invert(m_RefTransformMatrix, invert_refer);

	auto refer_raw = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Multiply4x4(invert_refer,m_RawTransformMatrix, refer_raw);

	auto calibrate_raw = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Multiply4x4(refer_raw,m_ToolTipCalibrationMatrix,calibrate_raw);
	//vtkMatrix4x4::Multiply4x4(m_RawTransformMatrix, m_ToolTipCalibrationMatrix, calibrate_raw); // disable reference test

	//m_FinTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Multiply4x4(m_RegisterTransformMatrix,calibrate_raw,m_FinTransformMatrix);

	m_FinTransform->SetMatrix(m_FinTransformMatrix);
	//m_FinTransformMatrix = m_RegisterTransform->GetMatrix();
#pragma endregion Core_ALGORITHM



	// put out
	m_marker_tobe_set[0] = m_FinTransform->GetPosition()[0];
	m_marker_tobe_set[1] = m_FinTransform->GetPosition()[1];
	m_marker_tobe_set[2] = m_FinTransform->GetPosition()[2];

	//this->m_PlaneX->SetSlicePosition(m_marker_tobe_set[0]);
	//this->m_PlaneY->SetSlicePosition(m_marker_tobe_set[1]);
	//this->m_PlaneZ->SetSlicePosition(m_marker_tobe_set[2]);

	m_ActorCollection = m_CurrentRenderer->GetActors();
	if (m_ActorCollection->GetNumberOfItems() > 0 && m_ActorCollection->GetNumberOfItems() > index)
	{
		//move actor here
		GetActorPointer(m_ActorCollection, index)->SetUserTransform(m_FinTransform);
		//this emit a signal to connect qt signal, to reslice 2D views
		this->InvokeEvent(QIN_S_VTK_EVENT, this);
		return 0;
	}
	else
		return 1;

}
int  vtkTracking3D::SetTransform(int index, vtkMatrix4x4* ma)
{
	m_ActorCollection = m_CurrentRenderer->GetActors();
	if (m_ActorCollection->GetNumberOfItems() > 0 && m_ActorCollection->GetNumberOfItems() > index)
	{
		//move actor here
		GetActorPointer(m_ActorCollection, index)->SetUserMatrix(ma);
		this->m_RenderWindow->Render();
		return 0;
	}
	else
		return 1;
}
/*
Description:
	Set Object Position
*/
int  vtkTracking3D::SetPosition(int index, double* coor)
{
	m_ActorCollection = m_CurrentRenderer->GetActors();
	if (m_ActorCollection->GetNumberOfItems() > 0 && m_ActorCollection->GetNumberOfItems() > index)
	{
		//move actor here
		GetActorPointer(m_ActorCollection, index)->SetPosition(coor);
		return 0;
	}
	else
		return 1;
}
int  vtkTracking3D::SetPosition(int index, double x, double y, double z)
{
	m_ActorCollection = m_CurrentRenderer->GetActors();
	if (m_ActorCollection->GetNumberOfItems() > 0 && m_ActorCollection->GetNumberOfItems() > index)
	{
		//move actor here
		GetActorPointer(m_ActorCollection, index)->SetPosition(x,y,z);
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
	m_RegisterTransform = vtkSmartPointer<vtkTransform>::New();
	m_RegisterTransform->SetMatrix(in);

	m_RegisterTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_RegisterTransformMatrix->DeepCopy(in);

	//m_Transform->SetMatrix(in);
	//m_Transform->PreMultiply();
	return 0;
}

/*
Description:
	Set the index of the reference sensor
	Count from 0;
*/
int vtkTracking3D::SetReferenceIndex(int id)
{
	m_RefID = id;
	return 0;
}

/*
Description:
	Remove actor by index
Input:
	Index count from 0
*/
int vtkTracking3D::RemoveObject(int index)
{
	m_CurrentRenderer->RemoveActor(GetActorPointer(m_ActorCollection,index));
	return 0;
}
int vtkTracking3D::RemoveObject(vtkActor* obj)
{
	m_CurrentRenderer->RemoveActor(obj);
	return 0;
}
int vtkTracking3D::PopObject()
{
	int total = m_CurrentRenderer->GetActors()->GetNumberOfItems();
	int index = total - 1;
	m_CurrentRenderer->RemoveActor(GetActorPointer(m_ActorCollection, index));
	return 0;
}


/*
Description:
	Insert actor by index or pointer
Input:
	Index, count from 0
Note:
	Do nothing if index is invalid
*/
int vtkTracking3D::ReplaceObject(int index, vtkSmartPointer< vtkActor > obj)
{
	m_CurrentRenderer->GetViewProps()->ReplaceItem(index,obj);
	return 0;
}

int vtkTracking3D::ReplacePolySource(int index, vtkSmartPointer<vtkPolyData> poly)
{

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->ScalarVisibilityOff();
#if VTK_MAJOR_VERSION <= 5	mapper->SetInput(poly);
#else
	mapper->SetInputData(poly);
#endif
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	this->ReplaceObject(index, actor);
	return 1;
}


void vtkTracking3D::SetInteractor(vtkSmartPointer<vtkRenderWindowInteractor> inct)
{
	this->m_Interactor = inct;
}
//Note: when you assign a new renderwindow, the interactor
//      and actor collection or other property will be reset
//      to the new window
void vtkTracking3D::SetWindow(vtkRenderWindow* win)
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
	Get output of registration transform (vtkTransform)
	Final transform after registration and calibration process
*/
vtkSmartPointer<vtkMatrix4x4> vtkTracking3D::GetRegisteredTransformMatrix()
{
	this->SetTransform(100, GetTransform(0));  // refresh, get the latest position and orientation matrix
	return m_FinTransformMatrix;
}
vtkSmartPointer<vtkTransform> vtkTracking3D:: GetRegisteredTransform()
{
	this->SetTransform(100, GetTransform(0)); // refresh
	return m_FinTransform;
}

/*
Get out the transform you set in..
*/
vtkSmartPointer<vtkTransform> vtkTracking3D::GetRegisterTransform()
{
	return m_RegisterTransform;
}
vtkSmartPointer<vtkMatrix4x4> vtkTracking3D::GetRegisterTransformMatrix()
{
	return m_RegisterTransformMatrix;
}

QIN_Transform_Type* vtkTracking3D::GetTransform(int id)
{
	QIN_Transform_Type* temp = m_tracker->GetTransform(m_RefID);
	if (temp == NULL)
	{
		temp = new QIN_Transform_Type;
		memset(temp, 0, sizeof(QIN_Transform_Type));
	}
	
	PivotCalibration2::TransformToMatrix(temp, m_RefTransformMatrix);
	m_RefTransform->Identity();
	m_RefTransform->SetMatrix(m_RefTransformMatrix);
	
	return this->m_tracker->GetTransform(id);
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
	m_tracker = in; 
};

/*
Description:
	Set up tool tip to tool calibration matrix	
*/
void vtkTracking3D::SetToolTipCalibrationMatrix(vtkMatrix4x4* matrix)
{
	m_ToolTipCalibrationMatrix->DeepCopy( matrix );
}


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
	auto pointPicker = vtkSmartPointer<vtkPointPicker>::New();
	auto propPicker = vtkSmartPointer<vtkPropPicker>::New();
	auto cellPicker = vtkSmartPointer<vtkCellPicker>::New();
	//m_Interactor->SetPicker(pointPicker);
	m_Interactor->SetPicker(propPicker);
	//m_Interactor->SetPicker(cellPicker);

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
void vtkTracking3D::ResetView()
{
	m_CurrentRenderer->ResetCamera();
	m_RenderWindow->Render();
}

void vtkTracking3D::RefreshView()
{
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
	m_TimerCallBack->SetClientData(this);
	m_TimerCallBack->SetCallback(TimerCallbackFunction);
	m_Interactor->AddObserver(vtkCommand::TimerEvent, m_TimerCallBack);
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
	m_Interactor->RemoveAllObservers();
}


/*
keypress functions, for test purpose
*/
void KeypressCallbackFunction(vtkObject* caller,long unsigned int eventId,void* clientData,void* callData)
{
	vtkSmartPointer<vtkTracking3D> tracking = static_cast<vtkTracking3D*> (clientData);
	
	vtkRenderWindowInteractor *iren =
		static_cast<vtkRenderWindowInteractor*>(caller);

	std::string key = iren->GetKeySym();
	
}



/*
Description:
	Timer callback
	When timer triggered, then new tool position is assigned.
	And you will see tool moving with your tracking device.

Note:
	This function will refresh the vtk view
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
		temp = tracking->GetTransform(it->second);//m_tracker->
		if (temp != NULL)
		{
			//memcpy(&trans, temp, sizeof(QIN_Transform_Type));
			tracking->SetTransform(it->first, temp);
		}
		else
		{
			std::cout << "transform invalid" << std::endl;
		}
	}
	tracking->InvokeEvent(QIN_S_VTK_EVENT, tracking);
	tracking->GetRenderWindow()->Render();
}


/*
Description:
	Timer callback

	you should add index: m_Obj_Tool_Map

	When timer triggered, then new tool position is assigned.
	And you will see tool moving with your tracking device.

Note:
	This function will not refresh the view, used to export
*/
void TimerCallbackFunction2(
	vtkObject* caller,
	long unsigned int eventId,
	void* clientData,
	void* callData
	)
{
	std::cout << "callback function2 called" << std::endl;
	vtkSmartPointer<vtkTracking3D> tracking = static_cast<vtkTracking3D*> (clientData);

	for (auto it = tracking->m_Obj_Tool_Map.begin(); it != tracking->m_Obj_Tool_Map.end(); ++it)
	{
		//QIN_Transform_Type trans;
		QIN_Transform_Type* temp;
		//memset(&trans, 0, sizeof(QIN_Transform_Type));
		temp = tracking->m_tracker->GetTransform(it->second);
		if (temp != NULL)
		{
			// construct transform
			double* coor = tracking->GetRegisteredTransform()->TransformPoint(temp->x, temp->y, temp->z);
			//double* temp = m_LandMarkTransform->TransformVector(trans->x, trans->y, trans->z);

			// put out
			tracking->m_marker_tobe_set[0] = coor[0];
			tracking->m_marker_tobe_set[1] = coor[1];
			tracking->m_marker_tobe_set[2] = coor[2];
			//this emit a signal to connect qt signal, to reslice 2D views
			tracking->InvokeEvent(QIN_S_VTK_EVENT, tracking);
			//std::cout << it->first << ":" << temp->x << " " << temp->y << " " << temp->z << std::endl;
		}
		else
		{
			std::cout << "transform invalid" << std::endl;
		}
	}

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

	//vtkSmartPointer<vtkPointPicker> pointPicker = static_cast<vtkPointPicker* >( tracking->GetInteractor()->GetPicker());
	//pointPicker->Pick(clickPos[0],clickPos[1],0,tracking->GetDefaultRenderer());
	//pointPicker->SetTolerance(0.01);

	vtkSmartPointer<vtkPropPicker> propPicker = static_cast<vtkPropPicker* >(tracking->GetInteractor()->GetPicker());
	propPicker->Pick(clickPos[0], clickPos[1], 0, tracking->GetDefaultRenderer());
	auto points = propPicker->GetPickPosition();

	//vtkSmartPointer<vtkCellPicker> cellPicker = static_cast<vtkCellPicker* >(tracking->GetInteractor()->GetPicker());
	//cellPicker->SetTolerance(0.01);
	//cellPicker->Pick(clickPos[0], clickPos[1], 0, tracking->GetDefaultRenderer());
	//auto points = cellPicker->GetPickPosition();

	std::cout << "Position is: " << points[0] << " " << points[1] << " " << points[2] << std::endl;
	memcpy(tracking->m_marker_tobe_set, points, 3 * sizeof(double));


	tracking->m_PlaneX->SetSlicePosition(points[0]);
	tracking->m_PlaneY->SetSlicePosition(points[1]);
	tracking->m_PlaneZ->SetSlicePosition(points[2]);

	tracking->m_SliceZ = tracking->m_PlaneZ->GetSliceIndex();
	tracking->m_SliceY = tracking->m_PlaneY->GetSliceIndex();
	tracking->m_SliceX = tracking->m_PlaneX->GetSliceIndex();


	//this emit a signal to connect qt signal
	tracking->InvokeEvent(QIN_S_VTK_EVENT , tracking);
}




/*
Description:
	1. A function to calculate the index of a point in world coordinate.
	2. A static function that can be called anywhere.
	3. You may not input a vtkImageData pointer into this class and get
		the 3d index you need.
	4. Make sure that your img is valid.
	5. Output array must be allocated before passed in.
Input:
	img:        vtkImageData pointer
	coordinate: real world coordinate
	out:        3d index (allocated int array)
*/
void vtkTracking3D::Find3DIndex(vtkImageData* img, double* coordinate, int* out)
{
	int pt_ID = 0;
	pt_ID = img->FindPoint(coordinate[0], coordinate[1], coordinate[2]);
	//std::cout << "Point ID is: " << pt_ID << std::endl;
	int extent[6];
	img->GetExtent(extent);
	int x_e = extent[1] - extent[0] + 1;
	int y_e = extent[3] - extent[2] + 1;
	int z_e = extent[5] - extent[4] + 1;
	out[2] = floor(pt_ID / (x_e*y_e));
	out[1] = floor(pt_ID % (x_e*y_e) / x_e);
	out[0] = pt_ID%x_e;
}
void vtkTracking3D::Find3DIndex(vtkImageData* img, double x, double y, double z, int * out)
{
	int pt_ID = 0;
	pt_ID = img->FindPoint(x, y, z);
	//std::cout << "Point ID is: " << pt_ID << std::endl;
	int extent[6];
	img->GetExtent(extent);
	int x_e = extent[1] - extent[0] + 1;
	int y_e = extent[3] - extent[2] + 1;
	int z_e = extent[5] - extent[4] + 1;
	out[2] = floor(pt_ID / (x_e*y_e));
	out[1] = floor(pt_ID % (x_e*y_e) / x_e);
	out[0] = pt_ID%x_e;
}

void vtkTracking3D::Find3DIndex(double x, double y, double z, int * out)
{
	Find3DIndex(m_Image,x,y,z,out);
}
void vtkTracking3D::Find3DIndex(double* in, int * out)
{
	Find3DIndex(m_Image,in,out);
}

void vtkTracking3D::Find3DIndex(double x,double y,double z)
{
	int pt_ID = 0;
	pt_ID = m_Image->FindPoint(x, y, z);
	//std::cout << "Point ID is: " << pt_ID << std::endl;
	int extent[6];
	m_Image->GetExtent(extent);
	int x_e = extent[1] - extent[0] + 1;
	int y_e = extent[3] - extent[2] + 1;
	int z_e = extent[5] - extent[4] + 1;
	m_SliceX = floor(pt_ID / (x_e*y_e));
	m_SliceY = floor(pt_ID % (x_e*y_e) / x_e);
	m_SliceZ = pt_ID%x_e;
	std::cout << "xxxxxxxxxxxxxxxxxxxxxxxx" << std::endl;
}
