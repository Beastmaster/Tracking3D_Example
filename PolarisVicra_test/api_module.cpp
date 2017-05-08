/**
Author: QIN Shuo
Date: 2017/05/08
Description:
	Example for Polaris Virca API
**/


#include <functional>
#include <sstream>
#include "api_module.h"


#define LOG(X) std::cout<<X<<std::endl

int sys_init()
{
	LOG("\n\n================   Testing sys_init   ===============");
	auto polaris = new PloarisVicraConfiguration();
	//=== set configuration filename
	// you'd better copy this file and make modifications
	auto ini_file = "D:/Project/Tracking3D_Example/build/TrackerPolarisVicra.ini";
	polaris->SetConfigureFile(ini_file);
	//=== configure communication port, or modify the .ini file
	//polaris->SetnDataBits(1);
	//polaris->SetnFlowControl(1);
	//polaris->SetnHardware(1);
	//polaris->SetWireless(1);

	//=== configure tracking tools SROM (the sensor)
	// you can modify the .ini file to skip the following operations
	auto tool1 = "D:/Project/Tracking3D_Example/PolarisVicra/configFile/ROM/8700340.rom";
	auto tool2 = "D:/Project/Tracking3D_Example/PolarisVicra/configFile/ROM/8700339.rom";
	polaris->SetSROMFileX(tool1, 0);
	polaris->SetSROMFileX(tool2, 1);

	//=== configuration tracker
	auto status = polaris->ConfigureTracker();
	if (status == 0)
		LOG("Success");
	else
		LOG("Fail");

	polaris->nCloseComPorts();
	return status;
}

int start_tracking()
{
	LOG("\n\n\n================   Testing start tracking   ===============");
	auto polaris = new PloarisVicraConfiguration();
	auto ini_file = "D:/Project/Tracking3D_Example/build/TrackerPolarisVicra.ini";
	polaris->SetConfigureFile(ini_file);
	auto status = polaris->ConfigureTracker();
	if (status != 0)
	{
		LOG("Configuration fail");
		return 1;
	}

	//=== start tracking
	auto nstatus = polaris->StartTracking();
	if (nstatus == 0)
	{
		LOG("System is Tracking");
		polaris->StopTracking();
		return 0;
	}
	else
	{
		LOG("System trcking fail");
		return 1;
	}
}

int stop_tracking()
{
	LOG("\n\n\n================   Testing stop tracking   ===============");
	auto polaris = new PloarisVicraConfiguration();
	auto ini_file = "D:/Project/Tracking3D_Example/build/TrackerPolarisVicra.ini";
	polaris->SetConfigureFile(ini_file);
	auto status = polaris->ConfigureTracker();
	if (status != 0)
	{
		LOG("Configuration fail");
		return 1;
	}

	//=== start tracking
	auto nstatus = polaris->StartTracking();
	if (nstatus != 0)
	{
		LOG("System trcking fail");
		return 1;
	}

	//=== stop tracking
	auto sstatus = polaris->StopTracking();
	if (sstatus == 0)
	{
		LOG("System tracking stopped");
		return 0;
	}
	else
	{
		LOG("System cannot stop tracking");
		return 1;
	}
}

int disconnect_sys()
{
	LOG("\n\n\n================   Testing disconnect system   ===============");
	LOG("Calling disconnect will disconnect the system");
	LOG("There is no explicit safe function to disconnect, it's safe to disconnect the hardware any time");
	return 0;
}

int error_check()
{
	LOG("\n\n\n================   Testing error check   ===============");
	LOG("Every function has an error code");
	LOG("Just check the return code: \n\t\t\t0, success; 1, error");
	return 0;
}

int activate_sensor()
{
	LOG("\n\n\n================   Testing disconnect system   ===============");
	LOG("Calling disconnect will disconnect the system");
	LOG("There is no explicit safe function to disconnect, it's safe to disconnect the hardware any time");
	return 0;
}

int get_matrix()
{
	LOG("\n\n\n================   Testing Getting matrix   ===============");
	auto polaris = new PloarisVicraConfiguration();
	auto ini_file = "D:/Project/Tracking3D_Example/build/TrackerPolarisVicra.ini";
	polaris->SetConfigureFile(ini_file);
	auto status = polaris->ConfigureTracker();
	if (status != 0)
	{
		LOG("Configuration fail");
		return 1;
	}
	//=== start tracking
	auto nstatus = polaris->StartTracking();
	if (nstatus != 0)
	{
		LOG("System trcking fail");
		return 1;
	}

	//===  get transform matrix
	LOG("Press enter to update, q to exit");
	while (getchar()!='q')
	{
		auto get_mat = [polaris](int i) 
		{
			//======= key block ======//
			NEW2DARR(double, mat);
			auto status = polaris->GetTransform(i, mat);
			auto mat2str = [mat]() 
				{	std::stringstream buffer;
					for (auto i = 0; i < 4; i++)
						buffer << mat[i][0] <<" , "<<mat[i][1] << " , " << mat[i][2] << " , " << mat[i][3] << std::endl;
					return buffer.str();};
			if (status == 0)
				LOG("id:" << i << "  get\n"<<"matrix: \n"<<mat2str());
			else
				LOG("id: " << i << "  not get");
			DEL2DARR(double, mat)
			//======= key block ======//
		};	
		
		get_mat(0);
		get_mat(1);
	}

	return 0;
}



int make_tracker_beep()
{
	LOG("\n\n\n================   Testing beep   ===============");
	auto polaris = new PloarisVicraConfiguration();
	auto ini_file = "D:/Project/Tracking3D_Example/build/TrackerPolarisVicra.ini";
	polaris->SetConfigureFile(ini_file);
	auto status = polaris->ConfigureTracker();
	if (status != 0)
	{
		LOG("Configuration fail");
		return 1;
	}
	auto nstatus = polaris->StartTracking();
	polaris->Beep(3);

	polaris->StopTracking();
	return 0;
}


