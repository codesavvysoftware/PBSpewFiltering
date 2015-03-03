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
	"[RFCOMM]"
	"[BtMonitorSvc]",
	"PhoneCore:",
	"BTPairApi!",
	"PhoneGetConnectedCall:",
	"CSyncManager:",
	"AEC:"
};

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
		"_Filtered_All.txt", 
		InclusiveFiliters
	} 
}; 

const char * cHFPFilterFields[] = { "HFP RX:",
									 "HFP TX:",
									 NULL };

const char * cAllFilterFields[] = { "HFP RX:",
                                    "HFP TX:",
									 "CBTPairSvc",
									 "ActionScript:",
									 "Phone:",
									 "HFPSvc:",
									 "CHFPSvcManager:",
									 "CBTPairSvc:",
									 "HFP:",
									 "[HCI]",
									 "[RFCOMM]"
									 "[BtMonitorSvc,",
									 "PhoneCore:",
									 "BTPairApi!",
									 "PhoneGetConnectedCall:",
									 "CSyncManager:",
									 "AEC:",
									 nullptr};

const char * pcFilterForLinesNotSeparatedETC[] = { nullptr };

const char * cActionScriptFilterFields[] = { "ActionScript:",
                                              NULL }; 
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

	bNoFilterSuccess = !pgsdFilterSpew(cFolder, cInputFile, uiMaxFileNameSize, FilterDescriptors, sErrorMsg);
    
	if (bNoFilterSuccess)
	{
		cout << "Failed to produce Filtered All Files" << sErrorMsg << endl;
	}

	return 0;
}

