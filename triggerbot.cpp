#include "triggerbot.h"
#include "options.hpp"
#include "helpers\math.hpp"
#include "helpers\input.hpp"
void triggerbot::createmove(CUserCmd* cmd)
{
	if (!vars.legit.active)
		return;
	if (!local)
		return;
	weapon_t* weapon = local->m_hActiveWeapon();
	if (!weapon)
		return;
	if (!vars.legit.weapons[weapon->m_Item().m_iItemDefinitionIndex()].trigger)
		return;
	if (!InputSys::Get().IsKeyDown(vars.legit.weapons[weapon->m_Item().m_iItemDefinitionIndex()].triggerkey))
		return;
	weapon_info_t* weapon_data = weapon->get_weapon_data();

	Vector dir; Math::AngleVectors(cmd->viewangles, dir);
	Vector start      = local->eye_pos();
	Vector direction  =  dir * weapon_data->flRange;
	Vector end        = start + direction;

	trace_t trace;
	Ray_t ray;
	CTraceFilter filter;

	ray.Init(start, end);
	filter.pSkip = local;

	g_EngineTrace->TraceRay(ray, 0x46004003, &filter, &trace);

	entity_t* entity = (entity_t*)trace.hit_entity;

	if (!entity)
		return;

	if (!entity->is_player())
		return;

	player_t* player = (player_t*)entity;

	if (!player)
		return;

	if (!player->alive()) //somehow we hit a dead player
		return;

	if (player->m_iTeamNum() == local->m_iTeamNum()) //russian proof triggerbot
		return; 

	if (player->IsDormant())
		return;

	if (player->m_bGunGameImmunity())
		return;
	if(1 / weapon->innacuracy() >= 
		vars.legit.weapons[weapon->m_Item().m_iItemDefinitionIndex()].thitchance * 1.5f) // so we don't hit pnoscopes
		if (weapon->can_fire()) //if we're not reloading or something
			cmd->buttons |= IN_ATTACK; //shoot at that nigga
}
