#include "Rage.h"
#include "options.hpp"


#define TICKS_TO_TIME(t) (g_GlobalVars->interval_per_tick * (t))
Animations anims;
CRagebot rage;
bool BreakLBY();
FORCEINLINE void VectorSubtract(const Vector& a, const Vector& b, Vector& c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

static bool lbymeme2;

/*void Animations::UpdateServerAnimations(CUserCmd* cmd, bool* bsendpacket)
{
m_pLocalPlayer = local;
if (!m_pLocalPlayer)
return;

bool allocate = (m_serverAnimState == nullptr);
bool change = (!allocate) && (m_pLocalPlayer->GetRefEHandle() != m_ulEntHandle);
bool reset = (!allocate && !change) && (m_pLocalPlayer->m_flSpawnTime() != m_flSpawnTime);

if (change)
{
ZeroMemory(m_serverAnimState, 0);
}

if (reset)
{
ResetAnimationState(m_serverAnimState);
m_flSpawnTime = m_pLocalPlayer->m_flSpawnTime();
}

if (allocate || change)
{
auto state = reinterpret_cast< CCSGOPlayerAnimState* > (malloc(0x344));

if (state != nullptr)
CreateAnimationState(state, m_pLocalPlayer);

m_ulEntHandle = m_pLocalPlayer->GetRefEHandle();
m_flSpawnTime = m_pLocalPlayer->m_flSpawnTime();

m_serverAnimState = state;
}

else if (!*(int*)((DWORD)g_ClientState + 0x4CB0))
{
m_angRealAngle = cmd->viewangles;

UpdateAnimationState(m_serverAnimState, m_angRealAngle);

float delta = std::abs(m_angRealAngle.yaw - m_pLocalPlayer->mlby());

if (m_serverAnimState->m_flSpeed > 0.1f)
{
m_flNextBodyUpdate = g_GlobalVars->curtime + 0.22f;
m_flFirstBodyUpdate = m_flNextBodyUpdate;
lbymeme2 = true;

}
else if (g_GlobalVars->curtime > m_flNextBodyUpdate)
{
if (m_flFirstBodyUpdate != m_flNextBodyUpdate)
lbymeme2 = true;
m_flNextBodyUpdate = g_GlobalVars->curtime + 1.1f;
}
}
}*/
void Animations::UpdateServerAnimations(CUserCmd *userCMD, bool *bsendpacket)
{
	if (!local->alive())
		return;
	bool
		allocate = (m_serverAnimState == nullptr),
		change = (!allocate) && (&local->GetRefEHandle() != m_ulEntHandle),
		reset = (!allocate && !change) && (local->m_flSpawnTime() != m_flSpawnTime);

	// player changed, free old animation state.
	if (change)
		ZeroMemory(m_serverAnimState, 0);

	// need to reset? (on respawn)
	if (reset) {
		// reset animation state.
		ResetAnimationState(m_serverAnimState);

		// note new spawn time.
		m_flSpawnTime = local->m_flSpawnTime();
	}

	// need to allocate or create new due to player change.
	if (allocate || change) {

		// only works with games heap alloc.
		CCSGOPlayerAnimState *state = (CCSGOPlayerAnimState*)malloc(sizeof(CCSGOPlayerAnimState));

		if (state != nullptr)
			CreateAnimationState(state, local);

		// used to detect if we need to recreate / reset.
		m_ulEntHandle = const_cast<CBaseHandle*>(&local->GetRefEHandle());
		m_flSpawnTime = local->m_flSpawnTime();

		// note anim state for future use.
		m_serverAnimState = state;
	}

	// back up some data to not mess with game.. 

	float cur_time = cur_time = TICKS_TO_TIME(g_GlobalVars->tickcount);
	if (!*(int*)((DWORD)g_ClientState + 0x4CB0))
	{
		m_serverAnimState->m_flSpeed = local->m_vecVelocity().Length();

		// calculate vars
		float delta = std::abs(Math::NormalizeYaw(userCMD->viewangles.yaw - local->mlby()));

		// walking, delay next update by .22s.
		if (m_serverAnimState->m_flSpeed > 0.1f &&
			(local->m_fFlags() & FL_ONGROUND))
			m_flNextBodyUpdate = cur_time + 0.22f;

		// standing, update every 1.1s.
		else if (cur_time >= m_flNextBodyUpdate) {

			if (delta > 35.f)
				; //server will update lby.

			m_flNextBodyUpdate = cur_time + 1.1f;
		}
	}

	// if was jumping and then onground and bsendpacket true, we're gonna update.
	lbymeme2 = (local->m_fFlags() & FL_ONGROUND) && ((m_flNextBodyUpdate - cur_time) <= g_GlobalVars->interval_per_tick);

	// bandaid fix, sends 1 command before break to clear commands, sends another for break. IDK how 2 fix yet.
	/*if (m_bBreakLowerBody)
	Global::bSendPacket = true;*/
}
void Animations::CreateAnimationState(CCSGOPlayerAnimState* state, player_t* player) {
	using CreateAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*, player_t*);
	static auto CreateAnimState = (CreateAnimState_t)Utils::PatternScan(GetModuleHandle(L"client.dll"), "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
	if (!CreateAnimState)
		return;

	CreateAnimState(state, player);

}

void Animations::UpdateAnimationState(CCSGOPlayerAnimState *state, QAngle angle)
{
	typedef void(__vectorcall *fnUpdateAnimState)(void*, void*, float, float, float, void*);

	static auto UpdateAnimState = (fnUpdateAnimState)Utils::PatternScan(GetModuleHandle(L"client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");

	if (!UpdateAnimState)
		return;

	UpdateAnimState(state, NULL, NULL, angle.yaw, angle.pitch, NULL);
}

void Animations::ResetAnimationState(CCSGOPlayerAnimState* state) {
	using ResetAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*);
	static auto ResetAnimState = (ResetAnimState_t)Utils::PatternScan(GetModuleHandle(L"client.dll"), "56 6A 01 68 ? ? ? ? 8B F1");
	if (!ResetAnimState)
		return;

	ResetAnimState(state);
}

void SetAngle(CUserCmd* cmd, bool& bSendPacket, QAngle real, QAngle fake)
{

	QAngle r = real, f = fake;
	r.Normalize();
	r.Clamp();
	f.Normalize();
	f.Clamp();

	static int poop = -1;
	poop++;

	if (poop< 1)
	{
		bSendPacket = true;
		cmd->viewangles = f; 
		globs.fake = f;//global
	}
	else
	{
		bSendPacket = false;
		cmd->viewangles = r;
		globs.real = r; //global
		poop = -1;
	}
	if (!bSendPacket) {
		if (BreakLBY())
			cmd->viewangles.yaw = local->mlby();
		else
			cmd->viewangles.yaw = local->mlby() + 120;
	}
		


}
void SetAngle(CUserCmd* cmd, bool& bSendPacket, QAngle real)
{

}
static bool right = true;
static float bestyaw = 0;
float CRagebot::getbestyaw(CUserCmd* cmd)
{
	float best_fov = 180.f;
	player_t* best_ent = nullptr;
	for (int ent_index = 0; ent_index < g_EntityList->GetHighestEntityIndex(); ent_index++)
	{
		player_t* n1ggerfagg0t = player_t::get_player_by_id(ent_index);
		if (!n1ggerfagg0t)
			continue;
		if (n1ggerfagg0t == local)
			continue;
		if (!n1ggerfagg0t->alive())
			continue;
		if (n1ggerfagg0t->IsDormant())
			continue;
		//if (n1ggerfagg0t->m_bGunGameImmunity())
		//	continue;
		if (local->m_iTeamNum() != n1ggerfagg0t->m_iTeamNum())
			continue;

		float fov_to_player = FovToPlayer(local->GetRenderOrigin() + local->m_vecViewOffset(),
			n1ggerfagg0t, HITBOX_HEAD, cmd);

		if (fov_to_player < best_fov)
		{
			best_fov = fov_to_player;
			best_ent = n1ggerfagg0t;
		}
	}

	if (!best_ent)
		return 0;

	Vector eye_pos = local->eye_pos();
	Vector enemy_eye_pos = best_ent->eye_pos();

	float byaw = -1;
	float bdmg = 100;
	for (int step = 0; step < 9; step++)
	{
		float ang = step * 45.f;
		Vector head_pos(cos(ang) + eye_pos.x, sin(ang) + eye_pos.y, eye_pos.z);
		float damage = Autowall::CanHit(enemy_eye_pos, head_pos, best_ent);

		//	Utils::ConsolePrint("%f\n",damage);
		if (damage < bdmg)
		{
			bdmg = damage;
			byaw = ang;
		}
	}
	if (byaw != -1)
		bestyaw = byaw;
	return bestyaw;
}
float get_yaw(CUserCmd* cmd)
{
	static float bestdist = FLT_MAX;
	static player_t* player;
	for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++)
	{

		auto n1ggerfagg0t = player_t::get_player_by_id(i);
		if (!n1ggerfagg0t)
			continue;
		if (n1ggerfagg0t == local)
			continue;

		if (n1ggerfagg0t->alive() && i < 65 && !n1ggerfagg0t->IsDormant())
		{
			if (local->m_iTeamNum() != n1ggerfagg0t->m_iTeamNum())
			{
				if (n1ggerfagg0t->m_bGunGameImmunity())
					continue;
				Vector delta;
				VectorSubtract(n1ggerfagg0t->eye_pos(), local->eye_pos(), delta);
				float dist = delta.Length();
				if (dist < bestdist)
				{
					bestdist = dist;
					player = n1ggerfagg0t;
				}
			}
		}
	}
	float original_yaw;
	if(true)
		original_yaw = cmd->viewangles.yaw;
	else {
		Vector pos = player->eye_pos();
		Vector viewoffset = local->GetRenderOrigin() + local->m_vecViewOffset();
		Vector delta;
		VectorSubtract(pos, viewoffset, delta);
		QAngle meme;
		Math::VectorAngles(delta, meme);
		original_yaw = meme.yaw;
	}
	const float jittler = 25;
	const float dist = 200.f;

	Vector my_head = local->eye_pos();


	Vector forward, right, up; Math::AngleVectors(cmd->viewangles, forward, right, up);
	Vector end = my_head + forward * 400;

	Vector my_left = Vector(dist * cos(original_yaw - 90) + my_head.x, dist * sin(original_yaw - 90) + my_head.y, my_head.z);
	Vector my_right = Vector(dist * cos(original_yaw + 90) + my_head.x, dist * sin(original_yaw + 90) + my_head.y, my_head.z);
	Vector end_left = Vector(dist * cos(original_yaw - 90) + end.x, dist * sin(original_yaw - 90) + end.y, end.z);
	Vector end_right = Vector(dist * cos(original_yaw - 90) + end.x, dist * sin(original_yaw - 90) + end.y, end.z);

	Ray_t ray;
	trace_t trace;
	CTraceFilter filter;
	filter.pSkip = local;
	ray.Init(my_head, end);
	g_EngineTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace);

	Ray_t ray_left;
	trace_t trace_left;
	CTraceFilter filter_left;
	filter_left.pSkip = local;
	ray_left.Init(my_head - right * 35, end_left - right * 35);
	globs.left1 = my_head - right * 35; globs.left1 = end_left - right * 35;
	g_EngineTrace->TraceRay(ray_left, MASK_SHOT_BRUSHONLY, &filter_left, &trace_left);

	Ray_t ray_right;
	trace_t trace_right;
	CTraceFilter filter_right;
	filter_right.pSkip = local;
	ray_right.Init(my_head + right * 35, end_left + right * 35);
	globs.right1 = my_head + right * 35; globs.right2 = end_left + right * 35;
	g_EngineTrace->TraceRay(ray_right, MASK_SHOT_BRUSHONLY, &filter_right, &trace_right);

	Vector delta;
	Vector delta_left;
	Vector delta_right;

	VectorSubtract(trace.endpos, my_head, delta); //no need for this but im too lazy to delete the code it gets optimized anyway
	VectorSubtract(trace_left.endpos, my_left, delta_left);
	VectorSubtract(trace_right.endpos, my_right, delta_right);

	float lenght_left = delta_left.Length();
	float lenght_right = delta_right.Length();

	bool backwards = (rand() % 15 == 5);

	//if(backwards)
	//	return original_yaw - 180 + (rand() % /*cooz it lookz b1g*/(int)jittler) / 2;

	if (lenght_left < lenght_right)
		return original_yaw - 90 + (rand() % /*cooz it lookz b1g*/(int)jittler) / 2;
	else if (lenght_left > lenght_right)
		return original_yaw + 90 + (rand() % /*cooz it lookz b1g*/(int)jittler) / 2;
	else
		return original_yaw - 180 + (rand() % /*cooz it lookz b1g*/(int)jittler) / 2;
}
static float last_lby;
bool lbyupdate()
{
	int screen_width, screen_height;
	g_EngineClient->GetScreenSize(screen_width, screen_height);

	static float next_lby_update[65];
	//static float last_lby[65];
	static float last_move;

	const float curtime = g_GlobalVars->curtime;

	if (local->m_vecVelocity().Length2D() > 0.1 /*&& !GLOBAL::is_fakewalking*/)
	{
		last_move = curtime;
		return true;
	}

	static float last_lby;
	float lby = local->mlby();
	if (last_lby != lby)
	{
		last_lby = lby;
		if (fabs(curtime - last_move) > 0.3 && fabs(Math::NormalizeYaw(lby - last_lby)) > 35.f)
			next_lby_update[local->EntIndex()] = curtime + 1.1 + g_GlobalVars->interval_per_tick;
	}

	if (next_lby_update[local->EntIndex()] < curtime)
	{
 		next_lby_update[local->EntIndex()] = curtime + 1.1;
	}

	float time_remain_to_update = next_lby_update[local->EntIndex()] - local->m_simtime();
	if (time_remain_to_update < 0)
	{	
		return true;
	}
	return false;
	//width = (((time_remain_to_update * (110 / 1.1f))));
	//RENDER::DrawFilledRect((20 - (screen_height - 15)) + 10, 10 + (screen_height - 20) + 4, width, 10 + (screen_height - 10) + 6, color);
}
bool BreakLBY()
{

	static float flOldCurtime = 0.f;
	static bool bWasMoving = false;
	static bool bBreakNextTick = false;

	//if (g::bSendPacket)
	//{
	if (local->m_vecVelocity().Length() > 0.1f)
	{
		flOldCurtime = g_GlobalVars->curtime;
		bWasMoving = true;
	}
	else
	{
		if (bWasMoving && g_GlobalVars->curtime - flOldCurtime >= 0.22f)
		{
			bWasMoving = false;
			flOldCurtime = g_GlobalVars->curtime;
			bBreakNextTick = true;
		}
		else if (g_GlobalVars->curtime - flOldCurtime >= 1.1f)
		{
			flOldCurtime = g_GlobalVars->curtime;
			bBreakNextTick = true;
		}
	}

	if (bBreakNextTick)
	{
		return true;
	}
	else
		return false;

}
void CRagebot::AntiAim(CUserCmd* cmd, bool& bSendPacket)
{
	//anims.UpdateServerAnimations(cmd, &bSendPacket);
	if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK)
		return;
	if (local->m_bGunGameImmunity() || local->m_fFlags() & FL_FROZEN || local->move_type() == MOVETYPE_LADDER 
		|| local->m_hActiveWeapon()->is_grenade())
		return;
	int x = AA_X_LBY;
	int y = AA_Y_DOWN; 
	int fakex = AA_X_JITTLERSPIN;
	getbestyaw(cmd);
	if (InputSys::Get().IsKeyDown(0x44)) // D key
		right = true;
	if (InputSys::Get().IsKeyDown(0x41)) // A key
		right = false;

	QAngle real = cmd->viewangles, fake = cmd->viewangles;
	/// real angle
	if (x != AA_X_NONE)
	{
		switch (x)
		{
		case AA_X_BACKWARDS:
			real.yaw -= 180;
			break;
		case AA_X_FASTSPIN:
			real.yaw = fmod(g_GlobalVars->curtime / 10.f * 360.f, 360.f) + 180;
			break;
		case AA_X_LBY:
			if (!(local->m_fFlags() & FL_ONGROUND)) {
				globs.blby = true;
				real.yaw = rand() % 360;
			}
			else
			{
				if (BreakLBY()) {
					globs.blby = false;
					real.yaw = local->mlby();
				}
				else {//pfix
					globs.blby = true;
					if (vars.rage.baa)
						if (vars.rage.baaside)
							real.yaw += 90;
						else
							real.yaw -= 90;
					else
							real.yaw = local->mlby() + 120;
				}
			}
			break;
		case AA_X_LBYBREAKER: /// no ideea how this works so i will check it later i guess
			real.yaw = local->mlby();
			break;
		case AA_X_RANDOM:
			real.yaw = rand() % 360 - 180;
			break;
		case AA_X_SIDE:
			if (right)
				real.yaw += 90;
			else
				real.yaw -= 90;
			break;
		case AA_X_SLOWSPIN:
			real.yaw = fmod(g_GlobalVars->curtime / 10.f * 360.f, 360.f) + 180;
			break;
		}
	}
	///fake angle
	switch (fakex)
	{
	case AA_X_JITTLERSPIN:
		static int memed;
		static int step;
		if ((int)(g_GlobalVars->realtime * 2) % 5 == 0) { //make it so it dissapears after some time
			if (!memed)
			{
				if (step == 8)
					step = 0;
				else
					step++;
				memed = true;
			}
		}
		else
			memed = false;
		fake.yaw = 45 * step + (rand() % 50) - 25;
		break;
	case AA_X_BACKWARDS:
		fake.yaw -= 180;
		break;
	case AA_X_FASTSPIN:
		fake.yaw = fmod(g_GlobalVars->curtime / 10.f * 360.f, 360.f) + 180;
		break;
	case AA_X_LBY:
		fake.yaw = bestyaw - 180;
		break;
	case AA_X_LBYBREAKER: /// no ideea how this works so i will check it later i guess
		fake.yaw = local->mlby();
		break;
	case AA_X_RANDOM:
		fake.yaw = rand() % 360 - 180;
		break;
	case AA_X_SIDE:
		if (!right)  ///so this is the opposite of teh true angle if they are both fake
			fake.yaw += 90;
		else
			fake.yaw -= 90;
		break;
	case AA_X_SLOWSPIN:
		fake.yaw = fmod(g_GlobalVars->curtime / 10.f * 360.f, 360.f) + 180;
		break;
	}
	/// y angle
	if (x != AA_Y_NONE)
	{
		switch (y)
		{
		case AA_Y_UP:
			real.pitch = -89;
			fake.pitch = -89;
			break;
		case AA_Y_RANDOM:
		{
			float randp = rand() % 178 - 89;
			real.pitch = randp;
			fake.pitch = randp;
		}
		break;
		case AA_Y_DOWN:
			real.pitch = 89;
			fake.pitch = 89;
			break;
		}
	}

	if (fakex == AA_X_NONE)
		SetAngle(cmd, bSendPacket, real);
	else
		SetAngle(cmd, bSendPacket, real, fake);

	if (InputSys::Get().IsKeyDown(VK_F1))
		g_EngineClient->SetViewAngles(cmd->viewangles);

}
bool CanHit(Vector point, int mindmg)
{
	float h = Autowall::CanHit(local->eye_pos(), point);
	//	Utils::ConsolePrint("%f\n", h);
	if (h >= mindmg)
		return true;
	else
		return false;
}
int l4rg3_r3s0lv3r(player_t* player)
{
	std::vector<int> HitBoxesToScan;
	HitBoxesToScan.push_back((int)HITBOX_PELVIS);
	HitBoxesToScan.push_back((int)HITBOX_LOWER_CHEST);
	HitBoxesToScan.push_back((int)HITBOX_UPPER_CHEST);
	HitBoxesToScan.push_back((int)HITBOX_HEAD);
	for (auto h : HitBoxesToScan)
	{
		if (CanHit(player->hitbox_pos(h), vars.rage.mindmg))
			return h;
	}
	return 0;
}
void AutoCockRevolver(weapon_t* weapon, CUserCmd *cmd)
{
	if (weapon->m_Item().m_iItemDefinitionIndex() != WEAPON_REVOLVER)
		return;

	if (!weapon->can_fire())
		return;

	static int delay = 0;
	delay++;

	if (delay <= 13)
		cmd->buttons |= IN_ATTACK;
	else
		delay = 0;
}
void CRagebot::Run(CUserCmd* cmd, bool& bSendPacket)
{
	if (g_EngineClient->IsInGame() && local->alive())
	{
		weapon_t* weapon = local->m_hActiveWeapon().Get();
		if (vars.rage.autostop)
		{
			Vector velocity = local->m_vecVelocity();
			Vector direction = velocity.Angle();
			float speed = velocity.Length();

			direction.y = cmd->viewangles.yaw - direction.y;

			Vector negated_direction = direction * -speed;
			if (velocity.Length() >= (weapon->get_weapon_data()->flMaxPlayerSpeed * .34f))
			{
				cmd->forwardmove = negated_direction.x;
				cmd->sidemove = negated_direction.y;
			}
		}
		if (vars.rage.aa)
			AntiAim(cmd, bSendPacket);
		//if (weapon->is_knife())
		//	return;
		if (!weapon->has_bullets())
			return;
		if (vars.rage.autorevolver)
			AutoCockRevolver(weapon, cmd);
		for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++)
		{

			auto n1ggerfagg0t = player_t::get_player_by_id(i);
			if (!n1ggerfagg0t)
				continue;
			if (n1ggerfagg0t == local)
				continue;

			if (n1ggerfagg0t->alive() && i < 65 && !n1ggerfagg0t->IsDormant())
			{
				if (local->m_iTeamNum() != n1ggerfagg0t->m_iTeamNum())
				{
					if (n1ggerfagg0t->m_bGunGameImmunity())
						continue;
					//if (Hitchance())
					//{
					int hitbox;
					//if (!(n1ggerfagg0t->m_fFlags() & FL_ONGROUND))
					//	hitbox = HITBOX_PELVIS;
					//else
					hitbox = HitScan(n1ggerfagg0t);

					
					if (hitbox != -1)
					{
						if (1 / weapon->innacuracy() >= vars.rage.hitchance * 1.5f)
						{
							Vector ViewOffset = local->GetRenderOrigin() + local->m_vecViewOffset();
							if (vars.rage.fov >= FovToPlayer(ViewOffset, n1ggerfagg0t, hitbox, cmd))
							{
								auto hvec = n1ggerfagg0t->hitbox_pos(hitbox);
								QAngle avec;
								Vector Delta(0, 0, 0);
								VectorSubtract(hvec, ViewOffset, Delta);
								Math::VectorAngles(Delta, avec);
								SetAngles(cmd, avec);
								//cmd->viewangles = avec;
								if (vars.rage.autoshoot) {
									globs.brute_ent = n1ggerfagg0t->EntIndex();
									if (!weapon->get_weapon_data()->bFullAuto) {
										static bool pistol;
										if(pistol)
										    cmd->buttons |= IN_ATTACK;
										pistol != pistol;
									}
									else {
										cmd->buttons |= IN_ATTACK;
									}
								}

							}
						}
					}
					//}

				}

			}
		}

	}
}

