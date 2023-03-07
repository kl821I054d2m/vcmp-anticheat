#ifndef ANTICHEAT_PLAYERMANAGER_HPP
#define ANTICHEAT_PLAYERMANAGER_HPP

#include "player.hpp"

#define PLAYERMGR_SUCCESS            0
#define PLAYERMGR_INVALID_PLAYER_ID  1
#define PLAYERMGR_PLAYER_SLOT_IN_USE 2

class PlayerManager
{
private:
	Player* players[MAX_PLAYERS];

public:
	PlayerManager();
	~PlayerManager();

	int NewPlayer(Player*& player, int32_t playerId);
	void DeletePlayer(Player* player);
	Player* GetPlayer(int32_t playerId);

	void ProcessPlayers(float elapsedTime);
	void SendMessage(const char* format, ...);
};

#endif
