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

	ReadLineInterface::ReadLineInterface()
	{
	}


	ReadLineInterface::~ReadLineInterface()
	{
	}

	bool ReadUnicodeLine::operator()
		(FILE * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile)
	{
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
			catch (int e)
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


	GenerateFilteredSpewData::GenerateFilteredSpewData()
	{
	}


	GenerateFilteredSpewData::~GenerateFilteredSpewData()
	{
	}
	void GenerateFilteredSpewData::StripLeadingBlanks(char * pcLineBuffer, char ** pcNewBuffer)
	{
		int i;

		for (i = 0; (pcLineBuffer[i] != '\n') && (pcLineBuffer[i] != '\r'); i++)
		{
			if ((pcLineBuffer[i] != ' ') && (pcLineBuffer[i] != '\t') && ((pcLineBuffer[i] >= 0x20) && (pcLineBuffer[i] <= 0x7f)))
			{
				break;
			}
		}

		*pcNewBuffer = &pcLineBuffer[i];
	}

	bool GenerateFilteredSpewData::FindFieldStr(const char * pcFields[], char * pcStartOfField)
	{
		if (pcFields[0] == nullptr)
		{
			return true;
		}

		int i = 0;

		const char * pcFieldStr;

		bool bFieldFound = false;

		while (pcFields[i] != nullptr)
		{
			pcFieldStr = pcFields[i];

			int iLenToCheck = strlen(pcFieldStr);

			unsigned int uiCurrentPos = 0;

			while (true)
			{
				if (!strncmp(pcFieldStr, &pcStartOfField[uiCurrentPos], iLenToCheck))

					/*		if (
					(pcStartOfPID[uiCurrentPos]     == 'P')
					&& (pcStartOfPID[uiCurrentPos + 1] == 'I')
					&& (pcStartOfPID[uiCurrentPos + 2] == 'D')
					&& (pcStartOfPID[uiCurrentPos + 3] == ':')
					)*/
				{
					pcStartOfField += uiCurrentPos;

					bFieldFound = true;

					break;
				}
				else if (pcStartOfField[uiCurrentPos] == '\n')
				{
					break;
				}
				else
				{
					uiCurrentPos++;
				}
			}

			i++;
		}

		return bFieldFound;
	}
	bool GenerateFilteredSpewData::operator()
		(const char * pcFolder, const char * pcInputFile, unsigned int uiMaxFileNameSize, vector<sFilterDescriptor> & lFilterDescriptors, string & sErrorMsgRet)
	{
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

		int iCurrentDescriptor = 0;
		
		string sInputFileName(pcInputFile);

		int iFileExtPos = sInputFileName.find('.');

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

		bool bProducedAFilteredSpew = false;

		char cLineBuffer[100000];

		unsigned int uiBufferSizeMax = 100000;

		unsigned int uiNumberOfCharsRead;

		bool bEndOfFile = false;

		char * pcLineBuffer = cLineBuffer;

		bool bFileHeaderInfoRead = false;

		ReadUnicodeLine rulReadUnicode;

		ReadASCIILine rulReadASCII;

		while (!bEndOfFile)
		{

			bool bLineReadSuccess = false;
			
			try
			{
				bLineReadSuccess = rulReadUnicode(fSrc, pcLineBuffer, uiBufferSizeMax, uiNumberOfCharsRead, bEndOfFile);
			}
			catch (int e)
			{
				osErrorMsgRet << "Error with source file probably not saved as in Unicode format.  Hopefully will be fixed shortly so all input format work" << GetLastError() << endl;

				sErrorMsgRet = osErrorMsgRet.str();

				return false;
			}

			
			if (bLineReadSuccess)
			{
				if (bEndOfFile)
				{
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

			int i = 0;

			for (i = 0; pcLineBuffer[i] != '\n'; i++)
			{
				if (pcLineBuffer[i] == '\r')
				{
					pcLineBuffer[i] = '\n';
				}
			}

			if (pcLineBuffer[0] != '\n')
			{
				FileLines.emplace_back(pcLineBuffer);

				fwrite(pcLineBuffer, sizeof(char), strlen(pcLineBuffer), fDest);

				fflush(fDest);


			}
		}

		if (!bEndOfFile)
		{
			sErrorMsgRet = osErrorMsgRet.str();

			return false;

		}

		fclose(fSrc);

		fclose(fDest);

		remove(cTempRenameFileName);

		string sOutput(pcInputFile);

		iFileExtPos = sOutput.find('.');

		if (iFileExtPos < 0)
		{
			sOutput += "_New_Unfiltered.txt";
		}
		else
		{
			int iNumberToErase = sOutput.length();

			iNumberToErase -= iFileExtPos;

			sOutput.erase(iFileExtPos, iNumberToErase);
			//erase(iFileExtPos, sOutput.end());

			sOutput += "_New_Unfiltered.txt";
		}

		//vector<string> sToFilterOn = { "Garbage", "Data" };
		bool bOutputRenamed = false;

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


		for (vector<sFilterDescriptor>::iterator itInputList = lFilterDescriptors.begin(); itInputList != lFilterDescriptors.end(); itInputList++)
		{
			fDest = fopen(cTempFilteredFileName, "w+");

			if (!fDest)
			{
				osErrorMsgRet << "Error opening destination file" << GetLastError() << endl;

				sErrorMsgRet = osErrorMsgRet.str();

				continue;
			}

			for (vector<string>::iterator iter = FileLines.begin(); iter != FileLines.end(); iter++)
			{
				vector<string>::iterator iterFilters = itInputList->sToFilterOn.begin();

				vector<string>::iterator iterFiltersEnd = itInputList->sToFilterOn.end();

				while (iterFilters != iterFiltersEnd)
				//while (0 != itInputList->sToFilterOn[i].length())
				{
					size_t found = iter->find(iterFilters->c_str());//   itInputList->sToFilterOn[i]);

					if (found != string::npos)
					{
						fwrite(iter->c_str(), sizeof(char), strlen(iter->c_str()), fDest);

						fflush(fDest);

						break;
					}
					iterFilters++;
				}
			}

			fclose(fDest);

			bOutputRenamed = false;

			string sOutput(pcInputFile);

			iFileExtPos = sOutput.find('.');

			if (iFileExtPos < 0)
			{
				sOutput += itInputList->sAppendToFileName;
			}
			else
			{
				int iNumberToErase = sOutput.length();

				iNumberToErase -= iFileExtPos;

				sOutput.erase(iFileExtPos, iNumberToErase);
				//erase(iFileExtPos, sOutput.end());

				sOutput += itInputList->sAppendToFileName;
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

}