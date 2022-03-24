#pragma once
#include "helpers/math.hpp"
#include "helpers/utils.hpp"
#include "helpers/input.hpp"
#include "valve_sdk/csgostructs.hpp"

#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10

namespace Autowall
{
	float CanHit(Vector &vecEyePos, Vector &point, player_t* filter = nullptr);
	bool FireBullet(weapon_t* pWeapon, Vector& direction, float& currentDamage, player_t* filter2);
	bool HandleBulletPenetration(weapon_info_t* weaponData, CGameTrace& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration);
	bool TraceToExit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, Vector direction);
	void ScaleDamage(CGameTrace &enterTrace, weapon_info_t *weaponData, float& currentDamage);
	bool IsBreakableEntity(IClientEntity* entity);
	void GetBulletTypeParameters(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type);
	void ClipTraceToPlayers(Vector& absStart, Vector absEnd, unsigned int mask, ITraceFilter* filter, CGameTrace* tr);
	void TraceLine(Vector& absStart, Vector& absEnd, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr);
}