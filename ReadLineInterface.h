#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include <array>
using namespace std;

namespace ProcessPBSpews {
	const unsigned int uiMaxFileNameSize = 150;

	typedef struct {
		string sAppendToFileName;

		vector<string> sToFilterOn;
	} sFilterDescriptor;
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
			(const char * pcFolder, const char * pcInputFile, unsigned int uiMaxFileNameSize, vector<sFilterDescriptor> & lFileDescriptors, string & sErrorMsgRet);
	protected:
		vector<string> FileLines;

	private:
		void StripLeadingBlanks(char * pcLineBuffer, char ** pcNewBuffer);
		bool FindFieldStr(const char * pcFields[], char * pcStartOfField);

	};

};