#ifndef ANTICHEAT_CONSOLE_HPP
#define ANTICHEAT_CONSOLE_HPP

#include <stdio.h>

// I'd use an enum class but I just find those too ugly.
#define CONPTYPE_INFO    0
#define CONPTYPE_WARNING 1
#define CONPTYPE_ERROR   2

class Console
{
private:
	FILE* logFile;

public:
	Console(bool enableLogging);
	~Console();

	void Log(int printType, const char* format, ...);
};

#endif