void CRagebot::SetAngles(CUserCmd* cmd, int x, int y)
{
}
bool CRagebot::CanHit(Vector point, int mindmg)
{
	float h = Autowall::CanHit(local->eye_pos(), point);
	//	Utils::ConsolePrint("%f\n", h);
	if (h >= mindmg)
		return true;
	else
		return false;
}
bool CanBaim(player_t* player)
{
	if (Autowall::CanHit(local->eye_pos(), player->hitbox_pos(HITBOX_PELVIS)) >
		player->m_iHealth())
		return true;
	return false;

}
int CRagebot::HitScan(player_t* player)
{
	//player_tplayer_visible(player, HITBOX_HEAD))
	if (!vars.rage.hitscan)
	{
		if (!vars.rage.awall)
		{
			if (local->player_visible(player, HITBOX_HEAD))
				return HITBOX_HEAD;
		}
		else
		{
			if (CanHit(player->hitbox_pos(HITBOX_HEAD), vars.rage.mindmg))
			{
				return HITBOX_HEAD;
			}
		}
	}
	else
	{ //straight apocalypse v1 paste
		std::vector<int> HitBoxesToScan;
		auto w = local->m_hActiveWeapon();
		if (!w)
			return -1;
		auto da = w->get_weapon_data();
		if (!da)
			return -1;
		if (!da->szWeaponName)
			return -1;
		auto name = da->szWeaponName;
		if (!name)
			return -1;
		if (name == (char*)0x01000101)
			return -1;
		if (strstr(name, "awp"))
		{
			HitBoxesToScan.push_back((int)HITBOX_PELVIS);
			HitBoxesToScan.push_back((int)HITBOX_LOWER_CHEST);
			HitBoxesToScan.push_back((int)HITBOX_PELVIS);
			HitBoxesToScan.push_back((int)HITBOX_UPPER_CHEST);
			HitBoxesToScan.push_back((int)HITBOX_HEAD);
		}
		else
		{
			if (CanBaim(player))
			{
				HitBoxesToScan.push_back((int)HITBOX_LOWER_CHEST);
				HitBoxesToScan.push_back((int)HITBOX_PELVIS);
				HitBoxesToScan.push_back((int)HITBOX_UPPER_CHEST);
				HitBoxesToScan.push_back((int)HITBOX_HEAD);
			}
			else
			{
				if (player->m_vecVelocity().Length()< 75 || player->m_iHealth() < 10)
				{
					HitBoxesToScan.push_back((int)HITBOX_HEAD);
					HitBoxesToScan.push_back((int)HITBOX_NECK);
					HitBoxesToScan.push_back((int)HITBOX_PELVIS);
					HitBoxesToScan.push_back((int)HITBOX_UPPER_CHEST);
					HitBoxesToScan.push_back((int)HITBOX_LOWER_CHEST);
					HitBoxesToScan.push_back((int)HITBOX_LEFT_UPPER_ARM);
					HitBoxesToScan.push_back((int)HITBOX_RIGHT_UPPER_ARM);
					HitBoxesToScan.push_back((int)HITBOX_LEFT_THIGH);
					HitBoxesToScan.push_back((int)HITBOX_RIGHT_THIGH);
					HitBoxesToScan.push_back((int)HITBOX_LEFT_HAND);
					HitBoxesToScan.push_back((int)HITBOX_RIGHT_HAND);
					HitBoxesToScan.push_back((int)HITBOX_LEFT_FOOT);
					HitBoxesToScan.push_back((int)HITBOX_RIGHT_FOOT);
					HitBoxesToScan.push_back((int)HITBOX_LEFT_CALF);
					HitBoxesToScan.push_back((int)HITBOX_RIGHT_CALF);
					HitBoxesToScan.push_back((int)HITBOX_RIGHT_FOREARM);
					HitBoxesToScan.push_back((int)HITBOX_LEFT_FOREARM);
				}
				else
				{
					HitBoxesToScan.push_back((int)HITBOX_HEAD);
					HitBoxesToScan.push_back((int)HITBOX_NECK);
					HitBoxesToScan.push_back((int)HITBOX_PELVIS);
					HitBoxesToScan.push_back((int)HITBOX_UPPER_CHEST);

				}
			}
		}

		for (auto h : HitBoxesToScan)
		{
			if (CanHit(player->hitbox_pos(h), vars.rage.mindmg))
				return h;
		}
	}
	return -1;
}

