#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include <array>
#include <memory>
using namespace std;

namespace ProcessPBSpews {
	const unsigned int uiMaxFileNameSize = 150;

	const unsigned int uiMaxBufferSize = 10000;

	typedef struct {
		string sAppendToFileName;

		vector<string> sToFilterOn;

		vector<string> FilteredFileLines;
	} sFilterDescriptor;
	typedef enum FileTypesForText{
		UTF32BigEndian,
		UTF32LittleEndian,
		UTF16BigEndian,
		UTF16LittleEndian,
		UTF8,
		ANSI
	}TextFileType;
	
	class ReadLineInterface;

	class ReadLineInterfaceFactory
	{
	public:
		ReadLineInterfaceFactory(){}

		~ReadLineInterfaceFactory(){}

		ReadLineInterfaceFactory(const ReadLineInterfaceFactory &) { }

		ReadLineInterfaceFactory &operator=(const ReadLineInterfaceFactory &) { return *this; }

		unique_ptr<ReadLineInterface> ReadLineInterfaceFactory::getReadLineInterface(FILE * fSrc, string & sErrorMsg);

	private:
		bool FetchTextFileType(FILE * fSrc, TextFileType & tftFileType, string & sErrorMsgRet);
	};

	class ReadLineInterface
	{
	public:
		ReadLineInterface();
		~ReadLineInterface();
		virtual bool operator()
			(FILE * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile) = 0;


	protected:

		vector<string> FileLines;
	};
	class ReadUnicodeLine : public ReadLineInterface
	{
	public:
		virtual bool operator()
			(FILE * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile);
	};

	class ReadUnicodeBigEndianLine final : public ReadLineInterface
	{
	public:
		virtual bool operator()
			(FILE * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile);
	};

	class ReadASCIILine : public ReadLineInterface
	{
	public:
		virtual bool operator()
			(FILE * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile);
	};

	class GenerateFilteredSpewData
	{
	public:
		GenerateFilteredSpewData();

		~GenerateFilteredSpewData();

		virtual bool operator()
			(const char *                pcFolder, 
			 const char *                pcInputFile, 
			 unsigned int                uiMaxFileNameSize, 
			 ReadLineInterface &         ReadSourceLine,
			 vector<sFilterDescriptor> & lFileDescriptors, 
			 string &                    sErrorMsgRet);

		bool GenerateFilteredSpewData::CollectUnfilteredFileLines(FILE * fSrc, ReadLineInterface&  ReadSourceLine, string & sErrorMsgRet);

		void GenerateFilteredSpewData::CollectFilteredFileLines(vector<string>& UnfilteredFileLines, sFilterDescriptor& InputList);

	private:
		void StripLeadingBlanks(char * pcLineBuffer, char ** pcNewBuffer);

		vector<string> UnfilteredFileLines;

	};

};