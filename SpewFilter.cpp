// SpewFilter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


//============================================================================
// Name        : SpewFilter.cpp
// Author      : Tom Haley
// Version     : v1.0
// Copyright   :
// Description : Filter WinCE Debug Spews for Specific Types of Debug Messages
//============================================================================

#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED 1
#endif

#include <iostream>
#include "SpewFiltering.hpp"
#include <strsafe.h>
#include <windows.h>
#include <system_error>

int main(int argc, char * argv[]) {
	using namespace SpewFilteringSpace;

	SpewFiltering sfFiltering;

	SpewFiltering::SpewFilteringParams FilterParams;

	int iRetval = 1;

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(argc, argv, FilterParams);

	if ((SUCCESS == Configured) || (HELP_SUCCESSFUL == Configured)) {
		if (sfFiltering.ProduceFilteredSpewFiles(FilterParams)) {
			iRetval = 0;
		}
	}
	else if (ERROR_IN_COMMAND_LINE == Configured) {
		std::cerr << "Command Line Entry Error" << std::endl;
	}
	else if (ERROR_UNHANDLED_EXCEPTION == Configured) {
		std::cerr << "Unhandled Exception Error" << std::endl;
	}
	else {
		std::cerr << "Unknown Error" << std::endl;
	}

	return iRetval;
}
