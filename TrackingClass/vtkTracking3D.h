/*
Author: QIN Shuo
Date: 2015/12/29
Organization: RC-MIC (CUHK)
Description:
	tracking 3d object
	add OrthogonalPlanes
*/

/*
Manual2:
	1. Set Tracker
	2. Configure Tracker
		This class use a base class of tracker, you should add a 
		tracker_configuration_class pointer by hand. This 
		pointer is not managed here, just call some functions.
	3. Connect actor and tracker tool; if no actor connected, let the index_actor = 0
	4. Use signal an slot to access coordinate (Find3DIndex to get slice index)
	6. Start Timer and tracking 
*/

#ifndef __VTKTRACKING3D_H__
#define __VTKTRACKING3D_H__

#include "vtkSmartPointer.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkActorCollection.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkImageData.h"
#include "vtkMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkCallbackCommand.h"
#include "vtkPropPicker.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineFilter.h"
#include "vtkImagePlaneWidget.h"

#include <vtkTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkPropAssembly.h>

#include <vtkCallbackCommand.h>
#include <vtkCommand.h>

#include <vector>
#include <map>
#include "TrackerBase.h"
//#include "PolarisVicraConfiguration.h"
//#include "ATC3DGConfiguration.h"

//----------  define user event id here  ---------//
#define QIN_MOVE_EVENT "new data"
#define QIN_ERROR_EVENT "error!!!"
#define QIN_S_VTK_EVENT 8888    // This is an unique event ID to connect mouse picked coordinate
								// First used in QtWrapvtkTracking3D.h

/*
typedef struct
{
	struct Position3d//Position3dStruct
	{
		float   x;
		float   y;
		float   z;
	} position;

	struct QuatRotation//QuatRotationStruct
	{
		float   q0;
		float   qx;
		float   qy;
		float   qz;
	} rotation;

} TransformInfo;
*/

/*
User Manual:
0. SetRenderWindow and RenderWindowInteractor
1. Configure view and install pipeline
2. Add 3D object / Set 3D image
3. Configure Tracker
	This class use a base class of tracker, you should add a 
	tracker_configuration_class pointer by hand. This 
	pointer is not managed here, just call some functions.
4. Connect tracker tool and actor
5. Set Transform matrix. For generation of Transform matrix, refer to vtkTrackingRegistration class.
6. Start Timer and tracking

Tips:
1. You don't have to SetImage() to run the 3d tracking view
2. If you don't SetImage(), then this class will only return a 3d coordinate of the selected point, not the 3d index
3. If plane view do not work somewhere, just call window->Render() function.
*/
class vtkTracking3D : public vtkObject
{
	//typedef  PloarisVicraConfiguration TrackerType;
	//typedef  ATC3DGConfiguration TrackerType;
	typedef TrackerBase TrackerType;

public:
	static vtkTracking3D* New();
	vtkTypeMacro(vtkTracking3D,vtkObject);

	//tracking function
	void StartTracking();
	void StopTracking();

	// Add Functions
	int AddObject( vtkSmartPointer< vtkActor > );
	int AddPolySource(vtkSmartPointer<vtkPolyData>);
	int EnableOrthogonalPlanes();
	int DisableOrthogonalPlanes();
	int ConnectObjectTracker(int,int);
	int DisConnectObjectTracker(int);

	// Remove Functions
	int RemoveObject(int index);
	int RemoveObject(vtkActor*);

	// Set Functions
	int SetImage(vtkSmartPointer<vtkImageData>);
	int SetOpacity(int index , float opacity);
	int SetEnableOutline(int index,bool en);
	int SetColor(int index, double r, double g, double b);
	int SetColor(int index, double* rgb);
	int SetTransform(int index, QIN_Transform_Type* in);
	int SetRegisterTransform(vtkMatrix4x4*);
	void SetInterval(unsigned long xx) { m_interval = xx; };
	void SetWindow( vtkRenderWindow* );
	void SetInteractor(vtkSmartPointer<vtkRenderWindowInteractor> inct);
	void SetInteractorStyle( vtkInteractorStyle* );
	void SetTracker(TrackerType* in);
	//int SetLandMarks(std::vector<double*>src, std::vector<double*> tgt);//test function

