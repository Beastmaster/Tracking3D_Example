/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/1/11

*/

#include "ResliceView.h"


reslice_view_base::reslice_view_base(vtkRenderWindow* winx,char a)
{
	// init para
	this->Set_Window(winx);
	this->slice_n = 0;
#if VTK_MAJOR_VERSION <= 5
#else
	extent_m = NULL;
#endif
	this->dimensions = NULL;
	this->img_to_view = NULL;
	this->img_to_mask = NULL;
	this->mask_actor  = NULL;
	this->new_render  = NULL;
	this->Interactor      = NULL;
	this->InteractorStyle = NULL;

	//map vtkaction and qt signal
	m_Connections_mouse_back = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	m_Connections_mouse_back->Connect(this->view_window->GetInteractor(),
		vtkCommand::MouseWheelBackwardEvent,this,SLOT(on_scroll_mouse_back(vtkObject*)));

	m_Connections_mouse_forward = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	m_Connections_mouse_forward->Connect(this->view_window->GetInteractor(),
		vtkCommand::MouseWheelForwardEvent,this,SLOT(on_scroll_mouse_forward(vtkObject*)));

	m_Connections_mouse_lft_click = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	m_Connections_mouse_lft_click->Connect(this->view_window->GetInteractor(),
		vtkCommand::LeftButtonPressEvent, this, SLOT(on_click_mouse_lft(vtkObject*)));

	//renderer init
	new_render	 = vtkSmartPointer<vtkRenderer>::New();
	actor		 = vtkSmartPointer<vtkImageActor>::New();
	mask_actor   = vtkSmartPointer<vtkImageActor>::New();
	WindowLevel1 = vtkSmartPointer<vtkImageMapToWindowLevelColors>::New();
	WindowLevel2 = vtkSmartPointer<vtkImageMapToWindowLevelColors>::New();

	this->InteractorStyle = vtkSmartPointer<reslice_interactor_style>::New();
	this->Interactor      = vtkSmartPointer<vtkRenderWindowInteractor>::New();

	//this->view_window->AddRenderer(this->new_render);
	//new_render->AddActor(this->actor);
	////set default interact null
	vtkSmartPointer<reslice_interactor_style> new_act_style = 
		vtkSmartPointer<reslice_interactor_style>::New();
	this->view_window->GetInteractor()->SetInteractorStyle(new_act_style);
	this->InstallPipeline();
	this->Set_Direction(a); // set direction here
	this->actor->SetOpacity(1);
	this->mask_actor->SetOpacity(0.5);


	/*
	//init slider
	m_Slider = vtkSmartPointer<vtkSliderRepresentation2D>::New();
	m_Slider->SetMinimumValue(0.0);
	m_Slider->SetMaximumValue(50.0);
	m_Slider->SetValue(25.0);
	m_Slider->SetTitleText("Slice");
	// Set color properties:
	m_Slider->GetSliderProperty()->SetColor(1, 0, 0);// Change the color of the knob that slides//red
	m_Slider->GetTitleProperty()->SetColor(1, 0, 0);// Change the color of the text indicating what the slider controls//red
	m_Slider->GetLabelProperty()->SetColor(1, 0, 0);// Change the color of the text displaying the value//red
	m_Slider->GetSelectedProperty()->SetColor(0, 1, 0);// Change the color of the knob when the mouse is held on it//green
	m_Slider->GetTubeProperty()->SetColor(1, 1, 0);// Change the color of the bar//yellow
	m_Slider->GetCapProperty()->SetColor(1, 1, 0);// Change the color of the ends of the bar//yellow
	//set position of slider bar
	m_Slider->GetPoint1Coordinate()->SetCoordinateSystemToDisplay();
	m_Slider->GetPoint1Coordinate()->SetValue(40, 40);
	m_Slider->GetPoint2Coordinate()->SetCoordinateSystemToDisplay();
	m_Slider->GetPoint2Coordinate()->SetValue(100, 40);

	vtkSmartPointer<vtkSliderWidget> sliderWidget =
		vtkSmartPointer<vtkSliderWidget>::New();
	sliderWidget->SetInteractor(this->view_window->GetInteractor());
	sliderWidget->SetRepresentation(m_Slider);
	sliderWidget->SetAnimationModeToAnimate();
	sliderWidget->EnabledOn();
	//sliderWidget->
	//sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);*/
}

