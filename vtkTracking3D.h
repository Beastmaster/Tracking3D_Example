/*
Author: QIN Shuo
Date: 2015/12/29
Organization: RC-MIC (CUHK)
Description:
	tracking 3d object
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
#define QIN_S_VTK_EVENT 8888    // this is an unique event ID to connect mouse picked coordinate
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

1. Configure view and install pipeline
2. Add 3D object
3. Configure Tracker
4. Connect tracker tool and actor
5. Start Timer and tracking

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
	void SetTrackerType(){};

	// Add Functions
	int AddObject( vtkSmartPointer< vtkActor > );
	int AddPolySource(vtkSmartPointer<vtkPolyData>);
	int AddOrthogonalPlanes(){ return 0; };    // to be continued
	int ConnectObjectTracker(int,int);
	int DisConnectObjectTracker(int);

	// Remove Functions
	int RemoveObject(int index);
	int RemoveObject(vtkActor*);

	// Set Functions
	int SetOpacity(int index , float opacity);
	int SetEnableOutline(int index,bool en);
	int SetColor(int index, double r, double g, double b);
	int SetColor(int index, double* rgb);
	int SetTransform(int index, QIN_Transform_Type* in);
	int SetRegisterTransform(vtkMatrix4x4*);
	void SetInterval(unsigned long xx) { m_interval = xx; };
	void SetWindow( vtkSmartPointer<vtkRenderWindow> );
	void SetInteractor(vtkSmartPointer<vtkRenderWindowInteractor> inct);
	void SetInteractorStyle( vtkInteractorStyle* );
	void SetTracker(TrackerType* in);
	//int SetLandMarks(std::vector<double*>src, std::vector<double*> tgt);//test function

	// Get Functions
	static vtkActor* GetActorPointer(vtkPropCollection*, int);
	vtkRenderer* GetDefaultRenderer();
	vtkSmartPointer<vtkRenderWindow> GetRenderWindow();
	vtkSmartPointer<vtkRenderWindowInteractor> GetInteractor();
	int* GetResliceIndex();
	void GetResliceIndex(int*);
	void EnablePick();
	void DisablePick();

	// install pipeline
	int InstallPipeline();
	void RefreshView();

	//marker control function
	int ValidMarker();
	int ClearMarkers();
	std::vector<double*> GetMarkerList();

	//tracking device configuration
	//PloarisVicraConfiguration* m_tracker;
	TrackerType* m_tracker;
	std::map<int, int> m_Obj_Tool_Map;   //map: link index of actor and trackingTool

	double m_marker_tobe_set[3];
	int m_index_tobe_set[3];
protected:
	vtkTracking3D();
	~vtkTracking3D();

	//protected function, register in install pipeline
	unsigned long m_interval; //timer interval in millisecond
	unsigned long m_TimerID;

	std::vector<double*> m_marker_list;

private:

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


#endif

