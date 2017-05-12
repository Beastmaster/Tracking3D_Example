/**
Author: QIN Shuo
Date: 2017/05/11
*/


#ifndef ICPREGISTRATIONFILTER_H
#define ICPREGISTRATIONFILTER_H
#include "vtkTrackingRegistration.h"
#include <functional>
#include <vector>
#include "vtkSmartPointer.h"

#include "AbstractFilter.h"
#include "vtkMatrix4x4.h"

class ICPRegistrationFilter : public AbstractFilter
{
	Q_OBJECT
public:
    explicit ICPRegistrationFilter(QObject *parent = 0);
    ~ICPRegistrationFilter();

    void SetInputPolyData(vtkPolyData* polydata);
    void SetInitialMatrix(vtkMatrix4x4* matrix);

    void AppendWorldCoordiantion(double* w_coord);
    void ClearAllPoints();

    double          GetOutputError();
    vtkMatrix4x4*   GetOutputMatrix();

public slots:
	virtual int Update();

private:
	void Compute();

    QList<double*>  m_worldCoordinationList;
    vtkPolyData*    m_polydata;

    vtkMatrix4x4*   m_initialMatrix;
    vtkMatrix4x4*   m_outputMatrix;
    double          m_outputError;
};

#endif // LANDMARKREGISTRATION_H
