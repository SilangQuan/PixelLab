#pragma once

#include<string>
#include<iostream>

//#define PLATFORM_BREAK() (__nop(), __debugbreak())

//#ifndef check
//#define check(expr)				CHECK_IMPL(expr)
//#define check(expr)				assert(expr)
//#endif

/*
// unnecessary instructions when using a lambda; hence the Exec() impl.
#define CHECK_IMPL(expr) \
{ \
	struct Impl \
	{ \
		static void ExecCheckImplInternal() \
		{ \
			//if(!expr) CheckVerifyFailed(__FILE__, __FUNCTION__, __LINE__); \
		} \
	}; \
	PLATFORM_BREAK(); \
}


void CheckVerifyFailedImpl(const char* fileName, const char* functionName, int lineNumber)
{
	std::string logString;

	logString.append("Check Failed");
	logString.append("......");

	logString.append(fileName);
	logString.append(" ");
	logString.append(functionName);
	logString.append("()");

	std::cout << logString << " line: " << lineNumber << " " << std::endl;
}
std::cout << logString << " line: " << lineNumber << " " << std::endl
*/