/**
Author: QIN Shuo
Date: 2017/05/08
Description:
Example for Polaris Virca API
**/


#ifndef  __APE_MODULE__
#define  __APE_MODULE__

#include "PolarisVicraConfiguration.h"


int sys_init();
int start_tracking();
int stop_tracking();
int disconnect_sys();

int error_check();

int activate_sensor();

int get_matrix();

int make_tracker_beep();

#endif //  __APE_MODULE__



