// SpewFilterTesting.cpp : Defines the entry point for the console application.
//
#define BOOST_TEST_ALTERNATIVE_INIT_API

//#include "stdafx.h"
#define BOOST_TEST_MODULE SpewFilter_UI
//#define BOOST_AUTO_TEST_MAIN
#define  BOOST_TEST_NO_MAIN 

#include <boost/test/included/unit_test.hpp>
#include "spewfiltering.hpp"
#include <direct.h>
#include <boost/filesystem.hpp>


static std::string UnicodeSource = "UnicodeTestSource_01.txt";
static std::string AnsiSource = "AnsiTestSource_01.txt";

static std::string RelFolder = "..\\Folder SandBox\\";
static std::string SourceFolderPath = "C:\\Spew Filter Test\\Test File Source\\";
static std::string SandoxFolderPath = "C:\\Spew Filter Test\\Folder SandBox\\";
using namespace boost::unit_test;

int main(int  argc,
	char **  argv
	)
{
	char * argv0 = argv[0];

	return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);

}
using namespace SpewFilteringSpace;

const char * progName = "SpewFilter";


struct SpewFilter_fixture
{
	SpewFiltering sfFiltering;

	std::string   sInputFileParent;

	std::string   sInputFileName;

	std::string   sInputOptionID;

	int           iArgumentNumberOfInputFile;

	std::string   sOutputFileParent;

	std::string   sOutputFileName;

	std::string   sOutputOptionID;

	int           iArgumentNumberOfOutputFile;

	std::string   sFilterOptionID;

	std::string   sFilterType;

	int           iArgumentNumberOfFilterOption;

	int           iNumberOfArgs;

	char *        ArguementList[50];

	std::string   sInputFileArg;

	std::string   sOutputFileArg;

	std::string   sFilterOptionArg;

	std::string   sInputFileLeadingWhiteSpace;

	std::string   sInputFileTrailingWhiteSpace;

	std::string   sOutputFileLeadingWhiteSpace;

	std::string   sOutputFileTrailingWhiteSpace;

	std::string   sOptionLeadingWhiteSpace;

	std::string   sOptionTrailingWhiteSpace;

	void ConfigureArgCArgV() {
		iNumberOfArgs = 2;

		std::string sFilePath;

		ArguementList[0] = const_cast<char *>(progName);

		// Input File Name configuration;
		sInputFileArg = sInputOptionID + sInputFileLeadingWhiteSpace + sInputFileParent + sInputFileName + sInputFileTrailingWhiteSpace;

		ArguementList[iArgumentNumberOfInputFile] = const_cast<char *>(sInputFileArg.c_str());

		if (!sOutputFileName.empty()) {
			iNumberOfArgs++;

			sOutputFileArg = sOutputOptionID + sOutputFileLeadingWhiteSpace + sOutputFileParent + sOutputFileName + sOutputFileTrailingWhiteSpace;

			ArguementList[iArgumentNumberOfOutputFile] = const_cast<char *>(sOutputFileArg.c_str());
		}

		if (!sFilterType.empty()) {
			iNumberOfArgs++;

			sFilterOptionArg = sFilterOptionID + sOptionLeadingWhiteSpace + sFilterType + sOptionTrailingWhiteSpace;

			ArguementList[iArgumentNumberOfFilterOption] = const_cast<char *>(sFilterOptionArg.c_str());
		}
	}

	void ClearArgvArgC() {
		sInputFileParent = "";

		sInputFileName = "";

		sInputOptionID = "";

		sInputFileLeadingWhiteSpace = "";

		sInputFileTrailingWhiteSpace = "";

		iArgumentNumberOfInputFile = 1;

		sOutputFileParent = "";

		sOutputFileName = "";

		sOutputOptionID = "";

		sOutputFileLeadingWhiteSpace = "";

		sOutputFileTrailingWhiteSpace = "";

		iArgumentNumberOfOutputFile = 2;

		sFilterOptionID = "";

		sFilterType = "";

		sOptionLeadingWhiteSpace = "";

		sOptionTrailingWhiteSpace = "";

		iArgumentNumberOfFilterOption = 3;

		iNumberOfArgs = 0;

		for (int i = 0; i < 50; i++) ArguementList[i] = nullptr;

		// Clear the Sandbox for previous test result files
		auto iDirChanged = _chdir(SandoxFolderPath.c_str());

		if (iDirChanged >= 0) {
			boost::filesystem::path full_path(boost::filesystem::current_path());

			boost::filesystem::directory_iterator it(full_path), eod;

			for (boost::filesystem::path const &p : it)
			{
				if (is_regular_file(p))
				{
					boost::filesystem::remove_all(p);
				}
			}
		}
		
		// Put the test files in the Sandbox
		iDirChanged = _chdir(SourceFolderPath.c_str());

		if (iDirChanged >= 0) {
			boost::filesystem::path full_path(boost::filesystem::current_path());

			boost::filesystem::directory_iterator it(full_path), eod;

			for (boost::filesystem::path const &sourcePath : it)
			{
				if (is_regular_file(sourcePath))
				{
					std::string dest = SandoxFolderPath;// +sourcePath.filename.string();

					boost::filesystem::path sourceName = sourcePath.filename();

					dest += sourceName.string();

					boost::filesystem::path destPath(dest);

					boost::filesystem::copy_file(sourcePath, destPath);
				}
			}
		}
	}
};

typedef struct SpewFilter_fixture SpewFilterTestFixture;

BOOST_AUTO_TEST_SUITE(test_SpewFilter);

#define SPEW_FILTER_TEST_CASE(name_) \
    BOOST_FIXTURE_TEST_CASE(SpewFilter_##name_, SpewFilter_fixture)

BOOST_AUTO_TEST_SUITE(test_SpewFilter_InputFileSel);

SPEW_FILTER_TEST_CASE(InptFile_RelPath_NoKey_No_Whitespace)
{
	ClearArgvArgC();
	
	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;
	
	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(NotExistInptFile_RelPath_NoKey_No_Whitespace)
{
	ClearArgvArgC();

	sInputFileName = "..//Debug//Garbage.txt";

	sInputFileParent = "";

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == ERROR_FILE_WAS_NOT_FOUND);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_SpewFilter_OutputFileSel);
SPEW_FILTER_TEST_CASE(NotExistOutptFile_RelPath_NoKey_No_Whitespace)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sOutputFileName = "..//Debug//Garbage.txt";

	sOutputFileParent = "";

	sOutputOptionID = "-o ";

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == ERROR_OUTPUT_FILE_PATH_NOT_FOUND);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

