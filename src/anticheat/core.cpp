#include "core.hpp"
#include "utils.hpp"

Core::Core(PluginFuncs* pluginFuncs)
{
	vcmpFunctions = pluginFuncs;
	console = new Console(IsLoggingEnabled());
	antiCheatSettings = new AntiCheatSettings;
	playerManager = new PlayerManager;
}

Core::~Core()
{
	delete playerManager;
	delete antiCheatSettings;
	delete console;
}
