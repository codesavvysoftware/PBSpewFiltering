//============================================================================
// Name        : SpewFilterTesting.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// SpewFilterTesting.cpp : Defines the entry point for the console application.
//
#define BOOST_TEST_ALTERNATIVE_INIT_API

//#include "stdafx.h"
#define BOOST_TEST_MODULE SpewFilter_UI
//#define BOOST_AUTO_TEST_MAIN
#define  BOOST_TEST_NO_MAIN

#include <boost/test/included/unit_test.hpp>
#include "SpewFiltering.hpp"
//#include <direct.h>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS //#include <boost/filesystem.hpp>
#include <fstream>


static std::string UnicodeSource = "UnicodeTestSource_01.txt";
static std::string AnsiSource = "AnsiTestSource_01.txt";
#ifdef GCC
#include <errno.h>
static std::string sTestOutputFileName = "ATestforSeparateOutputFiles";
static std::string RelFolder = "..//Folder_Sandbox//";
static std::string SourceFolderPath = "//home//tomh//SpewFilterTesting//Test_File_Source//";
static std::string SandboxFolderPath = "//home//tomh//SpewFilterTesting//Folder_Sandbox//";
static std::string SandboxOutputFolderPath = "//home//tomh//SpewFilterTesting//Output_Files_Go_Here//";
static std::string RelOutputFolder = "..\\Output_Files_Go_Here\\";
#else
static std::string sTestOutputFileName = "A Test for Separate Output Files";
static std::string RelFolder = "..\\Folder SandBox\\";
static std::string SourceFolderPath = "C:\\Spew Filter Test\\Test File Source\\";
static std::string SandboxFolderPath = "C:\\Spew Filter Test\\Folder SandBox\\";
static std::string SandboxOutputFolderPath = "C:\\Spew Filter Test\\Output Files Can Go Here\\";
static std::string RelOutputFolder = "..\\Output Files Can Go Here\\";
#endif
static std::string sInputFileOption = "-i ";
static std::string sOutputFileOption = "-o ";
static std::string sFilterAbvOption = "-t ";
static std::string sFilterFullOption = "--TypeOfFilter=";
static std::string sWhiteSpace_Leading = " \t\t  \t\t   \t\t\t   \t  \t    \t\t\t    ";
static std::string sWhiteSpace_Trailing = "\t\t\t      \t \t \t \t \t   ";

using namespace boost::unit_test;
struct LogToFile
{
	LogToFile()
	{
		std::string logFileName(boost::unit_test::framework::master_test_suite().p_name);
		logFileName.append(".xml");
		logFile.open(logFileName.c_str());
		boost::unit_test::unit_test_log.set_stream(logFile);
	}
	~LogToFile()
	{
		boost::unit_test::unit_test_log.test_finish();
		logFile.close();
		boost::unit_test::unit_test_log.set_stream(std::cout);
	}
	std::ofstream logFile;
};

BOOST_GLOBAL_FIXTURE(LogToFile);
/*struct MyConfig {
	MyConfig() : test_log("example.log")  { boost::unit_test::unit_test_log.set_stream(test_log); }
	~MyConfig()                             { boost::unit_test::unit_test_log.set_stream(std::cout); }

	std::ofstream test_log;
};

//____________________________________________________________________________//

BOOST_GLOBAL_FIXTURE(MyConfig); */

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

		int iNameLen = strlen(SandboxFolderPath.c_str());
		// Clear the Sandbox for previous test result files
		auto iDirChanged = chdir(SandboxFolderPath.c_str());

		if (iDirChanged >= 0) {
			boost::filesystem::path full_path(boost::filesystem::current_path());

			boost::filesystem::directory_iterator it(full_path), end;

			//directory_iterator end ;
			for (boost::filesystem::path const &p : it)
			{
				if (is_regular_file(p))
				{
					boost::filesystem::remove_all(p);
				}
			}
		}

		// Put the test files in the Sandbox
		iDirChanged = chdir(SourceFolderPath.c_str());

		if (iDirChanged >= 0) {
			boost::filesystem::path full_path(boost::filesystem::current_path());

			boost::filesystem::directory_iterator it(full_path), end;

			//directory_iterator end ;
			//for( it ; it != end ; ++it )
			for (boost::filesystem::path const &sourcePath : it)
			{
				//boost::filesystem::path sourcePath = *it;

				if (is_regular_file(sourcePath))
				{
					std::string dest = SandboxFolderPath;// +sourcePath.filename.string();

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

// Testing valid input file selections
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

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_No_Whitespace)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_NoKey_Whitespace)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputFileLeadingWhiteSpace = sWhiteSpace_Leading;

	sInputFileTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_Whitespace)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	sInputFileLeadingWhiteSpace = sWhiteSpace_Leading;

	sInputFileTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_AbsltPath_NoKey_No_Whitespace)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = SandboxFolderPath;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_AbsltPath_Key_No_Whitespace)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = SandboxFolderPath;

	sInputOptionID = sInputFileOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_AbsltPath_NoKey_Whitespace)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = SandboxFolderPath;

	// mix them up once in a while :)
	sInputFileLeadingWhiteSpace = sWhiteSpace_Trailing;

	sInputFileTrailingWhiteSpace = sWhiteSpace_Leading;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_AbsltPath_Key_Whitespace)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = SandboxFolderPath;

	sInputOptionID = sInputFileOption;

	sInputFileLeadingWhiteSpace = sWhiteSpace_Leading;

	sInputFileTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}


