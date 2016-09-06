#include<winsock2.h>
#include<Iphlpapi.h>
#include<stdio.h>
#include <string>
#include <vector>

#pragma comment(lib,"Iphlpapi.lib")


//mac_addr: char[17]
int GetAdaptersInfo(std::vector<std::string> & mac_addr)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	ULONG ulOutBufLen;
	pAdapterInfo=(PIP_ADAPTER_INFO)malloc(sizeof(IP_ADAPTER_INFO));
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	// 第一次调用GetAdapterInfo获取ulOutBufLen大小
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
	}

	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
    {
		pAdapter = pAdapterInfo;
		while (pAdapter) 
		{
			printf("Adapter Name: \t%s\n", pAdapter->AdapterName);
			printf("Adapter Desc: \t%s\n", pAdapter->Description);
			printf("MAC Addr: \t%02x-%02x-%02x-%02x-%02x-%02x\n", 
				pAdapter->Address[0],
				pAdapter->Address[1],
				pAdapter->Address[2],
				pAdapter->Address[3],
				pAdapter->Address[4],
				pAdapter->Address[5]);
			char temp[20];
			int n = sprintf(temp, "%02x-%02x-%02x-%02x-%02x-%02x",
				pAdapter->Address[0]-1,
				pAdapter->Address[1]-1,
				pAdapter->Address[2]-1,
				pAdapter->Address[3]-1,
				pAdapter->Address[4]-1,
				pAdapter->Address[5]-1);
			std::string mac_temp = temp;
			mac_addr.push_back(mac_temp);
			printf("Mac addr check: %s ,length: %d \n", mac_temp.c_str(), &n);

			printf("IP Address: \t%s\n", pAdapter->IpAddressList.IpAddress.String);
			printf("IP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String); 
			printf("Gateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
			pAdapter = pAdapter->Next;
		}
		return 1;
	}
	else
	{
		printf("Call to GetAdaptersInfo failed.\n");
		return 0;
	}
}

int DeCoding(std::string & key)
{
	char* temp = new char(key.size());

	memcpy(temp, key.c_str(), key.size()*sizeof(char));


	for (size_t i = 0; i < key.size(); i++)
	{
		temp[i] = temp[i] - 1;
	}

	key.clear();
	key = temp;

	return 1;
}

int GetMACFromLicense(std::string filename, std::string & key)
{
	FILE * pFile;
	pFile = fopen(filename.c_str(), "r");
	char content[100];
	char c;
	int i = 0;
	if (pFile != NULL)
	{
		while ((c = getc(pFile)) != EOF)
		{
			content[i++]=c;
		}
		content[i++] = '\0';
		printf("License file content: ");
		printf(content);
		printf("\n");

		fclose(pFile);

		key = std::string(content+8,17);
		
		//DeCoding(key);

		return 1;
	}
	return 0;
}

int GenerateLicense()
{
	return 0;
}




