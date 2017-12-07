#include "stdafx.h"
#pragma once

class UnknownArgError : public std::exception {
	std::string msg;
public:
	UnknownArgError(const std::string& message) : msg(message) {};
	const char* what() { return msg.c_str(); }
};