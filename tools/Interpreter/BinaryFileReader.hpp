#include "stdafx.h"
#pragma once

class BinaryFileReader {
	std::ifstream fin;
public:
	BinaryFileReader(std::string filename);

	template<typename T>
	BinaryFileReader& operator>>(T& val) {
		fin.read((char*)&val, sizeof(T));
		if (fin.bad()) throw Error("File is corrupted: cannot read expected entry at offset " + std::to_string((int)fin.tellg()) + ".");
		return *this;
	}
};