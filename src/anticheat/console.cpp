#include "console.hpp"
#include "main.hpp"

#ifdef OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
/* Assuming <Windows.h> includes <stdarg.h> */
#include <stdarg.h>
#endif

Console::Console(bool enableLogging)
{
	// Attempt to open server_log.txt in append mode if logging is enabled,
	// nullptr otherwise.
	logFile = enableLogging ? fopen("server_log.txt", "a") : nullptr;
}

Console::~Console()
{
	// Do we have a valid stream?
	if (logFile)
	{
		fclose(logFile);
	}
}

void Console::Log(int printType, const char* format, ...)
{
	// Set up our message.
	char message[1024];
	va_list ap;
	va_start(ap, format);
	vsnprintf(message, sizeof(message), format, ap);
	va_end(ap);

	// Do we have a valid stream?
	if (logFile)
	{
		// Write our message to it.
		fprintf(logFile, PLUGIN_NAME ": %s\n", message);
	}

#ifdef OS_WINDOWS
	// Attempt to retrieve a handle to the console.
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!consoleHandle || consoleHandle == INVALID_HANDLE_VALUE)
	{
		// We'd want to output anything in case of errors anyway.
		printf(PLUGIN_NAME ": %s\n", message);
		return;
	}

	// Attempt to retrieve console's current screen buffer info so that we can
	// reset its colour later.
	CONSOLE_SCREEN_BUFFER_INFO initialConsoleInfo;
	if (!GetConsoleScreenBufferInfo(consoleHandle, &initialConsoleInfo))
	{
		// Same reason as above.
		printf(PLUGIN_NAME ": %s\n", message);
		return;
	}

	// Determine colour attributes for the message tag.
	WORD messageTagColourAttributes;
	switch (printType)
	{
	default:
	case CONPTYPE_INFO:
		// Bright magenta.
		messageTagColourAttributes = FOREGROUND_RED | FOREGROUND_BLUE |
			FOREGROUND_INTENSITY;
		break;

	case CONPTYPE_WARNING:
		// Bright yellow.
		messageTagColourAttributes = FOREGROUND_RED | FOREGROUND_GREEN |
			FOREGROUND_INTENSITY;
		break;

	case CONPTYPE_ERROR:
		// Bright red.
		messageTagColourAttributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	}

	// Colourize message tag accordingly.
	SetConsoleTextAttribute(consoleHandle, messageTagColourAttributes);
	fputs(PLUGIN_NAME ": ", stdout);
	// Reset console colour to its initial state.
	SetConsoleTextAttribute(consoleHandle, initialConsoleInfo.wAttributes);
	puts(message);
#else
	// Determine ANSI colour code for the message tag.
	// https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit
	int messageTagColourCode;
	switch (printType)
	{
	default:
	case CONPTYPE_INFO:
		// Bright magenta.
		messageTagColourCode = 95;
		break;

	case CONPTYPE_WARNING:
		// Bright yellow.
		messageTagColourCode = 93;
		break;

	case CONPTYPE_ERROR:
		// Bright red.
		messageTagColourCode = 91;
		break;
	}

	printf("\x1B[%dm" PLUGIN_NAME ":\x1B[0m %s\n", messageTagColourCode,
		message);
#endif
}
