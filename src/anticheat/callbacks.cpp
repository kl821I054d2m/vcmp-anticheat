#include "callbacks.hpp"
#include "core.hpp"
#include "events.hpp"
#include "utils.hpp"
#include <inttypes.h>
#include <stdlib.h>

extern Core* core;

void Callback::OnServerShutdown(void)
{
	delete core;
	core = nullptr;
}

void Callback::OnServerFrame(float elapsedTime)
{
	if (!core)
	{
		return;
	}

	Event::OnServerFrame(elapsedTime);
}

void Callback::OnPlayerConnect(int32_t playerId)
{
	if (!core)
	{
		return;
	}

	// Attempt to allocate a new player.
	Player* newPlayer;
	int x = core->GetPlayerManager()->NewPlayer(newPlayer, playerId);
	if (x != PLAYERMGR_SUCCESS)
	{
		core->GetConsole()->Log(CONPTYPE_ERROR, "%s was unable to make player "
			"at index %" PRId32 " known for us (%d).", __func__, playerId, x);
		return;
	}

	Event::OnPlayerConnect(newPlayer);
}

void Callback::OnPlayerDisconnect(int32_t playerId,
	vcmpDisconnectReason reason)
{
	if (!core)
	{
		return;
	}

	PlayerManager* playerManager = core->GetPlayerManager();

	// Attempt to retrieve previously allocated player.
	Player* player = playerManager->GetPlayer(playerId);
	if (!player)
	{
		core->GetConsole()->Log(CONPTYPE_ERROR, "%s was unable to retrieve "
			"player at index %" PRId32 ".", __func__, playerId);
		return;
	}

	// We allocated a new player on join; now, logically, delete it on quit.
	playerManager->DeletePlayer(player);
}

void Callback::OnPlayerModuleList(int32_t playerId, const char* list)
{
	if (!core)
	{
		return;
	}

	// Attempt to retrieve previously allocated player.
	Player* player = core->GetPlayerManager()->GetPlayer(playerId);
	if (!player)
	{
		core->GetConsole()->Log(CONPTYPE_ERROR, "%s was unable to retrieve "
			"player at index %" PRId32 ".", __func__, playerId);
		return;
	}

	// Do we have an invalid list?
	if (!list)
	{
		core->GetConsole()->Log(CONPTYPE_ERROR, "%s provided an invalid "
			"module list for %s.", __func__, player->GetName());
		return;
	}

	// Attempt to make a copy of 'list' so that we can modify it.
	char* moduleList = StrDup(list);
	if (!moduleList)
	{
		core->GetConsole()->Log(CONPTYPE_ERROR, "%s was unable to make a copy "
			"of %s's module list.", __func__, player->GetName());
		return;
	}

	Event::OnPlayerModuleList(player, moduleList);
	free(moduleList);
}
