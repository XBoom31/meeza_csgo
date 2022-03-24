#include "backtrake.h"
#include "helpers/math.hpp"
#include "options.hpp"

#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

void TimeWarp::CreateMove(CUserCmd* cmd)
{

	int bestTargetIndex = -1;
	float bestFov = FLT_MAX;

	if (!local->alive())
		return;

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto pEntity = static_cast<player_t*>(g_EntityList->GetClientEntity(i));
		if (!pEntity || !local) continue;
		if (!pEntity->is_player()) continue;
		if (pEntity == local) continue;
		if (pEntity->IsDormant()) continue;
		if (!pEntity->alive()) continue;
		if (pEntity->m_iTeamNum() == local->m_iTeamNum()) continue;

		float simtime = pEntity->m_simtime();
		Vector hitboxPos = pEntity->hitbox_pos(0);

		TimeWarpData[i][cmd->command_number % (NUM_OF_TICKS + 1)] = StoredData{ simtime, hitboxPos };
		Vector ViewDir;
		Math::AngleVectors(cmd->viewangles + (local->m_aimPunchAngle() * 2.f), ViewDir);
		float FOVDistance = Math::DistancePointToLine(hitboxPos, local->eye_pos(), ViewDir);

		if (bestFov > FOVDistance)
		{
			bestFov = FOVDistance;
			bestTargetIndex = i;
		}
	}

	float bestTargetSimTime = -1;
	if (bestTargetIndex != -1)
	{
		float tempFloat = FLT_MAX;
		Vector ViewDir;
		Math::AngleVectors(cmd->viewangles + (local->m_aimPunchAngle() * 2.f), ViewDir);
		for (int t = 0; t < NUM_OF_TICKS; ++t)
		{
			float tempFOVDistance = Math::DistancePointToLine(TimeWarpData[bestTargetIndex][t].hitboxPos, local->eye_pos(), ViewDir);
			if (tempFloat > tempFOVDistance && TimeWarpData[bestTargetIndex][t].simtime > local->m_simtime() - 1)
			{
				if (local->player_visible(static_cast<player_t*>(g_EntityList->GetClientEntity(bestTargetIndex)), TimeWarpData[bestTargetIndex][t].hitboxPos))
				{
					tempFloat = tempFOVDistance;
					bestTargetSimTime = TimeWarpData[bestTargetIndex][t].simtime;
				}
			}
		}

		if (bestTargetSimTime >= 0 && cmd->buttons & IN_ATTACK)
			cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);
	}
}