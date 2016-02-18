/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/01/18

Description:
	This file is copied from PlusLib project

Copyright: See below
*/



/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef _PivotCalibration_h
#define _PivotCalibration_h


#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"

#include <list>
#include <set>
#include <vector>

// VNL includes
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_determinant.h"



//-----------------------------------------------------------------------------

/*!
  \class vtkPivotCalibrationAlgo 
  \brief Pivot calibration algorithm to calibrate a stylus. It determines the pose of the stylus tip relative to the marker attached to the stylus.
  
  The stylus tip position is computed by robust LSQR method, which detects and ignores outliers (that have much larger reprojection error than other points).
  
  The stylus pose is computed assuming that the marker is attached on the center of one of the stylus axes, which is often a good approximation.
  The axis that points towards the marker is the PivotPoint coordinate system's -Z axis (so that points in front of the stylus have positive Z coordinates
  in the PivotPoint coordinate system). The X axis of the PivotPoint coordinate system is
  aligned with the marker coordinate system's X axis (unless the Z axis of the PivotPoint coordinate system is parallel with the marker coordinate
  system's X axis; in this case the X axis of the PivotPoint coordinate system is aligned with the marker coordinate system's Y axis). The Y axis
  of the PivotPoint coordinate system is chosen to be the cross product of the Z and X axes.
  
  The method detects outlier points (points that have larger than 3x error than the standard deviation) and ignores them when computing the pivot point
  coordinates and the calibration error.
  
  \ingroup PlusLibCalibrationAlgorithm
*/
class  PivotCalibration : public vtkObject
{
public:
	vtkTypeMacro(PivotCalibration, vtkObject);
	static PivotCalibration *New();

  /*!
  * Read configuration
  * \param aConfig Root element of the device set configuration
  */
	int ReadConfiguration();// (vtkXMLDataElement* aConfig);

  /*!
    Remove all previously inserted calibration points.
    Call this method to get rid of previously added calibration points
    before starting a new calibration.
  */
  void RemoveAllCalibrationPoints();

  /*!
    Insert acquired point to calibration point list
    \param aMarkerToReferenceTransformMatrix New calibration point (tool to reference transform)
  */
  int InsertNextCalibrationPoint(vtkMatrix4x4* aMarkerToReferenceTransformMatrix);

  /*!
    Calibrate (call the minimizer and set the result)
    \param aTransformRepository Transform repository to save the results into
  */
  int DoPivotCalibration();

  /*!
    Get calibration result string to display
    \param aPrecision Number of decimals shown
    \return Calibration result (e.g. stylus tip to stylus translation) string
  */
  std::string GetPivotPointToMarkerTranslationString(double aPrecision=3);
  void GetToolTipToToolMatrix(vtkMatrix4x4* matrix);
  /*!
    Get the number of outlier points. It is recommended to display a warning to the user
    if the percentage of outliers vs total number of points is larger than a few percent.
  */
  int GetNumberOfDetectedOutliers();

public:

  vtkGetMacro(CalibrationError, double);

  vtkGetObjectMacro(PivotPointToMarkerTransformMatrix, vtkMatrix4x4); 

  vtkGetVector3Macro(PivotPointPosition_Reference, double);

  vtkGetStringMacro(ObjectMarkerCoordinateFrame);
  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkGetStringMacro(ObjectPivotPointCoordinateFrame);

protected:

  vtkSetObjectMacro(PivotPointToMarkerTransformMatrix, vtkMatrix4x4);

  vtkSetStringMacro(ObjectMarkerCoordinateFrame);
  vtkSetStringMacro(ReferenceCoordinateFrame);
  vtkSetStringMacro(ObjectPivotPointCoordinateFrame);

protected:
	PivotCalibration();
	virtual  ~PivotCalibration();

protected:
  /*! Compute the mean position error of the pivot point (in mm) */
  void ComputeCalibrationError();

  int GetPivotPointPosition(double* pivotPoint_Marker, double* pivotPoint_Reference);

protected:
  /*! Pivot point to marker transform (eg. stylus tip to stylus) - the result of the calibration */
  vtkMatrix4x4*        PivotPointToMarkerTransformMatrix;

  /*! Mean error of the calibration result in mm */
  double              CalibrationError;

  /*! Array of the input points */
  std::list< vtkMatrix4x4* > MarkerToReferenceTransformMatrixArray;

  /*! Name of the object marker coordinate frame (eg. Stylus) */
  char*               ObjectMarkerCoordinateFrame;

  /*! Name of the reference coordinate frame (eg. Reference) */
  char*               ReferenceCoordinateFrame;

  /*! Name of the object pivot point coordinate frame (eg. StylusTip) */
  char*               ObjectPivotPointCoordinateFrame;

  /*! Pivot point position in the Reference coordinate system */
  double              PivotPointPosition_Reference[4];

  /*! List of outlier sample indices */
  std::set<unsigned int> OutlierIndices;


  //====== static math function =========//
  /*
  
  */
  static int LSQRMinimize(
	  std::vector< vnl_vector<double> > aMatrix, 
	  std::vector<double>              bVector, 
	  vnl_vector<double>               xVector,
	  double* mean, 
	  double* stdev, 
	  vnl_vector<unsigned int>* notOutliersIndices )
  {
	  
	  return 0;
  };
  /*
  
  */
  static int ComputeMeanAndStdev(std::vector<double> errorValues, double mean, double stdev)
  {

	  return 0;
  };
};

#endif