//ayyware pasted function
float CRagebot::FovToPlayer(Vector ViewOffset, player_t* player, int hitbox, CUserCmd* cmd)
{
	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	QAngle angle = cmd->viewangles;
	// Get local view / eye position
	Vector Origin = ViewOffset;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	Math::AngleVectors(angle, Forward);
	Vector AimPos = player->hitbox_pos(HITBOX_HEAD);
	// Get delta vector between our local eye position and passed vector
	VectorSubtract(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Math::Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	float meme = acos(DotProduct);
	return (meme * (MaxDegrees / 3.14));
}

void CRagebot::SetAngles(CUserCmd* cmd, QAngle x)
{
	QAngle temp = x;
	if (vars.rage.norecoil)
	{
		QAngle xd = local->m_aimPunchAngle();
		temp -= xd * 2;
	}
	//Math::ClampAngles(temp);
	temp.Normalize();
	temp.Clamp();
	cmd->viewangles = temp;
	if (!vars.rage.sillent)
		g_EngineClient->SetViewAngles(cmd->viewangles);
}

bool CRagebot::Hitchance()
{
	weapon_t* weapon = local->m_hActiveWeapon().Get();
	float spread = weapon->innacuracy();
	if (vars.rage.hitchance == 0 || spread == 0)
		return true;

	//weapon_info_t* weaponinf = weapon->get_weapon_data();
	if (1 / spread < vars.rage.hitchance * 1.5)
		return true;
	return false;
}


