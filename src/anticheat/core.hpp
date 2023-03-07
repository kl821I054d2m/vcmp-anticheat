#ifndef ANTICHEAT_CORE_HPP
#define ANTICHEAT_CORE_HPP

#include "console.hpp"
#include "acsettings.hpp"
#include "playermanager.hpp"
#include <vcmp.h>

class Core
{
private:
	PluginFuncs* vcmpFunctions;
	Console* console;
	AntiCheatSettings* antiCheatSettings;
	PlayerManager* playerManager;

public:
	Core(PluginFuncs* pluginFuncs);
	~Core();

	PluginFuncs* GetVCMPFunctions() { return vcmpFunctions; }
	Console* GetConsole() { return console; }
	AntiCheatSettings* GetAntiCheatSettings() { return antiCheatSettings; }
	PlayerManager* GetPlayerManager() { return playerManager; }
};

#endif
