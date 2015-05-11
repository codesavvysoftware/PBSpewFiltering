#ifndef PROCESSPBSPEW_H_
#define PROCESSPBSPEW_H_
#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <fstream>

namespace ProcessPBSpews {
	const unsigned int uiMaxFileNameSize = 150;

	const unsigned int uiMaxBufferSize = 10000;

	typedef struct {
		std::string sAppendToFileName;

		std::vector<std::string> sToFilterOn;

		std::string sFileNameToWrite;

		std::vector<std::string> FilteredFileLines;
	} sFilterDescriptor;
	typedef enum FileTypesForText{
		UTF32BigEndian,
		UTF32LittleEndian,
		UTF16BigEndian,
		UTF16LittleEndian,
		UTF8,
		ANSI
	}TextFileType;
	
    extern std::vector<sFilterDescriptor> ProduceAllFilterDescriptor;

    extern std::vector<sFilterDescriptor> ProducePhoneFilterDescriptor;

    extern std::vector<sFilterDescriptor> ProduceInclusiveFilterDescriptor;

	extern std::vector<sFilterDescriptor> ProduceHFPFilterDescriptor;

	extern std::vector<sFilterDescriptor> ProduceActionScriptFilterDescriptor;


	class ReadLineInterface;

	class ReadLineInterfaceFactory
	{
	public:
		ReadLineInterfaceFactory(){}

		~ReadLineInterfaceFactory(){}

		ReadLineInterfaceFactory(const ReadLineInterfaceFactory &) { }

		ReadLineInterfaceFactory &operator=(const ReadLineInterfaceFactory &) { return *this; }

		std::unique_ptr<ReadLineInterface> getReadLineInterface(std::string sFile, std::string & sErrorMsg);

	private:
		bool FetchTextFileType(std::ifstream & fSrc, TextFileType & tftFileType, std::string & sErrorMsgRet);

		bool SetFilePtrPos(std::ifstream & f, long ulPos) {
			f.seekg(ulPos,std::ios::beg);
			return !f.bad();
		}

		bool GetFilePtrPos(std::ifstream & f, long & ulPos) {
			ulPos = f.tellg();
			return !f.bad();
		}

		bool ReadStreamData(std::ifstream & f, char * pByteBuffer, int iNumberOfBytes) {
			f.read(pByteBuffer,iNumberOfBytes);
			return !f.bad();
		}
	};

	class ReadLineInterface
	{
	public:
		ReadLineInterface();
		~ReadLineInterface();
		virtual bool operator()
			(std::ifstream * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile) = 0;


	protected:

		std::vector<std::string> FileLines;
	};
	class ReadUnicodeLine : public ReadLineInterface
	{
	public:
		virtual bool operator()
			(std::ifstream * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile);
	};

	class ReadUnicodeBigEndianLine final : public ReadLineInterface
	{
	public:
		virtual bool operator()
			(std::ifstream * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile);
	};

	class ReadASCIILine : public ReadLineInterface
	{
	public:
		virtual bool operator()
			(std::ifstream * f, char * pcLineBuffer, unsigned int uiBufferSizeMax, unsigned int & uiNumberOfCharsRead, bool & bEndOfFile);
	};

	class GenerateFilteredSpewData
	{
	public:
		GenerateFilteredSpewData();

		~GenerateFilteredSpewData();

		virtual bool operator()
			(std::string &               sInputFileName,
			 std::string &               sOutputFileName,
			 unsigned int                uiMaxFileNameSize, 
			 ReadLineInterface &         ReadSourceLine,
			 std::vector<sFilterDescriptor> & lFileDescriptors,
			 std::string &                    sErrorMsgRet);

		bool CollectUnfilteredFileLines(std::ifstream * fSrc, ReadLineInterface&  ReadSourceLine, std::string & sErrorMsgRet);

		void CollectFilteredFileLines(std::vector<std::string>& UnfilteredFileLines, sFilterDescriptor& InputList);

	private:
		void StripLeadingBlanks(char * pcLineBuffer, char ** pcNewBuffer);

		void AppendFileNameSuffix( std::string & sFileName, const std::string & sSuffix );

		std::vector<std::string> UnfilteredFileLines;

	};

};
#endif
