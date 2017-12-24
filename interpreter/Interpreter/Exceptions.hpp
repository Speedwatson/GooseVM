#include "stdafx.h"
#pragma once

class Error : public std::exception {
	std::string msg;
public:
	enum ErrorType { etFatal, etRuntime } type;
	Error(const std::string& message, ErrorType type = etFatal) : msg(message), type(type) {};
	const char* what() { return msg.c_str(); }
};