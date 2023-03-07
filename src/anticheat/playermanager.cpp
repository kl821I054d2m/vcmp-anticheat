#include "playermanager.hpp"
#include "core.hpp"
#include "utils.hpp"
#include <stdarg.h>

extern Core* core;

PlayerManager::PlayerManager() :
	players()
{
}

PlayerManager::~PlayerManager()
{
	// When shutting down the server, OnPlayerDisconnect gets called AFTER
	// OnServerShutdown and since OnServerShutdown handles core deletion,
	// OnPlayerDisconnect can no longer access the core and hence the player
	// manager for it to be able to delete a player; we have to therefore
	// handle hanging players deletion here because we would otherwise have
	// massive memory leaks.
	for (Player* player : players)
	{
		if (!player)
		{
			continue;
		}

		DeletePlayer(player);
	}
}

int PlayerManager::NewPlayer(Player*& player, int32_t playerId)
{
	// Do we have an invalid player ID?
	if (!IsPlayerIdValid(playerId))
	{
		return PLAYERMGR_INVALID_PLAYER_ID;
	}

	// Is requested player slot already in use?
	if (players[playerId])
	{
		return PLAYERMGR_PLAYER_SLOT_IN_USE;
	}

	// We're OK to allocate a new player at the requested index.
	player = players[playerId] = new Player(playerId);
	return PLAYERMGR_SUCCESS;
}

void PlayerManager::DeletePlayer(Player* player)
{
	// Make occupied slot by the player now available for use.
	players[player->GetId()] = nullptr;
	// Free that memory so we don't leak!
	delete player;
}

Player* PlayerManager::GetPlayer(int32_t playerId)
{
	// Do we have an invalid player ID?
	if (!IsPlayerIdValid(playerId))
	{
		return nullptr;
	}

	// Return player at the requested index.
	return players[playerId];
}

void PlayerManager::ProcessPlayers(float elapsedTime)
{
	for (Player* player : players)
	{
		if (!player)
		{
			continue;
		}

		player->Process(elapsedTime);
	}
}

void PlayerManager::SendMessage(const char* format, ...)
{
	// Set up our message.
	char message[512];
	va_list ap;
	va_start(ap, format);
	vsnprintf(message, sizeof(message), format, ap);
	va_end(ap);

	// Send message for everyone on the server.
	core->GetVCMPFunctions()->SendClientMessage(-1,
		// Add alpha (RGB -> RGBA) as required by this function.
		static_cast<uint32_t>((core->GetAntiCheatSettings()->
			GetMessageColour() << 8) | 0x000000FF),
		PLUGIN_NAME ": %s", message);
}
