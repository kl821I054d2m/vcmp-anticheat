#include "utils.hpp"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int StrCaseCmp(const char* string1, const char* string2)
{
#if defined(COMP_MSVC)
	return stricmp(string1, string2);
#elif defined(COMP_GCC)
	return strcasecmp(string1, string2);
#else
	const unsigned char* s1 = reinterpret_cast<const unsigned char*>(string1);
	const unsigned char* s2 = reinterpret_cast<const unsigned char*>(string2);
	while (*s1 && (tolower(*s1) == tolower(*s2)))
	{
		++s1;
		++s2;
	}

	return *s1 - *s2;
#endif
}

char* StrCaseStr(char* string, const char* subString)
{
#ifdef COMP_GCC
	return strcasestr(string, subString);
#else
	const char* str;
	const char* sub;
	while (true)
	{
		str = string;
		sub = subString;
		while (*str && (tolower(*str) == tolower(*sub)))
		{
			++str;
			++sub;
		}

		if (!*sub)
		{
			return string;
		}

		if (!*string)
		{
			return nullptr;
		}

		++string;
	}
#endif
}

const char* StrCaseStr(const char* string, const char* subString)
{
	return StrCaseStr(const_cast<char*>(string), subString);
}

char* StrDup(const char* string)
{
/* strdup is supported by MSVC and GCC */
// If you're using any other compiler that does support strdup as well add it
// here!
#if defined(COMP_MSVC) || defined(COMP_GCC)
	return strdup(string);
// Compiler does not support strdup. No worries!
#else
	size_t size = strlen(string) + 1; // +1 for the null terminator.
	void* copy = malloc(size);
	if (!copy)
	{
		return nullptr;
	}

	// Don't bother to null-terminate the string copy as it is automatically
	// done by memcpy, as we are literally copying every character from
	// 'string' (up to the null terminator of course as that's where strlen
	// stops).
	return static_cast<char*>(memcpy(copy, string, size));
#endif
}

char* LeftStrip(char* string)
{
	while (*string && // Can isspace('\0') return nonzero?
		isspace(*string))
	{
		*string++ = '\0';
	}

	return string;
}

char* RightStrip(char* string)
{
	// Get our string's length so that we can know where it ends.
	size_t length = strlen(string);
	// Is our string empty?
	if (!length)
	{
		return string;
	}

	// Last character position (the one preceding the null terminator).
	char* stringEnd = string + length - 1;
	// As long as we keep encountering whitespace characters...
	while (isspace(*stringEnd))
	{
		*stringEnd = '\0';
		// Are we at the beginning of the string already?
		if (stringEnd == string)
		{
			// We are done.
			break;
		}
		--stringEnd;
	}

	return string;
}

bool ReadLineFromFile(FILE* file, char* buffer, size_t bufferSize)
{
	// Are we at the end-of-file position?
	if (feof(file))
	{
		// We can't keep reading data past this point.
		return false;
	}

	int character;
	char* bufferEnd = buffer;
	while (true)
	{
		// Get current character from the stream.
		character = fgetc(file);
		// Are we at the end of the line (or the end of the file)?
		if (character == '\n' || character == EOF)
		{
			// Null-terminate our buffer.
			*bufferEnd = '\0';
			// There's still data left to be read!
			return true;
		}

		// Do we not have enough room to store a new character to our buffer?
		/* We add a cast to suppress C4018 warning on MSVC */
		if ((static_cast<size_t>(bufferEnd - buffer) + 1) >= bufferSize)
		{
			// Just don't store a new character to it but keep on with the loop
			// until we reach a newline or the end-of-file.
			continue;
		}

		// Append a new character to our buffer.
		*bufferEnd++ = character;
	}
}

void ParseConfigFileLine(char* line, char*& settingName, char*& settingValue)
{
	line = LeftStrip(line);
	if (!*line)
	{
		settingName = nullptr;
		settingValue = nullptr;
		return;
	}

	char* lineEnd = line;
	do
	{
		if (isspace(*lineEnd))
		{
			do
			{
				*lineEnd++ = '\0';
			}
			while (*lineEnd && isspace(*lineEnd));
			RightStrip(lineEnd);

			settingName = line;
			settingValue = *lineEnd ? lineEnd : nullptr;
			return;
		}

		++lineEnd;
	}
	while (*lineEnd);

	settingName = line;
	settingValue = nullptr;
}

bool IsLoggingEnabled()
{
	// Attempt to open server.cfg in read mode.
	FILE* configFile = fopen("server.cfg", "r");
	if (!configFile)
	{
		// Enabled by default.
		return true;
	}

	// Enabled by default.
	bool is = true;

	char line[256];
	char* settingName;
	char* settingValue;
	while (ReadLineFromFile(configFile, line, sizeof(line)))
	{
		ParseConfigFileLine(line, settingName, settingValue);
		if (!settingName || !settingValue)
		{
			continue;
		}

		if (!StrCaseCmp(settingName, "logging"))
		{
			is = !!strtol(settingValue, nullptr, 10);
		}
	}

	fclose(configFile);
	return is;
}

bool IsIPV4Valid(const char* address)
{
	int byte = 0;
	int byteCount = 0;
	char previousCharacter = '.';
	while (true)
	{
		if (isdigit(*address))
		{
			byte = byte * 10 + (*address - '0');
			if (byte > 255)
			{
				return false;
			}
		}
		else if (*address == '.' || !*address)
		{
			if (byteCount >= 4 || previousCharacter == '.')
			{
				return false;
			}

			if (!*address)
			{
				return true;
			}

			byte = 0;
			++byteCount;
		}
		else
		{
			return false;
		}

		previousCharacter = *address++;
	}
}

bool IsPlayerIdValid(int32_t playerId)
{
	return (playerId >= 0) && (playerId <= (MAX_PLAYERS - 1));
}

bool IsPlayerUniqueIdValid(const char* uniqueId)
{
	const char* uniqueIdEnd = uniqueId;
	// Inspect all characters from the UID.
	for (; *uniqueIdEnd; ++uniqueIdEnd)
	{
		// Is current character not hexadecimal?
		if (!isxdigit(*uniqueIdEnd))
		{
			// Invalid UID.
			return false;
		}
	}

	// Validate its length -- all UIDs shall have the same length.
	return (uniqueIdEnd - uniqueId) == PLAYER_UID_LENGTH;
}

Module* ParseModuleListLine(char* line)
{
	// <BASEADDRESS> S <SIZE> N <PATH>
	//               ^
	//               size
	char* size = strchr(line, 'S');
	if (!size || !size[1])
	{
		return nullptr;
	}

	// <BASEADDRESS> S <SIZE> N <PATH>
	//                 ^
	//                 size
	size += 2;

	// <BASEADDRESS> S <SIZE> N <PATH>
	//                        ^
	//                        path
	char* path = strchr(size, 'N');
	if (!path || !path[1])
	{
		return nullptr;
	}

	// <BASEADDRESS> S <SIZE> N <PATH>
	//                          ^
	//                          path
	path += 2;

	// In theory, no path would ever exceed MAX_PATH characters but one should
	// never assume things.
	if (strlen(path) > (MAX_PATH - 1))
	{
		return nullptr;
	}

	// Null-terminate 'size' so that it can be properly interpreted by strtoul.
	//
	// <BASEADDRESS> S <SIZE> N <PATH>
	//                 ^     ^
	//                 size  \0
	path[-3] = '\0';

	return new Module(strtoul(size, nullptr, 16), path);
}
