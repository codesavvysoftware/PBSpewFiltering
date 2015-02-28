#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>

using namespace std;

namespace ProcessPBSpews {
	class ReadLineInterface
	{
	public:
		ReadLineInterface();
		~ReadLineInterface();
		virtual bool operator()
			(FILE * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead,bool & bEndOfFile) = 0;
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

	class ProduceFilteredSpewData
	{
	public:
		virtual bool operator()
			(const char * pcFolder, const char * pcInputFile, const char * pcOutputFile, const char * pcFilter[], string & sErrorMsgRet);
	private:
		void StripLeadingBlanks(char * pcLineBuffer, char ** pcNewBuffer);
		bool FindFieldStr(const char * pcFields[], char * pcStartOfField);

	};
}

