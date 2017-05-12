/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2017/5/15
*/


/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/1/10

*/


#ifndef VTKTRACKERUTIL_TEST_WIN
#define VTKTRACKERUTIL_TEST_WIN

#include "ui_vtkTrackerUtil_test_Win.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <functional>
#include <algorithm>

#include <QMainWindow>
#include <QApplication>
#include <QFileDialog>
#include <QWheelEvent>
#include <QMessageBox>








namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();







private:
	Ui::MainWindow *ui;

};






#endif
