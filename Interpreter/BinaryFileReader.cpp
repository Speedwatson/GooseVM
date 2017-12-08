#include "BinaryFileReader.hpp"
#include "Exceptions.hpp"

BinaryFileReader::BinaryFileReader(std::string filename)
	: fin(filename, std::ios::binary)
{
	if (!fin.is_open()) {
		throw Error("File " + filename + " doesn't exist or you don't have permissions to read it.");
	}
}

template<typename T>
BinaryFileReader& BinaryFileReader::operator>>(T& val) {
	size_t size = sizeof(T);
	std::unique_ptr<char> buf(new char[size]);

	fin.read(buf.get(), size);
	if (fin.bad()) {
		throw Error("File is corrupted: cannot read expected entry at offset " + std::to_string((int)fin.tellg()) + ".");
	}

	for (int i = 0; i < size; ++i) {
		val |= buf.get()[i];
		if (i != size - 1) val <<= 8;
	}
	return *this;
}
