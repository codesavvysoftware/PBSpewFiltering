/*
 * SpewFiltering.hpp
 *
 *  Created on: Apr 2, 2015
 *      Author: tomh
 */

#ifndef SPEWFILTERING_HPP_
#define SPEWFILTERING_HPP_
#include <string>

namespace SpewFilteringSpace {

const size_t SUCCESS                          = 0;
const size_t ERROR_IN_COMMAND_LINE            = 1;
const size_t ERROR_UNHANDLED_EXCPTN           = 2;
const size_t HELP_SUCCESSFUL                  = 3;
const size_t ERROR_FILE_NOT_FOUND             = 4;
const size_t ERROR_OUTPUT_FILE_PATH_NOT_FOUND = 5;

class SpewFiltering {
public:
	SpewFiltering() {};

	virtual ~SpewFiltering() {};

	typedef enum {
		PRODUCE_EVERY_FILE,
		APPLY_ALL_FILTERS,
		ONLY_PHONE_FILTER,
		ONLY_ACTION_SCRIPT_FILTER,
		ONLY_HANDSFREE_FILTER,
		ONLY_MAP_FILTER,
		ONLY_MTP_FILTER,
		NO_FILTER
	} SPEW_FILTER;


	typedef struct {
		std::string sInputFile;
		std::string sOutputFile;
		SPEW_FILTER sfFilterToApply;
	} SpewFilteringParams;

	virtual int ConfigureFilteringParams( int & argc, char ** & argv, SpewFilteringParams & sfp );

	bool ProduceFilteredSpewFiles( SpewFilteringParams & sfp );

	bool ApplySpewFiltering( SpewFilteringParams & sfp );
};
}




#endif /* SPEWFILTERING_HPP_ */
