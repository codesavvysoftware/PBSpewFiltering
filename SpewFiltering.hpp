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

constexpr size_t ERROR_IN_COMMAND_LINE     = 1;
constexpr size_t SUCCESS                   = 0;
constexpr size_t ERROR_UNHANDLED_EXCEPTION = 2;

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
		ONLY_MAP_FILTER
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
