/*
Author: QIN Shuo
Organization: RC-MIC (CUHK)
Date: 2016/1/10
*/




#include "mainWindow.h"
#include "ui_mainWindow.h"

MainWindow::MainWindow(QWidget *parent):
QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	sys_Init();
}

MainWindow:: ~MainWindow()
{
}


void MainWindow::sys_Init()
{
	//auto render_temp = vtkSmartPointer<vtkRenderer>::New();
	//render_temp->SetBackground(0,0,0);
	//ui->axialWidget->GetRenderWindow()->AddRenderer(render_temp);
	//ui->sagittalWidget->GetRenderWindow()->AddRenderer(render_temp);
	//ui->coronalWidget->GetRenderWindow()->AddRenderer(render_temp);

	ui->axialWidget->GetRenderWindow()->Render();
	ui->sagittalWidget->GetRenderWindow()->Render();
	ui->coronalWidget->GetRenderWindow()->Render();
	ui->threeDWidget->GetRenderWindow()->Render();
}

void MainWindow::on_Load_Image()
{}
void MainWindow::on_Load_Atlas()
{}
void MainWindow::on_Sel_Tracker()
{}
void MainWindow::on_Config_Tracker()
{}
void MainWindow::on_StartTracking()
{}
void MainWindow::on_StopTracking()
{}
void MainWindow::on_OpacityMove(int)
{}


