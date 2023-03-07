#ifndef ANTICHEAT_PLAYER_HPP
#define ANTICHEAT_PLAYER_HPP

#include "module.hpp"
#include <vcmp.h>

class Player
{
private:
	int32_t id;
	mutable char name[MAX_PLAYER_NAME];
#if STRICT_ANTICHEAT
	char ipv4[MAX_PLAYER_IPV4];
	char uidv1[MAX_PLAYER_UID];
	char uidv2[MAX_PLAYER_UID];
#endif

	float totalElapsedTime;

public:
	unsigned int refusedCheckCount;
	ModuleList* moduleList;

	Player(int32_t playerId);
	~Player();

	void Kick() const;
	void Kick(const char* chatboxReason, const char* consoleReasonFormat,
		...) const;
	void Process(float elapsedTime);

	int32_t GetId() const { return id; }
	const char* GetName() const;
#if STRICT_ANTICHEAT
	const char* GetIPV4() const { return ipv4; }
	const char* GetUIdV1() const { return uidv1; }
	const char* GetUIdV2() const { return uidv2; }
#endif
	void GetModuleList() const;
};

#endif
