#include "stdafx.h"
#pragma once

class Error : public std::exception {
	std::string msg;
public:
	Error(const std::string& message) : msg(message) {};
	const char* what() { return msg.c_str(); }
};

