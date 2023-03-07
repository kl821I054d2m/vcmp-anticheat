#include "acsettings.hpp"
#include "core.hpp"
#include "utils.hpp"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

extern Core* core;

AntiCheatSettings::AntiCheatSettings()
{
	messageColour = 0x0B5FA5;
	checkInterval = 5000;
	maxCheckRefusalCount = 100;
	expectedClientSize = 0x00000000;
}

bool AntiCheatSettings::LoadUp()
{
	// Attempt to open configuration file in read mode.
	errno = 0;
	FILE* configFile = fopen(ANTICHEAT_CONFIG_FILE_PATH, "r");
	if (!configFile)
	{
		// https://cplusplus.com/reference/cstdio/fopen/
		// Return Value
		// "On most library implementations, the errno variable is also set to
		// a system-specific error code on failure."
		//
		// Above statement is enough to know that there is no guarantee that
		// errno was set.
		if (errno)
		{
			core->GetConsole()->Log(CONPTYPE_ERROR, "Unable to open "
				"configuration file '" ANTICHEAT_CONFIG_FILE_PATH "': %s.",
				strerror(errno));
		}
		else
		{
			core->GetConsole()->Log(CONPTYPE_ERROR, "Unable to open "
				"configuration file '" ANTICHEAT_CONFIG_FILE_PATH "'.");
		}

		// Failure.
		return false;
	}

	char line[256];
	char* settingName;
	char* settingValue;

	long value;
	char* rem;
	// Read through every line of the file.
	while (ReadLineFromFile(configFile, line, sizeof(line)))
	{
		// Parse current line.
		ParseConfigFileLine(line, settingName, settingValue);
		// Line is empty - it has no setting name and no setting value.
		if (!settingName)
		{
			continue;
		}

		// Line has setting name but no setting value.
		if (!settingValue)
		{
			core->GetConsole()->Log(CONPTYPE_WARNING, "Ignored "
				"'" ANTICHEAT_CONFIG_FILE_PATH "' setting '%s' as no value "
				"for it was provided.", settingName);
			continue;
		}

		if (!StrCaseCmp(settingName, "message_colour"))
		{
			value = strtol(settingValue, &rem, 16);
			if (*rem || value < 0x000000 || value > UINT24_MAX)
			{
				core->GetConsole()->Log(CONPTYPE_WARNING, "Ignored "
					"'" ANTICHEAT_CONFIG_FILE_PATH "' setting '%s' as its "
					"value must be a hexadecimal integer between 0 and %lX.",
					settingName, static_cast<unsigned long>(UINT24_MAX));
				continue;
			}

			messageColour = value;
		}
		else if (!StrCaseCmp(settingName, "check_interval"))
		{
			value = strtol(settingValue, &rem, 10);
			if (*rem || value < 1000 || value > 600000)
			{
				core->GetConsole()->Log(CONPTYPE_WARNING, "Ignored "
					"'" ANTICHEAT_CONFIG_FILE_PATH "' setting '%s' as its "
					"value must be an integer between 1000 and 600000.",
					settingName);
				continue;
			}

			checkInterval = value;
		}
		else if (!StrCaseCmp(settingName, "max_check_refusals"))
		{
			value = strtol(settingValue, &rem, 10);
			if (*rem || value < 0 || value > 1000)
			{
				core->GetConsole()->Log(CONPTYPE_WARNING, "Ignored "
					"'" ANTICHEAT_CONFIG_FILE_PATH "' setting '%s' as its "
					"value must be an integer between 0 and 1000.",
					settingName);
				continue;
			}

			maxCheckRefusalCount = static_cast<unsigned int>(value);
		}
		else if (!StrCaseCmp(settingName, "expected_client_size"))
		{
			errno = 0;
			value = strtol(settingValue, &rem, 16);
			if (*rem || value < 0x00000001 || errno == ERANGE)
			{
				core->GetConsole()->Log(CONPTYPE_WARNING, "Ignored "
					"'" ANTICHEAT_CONFIG_FILE_PATH "' setting '%s' as its "
					"value must be a hexadecimal integer between 1 and %lX.",
					settingName, static_cast<unsigned long>(LONG_MAX));
				continue;
			}

			expectedClientSize = value;
		}
		else
		{
			core->GetConsole()->Log(CONPTYPE_WARNING, "Ignored "
				"'" ANTICHEAT_CONFIG_FILE_PATH "' setting '%s' as it is an "
				"invalid setting name.", settingName);
		}
	}

	if (!expectedClientSize)
	{
		core->GetConsole()->Log(CONPTYPE_ERROR,
			"'" ANTICHEAT_CONFIG_FILE_PATH "' setting 'expected_client_size' "
			"is either missing, its value is invalid, or no value for it was "
			"provided at all.");
		fclose(configFile);
		return false;
	}

	// Success.
	fclose(configFile);
	return true;
}
