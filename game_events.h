#pragma once
#include <iostream>
#include "options.hpp"
#include "valve_sdk/sdk.hpp"
#include "hooks.hpp"
#include "options.hpp"
#include "notificationsystem.h"
#include "hitmarker.h"
#pragma comment(lib, "winmm.lib")


std::wstring missed_shot_memes[] = { L"[meeza.cc] Missed shot due to Earth's curvature.", L"[meeza.cc] Missed shot due to enemy being autistic."
,L"[meeza.cc] Missed shot due to trash resolver.", L"[meeza.cc] Missed shot due to small dick.", L"[penguware] Missed shot due to config.",
L"[gamesense.pub] Missed shot due to spread", L"[ApocalypseCheats] Missed shot due to menu colors.",
L"[meeza.cc] Missed shot due to the cheat refusing to look at gay men.", L"[meeza.cc] Missed shot due to {insert-excuse-for-bad-cheat-here}" };


class C_EventListener : public IGameEventListener2, public Singleton<C_EventListener>
{
public:
	C_EventListener()
	{
		g_GameEvents->AddListener(this, "player_hurt", false);
		g_GameEvents->AddListener(this, "player_death", false);
		g_GameEvents->AddListener(this, "weapon_fire", false);
	}
	~C_EventListener()
	{
		g_GameEvents->RemoveListener(this);
	}

	virtual void FireGameEvent(IGameEvent* event)
	{
		if (!g_GameEvents)
			return;

		int local_id = g_EngineClient->GetLocalPlayer();
		if (!local_id) return;

		player_t* local = (player_t*)g_EntityList->GetClientEntity(local_id);
		if (!local) return;

		player_info_t local_info;
		if (!g_EngineClient->player_info(local->EntIndex(), &local_info))
			return;

		if (!(strcmp(event->GetName(), "player_death")))
		{
			int victim_id = event->GetInt("userid");
			int attacker_id = event->GetInt("attacker");
			auto weapon = event->GetString("weapon");

			int victim = g_EngineClient->GetPlayerForUserID(victim_id);
			int attacker = g_EngineClient->GetPlayerForUserID(attacker_id);

			//if (attacker == local->EntIndex())
			//	configs::killed();
		}

		if (!(strcmp(event->GetName(), "player_hurt")))
		{
			int victim_id = event->GetInt("userid");
			int attacker_id = event->GetInt("attacker");
			auto weapon = event->GetString("weapon");

			int victim = g_EngineClient->GetPlayerForUserID(victim_id);
			int attacker = g_EngineClient->GetPlayerForUserID(attacker_id);

			if (attacker == local->EntIndex())
			{

				globs.missed_shots[globs.brute_ent][globs.brute_id]--;
				if(vars.misc.hitmarker)
				hitmarker.hit();
				NotificatonSys.remove();
			}
		}

		if (!(strcmp(event->GetName(), "weapon_fire"))) {
			int victim_id = event->GetInt("userid");
			int victim = g_EngineClient->GetPlayerForUserID(victim_id);
			if (victim == local->EntIndex() && vars.rage.active)
			{
				globs.missed_shots[globs.brute_ent][globs.brute_id]++;
				NotificatonSys.output(missed_shot_memes[rand() % 9], Color(200, 200, 200));
			}
		}
	}

	int GetEventDebugID() override
	{
		return 42;
	}
};

