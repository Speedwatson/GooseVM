#include "stdafx.h"
#pragma once

class BinaryFileReader {
	std::ifstream fin;
public:
	BinaryFileReader(std::string filename);

	template<typename T>
	BinaryFileReader& operator>>(T& val) {
		size_t size = sizeof(T);
		std::unique_ptr<char> buf(new char[size]);

		fin.read(buf.get(), size);
		if (fin.bad()) {
			throw Error("File is corrupted: cannot read expected entry at offset " + std::to_string((int)fin.tellg()) + ".");
		}

		// reading of LITTLE-ENDIAN byte order is implemented here
		val = 0;
		for (int i = size - 1; i >= 0; --i) {
			val |= buf.get()[i];
			if (i != 0) val <<= 8;
		}
		return *this;
	}
};