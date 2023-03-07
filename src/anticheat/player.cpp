#include "player.hpp"
#include "core.hpp"
#include <stdarg.h>

extern Core* core;

Player::Player(int32_t playerId)
{
	id = playerId;
	*name = '\0';

#if STRICT_ANTICHEAT
	*ipv4 = *uidv1 = *uidv2 = '\0';
	PluginFuncs* vcmpFunctions = core->GetVCMPFunctions();
	vcmpFunctions->GetPlayerIP(playerId, ipv4, sizeof(ipv4));
	vcmpFunctions->GetPlayerUID(playerId, uidv1, sizeof(uidv1));
	vcmpFunctions->GetPlayerUID2(playerId, uidv2, sizeof(uidv2));
#endif

	totalElapsedTime = 0.0f;
	refusedCheckCount = 0;
	moduleList = nullptr;
}

Player::~Player()
{
	delete moduleList;
}

void Player::Kick() const
{
	core->GetVCMPFunctions()->KickPlayer(id);
}

void Player::Kick(const char* chatboxReason, const char* consoleReasonFormat,
	...) const
{
	// Set up kick reason for console output.
	char consoleReason[1024];
	va_list ap;
	va_start(ap, consoleReasonFormat);
	vsnprintf(consoleReason, sizeof(consoleReason), consoleReasonFormat, ap);
	va_end(ap);

	// Update player's name.
	GetName();

	// Log kick details to the console.
	core->GetConsole()->Log(CONPTYPE_INFO, "Kicking out %s. Reason: %s.", name,
		consoleReason);
	// Log kick details to the chatbox.
	core->GetPlayerManager()->SendMessage("Kicking out %s. Reason: %s.", name,
		chatboxReason);

	// Finally, kick player out.
	Kick();
}

void Player::Process(float elapsedTime)
{
	// Should player not be checked yet?
	if (((totalElapsedTime += elapsedTime) * 1000) <
		core->GetAntiCheatSettings()->GetCheckInterval())
	{
		return;
	}

	// Check them.
	GetModuleList();
	// Player has just been checked so acknowledge that.
	totalElapsedTime = 0.0f;

	unsigned int maxCheckRefusalCount = core->GetAntiCheatSettings()->
		GetMaxCheckRefusalCount();
	// Can we proceed?
	if (maxCheckRefusalCount &&
		// Has their client refused too many check requests?
		(++refusedCheckCount >= maxCheckRefusalCount))
	{
		// We can't keep up.
		Kick("refused too many check requests", "refused %u check request%c",
			refusedCheckCount, (refusedCheckCount != 1) ? 's' : '\0');
		return;
	}
}

const char* Player::GetName() const
{
	// Update player name on every call to this function since other plugins
	// can suppress original player name at any time.
	core->GetVCMPFunctions()->GetPlayerName(id, name, sizeof(name));
	return name;
}

void Player::GetModuleList() const
{
	core->GetVCMPFunctions()->GetPlayerModuleList(id);
}
