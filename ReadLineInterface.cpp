#include "ReadLineInterface.h"
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <string.h>
#include <boost/locale.hpp>
#include <fstream>

//#include <codecvt>


namespace ProcessPBSpews {
    std::vector<std::string> AsFileFilters = { "ActionScript:" };

    std::vector<std::string> HFPFilters = { "HFP RX:", "HFP TX:" };

    std::vector<std::string> BTPhoneFilters = {
		"HFP RX:",
		"HFP TX:",
		"CBTPairSvc",
		"Phone:",
		"HFPSvc:",
		"CHFPSvcManager:",
		"CBTPairSvc:",
		"HFP:",
		"PhoneCore:",
		"BTPairApi!",
		"PhoneGetConnectedCall:",
		"CSyncManager:",
     	"CMAPManager:" };

    std::vector<std::string> ReadableFilters = { " " };

    std::vector<std::string> InclusiveFiliters = {
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

    std::vector<std::string> MAPFilters = {	"CMAPManager:" };

    std::vector<sFilterDescriptor> ProduceAllFilterDescriptor
    {
    	{
    		"_Filtered_AS.txt",
    		//Only because VS 2013 express version I have doesn't work with this c-11 initialization
    		AsFileFilters //{ { "ActionScript:" } }
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
    	},
    	{
    			"_Filtered_Phone.txt",
    			BTPhoneFilters
    	}
    };

    std::vector<sFilterDescriptor> ProducePhoneFilterDescriptor
    {
		{
			"_Filtered_Phone.txt",
			BTPhoneFilters
		}
    };

    std::vector<sFilterDescriptor> ProduceInclusiveFilterDescriptor
	{
		{
			"_Filtered_All.txt",
			InclusiveFiliters
		}
	};

	std::vector<sFilterDescriptor> ProduceActionScriptFilterDescriptor
	{
		{
			"_Filtered_AS.txt",
			//Only because VS 2013 express version I have doesn't work with this c-11 initialization
			AsFileFilters //{ { "ActionScript:" } }
		}
	};

	std::vector<sFilterDescriptor> ProduceHFPFilterDescriptor
	{
		{
			"_Filtered_HFP.txt",
			HFPFilters
		}
	};
	std::unique_ptr<ReadLineInterface> ReadLineInterfaceFactory::getReadLineInterface(std::string sFile, std::string & sErrorMsg)
	{
		TextFileType tft;

		std::string s;

		sErrorMsg = "";

		FILE * fSrc = fopen(sFile.c_str(), "r");

		if (!fSrc)
		{
			sErrorMsg = "Error Opening Source File";

			return std::unique_ptr<ReadLineInterface>(nullptr);
		}

		bool bFileTypeWasFetched = FetchTextFileType(fSrc, tft, s);

		fclose(fSrc);

		if (bFileTypeWasFetched) {
			switch (tft)
			{
			case UTF16LittleEndian:

				return std::unique_ptr<ReadLineInterface>(new ReadUnicodeLine());

			case UTF16BigEndian:

				return std::unique_ptr<ReadLineInterface>(new ReadUnicodeBigEndianLine());

			case UTF8:
			case ANSI:

				return std::unique_ptr<ReadLineInterface>(new ReadASCIILine());

			defualt:

				break;

			}
		}

		sErrorMsg = "Text File Type Not Processed By this Program: " + s;

		return std::unique_ptr<ReadLineInterface>(nullptr);
	}

	bool ReadLineInterfaceFactory::FetchTextFileType(FILE * fSrc, TextFileType & tftFileType, std::string & sErrorMsgRet) {
		if (!fSrc) return false;

		std::ostringstream osErrorMsgRet;

		osErrorMsgRet << "";

		long lCurrentStreamPos = ftell(fSrc);

		if (fseek(fSrc, 0, SEEK_SET))
		{
			return false;
		}

		char c[20];

		int result = fread(c, 1, 1, fSrc);

		if (result != 1)
		{
			return false;
		}

		unsigned char uc = c[0];

		/* Bytes 	Encoding Form
			  00 00 FE FF 	UTF - 32, big - endian
			  FF FE 00 00 	UTF - 32, little - endian
			  FE FF 	        UTF - 16, big - endian
			  FF FE 	        UTF - 16, little - endian
			  EF BB BF 	    UTF - 8 */
		if (uc == 0)
		{
			fseek(fSrc, lCurrentStreamPos, SEEK_SET);

			tftFileType = UTF32BigEndian;

			return true;
		}

		if (uc == 0xFE)
		{
			fseek(fSrc, lCurrentStreamPos, SEEK_SET);

			tftFileType = UTF16BigEndian;

			return true;
		}

		if (uc == 0xEF)
		{
			fseek(fSrc, lCurrentStreamPos, SEEK_SET);

			tftFileType = UTF8;

			return true;
		}

		if (uc == 0xFF)
		{
			if (fseek(fSrc, 2, SEEK_SET))
			{
				return false;
			}

			int result = fread(c, 1, 1, fSrc);

			if (result != 1)
			{
				return false;
			}

			if (c[0] == 0)
			{
				tftFileType = UTF32LittleEndian;
			}
			else
			{
				tftFileType = UTF16LittleEndian;
			}

			fseek(fSrc, lCurrentStreamPos, SEEK_SET);

			return true;
		}

		tftFileType = ANSI;

		fseek(fSrc, lCurrentStreamPos, SEEK_SET);

		return true;

	}
	ReadLineInterface::ReadLineInterface()
	{
	}


	ReadLineInterface::~ReadLineInterface()
	{
	}

	bool ReadUnicodeLine::operator()
		(FILE * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile)
	{
		if (!f || !pcLineBuffer) return false;

		bEndOfFile = false;

		wchar_t wideBuffer[20000];

		wchar_t * pWideBuf = wideBuffer;

		unsigned int uiNumRead = 0;

		long lSize = 2;

		size_t result;

		bool bLineFeedNotRead = true;

		setlocale(LC_ALL, "en-US");

		const std::locale locale("");

		while (bLineFeedNotRead)
		{
			try
			{
				result = fread(pWideBuf, 1, lSize, f);
			}
			catch (...)
			{
				return false;
			}

			uiNumRead += lSize;

			if (result != lSize)
			{
				bEndOfFile = true;

				break;
			}

			if ((*pWideBuf == L'\n') || (*pWideBuf == L'\r'))
			{
				if ((*pWideBuf == L'\r'))
				{
					*pWideBuf = L'\n';
				}

				pWideBuf++;

				*pWideBuf = L'\0';

				std::string s = boost::locale::conv::utf_to_utf<char>(wideBuffer, pWideBuf);

				strcpy(pcLineBuffer, s.c_str());

				bLineFeedNotRead = false;
			}

			pWideBuf++;
		}

		uiNumberOfCharsRead = uiNumRead;

		return !bLineFeedNotRead;
	}

	bool ReadASCIILine::operator()
		(FILE * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile)
	{
		if (!f || !pcLineBuffer) return false;

		bEndOfFile = false;

		char narrowBuffer[2000];

		char * pNarrowBuf = narrowBuffer;

		unsigned int uiNumRead = 0;

		long lSize = sizeof(char);

		size_t result;

		bool bLineFeedNotRead = true;

		while (bLineFeedNotRead)
		{
			result = fread(pNarrowBuf, 1, lSize, f);

			uiNumRead += lSize;

			if (result != lSize)
			{
				bEndOfFile = true;

				break;
			}


			if ((*pNarrowBuf == '\n') || (*pNarrowBuf == '\r'))
			{
				if ((*pNarrowBuf == '\r'))
				{
					*pNarrowBuf = '\n';
				}

				pNarrowBuf++;

				*pNarrowBuf = '\0';

				strcpy(pcLineBuffer, narrowBuffer);

				bLineFeedNotRead = false;
			}

			pNarrowBuf++;
		}

		uiNumberOfCharsRead = uiNumRead;

		return !bLineFeedNotRead;
	}


	bool ReadUnicodeBigEndianLine::operator()
		(FILE * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile)
	{
		if (!f || !pcLineBuffer) return false;

		bEndOfFile = false;

		wchar_t wideBuffer[20000];

		wchar_t * pWideBuf = wideBuffer;

		char EndianReadFirst;

		char EndianReadSecond;

		unsigned int uiNumRead = 0;

		long lSize = 2;

		size_t result;

		bool bLineFeedNotRead = true;

		setlocale(LC_ALL, "en-US");

		const std::locale locale("");

		while (bLineFeedNotRead)
		{
			wchar_t wct = 0;

			result = fread(&wct, 1, lSize, f);

			if (result != lSize)
			{
				bEndOfFile = true;

				break;
			}

			char cSwap = (wct & 0xff00) >> 8;

			wct &= 0xff;

			wct <<= 8;

			wct |= cSwap;

			*pWideBuf = wct;

			uiNumRead += lSize;

			if ((*pWideBuf == L'\n') || (*pWideBuf == L'\r'))
			{
				if ((*pWideBuf == L'\r'))
				{
					*pWideBuf = L'\n';
				}

				pWideBuf++;

				*pWideBuf = L'\0';

				std::string s = boost::locale::conv::utf_to_utf<char>(wideBuffer, pWideBuf);

				strcpy(pcLineBuffer, s.c_str());

				bLineFeedNotRead = false;
			}

			pWideBuf++;
		}

		uiNumberOfCharsRead = uiNumRead;

		return !bLineFeedNotRead;
	}


	GenerateFilteredSpewData::GenerateFilteredSpewData()
	{
	}


	GenerateFilteredSpewData::~GenerateFilteredSpewData()
	{
	}

	void GenerateFilteredSpewData::StripLeadingBlanks(char * pcLineBuffer, char ** pcNewBuffer)
	{
		if (!pcLineBuffer || !pcNewBuffer || !(*pcNewBuffer)) return;

		auto i = 0;

		for (i = 0; (pcLineBuffer[i] != '\n') && (pcLineBuffer[i] != '\r'); i++)
		{
			if ((pcLineBuffer[i] != ' ') && (pcLineBuffer[i] != '\t') && ((pcLineBuffer[i] >= 0x20) && (pcLineBuffer[i] <= 0x7f)))
			{
				break;
			}
		}

		*pcNewBuffer = &pcLineBuffer[i];
	}

	bool GenerateFilteredSpewData::operator()
		(std::string  &                   sInputFileName,
	     std::string  &                   sOutputFileName,
		 unsigned int                     uiMaxFileNameSize,
		 ReadLineInterface &              ReadSourceLine,
		 std::vector<sFilterDescriptor> & lFilterDescriptors,
		 std::string &                    sErrorMsgRet)
	{
		if (sInputFileName.empty() || sOutputFileName.empty()) return false;

		const std::string sInputFileSuffix(".txt");

		const std::string sOutputFileUnfilteredSuffix("_New_Unfiltered.txt");

		std::ostringstream osErrorMsgRet;

		osErrorMsgRet << "";

		std::string sInputFile = sInputFileName;

		std::string sOutputFileUnfiltered = sOutputFileName;

		AppendFileNameSuffix(sInputFile, sInputFileSuffix);

        AppendFileNameSuffix(sOutputFileUnfiltered,sOutputFileUnfilteredSuffix );

		//
		// Open the input file read it and fix lines terminate at '\r' only saving the output in a temporary file
		//
		FILE * fSrc = fopen(sInputFile.c_str(), "r");

		if (!fSrc)
		{
			osErrorMsgRet << "Error Opening Source File" << std::endl;//GetLastError() << endl;

			sErrorMsgRet = osErrorMsgRet.str();

			return false;
		}

		FILE * fDest = fopen(sOutputFileUnfiltered.c_str(), "w+");

		if (!fDest)
		{
			fclose(fSrc);

			osErrorMsgRet << "Error opening unfiltered output file for write" << std::endl;//GetLastError() << endl;

			sErrorMsgRet = osErrorMsgRet.str();

			return false;
		}

		bool bUnfilteredFileLinesCollected = CollectUnfilteredFileLines(fSrc, ReadSourceLine, sErrorMsgRet);

		if (bUnfilteredFileLinesCollected) {

			//for (std::vector<std::string>::iterator iter = FileLines.begin(); iter != FileLines.end(); iter++)
			for (std::string& iterS : UnfilteredFileLines) {
				fwrite(iterS.c_str(), sizeof(char), strlen(iterS.c_str()), fDest);

				fflush(fDest);
			}
		}
		
		fclose(fSrc);

		fclose(fDest);

		if (!bUnfilteredFileLinesCollected) return false;
		
		//
		// At this point the input file has been read and processed.  All lines that end in \r only have been changed
		// so that they now contain a \n instead.  sTempFilteredFie contains the processed sInputFileName file.
		//

		for (sFilterDescriptor& InputList : lFilterDescriptors)
		{
			std::string sOutputFileUnfiltered = sOutputFileName;

	        AppendFileNameSuffix(sOutputFileUnfiltered,InputList.sAppendToFileName );

	        InputList.sFileNameToWrite = sOutputFileUnfiltered;

			fDest = fopen(sOutputFileUnfiltered.c_str(), "w+");

			if (!fDest)
			{
				osErrorMsgRet << "Error opening destination file" << std::endl; //GetLastError() << endl;

				sErrorMsgRet = osErrorMsgRet.str();

				continue;
			}

			CollectFilteredFileLines(UnfilteredFileLines, InputList);

			for (std::string & iterS : InputList.FilteredFileLines) {
				
				fwrite(iterS.c_str(), sizeof(char), strlen(iterS.c_str()), fDest);

				fflush(fDest);
			}

			fclose(fDest);

		}

		return true;
	}

	void GenerateFilteredSpewData::CollectFilteredFileLines(std::vector<std::string>& UnfilteredFileLines, sFilterDescriptor& InputList)
	{
		//for (std::vector<std::string>::iterator iter = FileLines.begin(); iter != FileLines.end(); iter++)
		for (std::string& iterS : UnfilteredFileLines)
		{
			std::vector<std::string> vStart = InputList.sToFilterOn;

			for (std::string& sFilter : vStart)// = StartIterating; iterFilters != EndIterating; iterFilters++ )
			{
				auto found = iterS.find(sFilter.c_str());// iterFilters->c_str());//   itInputList->sToFilterOn[i]);

				if (found != std::string::npos)
				{
					InputList.FilteredFileLines.push_back(iterS);
				}
			}
		}
	}

	bool GenerateFilteredSpewData::CollectUnfilteredFileLines
	                    (FILE * fSrc, ReadLineInterface &  ReadSourceLine, std::string & sErrorMsgRet) {
		char cLineBuffer[100000];

		std::ostringstream osErrorMsgRet;

		osErrorMsgRet << "";

		unsigned int uiNumberOfCharsRead = 0;

		unsigned int uiBufferSizeMax = uiMaxBufferSize;

		auto bEndOfFile = false;

		auto pcLineBuffer = cLineBuffer;

		while (!bEndOfFile)
		{

			auto bLineReadNotSuccessful = true;

			try
			{
				bLineReadNotSuccessful = !ReadSourceLine(fSrc, pcLineBuffer, uiBufferSizeMax, uiNumberOfCharsRead, bEndOfFile);
			}
			catch (...)
			{
				osErrorMsgRet << "Spew source incompatible with supported test file formats: " << std::endl; //GetLastError() << endl;

				sErrorMsgRet = osErrorMsgRet.str();

				return false;
			}

			if (bLineReadNotSuccessful)
			{
				if (bEndOfFile)
				{
					bEndOfFile = true;

					break;
				}
				else if (uiNumberOfCharsRead == uiBufferSizeMax)
				{
					osErrorMsgRet << "Read Buffer Not Big Enough" << std::endl;

					break;
				}
				else
				{
					osErrorMsgRet << "Error Reading File" << std::endl;

					break;
				}
			}

			StripLeadingBlanks(pcLineBuffer, &pcLineBuffer);

			for (auto i = 0; pcLineBuffer[i] != '\n'; i++)
			{
				if (pcLineBuffer[i] == '\r')
				{
					pcLineBuffer[i] = '\n';
				}
			}

			if (pcLineBuffer[0] != '\n')
			{
				UnfilteredFileLines.emplace_back(pcLineBuffer);
			}
		}

		if (!bEndOfFile)
		{
			sErrorMsgRet = osErrorMsgRet.str();

			return false;
		}

		return true;
	}
	void GenerateFilteredSpewData::AppendFileNameSuffix( std::string & sFileName, const std::string & sSuffix )
	{
		auto iFileExtPos = sFileName.find('.');

		if (iFileExtPos < 0) {
			sFileName += sSuffix;
		}
		else {
			auto iNumberToErase = sFileName.length();

			iNumberToErase -= iFileExtPos;

			sFileName.erase(iFileExtPos, iNumberToErase);

			sFileName += sSuffix;
		}
	}
}
