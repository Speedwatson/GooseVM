#pragma once

#include "BinaryFileReader.hpp"

typedef int8_t imm8_t;
typedef int16_t imm16_t;
typedef int32_t imm32_t;
typedef int64_t imm64_t;
typedef uint16_t ofs16_t;


class ExecutableFile {
public:
	ExecutableFile(std::string filename);

	struct ModuleHeader {
		const uint32_t default_signature1 = 0x08;
		const uint64_t default_signature2 = 0x2e42542e50542e50;

		uint32_t signature1;
		uint64_t signature2;
		int32_t formatVersion;
		int32_t platformNameIndex;
		int32_t platformVersion;
		int64_t entryPoint;
		void read(BinaryFileReader& bin);
	} moduleHeader;

	struct TablesHeader {
		int32_t sectionsCount;
		int32_t symbolsCount;
		int32_t sourceFilesCount;
		int32_t sourceTextRangesCount;
		int32_t sourceCodePointsCount;
		int32_t blobsCount;
		int32_t stringsCount;
		void read(BinaryFileReader& bin);
	} tablesHeader;

	struct Section {
		int32_t blobIndex;
		int32_t bankNameIndex;
		int64_t startAddress;

		int16_t kind_raw;
		enum Kind {
			kndCode = 0x01,
			kndData = 0x02,
			kndConst = 0x03,
			kndCustom = 0xFF,
		} kind;

		int32_t customSectionNameIndex;

		int16_t accessMode;
		enum AccessMode {
			acmNone = 0,
			acmRead = 1 << 0,
			acmWrite = 1 << 1,
			acmExecute = 1 << 2,
		};
		bool is_available(AccessMode mode) { return (mode == acmNone) ? true :  accessMode & mode; }

		void read(BinaryFileReader& bin);
	};

	struct Symbol {
		int32_t sectionIndex;
		int64_t blobEntryIndex;
		int32_t nameIndex;
		void read(BinaryFileReader& bin);
	};

	struct SourceFile {
		int32_t fileNameIndex;
		int32_t sha256hashBytesIndex;
		void read(BinaryFileReader& bin);
	};

	struct SourceTextRange {
		int32_t sourceFileIndex;
		int32_t position;
		int32_t length;
		int32_t line;
		int32_t column;
		void read(BinaryFileReader& bin);
	};

	struct SourceCodePoint {
		int64_t address;
		int32_t sourceOperationRangeIndex;
		void read(BinaryFileReader& bin);
	};

	struct Blob {
		uint8_t* bytes = nullptr;
		int32_t len;

		~Blob() { if (bytes) delete[] bytes; }
		void read_length(BinaryFileReader& bin);
		void read_data(BinaryFileReader& bin);
	};

	std::vector<Section> sections;
	std::vector<Symbol> symbols;
	std::vector<SourceFile> sourceFiles;
	std::vector<SourceTextRange> sourceTextRanges;
	std::vector<SourceCodePoint> sourceCodePoints;
	std::vector<Blob> blobs;
	std::vector<std::string> strings;

	Blob* code = nullptr;
	Blob* data = nullptr;
	Blob* consts = nullptr;
	ofs16_t start = 0;

	void* getDataPtr(ofs16_t ofs);
	void* getCodePtr(ofs16_t ofs);
	void* getConstsPtr(ofs16_t ofs);

private:
	void* getPtrFromBlob(Blob* blob, ofs16_t ofs, std::string blobName);

};
