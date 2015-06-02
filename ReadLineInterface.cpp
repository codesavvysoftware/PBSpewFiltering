#include "ReadLineInterface.h"
#include <iostream>
#include <fstream>
#include <locale>
#include <sstream>
#include <string>
#include <string.h>
#include <boost/locale.hpp>
#include <system_error>
#ifdef GCC
#include <errno.h>
#include <stdio.h>
#endif
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

	std::vector<std::string> MTPFilters = { "MPCore:",
		"MPMtpDevice:",
		"AuxMedia:",
		"CSMHandler:",
		"CMedia:",
		"CMtpDevice:",
		"CMTPSourceClass:",
		"MemMapFileURLStream:",
		"CMTPBrowse:",
		"AlbumArt:",
		"Image:",
		"MetadataCollectors:"
	};
	std::vector<std::string> MAPFilters = { "CMAPManager:" };

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
    	},
		{
		    	"_Filtered_MTP.txt",
				MTPFilters
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
	std::vector<sFilterDescriptor> ProduceMTPFilterDescriptor
	{
		{
			"_Filtered_MTP.txt",
			MTPFilters
		}
	};

	std::unique_ptr<ReadLineInterface> ReadLineInterfaceFactory::getReadLineInterface(std::string sFile, std::string & sErrorMsg)
	{
		TextFileType tft;

		std::string s;

		sErrorMsg = "";

		//FILE * fSrc;

		std::ifstream fSrc(const_cast<char *>(sFile.c_str()),std::fstream::binary);

		if (!fSrc.is_open()) {
			sErrorMsg = "Error Opening Source File.  Error";

			return std::unique_ptr<ReadLineInterface>(nullptr);
		}

		bool bFileTypeWasFetched = FetchTextFileType(fSrc, tft, s);

		fSrc.close();

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

	bool ReadLineInterfaceFactory::FetchTextFileType(std::ifstream & fSrc, TextFileType & tftFileType, std::string & sErrorMsgRet) {
		if (!fSrc) return false;

		std::ostringstream osErrorMsgRet;

		osErrorMsgRet << "";

		long lCurrentStreamPos = fSrc.tellg();

		if ( !GetFilePtrPos(fSrc, lCurrentStreamPos) ) return false;

		if ( !SetFilePtrPos(fSrc, 0)) return false;

		char c[20];

		if (!ReadStreamData(fSrc, c, sizeof(char))) return false;

		unsigned char uc = c[0];

		/* Bytes 	Encoding Form
			  00 00 FE FF 	UTF - 32, big - endian
			  FF FE 00 00 	UTF - 32, little - endian
			  FE FF 	        UTF - 16, big - endian
			  FF FE 	        UTF - 16, little - endian
			  EF BB BF 	    UTF - 8 */
		if (uc == 0)
		{
			SetFilePtrPos(fSrc, lCurrentStreamPos);

			tftFileType = UTF32BigEndian;

			return true;
		}

		if (uc == 0xFE)
		{
			SetFilePtrPos(fSrc, lCurrentStreamPos);

			tftFileType = UTF16BigEndian;

			return true;
		}

		if (uc == 0xEF)
		{
			SetFilePtrPos(fSrc, lCurrentStreamPos);

			tftFileType = UTF8;

			return true;
		}

		if (uc == 0xFF)
		{
			if (!SetFilePtrPos(fSrc, 2)) return false;

			if (!ReadStreamData(fSrc, c, sizeof(char))) return false;

			if (c[0] == 0)
			{
				tftFileType = UTF32LittleEndian;
			}
			else
			{
				tftFileType = UTF16LittleEndian;
			}

			fSrc.seekg(lCurrentStreamPos, std::ios::beg);

			return true;
		}

		tftFileType = ANSI;

		SetFilePtrPos(fSrc, lCurrentStreamPos);

		return true;

	}
	ReadLineInterface::ReadLineInterface()
	{
	}


	ReadLineInterface::~ReadLineInterface()
	{
	}

	bool ReadUnicodeLine::operator()
		(std::ifstream * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile)
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
				f->read(reinterpret_cast<char *>(pWideBuf), lSize);
			}
			catch (...)
			{
				return false;
			}

			uiNumRead += lSize;

			if (f->eof())
			{
				bEndOfFile = true;

				break;
			}
			else if (f->bad())
			{
				return false;
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
		(std::ifstream * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile)
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
			f->read(pNarrowBuf, lSize);

			uiNumRead += lSize;

			bool bReadError = false;

			bool bFileEnd = false;

			if (f->bad())
			{
				bReadError = true;
			}
			else if (f->eof())
			{
				bFileEnd = true;
			}


			if (bReadError || bFileEnd)
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
		(std::ifstream * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile)
	{
		if (!f || !pcLineBuffer) return false;

		bEndOfFile = false;

		wchar_t wideBuffer[20000];

		wchar_t * pWideBuf = wideBuffer;

		char EndianReadFirst;

		char EndianReadSecond;

		unsigned int uiNumRead = 0;

		long lSize = 2;

		bool bLineFeedNotRead = true;

		setlocale(LC_ALL, "en-US");

		const std::locale locale("");

		while (bLineFeedNotRead)
		{
			wchar_t wct;

			f->read(reinterpret_cast<char *>(&wct), lSize);

			if (f->bad())
			{
				return false;
			}
			else if (f->eof())
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
		std::ifstream fSrc(const_cast<char *>(sInputFile.c_str()), std::ios::binary);

		if (!fSrc.is_open())
		{
			osErrorMsgRet << "Error Opening Source File " << std::endl;

			sErrorMsgRet = osErrorMsgRet.str();

			return false;
		}

		std::ofstream fDest(const_cast<char *>(sOutputFileUnfiltered.c_str()), std::ios::trunc);

		if (!fDest.is_open())
		{
			fSrc.close();

			osErrorMsgRet << "Error opening unfiltered output file for write " << std::endl;

			sErrorMsgRet = osErrorMsgRet.str();

			return false;
		}

		bool bUnfilteredFileLinesCollected = CollectUnfilteredFileLines(&fSrc, ReadSourceLine, sErrorMsgRet);

		if (bUnfilteredFileLinesCollected) {

			//for (std::vector<std::string>::iterator iter = FileLines.begin(); iter != FileLines.end(); iter++)
			for (std::string& iterS : UnfilteredFileLines) {
				const char * pWriteData = iterS.c_str();

				int    nWrite = strlen(pWriteData);

				fDest.write(pWriteData, nWrite);

				fDest.flush();
			}
		}
		
		fSrc.close();

		fDest.close();

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

			std::ofstream fDest(const_cast<char *>(sOutputFileUnfiltered.c_str()), std::ios::trunc);

			if (!fDest.is_open())
			{
				osErrorMsgRet << "Error opening destination file" << std::endl; //GetLastError() << endl;

				sErrorMsgRet = osErrorMsgRet.str();

				continue;
			}

			CollectFilteredFileLines(UnfilteredFileLines, InputList);

			for (std::string & iterS : InputList.FilteredFileLines) {
				
				const char * pWriteData = iterS.c_str();

				int    nWrite = strlen(pWriteData);

				fDest.write(pWriteData, nWrite);

				fDest.flush();
			}

			fDest.close();

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
				int found = iterS.find(sFilter.c_str());// iterFilters->c_str());//   itInputList->sToFilterOn[i]);

				if (found != std::string::npos)
				{
					InputList.FilteredFileLines.push_back(iterS);
				}
			}
		}
	}

	bool GenerateFilteredSpewData::CollectUnfilteredFileLines
	                    (std::ifstream * fSrc, ReadLineInterface &  ReadSourceLine, std::string & sErrorMsgRet) {
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
		int iFileExtPos = sFileName.find('.');

		if (iFileExtPos < 0) {
			sFileName += sSuffix;
		}
		else {
			int iNumberToErase = sFileName.length();

			iNumberToErase -= iFileExtPos;

			sFileName.erase(iFileExtPos, iNumberToErase);

			unsigned int uiCapacity = sFileName.capacity();

			sFileName += sSuffix;
		}
	}
}
