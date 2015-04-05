/*
 * SpewFiltering.cpp
 *
 *  Created on: Apr 2, 2015
 *      Author: tomh
 */
#include "SpewFiltering.hpp"
#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "OptionPrinter.hpp"
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <string>
#include <locale>
#include "ReadLineInterface.h"
//#include <codecvt>

namespace SpewFilteringSpace {
int SpewFiltering::ConfigureFilteringParams( int & argc, char ** & argv, SpewFilteringParams & sfp ) {
	const char * pcInputFileKey     = ",i";

	const char * pcOutputFileKey    = ",o";

	const char * pcTypeOfFilterKey  = "TypeOfFilter,t";

	const char * pcHelpKey          = "help,h";

	const char * pcInputFileOption  = "File To Filter";

	const char * pcOutputFileOption = "Filtered Output File Prefix";

	const char * pcHelpOption       = "help";

	const char * pcFilterOptions =
			  "a - Produce all of the filter files\n"
			  "                                e - Produce a file with all the filters included\n"
			  "                                h - HFP debug lines only\n"
			  "                                s - Action Script messages only\n"
			  "                                p - Bluetooth phone messages only\n";
			  "                                m - MAP related phone messages only";

	const char * pcERROR_OptionOutOfOrder  = "ERROR: File to Filter needs to be the first option entered";
	const char * pcERROR_InvalidFilterType = "ERROR: Invalid Filter type entered";
	const char * pcERROR                   = "ERROR: ";

	const char * pcInputFileOptionID    = "-i";
	const char * pcOutputFileOptionID   = "-o";
	const char * pcTypeOfFilterOptionID = "TypeOfFilter";

	const char * pcInfileEnteredPrompt  = "Input File Entered: ";
	const char * pcOutfileEnteredPrompt = "Output File Entered: ";
	const char * pcTypeOfFilterPrompt   = "Type of Filter Entered: ";
	const char * pcUnhandledErrorMsg    = "Unhandled Exception reached the top of main: ";
	const char * pcAppExitErrorMsg      = "application will now Exit";

	const std::string caption = "All Options";


	try {
		std::string appName = boost::filesystem::basename(argv[0]);

		namespace po = boost::program_options;

		po::options_description desc(caption);

		desc.add_options()
				  (pcInputFileKey,   po::value<std::string>()->required(), pcInputFileOption)
				  (pcOutputFileKey,  po::value<std::string>(),             pcOutputFileOption)
				  (pcTypeOfFilterKey,po::value<std::string>(),             pcFilterOptions )
				  (pcHelpKey,                                              pcHelpOption)
				  ;

		po::positional_options_description positionalOptions;

		positionalOptions.add(pcInputFileOptionID, 1);

		po::variables_map vm;
		try {
			po::basic_parsed_options<char> bpo =
			    po::command_line_parser(argc, argv).options(desc).positional(positionalOptions).run();

			po::store(bpo, vm);

			/** --help option */
			if ( vm.count(pcHelpOption)  ) {
				rad::OptionPrinter::printStandardAppDesc(appName,
				                                         std::cout,
	                                                     desc,
	                                                     &positionalOptions);

				return SUCCESS;
	        }

		    // throws on error, so do after help in case there are any problems
		    po::notify(vm);

		    if (bpo.options[0].string_key != pcInputFileOptionID) {
		    	std::cerr << pcERROR_OptionOutOfOrder  << std::endl;

	            rad::OptionPrinter::printStandardAppDesc(appName,
		                                                 std::cout,
		                                                 desc,
		                                                   &positionalOptions);
		        return ERROR_IN_COMMAND_LINE;
		    }

		    sfp.sfFilterToApply = PRODUCE_EVERY_FILE;

		    if (vm.count(pcTypeOfFilterOptionID)) {
		    	std::string s = vm[pcTypeOfFilterOptionID].as<std::string>();

		    	if ( 1 != s.length()) {
			    	std::cerr << pcERROR_InvalidFilterType  << std::endl;

		            rad::OptionPrinter::printStandardAppDesc(appName,
			                                                 std::cout,
			                                                 desc,
			                                                 &positionalOptions);
			        return ERROR_IN_COMMAND_LINE;

		    	}
		    	switch(s[0]) {
		    	case 'a':
		    		sfp.sfFilterToApply = PRODUCE_EVERY_FILE;

		    		break;

		    	case 'e':
		    		sfp.sfFilterToApply = APPLY_ALL_FILTERS;

		    		break;

		    	case 's':
		    		sfp.sfFilterToApply = ONLY_ACTION_SCRIPT_FILTER;

		    		break;

		    	case 'u':
		    		sfp.sfFilterToApply = ONLY_PHONE_FILTER;

		    		break;

		    	case 'h':
		    		sfp.sfFilterToApply = ONLY_HANDSFREE_FILTER;

		    		break;

		    	case 'm':
		    		sfp.sfFilterToApply = ONLY_MAP_FILTER;

		    	    break;

		    	default:
		    		std::cerr << pcERROR_InvalidFilterType  << std::endl;

		            rad::OptionPrinter::printStandardAppDesc(appName,
			                                                 std::cout,
			                                                 desc,
			                                                 &positionalOptions);
			        return ERROR_IN_COMMAND_LINE;
		    	}
		    }
		}
		catch(boost::program_options::required_option& e) {
			rad::OptionPrinter::formatRequiredOptionError(e);

			std::cerr << pcERROR << e.what() << std::endl << std::endl;

			rad::OptionPrinter::printStandardAppDesc(appName,
	                                                 std::cout,
	                                                 desc,
	                                                 &positionalOptions);
			return ERROR_IN_COMMAND_LINE;
	    }
	    catch(boost::program_options::error& e) {
	    	std::cerr << pcERROR << e.what() << std::endl << std::endl;

	    	rad::OptionPrinter::printStandardAppDesc(appName,
	                                                 std::cout,
	                                                 desc,
	                                                 &positionalOptions);
	        return ERROR_IN_COMMAND_LINE;
	    }

	    if ( vm.count(pcInputFileOptionID) ) {
	    	sfp.sInputFile = vm[pcInputFileOptionID].as<std::string>();
	    }

	    sfp.sOutputFile = sfp.sInputFile;

	    if (vm.count(pcOutputFileOptionID)) {
	    	sfp.sOutputFile = vm[pcOutputFileOptionID].as<std::string>();
	    }
	}
	catch(std::exception& e)
	{
		std::cerr << pcUnhandledErrorMsg << e.what() << pcAppExitErrorMsg << std::endl;

	    return ERROR_UNHANDLED_EXCEPTION;
	}

	return SUCCESS;
}
bool SpewFiltering::ProduceFilteredSpewFiles( SpewFilteringParams & sfp ) {

	namespace pbs = ProcessPBSpews;

	constexpr char * pcBlankInputLineError = "Error: Blank Input File Name Entered";

	if (sfp.sInputFile.empty()) {
		std::cerr << pcBlankInputLineError << std::endl;

		return false;
	}

	std::vector<pbs::sFilterDescriptor> * pfdToProcess;

	switch(sfp.sfFilterToApply) {

	case PRODUCE_EVERY_FILE:
		pfdToProcess = &pbs::ProduceAllFilterDescriptor;

		break;


	case ONLY_ACTION_SCRIPT_FILTER:
		pfdToProcess = &pbs::ProduceActionScriptFilterDescriptor;

		break;


	case ONLY_PHONE_FILTER:
		pfdToProcess = &pbs::ProducePhoneFilterDescriptor;

		break;

	case APPLY_ALL_FILTERS:
		pfdToProcess = &pbs::ProduceInclusiveFilterDescriptor;

		break;

	case ONLY_HANDSFREE_FILTER:
		pfdToProcess = &pbs::ProduceHFPFilterDescriptor;

		break;

	default:
		std::cerr << "Filter Type Not Handled" << std::endl;//GetLastError() << endl;

		return false;
	}

	pbs::GenerateFilteredSpewData pgsdFilterSpew;

	pbs::ReadLineInterfaceFactory rlif;

	std::string  sErrorMsg;

	std::unique_ptr<pbs::ReadLineInterface> rli = rlif.getReadLineInterface(sfp.sInputFile, sErrorMsg);

	bool bNoFilterSuccess = !pgsdFilterSpew
			    				(sfp.sInputFile,
			    				 sfp.sOutputFile,
			    				 ProcessPBSpews::uiMaxFileNameSize,
			    				 *rli,
			    				 *pfdToProcess,
			    				 sErrorMsg);

	if (bNoFilterSuccess)
	{
		std::cerr << "Failed to produce Filtered All Files" << sErrorMsg << std::endl;

		return false;
	}

	return true;
}

}







