/*
Author:	QIN Shuo
Date:	2016/3/14
Organization: RC-MIC(CUHK)

Description:
	Parse a configuration file

*/

#include "ConfigurationParse.h"



ConfigurationParse::ConfigurationParse()
{
	buffer = NULL;
}
ConfigurationParse::~ConfigurationParse()
{
	if (buffer!=NULL)
	{
		free(buffer);
	}

}


/*
Set File Name
*/
void ConfigurationParse::SetFileName(std::string name)
{
	m_FileName = name;
}
void ConfigurationParse::SetFileName(char* name)
{
	m_FileName = name;
}

/*
Description:
	Check the existance or validation of the file
Return:
	0:	Success
	1:	Fail
*/
int ConfigurationParse::ReadFile()
{
	FILE* pFile;
	long lSize;
	size_t result;

	pFile = fopen(m_FileName.c_str(),"rb");
	if (pFile == NULL)
	{
		std::cout << "Error Opening " << m_FileName << std::endl;
		return 1;
	}

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	buffer = (char*)malloc(sizeof(char)*lSize);
	if (buffer == NULL) 
	{ 
		fputs("Memory error", stderr);
		return 2;
	}

	// copy the file into the buffer:
	result = fread(buffer, 1, lSize, pFile);
	if (result != lSize) 
	{
		fputs("Reading error", stderr); 
		return 3;
	}
	fclose(pFile);

	char* temp = buffer;
	while (*temp != '\0') // end of char*
	{
		if (*temp == '#')
		{
			SearchCommnets(temp);
		}
		if (*temp == '<')
		{
			SearchBlock(temp);
		}
		temp++;
	}

	// eliminate all the '\n' '\t' '\r'
	for (auto it = m_Config.begin(); it != m_Config.end(); it++)
	{
		;
	}

	return 0;
}



int ConfigurationParse::UpdateFile()
{
	std::string temp;

	for (auto it = m_Config.begin(); it != m_Config.end(); it++)
	{
		// append block head
		temp.append("\n<");
		temp.append(it->first);
		temp.append(">\n\r");
	
		// block content
		temp.append(it->second);

		// append block end
		temp.append("\n</");
		temp.append(it->first);
		temp.append(">\n\r");
	}

	auto pFile = fopen(m_FileName.c_str(), "w");
	if (pFile!=NULL)
	{
		fputs(temp.c_str(),pFile);
		fclose(pFile);
	}

	return 0;
}


/*
Check # character
Move the pointer to the begining of the next line
*/
void ConfigurationParse::SearchCommnets(char* &buf)
{
	if (*buf == '#')
	{
		while ((*buf != '\n'))
		{
			if (*buf == '\0')
			{
				return;
			}
			if (*(buf+1) == '\0')
			{
				return;
			}
			buf++;
		}
		return;
	}
}

void ConfigurationParse::SearchBlock(char* &buf)
{
	std::string head;
	std::string head2;

	std::string content;

	if (*buf == '<')
	{
		if (*(buf + 1) == '/') // format error
			return;
		int i = 0;
		while (*(buf+i)!='>')
		{
			i++;
		}
		head.assign(buf + 1, i-1);
		buf = buf + i;

		i = 0;
		while ( !((*(buf + i) == '<')&(*(buf + i + 1) == '/')) )
		{
			i++;
		}
		content.assign(buf + 1, i-1);
		buf = buf + i;

		while (content.find("\r") != std::string::npos)
			content.erase(content.find("\r"),1);
		while (content.find("\t") != std::string::npos)
			content.erase(content.find("\t"), 1);

		while (content.front()=='\n')
		{
			content.erase(content.begin());
		}

		while ((content.size()>0)&&(content.back() == '\n'))
		{
			content.pop_back();
		}

		//reach the end of head
		i = 0;
		while (*(buf+i) !='>')
		{
			i++;
		}
		head2.assign(buf + 2, i-2);
		buf = buf + i;

		if (head == head2)
		{
			m_Config[head] = content;
		}
	}
}











