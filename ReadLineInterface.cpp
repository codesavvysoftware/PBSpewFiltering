#include "stdafx.h"
#include "ReadLineInterface.h"
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <string>
#include <locale>
#include <codecvt>
#include <sstream>
#include <string>

using namespace std;


namespace ProcessPBSpews
{
	unique_ptr<ReadLineInterface> ReadLineInterfaceFactory::getReadLineInterface(FILE * fSrc, string & sErrorMsg)
	{
		TextFileType tft;

		string s;

		sErrorMsg = "";

		if (FetchTextFileType(fSrc, tft, s))
		{
			switch (tft)
			{
			case UTF16LittleEndian:

				return unique_ptr<ReadLineInterface>(new ReadUnicodeLine());

			case UTF16BigEndian:

				return unique_ptr<ReadLineInterface>(new ReadUnicodeBigEndianLine());

			case UTF8:
			case ANSI:

				return unique_ptr<ReadLineInterface>(new ReadASCIILine());

			defualt:

				break;

			}
		}

		sErrorMsg = "Text File Type Not Processed By this Program: " + s;

		return unique_ptr<ReadLineInterface>(nullptr);
	}

	bool ReadLineInterfaceFactory::FetchTextFileType(FILE * fSrc, TextFileType & tftFileType, string & sErrorMsgRet) {
		if (!fSrc) return false;

		ostringstream osErrorMsgRet;

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

		long lSize = sizeof(wchar_t);

		size_t result;

		bool bLineFeedNotRead = true;

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

				setlocale(LC_ALL, "en-US");

				const locale locale("");

				pWideBuf++;

				*pWideBuf = L'\0';

				wstring ws(wideBuffer);

				typedef codecvt_utf8<wchar_t> convert_typeX;

				wstring_convert<convert_typeX, wchar_t> converterX;

				unsigned int ui = ws.length();

				string s = converterX.to_bytes(ws);

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

		long lSize = sizeof(wchar_t);

		size_t result;

		bool bLineFeedNotRead = true;

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

				setlocale(LC_ALL, "en-US");

				const locale locale("");

				pWideBuf++;

				*pWideBuf = L'\0';

				wstring ws(wideBuffer);

				typedef codecvt_utf8<wchar_t> convert_typeX;

				wstring_convert<convert_typeX, wchar_t> converterX;

				unsigned int ui = ws.length();

				string s = converterX.to_bytes(ws);

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
		(const char *                pcFolder,
		const char *                pcInputFile,
		unsigned int                uiMaxFileNameSize,
		ReadLineInterface &         ReadSourceLine,
		vector<sFilterDescriptor> & lFilterDescriptors,
		string &                    sErrorMsgRet)
	{
		if (!pcFolder || !pcInputFile) return false;

		ostringstream osErrorMsgRet;

		osErrorMsgRet << "";

		const char cTempFilteredFileName[] = "Temp.txt";

		const char cTempRenameFileName[] = "Temp.$$$";

		if (!SetCurrentDirectory(pcFolder))
		{
			osErrorMsgRet << "Could not set directory path for the file using default path" << endl;

			sErrorMsgRet = osErrorMsgRet.str();

			return false;
		}

		auto iCurrentDescriptor = 0;

		string sInputFileName(pcInputFile);

		auto iFileExtPos = sInputFileName.find('.');

		if (iFileExtPos < 0)
		{
			sInputFileName += ".txt";
		}

		FILE * fSrc = fopen(sInputFileName.c_str(), "r");

		if (!fSrc)
		{
			osErrorMsgRet << "Error Opening Source File" << GetLastError() << endl;

			sErrorMsgRet = osErrorMsgRet.str();

			return false;
		}

		FILE * fDest = fopen(cTempFilteredFileName, "w+");

		if (!fDest)
		{
			fclose(fSrc);

			osErrorMsgRet << "Error opening destination file" << GetLastError() << endl;

			sErrorMsgRet = osErrorMsgRet.str();

			return false;
		}

		bool bUnfilteredFileLinesCollected = CollectUnfilteredFileLines(fSrc, ReadSourceLine, sErrorMsgRet);

		if (bUnfilteredFileLinesCollected) {

			//for (vector<string>::iterator iter = FileLines.begin(); iter != FileLines.end(); iter++)
			for (string& iterS : UnfilteredFileLines) {
				fwrite(iterS.c_str(), sizeof(char), strlen(iterS.c_str()), fDest);

				fflush(fDest);
			}
		}
		
		fclose(fSrc);

		fclose(fDest);

		if (!bUnfilteredFileLinesCollected) return false;
		
		remove(cTempRenameFileName);

		string sOutput(pcInputFile);

		iFileExtPos = sOutput.find('.');

		if (iFileExtPos < 0)
		{
			sOutput += "_New_Unfiltered.txt";
		}
		else
		{
			auto iNumberToErase = sOutput.length();

			iNumberToErase -= iFileExtPos;

			sOutput.erase(iFileExtPos, iNumberToErase);

			sOutput += "_New_Unfiltered.txt";
		}

		//vector<string> sToFilterOn = { "Garbage", "Data" };
		auto bOutputRenamed = false;

		if (fDest = fopen(sOutput.c_str(), "r"))
		{
			fclose(fDest);

			if (!rename(sOutput.c_str(), cTempRenameFileName))
			{
				bOutputRenamed = true;
			}

		}

		rename(cTempFilteredFileName, sOutput.c_str());

		if (bOutputRenamed)
		{
			remove(cTempRenameFileName);
		}


		for (sFilterDescriptor& InputList : lFilterDescriptors)
		{
			fDest = fopen(cTempFilteredFileName, "w+");

			if (!fDest)
			{
				osErrorMsgRet << "Error opening destination file" << GetLastError() << endl;

				sErrorMsgRet = osErrorMsgRet.str();

				continue;
			}

			CollectFilteredFileLines(UnfilteredFileLines, InputList);

			for (string & iterS : InputList.FilteredFileLines) {
				
				fwrite(iterS.c_str(), sizeof(char), strlen(iterS.c_str()), fDest);

				fflush(fDest);
			}

			fclose(fDest);

			bOutputRenamed = false;

			string sOutput(pcInputFile);

			iFileExtPos = sOutput.find('.');

			if (iFileExtPos < 0)
			{
				sOutput += InputList.sAppendToFileName;
			}
			else
			{
				int iNumberToErase = sOutput.length();

				iNumberToErase -= iFileExtPos;

				sOutput.erase(iFileExtPos, iNumberToErase);

				sOutput += InputList.sAppendToFileName;
			}

			if (fDest = fopen(sOutput.c_str(), "r"))
			{
				fclose(fDest);

				if (!rename(sOutput.c_str(), cTempRenameFileName))
				{
					bOutputRenamed = true;
				}

			}

			rename(cTempFilteredFileName, sOutput.c_str());

			if (bOutputRenamed)
			{
				remove(cTempRenameFileName);
			}
		}

		return true;
	}

	void GenerateFilteredSpewData::CollectFilteredFileLines(vector<string>& UnfilteredFileLines, sFilterDescriptor& InputList)
	{
		//for (vector<string>::iterator iter = FileLines.begin(); iter != FileLines.end(); iter++)
		for (string& iterS : UnfilteredFileLines)
		{
			vector<string> vStart = InputList.sToFilterOn;

			for (string& sFilter : vStart)// = StartIterating; iterFilters != EndIterating; iterFilters++ )
			{
				auto found = iterS.find(sFilter.c_str());// iterFilters->c_str());//   itInputList->sToFilterOn[i]);

				if (found != string::npos)
				{
					InputList.FilteredFileLines.push_back(iterS);
				}
			}
		}
	}

	bool GenerateFilteredSpewData::CollectUnfilteredFileLines(FILE * fSrc, ReadLineInterface &  ReadSourceLine, string & sErrorMsgRet) {
		char cLineBuffer[100000];

		ostringstream osErrorMsgRet;

		osErrorMsgRet << "";

		unsigned int uiNumberOfCharsRead = 0;

		unsigned int uiBufferSizeMax = uiMaxBufferSize;

		auto bCollectionSuccess = true;

		auto bEndOfFile = false;

		auto pcLineBuffer = cLineBuffer;

		auto bFileHeaderInfoRead = false;

		while (!bEndOfFile)
		{

			auto bLineReadNotSuccessful = true;

			try
			{
				bLineReadNotSuccessful = !ReadSourceLine(fSrc, pcLineBuffer, uiBufferSizeMax, uiNumberOfCharsRead, bEndOfFile);
			}
			catch (...)
			{
				osErrorMsgRet << "Spew source incompatible with supported test file formats: " << GetLastError() << endl;

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
					osErrorMsgRet << "Read Buffer Not Big Enough" << endl;

					break;
				}
				else
				{
					osErrorMsgRet << "Error Reading File" << endl;

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
}