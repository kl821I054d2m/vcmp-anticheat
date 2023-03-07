#include "events.hpp"
#include "core.hpp"
#include "utils.hpp"
#include <string.h>

extern Core* core;

void Event::OnServerFrame(float elapsedTime)
{
	// Process all players so that we can know when it's prudent to request a
	// player's module list.
	core->GetPlayerManager()->ProcessPlayers(elapsedTime);
}

void Event::OnPlayerConnect(Player* player)
{
#if STRICT_ANTICHEAT
	// Validate player's IPv4 address.
	if (!IsIPV4Valid(player->GetIPV4()))
	{
		player->Kick("invalid IPv4 address detected", "invalid IPv4 address "
			"'%s' detected", player->GetIPV4());
		return;
	}

	// Validate player's UIDv1.
	if (!IsPlayerUniqueIdValid(player->GetUIdV1()))
	{
		player->Kick("invalid UIDv1 detected", "invalid UIDv1 '%s' detected",
			player->GetUIdV1());
		return;
	}

	// Validate player's UIDv2.
	if (!IsPlayerUniqueIdValid(player->GetUIdV2()))
	{
		player->Kick("invalid UIDv2 detected", "invalid UIDv2 '%s' detected",
			player->GetUIdV2());
		return;
	}
#endif

	// Request player's module list as soon as they connect so that we can set
	// up module list data for later comparison.
	player->GetModuleList();
}

void Event::OnPlayerModuleList(Player* player, char* list)
{
	// Player's client has just authorized us to look into player's module
	// list - exempt them from being kicked.
	player->refusedCheckCount = 0;

	ModuleList* moduleList = player->moduleList;
	// Second or more check.
	if (moduleList)
	{
#if STRICT_ANTICHEAT
		PluginFuncs* vcmpFunctions = core->GetVCMPFunctions();

		// Temporary array in which we'll store player's current IPv4, UIDv1
		// and UIDv2, if necessary.
		char tmp[(MAX_PLAYER_IPV4 > MAX_PLAYER_UID) ? MAX_PLAYER_IPV4 :
			MAX_PLAYER_UID];

		// Get player's current IPv4 address.
		*tmp = '\0';
		vcmpFunctions->GetPlayerIP(player->GetId(), tmp, sizeof(tmp));
		// Does it differ from the one they had on connect?
		if (strcmp(tmp, player->GetIPV4()))
		{
			player->Kick("IPv4 address change detected", "IPv4 address change "
				"from '%s' to '%s' detected", player->GetIPV4(), tmp);
			return;
		}

		// Get player's current UIDv1.
		*tmp = '\0';
		vcmpFunctions->GetPlayerUID(player->GetId(), tmp, sizeof(tmp));
		// Does it differ from the one they had on connect?
		if (strcmp(tmp, player->GetUIdV1()))
		{
			player->Kick("UIDv1 change detected", "UIDv1 change from '%s' to "
				"'%s' detected", player->GetUIdV1(), tmp);
			return;
		}

		// Get player's current UIDv2.
		*tmp = '\0';
		vcmpFunctions->GetPlayerUID2(player->GetId(), tmp, sizeof(tmp));
		// Does it differ from the one they had on connect?
		if (strcmp(tmp, player->GetUIdV2()))
		{
			player->Kick("UIDv2 change detected", "UIDv2 change from '%s' to "
				"'%s' detected", player->GetUIdV2(), tmp);
			return;
		}
#endif

		// Set up player's current module list.
		ModuleList currentModuleList(list);
		// Compare it with the one we set up on player connect.
		Module* differingModule = currentModuleList == *moduleList;
		// They differ - something fishy is going on!
		if (differingModule)
		{
			player->Kick("module list change detected", "module list change "
				"detected at module '%s\\%s' with size %.8lX",
				differingModule->GetPath(), differingModule->GetName(),
				differingModule->GetSize());
			return;
		}
	}
	// First check i.e. on player connect.
	else
	{
		// We don't have to worry about freeing this memory since Player
		// destructor will do it for us.
		moduleList = player->moduleList = new ModuleList(list);

		// Attempt to retrieve client module vcmp-game.dll.
		Module* clientModule = moduleList->FindModule("vcmp-game.dll");
		if (!clientModule)
		{
			// vcmp-game.dll module is absent which could possibly mean that
			// our player is running the Steam version of the game.
			clientModule = moduleList->FindModule("vcmp-steam.dll");
			if (!clientModule)
			{
				// Neither vcmp-game.dll nor vcmp-steam.dll modules are
				// present. Whoa?
				player->Kick("absent client module", "absent vcmp-game.dll "
					"and vcmp-steam.dll modules");
				return;
			}
		}

		unsigned long expectedClientSize = core->GetAntiCheatSettings()->
			GetExpectedClientSize();
		// Does player's client module size differ from the expected one?
		if (clientModule->GetSize() != expectedClientSize)
		{
			player->Kick("unsupported client version", "module '%s\\%s' size "
				"(%.8lX) differs from the expected one (%.8lX)",
				clientModule->GetPath(), clientModule->GetName(),
				clientModule->GetSize(), expectedClientSize);
			return;
		}

		// Attempt to retrieve gta-vc.exe module so that we can know what
		// player's game directory path is.
		Module* gtaModule = moduleList->FindModule("gta-vc.exe");
		if (!gtaModule)
		{
			player->Kick("absent GTA module", "absent gta-vc.exe module");
			return;
		}

		// Look up for blacklisted modules.
		Module* forbiddenModule = moduleList->FindForbiddenModule(
			gtaModule->GetPath(), clientModule->GetPath());
		if (forbiddenModule)
		{
			player->Kick("suspicious module detected", "suspicious module "
				"'%s\\%s' with size %.8lX detected",
				forbiddenModule->GetPath(), forbiddenModule->GetName(),
				forbiddenModule->GetSize());
			return;
		}
	}
}
