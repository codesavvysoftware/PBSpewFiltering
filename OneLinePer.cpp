// OneLinePer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <locale>
#include <codecvt>

#include "ReadLineInterface.h"

using namespace std;

using namespace ProcessPBSpews;

vector<string> AsFileFilters = { "ActionScript:" };
vector<string> HFPFilters = { "HFP RX:", "HFP TX:" };
vector<string> ReadableFilters = { " " };
vector<string> InclusiveFiliters = {
	"HFP RX:",
	"HFP TX:",
	"CBTPairSvc",
	"ActionScript:",
	"Phone:",
	"HFPSvc:",
	"CHFPSvcManager:",
	"CBTPairSvc:",
	"HFP:",
	"[HCI]",
	"[RFCOMM]",
	"[BtMonitorSvc]",
	"PhoneCore:",
	"BTPairApi!",
	"PhoneGetConnectedCall:",
	"CSyncManager:",
	"AEC:",
	"CMAPManager:"
};
vector<string> MAPFilters = {	"CMAPManager:" };

vector<sFilterDescriptor> FilterDescriptors
{ 
	{ 
		"_Filtered_AS.txt", 
		AsFileFilters//{ { "ActionScript:" } }
	},
	{ 
		"_Filtered_HFP.txt",
		HFPFilters
	},
	{
		"_Filtered_MAP.txt",
		MAPFilters
	},
	{
		"_Filtered_All.txt", 
		InclusiveFiliters
	} 
}; 

int _tmain(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Command invocation error" << endl;

		return 1;
	}
	GenerateFilteredSpewData pgsdFilterSpew;

	bool bNoFilterSuccess = false;

	char cFolder[] = "C:\\Users\\v-tohale/Documents\\Visual Studio 2013\\Projects\\OneLinePer\\Debug\\";

	char cInputFile[uiMaxFileNameSize];

	string sErrorMsg;

	strncpy(cInputFile, argv[1], uiMaxFileNameSize - 2);

	if (!SetCurrentDirectory(cFolder))
	{
		cout << "Could not set directory path for the file using default path" << endl;

		return 1;
	}

	string sInputFileName(cInputFile);

	auto iFileExtPos = sInputFileName.find('.');

	if (iFileExtPos < 0)
	{
		sInputFileName += ".txt";
	}

	FILE * fSrc = fopen(sInputFileName.c_str(), "r");

	if (!fSrc)
	{
		cout << "Error Opening Source File" << GetLastError() << endl;

		return 1;
	}

	ReadLineInterfaceFactory rlif;
	
	unique_ptr<ReadLineInterface> rli = rlif.getReadLineInterface(fSrc, sErrorMsg);

	if (rli == nullptr)
	{
		cout << "Text File Type Not Processed By this Program" << endl;

		fclose(fSrc);

		return 1;
	}

	bNoFilterSuccess = !pgsdFilterSpew(cFolder, cInputFile, uiMaxFileNameSize, *rli, FilterDescriptors, sErrorMsg);
    
	if (bNoFilterSuccess)
	{
		cout << "Failed to produce Filtered All Files" << sErrorMsg << endl;
	}

	return 0;
}

