/**
Author: QIN Shuo
Date: 2017/05/11
*/
#ifndef _LANDMARKREGISTRATIONFILTER_H___
#define _LANDMARKREGISTRATIONFILTER_H___

#include "vtkTrackingRegistration.h"
#include <functional>
#include <vector>
#include "vtkSmartPointer.h"

#include "AbstractFilter.h"
#include "vtkMatrix4x4.h"
#include <QList>

class LandmarkRegistrationFilter : public AbstractFilter
{
	Q_OBJECT
public:
    explicit LandmarkRegistrationFilter(QObject *parent = 0);
    ~LandmarkRegistrationFilter();

    void AppendPairedPoint(double* i_coord, double* w_coord);
    void SetPairedPoint(int index, double* i_coord, double* w_coord);
    void ClearAllPairedPoint();

    double          GetOutputError();
    vtkMatrix4x4*   GetOutputMatrix();

public slots:
	virtual int Update();

private:
	void Compute();

    QList<double*>  m_imageCoordinationList;
    QList<double*>  m_worldCoordinationList;

    vtkMatrix4x4*   m_outputMatrix;
    double          m_outputError;

};

#endif // LANDMARKREGISTRATION_H