reslice_view_base::~reslice_view_base()
{
	m_Connections_mouse_back->Disconnect(this->view_window->GetInteractor(),
		vtkCommand::MouseWheelBackwardEvent,this,SLOT(on_scroll_mouse_back(vtkObject*)));
	m_Connections_mouse_forward->Disconnect(this->view_window->GetInteractor(),
		vtkCommand::MouseWheelForwardEvent,this,SLOT(on_scroll_mouse_forward(vtkObject*)));
}
void reslice_view_base::InstallPipeline()
{
	if (this->view_window && this->new_render)
	{
		this->view_window->AddRenderer(this->new_render);
	}

	if (this->new_render && this->actor)
	{
		this->new_render->AddViewProp(this->actor);
	}

	if (this->actor && this->WindowLevel1)
	{
#if VTK_MAJOR_VERSION <= 5
		this->actor->SetInput(this->WindowLevel1->GetOutput());
#else
		this->actor->SetInputData(this->WindowLevel1->GetOutput());
#endif
	}
}

/*
Locate the position
*/
void reslice_view_base::SetSlice(int x)
{ 
	this->slice_n = x; 

	//std::cout<<"direct  "<<this->direction<<"  call mouse scroll  "<<this->slice_n<<std::endl;
	if (extent_m == NULL)
	{
		return;
	}
	switch (this->direction)
	{
	case 'a':
	{
		//this->slice_n++;
		if (this->slice_n>this->extent_m[5])
		{
			this->slice_n = this->extent_m[4];
		}
		else if (this->slice_n<extent_m[4])
		{
			this->slice_n = this->extent_m[5];
		}
		center[2] = origin[2] + spacing[2] * this->slice_n;
		//center[2] = spacing[2]*this->slice_n;
		break;
	}
	case 'c':
	{
		//this->slice_n++;
		if (this->slice_n>this->extent_m[3])
		{
			this->slice_n = this->extent_m[2];
		}
		else if (this->slice_n<extent_m[2])
		{
			this->slice_n = this->extent_m[3];
		}
		center[1] = origin[1] + spacing[1] * this->slice_n;
		//center[1] = spacing[1]*this->slice_n;
		break;
	}
	case 's':
	{
		//this->slice_n++;
		if (this->slice_n>this->extent_m[1])
		{
			this->slice_n = this->extent_m[0];
		}
		else if (this->slice_n<extent_m[0])
		{
			this->slice_n = this->extent_m[1];
		}
		center[0] = origin[0] + spacing[0] * this->slice_n;
		//center[0] = spacing[0]*this->slice_n;
		break;
	}
	default:
	{
		//this->slice_n++;
		if (this->slice_n>this->extent_m[1])
		{
			this->slice_n = this->extent_m[0];
		}
		else if (this->slice_n<extent_m[0])
		{
			this->slice_n = this->extent_m[1];
		}
		center[0] = origin[0] + spacing[0] * this->slice_n;
		//center[0] = spacing[0]*this->slice_n;
		break;
	}
	}
	this->RenderView();
}