	// Get Functions
	static vtkActor* GetActorPointer(vtkPropCollection*, int);
	vtkRenderer* GetDefaultRenderer();
	vtkSmartPointer<vtkRenderWindow> GetRenderWindow();
	vtkSmartPointer<vtkRenderWindowInteractor> GetInteractor();
	vtkSmartPointer<vtkTransform> GetRegisterTransform();

	void GetResliceIndex(int*);
	void EnablePick();
	void DisablePick();

	// install pipeline
	int InstallPipeline();
	void RefreshView();

	//marker control function
	int ValidMarker();
	int ClearMarkers();
	int SetMarkerList(std::vector<double*>);
	std::vector<double*> GetMarkerList();

	//static function to find point 3d index from world coordinate
	static void Find3DIndex(vtkImageData*, double* coordinate,int*);
	static void Find3DIndex(vtkImageData*, double x,double y,double z,int *);

	//tracking device configuration
	//PloarisVicraConfiguration* m_tracker;
	TrackerType* m_tracker;
	std::map<int, int> m_Obj_Tool_Map;   //map: link index of actor and trackingTool

	double m_marker_tobe_set[3];

	int m_SliceX;  
	int m_SliceY;  
	int m_SliceZ;  
	vtkSmartPointer<vtkImagePlaneWidget> m_PlaneX;
	vtkSmartPointer<vtkImagePlaneWidget> m_PlaneY;
	vtkSmartPointer<vtkImagePlaneWidget> m_PlaneZ;

protected:
	vtkTracking3D();
	~vtkTracking3D();

	//protected function, register in install pipeline
	unsigned long m_interval; //timer interval in millisecond
	unsigned long m_TimerID;

	std::vector<double*> m_marker_list;

private:
	vtkSmartPointer<vtkImageData> m_Image;
	/*
	This Transform is used to transform raw tracking device position to
	registered coordinate.
	*/
	vtkSmartPointer<vtkTransform> m_Transform;
	//vtkSmartPointer<vtkLandmarkTransform> m_LandMarkTransform;  //test
	//
	vtkSmartPointer<vtkRenderer> m_CurrentRenderer;
	// renderer collections
	vtkSmartPointer< vtkRendererCollection > m_RendererCollection;
	// renderwindow
	vtkSmartPointer<vtkRenderWindow> m_RenderWindow;
	// callback command
	vtkSmartPointer<vtkCallbackCommand> m_KeyPressCallBack;
	vtkSmartPointer<vtkCallbackCommand> m_TimerCallBack;
	vtkSmartPointer<vtkCallbackCommand> m_MouseCallback;
	// interactor
	vtkSmartPointer<vtkRenderWindowInteractor> m_Interactor;
	// interactor style
	vtkSmartPointer< vtkInteractorStyle > m_InteractorStyle;
	// actor collections
	vtkSmartPointer< vtkActorCollection > m_ActorCollection;
	vtkSmartPointer< vtkPropCollection > m_PropCollection;  //no use by now

};


//------------- interact callback --------//
static void KeypressCallbackFunction(
	vtkObject* caller,
	long unsigned int eventId,
	void* clientData,
	void* callData
	);

static void MouseclickCallbackFunction(
	vtkObject* caller,
	long unsigned int eventId,
	void* clientData,
	void* callData
	);


static void TimerCallbackFunction(
	vtkObject* caller,
	long unsigned int eventId,
	void* clientData,
	void* callData
	);
static void TimerCallbackFunction2(
	vtkObject* caller,
	long unsigned int eventId,
	void* clientData,
	void* callData
	);

#endif

