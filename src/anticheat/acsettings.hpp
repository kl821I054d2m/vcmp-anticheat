#ifndef ANTICHEAT_ACSETTINGS_HPP
#define ANTICHEAT_ACSETTINGS_HPP

class AntiCheatSettings
{
private:
	unsigned long messageColour : 24; // RGB only.
	unsigned long checkInterval;
	unsigned int maxCheckRefusalCount;
	unsigned long expectedClientSize;

public:
	AntiCheatSettings();

	bool LoadUp();

	unsigned long GetMessageColour() { return messageColour; }
	unsigned long GetCheckInterval() { return checkInterval; }
	unsigned int GetMaxCheckRefusalCount() { return maxCheckRefusalCount; }
	unsigned long GetExpectedClientSize() { return expectedClientSize; }
};

#endif