void reslice_view_base::Set_View_Img(vtkSmartPointer<vtkImageData> img)
{
	this->img_to_view = vtkSmartPointer<vtkImageData>::New();
	this->img_to_view = img;
	this->dimensions = new int[3];
	this->img_to_view->GetDimensions(this->dimensions);
	this->slice_n = int(dimensions[0]/2);
	//std::cout<<"dimension is :"<<dimensions[0]<<dimensions[1]<<dimensions[2]<<std::endl;
	this->calculate_img_center(img_to_view);
	
	//get vtkimagedata range
	//min = vtkImageData->GetOutput()->GetScalarRange()[0] and max = GetScalarRange()[1].
	double* valuesRange;
	valuesRange = img_to_view->GetScalarRange();
	//vtkDataArray* scalar_pointer = this->img_to_view->GetPointData()->GetScalars();
	//scalar_pointer->GetRange(valuesRange,-1);
	std::cout<<"range is "<<valuesRange[0]<<"|"<<valuesRange[1]<<std::endl;
	//set window level
	this->WindowLevel1->SetWindow(valuesRange[1]-valuesRange[0]);
	this->WindowLevel1->SetLevel((valuesRange[1]+valuesRange[0])*1/2);

}

void reslice_view_base::Set_Mask_Img(vtkSmartPointer<vtkImageData> img)
{
	this->img_to_mask = vtkSmartPointer<vtkImageData>::New();
	this->img_to_mask = img;

	//get vtkimagedata range
	double valuesRange[2];
	vtkDataArray* scalar_pointer = this->img_to_mask->GetPointData()->GetScalars();
	scalar_pointer->GetRange(valuesRange,-1);
	//set window level
	this->WindowLevel2->SetWindow(valuesRange[1]-valuesRange[0]);
	this->WindowLevel2->SetLevel((valuesRange[1]+valuesRange[0])/2);
	//add an lookup table to windowlevel2
	auto lookUp = vtkSmartPointer<vtkLookupTable>::New();
	lookUp->SetNumberOfTableValues(int(valuesRange[1] - valuesRange[0]));
	lookUp->SetRange(0.0,255.0);
	lookUp->Build();
	WindowLevel2->SetLookupTable(lookUp);
}

void reslice_view_base::RemoveMask()
{
	if (this->img_to_mask == NULL)
	{
		return;
	}

	//uninsatall pipeline
	if (this->new_render && this->mask_actor)
	{
		this->new_render->RemoveViewProp(this->mask_actor);
	}

	if (this->mask_actor && this->WindowLevel2)
	{
		
#if VTK_MAJOR_VERSION <= 5
		this->mask_actor->SetInput(NULL);
#else
		this->mask_actor->SetInputData(NULL);
#endif
	}

	//delete this mask
	this->img_to_mask = NULL;
}

void reslice_view_base::Set_Window(vtkRenderWindow* win)
{
	this->view_window = vtkSmartPointer<vtkRenderWindow>::New();
	this->view_window = win;
}

void reslice_view_base::RenderView()
{

	this->reslice = vtkSmartPointer<vtkImageReslice>::New();
#if VTK_MAJOR_VERSION <= 5
	this->reslice->SetInput(this->img_to_view);
#else
	this->reslice->SetInputData(this->img_to_view);
#endif

	this->reslice->SetOutputDimensionality(2);
	this->reslice->SetResliceAxesDirectionCosines(this->view_dirX,this->view_dirY,this->view_dirZ);
	this->reslice->SetResliceAxesOrigin(center);
	this->reslice->SetInterpolationModeToLinear();
	//Attention  !!!!
	//Attention  !!!!
	//Attention  !!!!
	// You must Update here!!!
	this->reslice->Update();
#if VTK_MAJOR_VERSION <= 5
	this->WindowLevel1->SetInput(this->reslice->GetOutput());
#else
	//Attention  !!!!
	//Attention  !!!!
	//Attention  !!!!
	// You must Update here!!!
	this->WindowLevel1->SetInputData(this->reslice->GetOutput());
	this->WindowLevel1->Update();
#endif
	if (this->img_to_mask != NULL)
	{
		//install pipline here
		if (this->new_render && this->mask_actor)
		{
			this->new_render->AddViewProp(this->mask_actor);
		}
		if (this->mask_actor && this->WindowLevel2)
		{
			
#if VTK_MAJOR_VERSION <= 5
			this->mask_actor->SetInput(this->WindowLevel2->GetOutput());
#else
			this->mask_actor->SetInputData(this->WindowLevel2->GetOutput());
#endif
		}
		this->mask_reslice = vtkSmartPointer<vtkImageReslice>::New();
#if VTK_MAJOR_VERSION <= 5
		this->mask_reslice->SetInput(this->img_to_mask);
#else
		this->mask_reslice->SetInputData(this->img_to_mask);
#endif
		this->mask_reslice->SetOutputDimensionality(2);
		this->mask_reslice->SetResliceAxesDirectionCosines(this->view_dirX,this->view_dirY,this->view_dirZ);
		this->mask_reslice->SetResliceAxesOrigin(center);
		this->mask_reslice->SetInterpolationModeToLinear();
		//Attention  !!!!
		//Attention  !!!!
		//Attention  !!!!
		// You must Update here!!!
		this->mask_reslice->Update();
#if VTK_MAJOR_VERSION <= 5
		this->WindowLevel2->SetInput(this->mask_reslice->GetOutput());
#else
		this->WindowLevel2->SetInputData(this->mask_reslice->GetOutput());
		//Attention  !!!!
		//Attention  !!!!
		//Attention  !!!!
		// You must Update here!!!
		this->WindowLevel2->Update();
#endif
	}

	this->new_render->ResetCamera();
	this->view_window->Render();
}


