#include "stdafx.h"
#include "ExecutableFile.hpp"
#include "BinaryFileReader.hpp"
#include "Exceptions.hpp"

ExecutableFile::ExecutableFile(std::string filename) {
	BinaryFileReader bin(filename);

	moduleHeader.read(bin);
	tablesHeader.read(bin);
	
	sections			.resize(tablesHeader.sectionsCount);
	symbols				.resize(tablesHeader.symbolsCount);
	sourceFiles			.resize(tablesHeader.sourceFilesCount);
	sourceTextRanges	.resize(tablesHeader.sourceTextRangesCount);
	sourceCodePoints	.resize(tablesHeader.sourceCodePointsCount);
	blobs				.resize(tablesHeader.blobsCount);
	strings				.resize(tablesHeader.stringsCount);

	for (auto& section : sections)						section.read(bin);
	for (auto& symbol : symbols)						symbol.read(bin);
	for (auto& sourceFile : sourceFiles)				sourceFile.read(bin);
	for (auto& sourceTextRange : sourceTextRanges)		sourceTextRange.read(bin);
	for (auto& sourceCodePoint : sourceCodePoints)		sourceCodePoint.read(bin);
	for (auto& blob : blobs)							blob.read_length(bin);
	for (auto& blob : blobs)							blob.read_opcode(bin);

	for (auto& string : strings) {
		uint32_t len;
		bin >> len;
		string.resize(len);

		uint8_t byte;
		for (uint32_t i = 0; i < len; ++i) {
			bin >> byte;
			string[i] = byte;
		}
	}
	
}

void ExecutableFile::ModuleHeader::read(BinaryFileReader& bin){
	bin >> signature1
		>> signature2
		>> formatVersion
		>> platformNameIndex
		>> platformVersion
		>> entryPoint;
	
	if (signature1 != default_signature1 || signature2 != default_signature2) {
		throw Error("File is corrupted or not an executable: wrong signature.");
	}
}

void ExecutableFile::TablesHeader::read(BinaryFileReader & bin) {
	bin >> sectionsCount \
		>> symbolsCount				>> sourceFilesCount \
		>> sourceTextRangesCount	>> sourceCodePointsCount	>> blobsCount \
		>> stringsCount;
}

void ExecutableFile::Section::read(BinaryFileReader & bin) {
	bin >> blobIndex	>> bankNameIndex			>> startAddress \
		>> kind_raw		>> customSectionNameIndex	>> accessMode;

	switch (kind_raw)
	{
	case 0:		kind = kndCode; break;
	case 1:		kind = kndData; break;
	case 2:		kind = kndConst; break;
	default:	kind = kndCustom; break;
	}
}

void ExecutableFile::Symbol::read(BinaryFileReader & bin) {
	bin >> sectionIndex >> blobEntryIndex >> nameIndex;
}

void ExecutableFile::SourceFile::read(BinaryFileReader & bin) {
	bin >> fileNameIndex >> sha256hashBytesIndex;
}

void ExecutableFile::SourceTextRange::read(BinaryFileReader & bin) {
	bin >> sourceFileIndex >> position >> length >> line >> column;
}

void ExecutableFile::SourceCodePoint::read(BinaryFileReader & bin) {
	bin >> address >> sourceOperationRangeIndex;
}

void ExecutableFile::Blob::read_length(BinaryFileReader & bin) {
	uint32_t len_bytes;
	bin >> len_bytes;
	bits.resize(len_bytes * 8);
}

void ExecutableFile::Blob::read_opcode(BinaryFileReader & bin) {
	for (int byte_i = 0; byte_i < bits.size() / 8; ++byte_i) {
		uint8_t byte;
		bin >> byte;

		uint8_t mask = 0b10000000;
		for (int bit_i = 0; bit_i < 8; ++bit_i) {
			bits[byte_i * 8 + bit_i] = byte & mask;
			mask >>= 1;
		}
	}
}
