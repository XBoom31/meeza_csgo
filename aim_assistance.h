#pragma once
#include "hooks.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "helpers/math.hpp"
#include "../CSGOSimple/valve_sdk/sdk.hpp"

class CLegitBot
{
public:
	void move(CUserCmd* cmd2, bool& bSendPacket);
	void legitaa(CUserCmd* cmd, bool& bSendPacket);
private:
	inline player_t* gettarget();
	void trigger();
	wconf config;
	inline void getconf();
	inline int gethitbox();
	float curAimTime, deltaTime;
	player_t* target;
	Vector viewoffset;
	CUserCmd* cmd;
	float noise();
	int hitbox;
	bool locked;
}; extern CLegitBot LegitBot;

struct hitscanmeme
{
	hitscanmeme()
	{
		hitbox = HITBOX_HEAD;
		fov = 999.f;
	}
	int hitbox;
	float fov;
};