// Testing valid filter option selections
SPEW_FILTER_TEST_CASE(InptFile_RelPath_NoKey_NoW_FFiltAll)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "a";

	sFilterOptionID = sFilterFullOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_FFiltEvery)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "e";

	sFilterOptionID = sFilterFullOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_FFiltHFP)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "h";

	sFilterOptionID = sFilterFullOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_FFiltAS)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "s";

	sFilterOptionID = sFilterFullOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_FFiltBT)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "p";

	sFilterOptionID = sFilterFullOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_FFiltMap)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "m";

	sFilterOptionID = sFilterFullOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}
SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_FFiltUSB)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "u";

	sFilterOptionID = sFilterFullOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}
SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_AFiltAll)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "a";

	sFilterOptionID = sFilterAbvOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_AFiltEvery)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "e";

	sFilterOptionID = sFilterAbvOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_AFiltHFP)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "h";

	sFilterOptionID = sFilterAbvOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_AFiltAS)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "s";

	sFilterOptionID = sFilterAbvOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_AFiltBT)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "p";

	sFilterOptionID = sFilterAbvOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_AFiltMap)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "m";

	sFilterOptionID = sFilterAbvOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}
SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_NoW_AFiltUSB)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "u";

	sFilterOptionID = sFilterAbvOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}
SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_FFiltAll)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "a";

	sFilterOptionID = sFilterFullOption;

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_FFiltEvery)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "e";

	sFilterOptionID = sFilterFullOption;

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_FFiltHFP)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "h";

	sFilterOptionID = sFilterFullOption;

	// mixing it up
	sOptionLeadingWhiteSpace = sWhiteSpace_Trailing;

	sOptionTrailingWhiteSpace = sWhiteSpace_Leading;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_FFiltAcnScrpt)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "s";

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	sFilterOptionID = sFilterFullOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_FFiltBT)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "p";

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	sFilterOptionID = sFilterFullOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_FFiltMap)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "m";

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	sFilterOptionID = sFilterFullOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}
SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_FFiltUSB)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "u";

	sFilterOptionID = sFilterFullOption;

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}
SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_AFiltAll)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "a";

	sFilterOptionID = sFilterAbvOption;

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_AFiltEvery)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "e";

	sFilterOptionID = sFilterAbvOption;

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_AFiltHFP)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "h";

	sFilterOptionID = sFilterAbvOption;

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_AFiltActnScrpt)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "s";

	sFilterOptionID = sFilterAbvOption;

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_AFiltBT)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "p";

	sFilterOptionID = sFilterAbvOption;

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_AFiltMap)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "m";

	sFilterOptionID = sFilterAbvOption;

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}
SPEW_FILTER_TEST_CASE(InptFile_RelPath_Key_W_AFiltUSB)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	sInputOptionID = sInputFileOption;

	iArgumentNumberOfFilterOption = iArgumentNumberOfInputFile + 1;

	sFilterType = "u";

	sFilterOptionID = sFilterAbvOption;

	sOptionLeadingWhiteSpace = sWhiteSpace_Leading;

	sOptionTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}

// Testing valid output file selections
SPEW_FILTER_TEST_CASE(InptFile_RelPath_NoKey_NoW_Oabs)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	iArgumentNumberOfOutputFile = iArgumentNumberOfInputFile + 1;

	sOutputFileName = sTestOutputFileName;

	sOutputOptionID = sOutputFileOption;

	sOutputFileParent = SandboxOutputFolderPath;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}
SPEW_FILTER_TEST_CASE(InptFile_RelPath_NoKey_NoW_Orel)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	iArgumentNumberOfOutputFile = iArgumentNumberOfInputFile + 1;

	sOutputFileName = sTestOutputFileName;

	sOutputOptionID = sOutputFileOption;

	sOutputFileParent = RelOutputFolder;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}
SPEW_FILTER_TEST_CASE(InptFile_RelPath_NoKey_W_Oabs)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	iArgumentNumberOfOutputFile = iArgumentNumberOfInputFile + 1;

	sOutputFileName = sTestOutputFileName;

	sOutputOptionID = sOutputFileOption;

	sOutputFileParent = SandboxOutputFolderPath;

	sOutputFileLeadingWhiteSpace = sWhiteSpace_Leading;

	sOutputFileTrailingWhiteSpace = sWhiteSpace_Trailing;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == SUCCESS);
}
SPEW_FILTER_TEST_CASE(InptFile_RelPath_NoKey_W_Orel)
{
	ClearArgvArgC();

	sInputFileName = UnicodeSource;

	sInputFileParent = RelFolder;

	iArgumentNumberOfOutputFile = iArgumentNumberOfInputFile + 1;

	sOutputFileName = sTestOutputFileName;

	sOutputOptionID = sOutputFileOption;

	sOutputFileParent = RelOutputFolder;

	sOutputFileLeadingWhiteSpace = sWhiteSpace_Leading;

	sOutputFileTrailingWhiteSpace = sWhiteSpace_Trailing;

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

	sOutputOptionID = sOutputFileOption;

	ConfigureArgCArgV();

	SpewFiltering::SpewFilteringParams FilterParams;

	char ** pc = &ArguementList[0];

	std::size_t Configured = sfFiltering.ConfigureFilteringParams(iNumberOfArgs, pc, FilterParams);

	BOOST_CHECK(Configured == ERROR_OUTPUT_FILE_PATH_NOT_FOUND);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