//slots: when mouse wheel scroll back, next slice of image show
void reslice_view_base::on_scroll_mouse_back(vtkObject* obj)
{
	vtkSmartPointer<vtkRenderWindowInteractor> iren = 
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren = vtkRenderWindowInteractor::SafeDownCast(obj);

	//std::cout<<"direct  "<<this->direction<<"  call mouse scroll  "<<this->slice_n<<std::endl;
	if (extent_m == NULL)
	{
		return;
	}
	switch(this->direction)
	{
	case 'a':
		{
			this->slice_n++;
			if (this->slice_n>this->extent_m[5])
			{
				this->slice_n = this->extent_m[4];
			}
			else if (this->slice_n<extent_m[4])
			{
				this->slice_n = this->extent_m[5];
			}
			center[2] = origin[2]+spacing[2]*this->slice_n;
			//center[2] = spacing[2]*this->slice_n;
			break;
		}
	case 'c':
		{
			this->slice_n++;
			if (this->slice_n>this->extent_m[3])
			{
				this->slice_n = this->extent_m[2];
			}
			else if (this->slice_n<extent_m[2])
			{
				this->slice_n = this->extent_m[3];
			}
			center[1] = origin[1]+spacing[1]*this->slice_n;
			//center[1] = spacing[1]*this->slice_n;
			break;
		}
	case 's':
		{
			this->slice_n++;
			if (this->slice_n>this->extent_m[1])
			{
				this->slice_n = this->extent_m[0];
			}
			else if (this->slice_n<extent_m[0])
			{
				this->slice_n = this->extent_m[1];
			}
			center[0] = origin[0]+spacing[0]*this->slice_n;
			//center[0] = spacing[0]*this->slice_n;
			break;
		}
	default:
		{
			this->slice_n++;
			if (this->slice_n>this->extent_m[1])
			{
				this->slice_n = this->extent_m[0];
			}
			else if (this->slice_n<extent_m[0])
			{
				this->slice_n = this->extent_m[1];
			}
			center[0] = origin[0]+spacing[0]*this->slice_n;
			//center[0] = spacing[0]*this->slice_n;
			break;
		}
	}
	this->RenderView();
}
void reslice_view_base::on_scroll_mouse_forward(vtkObject* obj)
{
	vtkSmartPointer<vtkRenderWindowInteractor> iren = 
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren = vtkRenderWindowInteractor::SafeDownCast(obj);

	//std::cout<<"direct  "<<this->direction<<"  call mouse scroll  "<<this->slice_n<<std::endl;
	if (extent_m == NULL)
	{
		return;
	}
	switch(this->direction)
	{
	case 'a':
		{
			this->slice_n--;
			if (this->slice_n>this->extent_m[5])
			{
				this->slice_n = this->extent_m[4];
			}
			else if (this->slice_n<extent_m[4])
			{
				this->slice_n = this->extent_m[5];
			}
			center[2] = origin[2]+spacing[2]*this->slice_n;
			//center[2] = spacing[2]*this->slice_n;
			break;
		}
	case 'c':
		{
			this->slice_n--;
			if (this->slice_n>this->extent_m[3])
			{
				this->slice_n = this->extent_m[2];
			}
			else if (this->slice_n<extent_m[2])
			{
				this->slice_n = this->extent_m[3];
			}
			center[1] = origin[1]+spacing[1]*this->slice_n;
			//center[1] = spacing[1]*this->slice_n;
			break;
		}
	case 's':
		{
			this->slice_n--;
			if (this->slice_n>this->extent_m[1])
			{
				this->slice_n = this->extent_m[0];
			}
			else if (this->slice_n<extent_m[0])
			{
				this->slice_n = this->extent_m[1];
			}
			center[0] = origin[0]+spacing[0]*this->slice_n;
			//center[0] = spacing[0]*this->slice_n;
			break;
		}
	default:
		{
			this->slice_n--;
			if (this->slice_n>this->extent_m[1])
			{
				this->slice_n = this->extent_m[0];
			}
			else if (this->slice_n<extent_m[0])
			{
				this->slice_n = this->extent_m[1];
			}
			center[0] = origin[0]+spacing[0]*this->slice_n;
			break;
		}
	}
	this->RenderView();
}
void reslice_view_base:: on_click_mouse_lft(vtkObject* obj)
{
	vtkSmartPointer<vtkRenderWindowInteractor> iren = 
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	//get interactor first
	iren = vtkRenderWindowInteractor::SafeDownCast(obj);
	vtkSmartPointer<vtkInteractorStyle> style = 
		vtkSmartPointer<vtkInteractorStyle>::New();
	style = vtkInteractorStyle::SafeDownCast(iren->GetInteractorStyle());

	int eve_pos[2];
	iren->GetEventPosition(eve_pos);// here we pick the world coordinate which is not suitable, we should get pixel position

	//use vtkPropPicker 
	vtkSmartPointer<vtkPropPicker> propPicker = 
		vtkSmartPointer<vtkPropPicker>::New();
	propPicker->PickFromListOn();
	propPicker->AddPickList(this->actor);

	propPicker->Pick(iren->GetEventPosition()[0],
					 iren->GetEventPosition()[1],
					 0.0,
					 this->new_render);
	// There could be other props assigned to this picker, so 
	// make sure we picked the image actor
	vtkAssemblyPath* path = propPicker->GetPath();
	bool validPick = false;
	if (path)
	{
		vtkCollectionSimpleIterator sit;
		path->InitTraversal(sit);
		vtkAssemblyNode *node;
		for (int i = 0; i < path->GetNumberOfItems() && !validPick; ++i)
		{
			node = path->GetNextNode(sit);
			if (actor == vtkImageActor::SafeDownCast(node->GetViewProp()))
			{
				validPick = true;
			}
		}
	}

	double pos[3];
	propPicker->GetPickPosition(pos);

	std::cout<<"click mouse left button "<<eve_pos[0]<<" , "<<eve_pos[1]<<" , "<<this->slice_n<<std::endl;
	//std::cout<<"click mouse left button "<<vtkMath::Round(pos[0])<<" , "<<pos[1]<<" , "<<pos[2]<<std::endl;
	on_emit_coordinate(eve_pos[0], eve_pos[1],this->slice_n);
}


