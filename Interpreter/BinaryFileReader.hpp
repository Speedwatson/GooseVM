#include "stdafx.h"
#pragma once

class BinaryFileReader {
	std::ifstream fin;
public:
	BinaryFileReader(std::string filename);

	template <typename T>
	BinaryFileReader& operator>>(T& val);
};