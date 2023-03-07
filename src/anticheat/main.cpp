#include "main.hpp"
#include "core.hpp"
#include "callbacks.hpp"
#include <string.h>

Core* core;

uint32_t VcmpPluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls,
	PluginInfo* pluginInfo)
{
	// Set up plugin info.
	strcpy(pluginInfo->name, PLUGIN_NAME);
	pluginInfo->pluginVersion = PLUGIN_VERSION;
	pluginInfo->apiMajorVersion = PLUGIN_API_MAJOR;
	pluginInfo->apiMinorVersion = PLUGIN_API_MINOR;

	// Set up plugin callbacks.
	using namespace Callback;
	/* Server */
	pluginCalls->OnServerShutdown = OnServerShutdown;
	pluginCalls->OnServerFrame = OnServerFrame;
	/* Player */
	pluginCalls->OnPlayerConnect = OnPlayerConnect;
	pluginCalls->OnPlayerDisconnect = OnPlayerDisconnect;
	pluginCalls->OnPlayerModuleList = OnPlayerModuleList;

	// Instantiate the core and hook into the server.
	core = new Core(pluginFuncs);
	// Attempt to parse user data.
	if (!core->GetAntiCheatSettings()->LoadUp())
	{
		// We failed hard so clean up and abort.
		delete core;
		core = nullptr;
		return 0;
	}

	// Success.
	return 1;
}
