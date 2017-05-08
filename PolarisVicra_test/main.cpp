/**
Author: QIN Shuo
Date: 2017/05/08
Description:
	Example for Polaris Virca API
**/

#include <stdlib.h>
#include <stdio.h>

#include "api_module.h"

#define PAUSE system("pause")


int main()
{	
	//sys_init();
	//PAUSE;
	//start_tracking();
	//PAUSE;
	//stop_tracking();
	//PAUSE;
	//disconnect_sys();
	//PAUSE;
	//error_check();
	//PAUSE;
	//activate_sensor();
	//PAUSE;

	get_matrix();
	PAUSE;

	make_tracker_beep();
	PAUSE;

	return 0;
}







