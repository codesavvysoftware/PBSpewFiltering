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
			result = fread(pWideBuf, 1, lSize, f);

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

	void ProduceFilteredSpewData::StripLeadingBlanks(char * pcLineBuffer, char ** pcNewBuffer)
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

	bool ProduceFilteredSpewData::FindFieldStr(const char * pcFields[], char * pcStartOfField)
	{
		int i = 0;

		const char * pcFieldStr;

		bool bFieldFound = false;

		while (pcFields[i] != NULL)
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
	bool ProduceFilteredSpewData::operator()
		(const char * pcFolder, const char * pcInputFile, const char * pcOutputFile, const char * pcFilter[], string & sErrorMsgRet)
	{
		char cPath[1000];

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

		FILE * fSrc = fopen(pcInputFile, "r");

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

			if (!rulReadUnicode(fSrc, pcLineBuffer, uiBufferSizeMax, uiNumberOfCharsRead, bEndOfFile))
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

		if (!rename(cTempFilteredFileName, cTempRenameFileName))
		{
			FILE * fSrc = fopen(cTempRenameFileName, "r");

			if (!fSrc)
			{
				osErrorMsgRet << "Error Opening stripped leading blanks file" << GetLastError() << endl;

				sErrorMsgRet = osErrorMsgRet.str();

				return false;
			}

			FILE * fDest = fopen(cTempFilteredFileName, "w+");

			if (!fDest)
			{
				fclose(fSrc);

				osErrorMsgRet << "Error opening filtered  file" << GetLastError() << endl;

				sErrorMsgRet = osErrorMsgRet.str();

				return false;
			}


			bEndOfFile = false;

			pcLineBuffer = cLineBuffer;

			char * pc = pcLineBuffer;

			while (!bEndOfFile)
			{

				if (!rulReadASCII(fSrc, pcLineBuffer, uiBufferSizeMax, uiNumberOfCharsRead, bEndOfFile))
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

				if (FindFieldStr(pcFilter, pc))
				{
					fwrite(cLineBuffer, sizeof(char), strlen(cLineBuffer), fDest);

					fflush(fDest);
				}
			}


			fclose(fSrc);
			fclose(fDest);

			if (!bEndOfFile)
			{
				sErrorMsgRet = osErrorMsgRet.str();

				return false;

			}

			remove(cTempRenameFileName);

			if (strlen(pcOutputFile))
			{
				bool bOutputRenamed = false;

				if (fDest = fopen(pcOutputFile, "r"))
				{
					fclose(fDest);

					if (!rename(pcOutputFile,cTempRenameFileName))
					{
						bOutputRenamed = true;
					}

				}


				rename(cTempFilteredFileName, pcOutputFile);

				if (bOutputRenamed)
				{
					remove(cTempRenameFileName);
				}
			}

			bProducedAFilteredSpew = true;
		}

		return bProducedAFilteredSpew;
	}
}