#include "aim_assistance.h"
#include "time.h"
FORCEINLINE void VectorSubtract(const Vector& a, const Vector& b, Vector& c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}
#define PI 3.14f

wconf config;
CLegitBot LegitBot;

inline float fovtoplayer(Vector ViewOffset, player_t* player, int hitbox, CUserCmd* cmd)
{
	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	QAngle xd = local->m_aimPunchAngle() * 2;
	QAngle angle = cmd->viewangles + xd;
	// Get local view / eye position
	Vector Origin = ViewOffset;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	Math::AngleVectors(angle, Forward);

	Vector AimPos = player->hitbox_pos(hitbox);
	// Get delta vector between our local eye position and passed vector
	VectorSubtract(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Math::Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	float meme2 = MaxDegrees / 3.14f;
	float meme3 = acos(DotProduct);
	float meme = meme2 * meme3;
	if (meme < 0)
		return 179.f;
	return meme;
}


inline bool CanSee(player_t* target)
{
	for (int x = 0; x < HITBOX_MAX; x++) //x representing the hitbox  1 - head etc
	{
		if (local->player_visible(target, x))
		{
			return true;
		}
	}
	return false;
}

QAngle VectorToQAngle2(Vector x)
{
	return QAngle(x.x, x.y, 0);
}

void CLegitBot::move(CUserCmd* cmd2, bool& bSendPacket)
{
	static bool tappin;
	cmd = cmd2;
	if (!g_EngineClient->IsInGame() || !local || !local->alive())
	{
		locked = false;
		return;
	}
	getconf();



	auto weap = local->m_hActiveWeapon();
	if (!config.active || weap->is_knife() || weap->is_reloading())
	{
		locked = false;
		return;
	}
	if (cmd->viewangles.yaw > 172.f || cmd->viewangles.yaw < -172.f)
	{
		globs.aim = false;
		return;
	}
	else
	{
		globs.aim = true;
	}
	if (!InputSys::Get().IsKeyDown(VK_LBUTTON))
	{
		locked = false;
		if (tappin)
			cmd->buttons &= IN_ATTACK;
		return;
	}
	viewoffset = local->GetRenderOrigin() + local->m_vecViewOffset();
	if (locked)
	{
		auto fov = fovtoplayer(viewoffset, target, HITBOX_UPPER_CHEST, cmd);
		if (config.fov >= fov && target->alive()
			&& !target->IsDormant() && CanSee(target))
		{ //still locked
		  //if (!locked)
			if (config.tap)
			{
				if (fov <= config.tapsnap) //if we can snap to his head and 1tap that nigga uff yaa $
				{
					auto hvec = target->hitbox_pos(0);
					QAngle avec;  // where the enemy is 
					Vector Delta(0, 0, 0);
					VectorSubtract(hvec, viewoffset, Delta);
					Math::VectorAngles(Delta, avec);
					avec.Normalize();
					avec.Clamp();
					cmd2->viewangles = avec; //yes, silent aim
					tappin = false;
				}
				else // if we can't snap and we gotta go slow like with a virgin pussy
				{
					tappin = true;
					auto hvec = target->hitbox_pos(0);
					QAngle avec;  // where the enemy is 
					Vector Delta(0, 0, 0);
					VectorSubtract(hvec, viewoffset, Delta);
					Math::VectorAngles(Delta, avec);
					avec.Normalize();
					avec.Clamp();
					double smoothness;
					if (config.smooth > 0.1f)
					{
						if (config.humanize > 0.1f)
							smoothness = (double)(1.05f - (double)(config.smooth + noise()) / 100);
						else
							smoothness = (double)(1.05f - (double)(config.smooth) / 100);
					}
					else
						smoothness = 0;
					QAngle rcs = local->m_aimPunchAngle();
					QAngle final = avec - rcs * config.rcs / 50;
					QAngle delta = final - cmd2->viewangles;
					if (delta.Length() > smoothness)
					{
						delta *= smoothness;
					}
					QAngle out = cmd2->viewangles + delta;
					out.Normalize();
					out.Clamp();
					cmd2->viewangles = out;
					g_EngineClient->SetViewAngles(cmd2->viewangles);
					cmd->buttons &= ~IN_ATTACK; //pretty much delaying out shot
					if (config.tapsafe)
						trigger(); // if the 
				}
			}
			else
			{
				hitbox = gethitbox();
				auto hvec = target->hitbox_pos(hitbox);
				QAngle avec;  // where the enemy is 
				Vector Delta(0, 0, 0);
				VectorSubtract(hvec, viewoffset, Delta);
				Math::VectorAngles(Delta, avec);
				avec.Normalize();
				avec.Clamp();
				double smoothness;
				if (config.smooth > 0.1f)
				{
					if (config.humanize > 0.1f)
						smoothness = (double)(1.05f - (double)(config.smooth + noise()) / 100);
					else
						smoothness = (double)(1.05f - (double)(config.smooth) / 100);
				}

				if (config.smooth == 0 || config.silent)
				{
					QAngle rcs = local->m_aimPunchAngle();
					QAngle final = avec - rcs * config.rcs / 50;
					final.Normalize();
					final.Clamp();
					cmd2->viewangles = final;
					if (!config.silent)
						g_EngineClient->SetViewAngles(cmd2->viewangles);
				}
				else
				{
					QAngle rcs = local->m_aimPunchAngle();
					QAngle final = avec - rcs * config.rcs / 50;
					QAngle delta = final - cmd2->viewangles;
					if (delta.Length() > smoothness)
					{
						delta *= smoothness;
					}
					QAngle out = cmd2->viewangles + delta;
					out.Normalize();
					out.Clamp();
					cmd2->viewangles = out;
					if (!config.silent)
						g_EngineClient->SetViewAngles(cmd2->viewangles);
				}
			}
			locked = true;
		}
		else
		{
			locked = false;
		}
	}
	else
	{
		gettarget();
		if (target)
			locked = true;
	}

}

void CLegitBot::legitaa(CUserCmd* cmd, bool& bSendPacket)
{
	if (!config.legitaa || (cmd->buttons & IN_ATTACK))
		return;
	return;
	static int poop = -1;
	poop++;

	if (poop< 1)
	{
		bSendPacket = true;
		globs.fake = cmd->viewangles;
	}
	else
	{
		bSendPacket = false;
		static float ang = 0;
		for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
		{
			player_t* entity = (player_t*)g_EntityList->GetClientEntity(i);

			if (!entity) continue;
			if (entity == local) continue;
			if (entity->IsDormant()) continue;
			if (entity->m_iTeamNum() == local->m_iTeamNum()) continue;
			if (!entity->alive()) continue;

			Vector dir; Math::AngleVectors(entity->m_angEyeAngles(), dir);
			Vector start = entity->eye_pos();
			Vector direction = dir * 8192;
			Vector end = start + direction;

			trace_t trace;
			Ray_t ray;
			CTraceFilter filter;

			ray.Init(start, end);
			filter.pSkip = entity;

			g_EngineTrace->TraceRay(ray, 0x46004003, &filter, &trace);

			entity_t* hentity = (entity_t*)trace.hit_entity;

			if (!hentity)
				return;

			if (!hentity->is_player())
				return;

			player_t* player = (player_t*)hentity;

			if (!player)
				return;

			if (!player->alive()) //somehow we hit a dead player
				return;

			if (player == local) //russian proof triggerbot
			{
				if (trace.hitbox == HITBOX_HEAD)
				{
					int random = rand() % 100;
					ang -= 180;
					ang -= random - 50;
				}
			}

		}
		QAngle shit(cmd->viewangles.pitch, ang, 0);
		shit.Normalize();
		cmd->viewangles = shit;
		globs.real = shit;
		poop = -1;
	}
}

inline float CLegitBot::noise()
{
	return (rand() % ((int)(config.humanize / 6.7f))) - (config.humanize / 6.7f);
}

inline player_t* CLegitBot::gettarget()
{
	float bestfov = 100;
	player_t* bestent = nullptr;
	for (int i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
	{
		auto entity2 = player_t::get_player_by_id(i);
		if (!entity2)
			continue;
		if (entity2 == local)
			continue;
		if (entity2->alive() && i < 65 && !entity2->IsDormant())
		{
			if (local->m_iTeamNum() != entity2->m_iTeamNum())
			{
				float fov = fovtoplayer(viewoffset, entity2, HITBOX_UPPER_CHEST, cmd);
				if (fov < config.fov)
				{
					if (bestfov > fov)
					{
						bestfov = fov;
						bestent = entity2;
					}
				}
			}
		}

	}
	target = bestent;
	return bestent;
}

void CLegitBot::trigger()
{
	weapon_t* weapon = local->m_hActiveWeapon();

	weapon_info_t* weapon_data = weapon->get_weapon_data();

	Vector dir; Math::AngleVectors(cmd->viewangles, dir);
	Vector start = local->eye_pos();
	Vector direction = dir * weapon_data->flRange;
	Vector end = start + direction;

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
	if (1 / weapon->innacuracy() >=
		50 * 1.5f) // so we don't hit pnoscopes
		if (weapon->can_fire()) //if we're not reloading or something
			cmd->buttons |= IN_ATTACK; //shoot at that nigga
}

inline void CLegitBot::getconf()
{
	switch (vars.legit.mode)
	{
	case 0:
		config = vars.legit.conf;
	case 2:
		auto w = local->m_hActiveWeapon();
		if (!w) return;
		config = vars.legit.weapons[local->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex()];
	}

}

inline int CLegitBot::gethitbox()
{
	
		int besthitbox = HITBOX_HEAD;
		float bestfov = 100;
		for (int x = 0; x < HITBOX_MAX; x++)
		{
			float fov = fovtoplayer(viewoffset, target, x, cmd);
			if (bestfov > fov)
			{
				bestfov = fov;
				besthitbox = x;
			}
		}
		return besthitbox;

}
