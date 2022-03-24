#include "csgostructs.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"
#include "../options.hpp"
bool entity_t::is_player()
{
	//index: 152
	//ref: "effects/nightvision"
	//sig: 8B 92 ? ? ? ? FF D2 84 C0 0F 45 F7 85 F6
	return CallVFunction<bool(__thiscall*)(entity_t*)>(this, 152)(this);
}

bool entity_t::is_weapon()
{
	//index: 160
	//ref: "CNewParticleEffect::DrawModel"
	//sig: 8B 80 ? ? ? ? FF D0 84 C0 74 6F 8B 4D A4
	return CallVFunction<bool(__thiscall*)(entity_t*)>(this, 160)(this);
}

bool entity_t::is_bomb()
{
	return GetClientClass()->m_ClassID == ClassId_CPlantedC4;
}

bool entity_t::is_defuse()
{
	return GetClientClass()->m_ClassID == ClassId_CBaseAnimating;
}
/*
weapon_info_t* weapon_t::get_weapon_data()
{
	static auto fnGetWpnData
		= reinterpret_cast<weapon_info_t*(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8D 8B")
			);
	return fnGetWpnData(this);
}*/

weapon_info_t* weapon_t::get_weapon_data()
{
	using fn = weapon_info_t * (__thiscall*)(void*);
	return (*(fn**)this)[461](this);
}

bool weapon_t::has_bullets()
{
	return !is_reloading() && m_iClip1() > 0;
}

bool weapon_t::can_fire()
{
	if (is_reloading() || m_iClip1() <= 0)
		return false;

	if (!local)
		return false;

	float flServerTime = local->m_nTickBase() * g_GlobalVars->interval_per_tick;

	return m_flNextPrimaryAttack() <= flServerTime;
}

bool weapon_t::is_grenade()
{
	return get_weapon_data()->WeaponType == CSWeaponType::WEAPONTYPE_GRENADE_C4;
}

bool weapon_t::is_knife()
{
	return get_weapon_data()->WeaponType == CSWeaponType::WEAPONTYPE_KNIFE;
}

bool weapon_t::is_rifle()
{
	switch (get_weapon_data()->WeaponType)
	{
	case CSWeaponType::WEAPONTYPE_RIFLE:
		return true;
	default:
		return false;
	}
}

bool weapon_t::is_pistol()
{
	return get_weapon_data()->WeaponType == CSWeaponType::WEAPONTYPE_PISTOL;
}

bool weapon_t::is_awp()
{
	return false;
}

bool weapon_t::is_reloading()
{
	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

float weapon_t::innacuracy()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 483)(this);
}

float weapon_t::spread()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 453)(this);
}

void weapon_t::update_accurracy_penalty()
{
	CallVFunction<void(__thiscall*)(void*)>(this, 484)(this);
}

CUserCmd*& player_t::m_pCurrentCommand()
{
	static auto currentCommand = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "89 BE ? ? ? ? E8 ? ? ? ? 85 FF") + 2);
	return *(CUserCmd**)((uintptr_t)this + currentCommand);
}

Vector player_t::eye_pos()
{
	return m_vecOrigin() + m_vecViewOffset();
}

player_info_t player_t::player_info()
{
	player_info_t info;
	g_EngineClient->player_info(EntIndex(), &info);
	return info;
}

bool player_t::alive()
{
	//Utils::ConsolePrint((const char*)m_lifeState());
	auto hp = m_iHealth();
	return ( bool )(hp > 0);//m_lifeState() == LIFE_ALIVE;
}

bool player_t::has_bomb()
{
	static auto fnhas_bomb
		= reinterpret_cast<bool(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandleW(L"client.dll"), "56 8B F1 85 F6 74 31")
			);

	return fnhas_bomb(this);
}

animation_layer& player_t::get_anim_overlay(int Index)
{
	return (*(animation_layer**)((DWORD)this + 0x2970))[Index];
}
int player_t::get_sequence_activity(int sequence)
{
	auto hdr = g_MdlInfo->GetStudiomodel(this->GetModel());

	if (!hdr)
		return -1;

	// c_csplayer vfunc 242, follow calls to find the function.

	// (DWORD)Utils::PatternScan(client, "55 8B EC 83 7D 08 FF 56 8B F1 74 3D")
	static auto get_sequence_activity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(globs.gsa);

	return get_sequence_activity(this, hdr, sequence);
}
Vector player_t::hitbox_pos(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		auto m = GetModel();
		auto studio_model = g_MdlInfo->GetStudiomodel(m);
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
				auto
					min = Vector{},
					max = Vector{};

				Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
				Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

				return (min + max) / 2.0f;
			}
		}
	}
	return Vector{};
}

bool player_t::hitbox_pos(int hitbox, Vector &output)
{
	if (hitbox >= HITBOX_MAX)
		return false;

	const model_t *model = this->GetModel();
	if (!model)
		return false;

	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(model);
	if (!studioHdr)
		return false;

	matrix3x4_t matrix[MAXSTUDIOBONES];
	if (!this->SetupBones(matrix, MAXSTUDIOBONES, 0x100, 0))
		return false;

	mstudiobbox_t *studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
	if (!studioBox)
		return false;

	Vector min, max;

	Math::VectorTransform(studioBox->bbmin, matrix[studioBox->bone], min);
	Math::VectorTransform(studioBox->bbmax, matrix[studioBox->bone], max);

	output = (min + max) * 0.5f;

	return true;
}

Vector player_t::bone_pos(int bone)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.0f)) {
		return boneMatrix[bone].at(3);
	}
	return Vector{};
}

float_t player_t::m_flSpawnTime()
{
	return *(float_t*)((uintptr_t)this + 0xA290);
}

bool player_t::player_visible(player_t* player, int hitbox)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto endpos = player->hitbox_pos(hitbox);

	ray.Init(eye_pos(), endpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}


bool player_t::player_visible(player_t* player, const Vector& pos)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto start = eye_pos();
	auto dir = (pos - start).Normalized();

	ray.Init(start, pos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}