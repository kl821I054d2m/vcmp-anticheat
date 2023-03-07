#ifndef ANTICHEAT_EVENTS_HPP
#define ANTICHEAT_EVENTS_HPP

#include "player.hpp"

// Events whose arguments are validated before passing them to the event.
namespace Event
{
	void OnServerFrame(float elapsedTime);

	void OnPlayerConnect(Player* player);
	void OnPlayerModuleList(Player* player, char* list);
}

#endif