// private method: set view direction
void reslice_view_base::Set_Direction(char x)
{
	this->direction = x ;
	switch (x)
	{
	case 'a':
		{
			m_ViewDirText = vtkSmartPointer<vtkTextActor>::New();
			m_ViewDirText->SetInput("Axial View");
			m_ViewDirText->SetPosition2(10, 40);
			m_ViewDirText->GetTextProperty()->SetFontSize(20);
			m_ViewDirText->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
			this->new_render->AddActor2D(m_ViewDirText);

			double axialX[3] = {-1,0,0};
			double axialY[3] = {0,-1,0};
			double axialZ[3] = {0,0, 1};//double axialZ[3] = {0,0,-1};

			for (int i=0;i<3;i++)
			{
				this->view_dirX[i] = axialX[i];
				this->view_dirY[i] = axialY[i];
				this->view_dirZ[i] = axialZ[i];
			}

			break;
		}
	case 'c':
		{
			m_ViewDirText = vtkSmartPointer<vtkTextActor>::New();
			m_ViewDirText->SetInput("Coronal View");
			m_ViewDirText->SetPosition2(10, 40);
			m_ViewDirText->GetTextProperty()->SetFontSize(20);
			m_ViewDirText->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
			this->new_render->AddActor2D(m_ViewDirText);

			double coronalX[3] = { 1, 0, 0 };// double coronalX[3] = { -1, 0, 0 };
			double coronalY[3] = {0,0,1};
			double coronalZ[3] = {0,1,0};

			for (int i=0;i<3;i++)
			{
				this->view_dirX[i] = coronalX[i];
				this->view_dirY[i] = coronalY[i];
				this->view_dirZ[i] = coronalZ[i];
			}
			break;
		}
	case 's':
		{
			m_ViewDirText = vtkSmartPointer<vtkTextActor>::New();
			m_ViewDirText->SetInput("Sagittal View");
			m_ViewDirText->SetPosition2(10, 40);
			m_ViewDirText->GetTextProperty()->SetFontSize(20);
			m_ViewDirText->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
			this->new_render->AddActor2D(m_ViewDirText);

			double sagittalX[3] = {0,1,0};
			double sagittalY[3] = {0,0,1};
			double sagittalZ[3] = { 1, 0, 0 };
			for (int i=0;i<3;i++)
			{
				this->view_dirX[i] = sagittalX[i];
				this->view_dirY[i] = sagittalY[i];
				this->view_dirZ[i] = sagittalZ[i];
			}
			break;
		}
	default:
		{
			m_ViewDirText = vtkSmartPointer<vtkTextActor>::New();
			m_ViewDirText->SetInput("Default View");
			m_ViewDirText->SetPosition2(10, 40);
			m_ViewDirText->GetTextProperty()->SetFontSize(20);
			m_ViewDirText->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
			this->new_render->AddActor2D(m_ViewDirText);

			double axialX[3] = {1,0,0};
			double axialY[3] = {0,1,0};
			double axialZ[3] = {0,0,1};

			for (int i=0;i<3;i++)
			{
				this->view_dirX[i] = axialX[i];
				this->view_dirY[i] = axialY[i];
				this->view_dirZ[i] = axialZ[i];
			}
			break;
		}
	}
}
//private method: calculate imge center of a 3D image
double* reslice_view_base::calculate_img_center(vtkSmartPointer<vtkImageData> img)
{
#if VTK_MAJOR_VERSION <= 5
	img->GetWholeExtent(extent_m);
#else
	extent_m = img->GetExtent();
#endif

	img->GetSpacing(spacing);
	img->GetOrigin(origin);

	double center[3];
	for (int i=0;i<3;i++)
	{
		center[i] = origin[i]+spacing[i]*0.5*(extent_m[2*i]+extent_m[2*i+1]);//
		//center[i] = origin[i]+spacing[i]*0.5*(dimensions[i]-1);
		this->center[i] = center[i];
	}

	return center;
}


//this line is badly need to inhert a new class
//vtkObjectFactory.h must include!
vtkStandardNewMacro(reslice_interactor_style);




/*
Build up an lookup table for visulizaiton
*/
vtkSmartPointer<vtkLookupTable> reslice_view_base::BuildUpLookupTable()
{
	vtkSmartPointer<vtkLookupTable> rgbLut =
		vtkSmartPointer<vtkLookupTable>::New();

	rgbLut->SetNumberOfTableValues(256);
	rgbLut->SetRange(0.0, 255.0);

	rgbLut->Build();
	return rgbLut;
}
