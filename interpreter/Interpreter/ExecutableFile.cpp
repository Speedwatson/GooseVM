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
	for (auto& blob : blobs)							blob.read_data(bin);

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
	
	// finding important blobs
	int codeSectionIndex = -1;
	int foundMask = 0b000;
	for (int i = 0; i < sections.size(); ++i) {
		auto& section = sections[i];

		if (strings[section.bankNameIndex] == "__data") {
			data = &blobs[section.blobIndex];
			foundMask |= 0b001;
		}
		else if (strings[section.bankNameIndex] == "__consts") {
			consts = &blobs[section.blobIndex];
			foundMask |= 0b010;
		}
		else if (strings[section.bankNameIndex] == "__code") {
			code = &blobs[section.blobIndex];
			codeSectionIndex = i;
			foundMask |= 0b100;
		}

		if (foundMask == 0b111) break;
	}

	if (codeSectionIndex == -1) throw Error("Bad executable: no __code section found.");

	// finding __start
	for (auto symbol : symbols) {
		if (strings[symbol.nameIndex] == "__start") {
			start = symbol.blobEntryIndex;
			break;
		}
	}
}

void* ExecutableFile::getDataPtr(ofs16_t ofs) {
	return getPtrFromBlob(data, ofs, "data");
}

void* ExecutableFile::getCodePtr(ofs16_t ofs) {
	return getPtrFromBlob(code, ofs, "code");
}

void* ExecutableFile::getConstsPtr(ofs16_t ofs) {
	return getPtrFromBlob(consts, ofs, "consts");
}

void* ExecutableFile::getPtrFromBlob(Blob* blob, ofs16_t ofs, std::string blobName) {
	if (!blob) {
		assert(false);
		throw Error("Attempt to operate with __" + blobName + " by offset " + std::to_string(ofs) + "when there's no such a section");
	}
	if (ofs >= blob->len) {
		assert(false);
		throw Error("Attempt to gain access to __" + blobName + " by offset " + std::to_string(ofs) + " - access violation");
	}

	return blob->bytes + ofs;
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
	bin >> sectionsCount
		>> symbolsCount
		>> sourceFilesCount
		>> sourceTextRangesCount
		>> sourceCodePointsCount
		>> blobsCount
		>> stringsCount;
}

void ExecutableFile::Section::read(BinaryFileReader & bin) {
	bin >> blobIndex
		>> bankNameIndex
		>> startAddress
		>> kind_raw	
		>> customSectionNameIndex
		>> accessMode;

	switch (kind_raw)
	{
	case 0:		kind = kndCode; break;
	case 1:		kind = kndData; break;
	case 2:		kind = kndConst; break;
	default:	kind = kndCustom; break;
	}
}

void ExecutableFile::Symbol::read(BinaryFileReader & bin) {
	bin >> sectionIndex 
		>> blobEntryIndex 
		>> nameIndex;
}

void ExecutableFile::SourceFile::read(BinaryFileReader & bin) {
	bin >> fileNameIndex 
		>> sha256hashBytesIndex;
}

void ExecutableFile::SourceTextRange::read(BinaryFileReader & bin) {
	bin >> sourceFileIndex 
		>> position 
		>> length
		>> line 
		>> column;
}

void ExecutableFile::SourceCodePoint::read(BinaryFileReader & bin) {
	bin >> address 
		>> sourceOperationRangeIndex;
}

void ExecutableFile::Blob::read_length(BinaryFileReader & bin) {
	bin >> len;
	if (len) bytes = new uint8_t[len];
}

void ExecutableFile::Blob::read_data(BinaryFileReader & bin) {
	for (int i = 0; i < len; ++i) bin >> bytes[i];
}
  