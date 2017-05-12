/*
Author:	QIN Shuo
Date:	2016/3/14
Organization: RC-MIC(CUHK)

Description:
Parse a configuration file

*/

#ifndef __CONFIGURATIONPARSE_H__
#define __CONFIGURATIONPARSE_H__

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <iostream>
#include <fstream>

class ConfigurationParse
{
public:
	ConfigurationParse();
	~ConfigurationParse();

	void SetFileName(std::string);
	void SetFileName(char*);
	int ReadFile();
	int UpdateFile();

private:
	std::string m_FileName;
	char * buffer;
	std::map< std::string, std::string > m_Config; // < Index ; Content  >

	

	void SearchCommnets(char* &);
	void SearchBlock(char* &);

};






#endif









