#include "stdafx.h"
#include "BinaryFileReader.hpp"
#include "Exceptions.hpp"

BinaryFileReader::BinaryFileReader(std::string filename)
	: fin(filename, std::ios::binary)
{
	if (!fin.is_open()) {
		throw Error("File " + filename + " doesn't exist or you don't have permissions to read it.");
	}
}

