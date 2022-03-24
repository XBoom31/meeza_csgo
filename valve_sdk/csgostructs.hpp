#pragma once

#include "sdk.hpp"

#define NETVAR(type, name, table, netvar)                           \
    type& name() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
    }

#define PNETVAR(type, name, table, netvar)                           \
    type* name() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
    }

enum class CSWeaponType : int
{
	WEAPONTYPE_GRENADE_C4 = 0,
	WEAPONTYPE_KNIFE,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SMG,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_HEAVY
};


class entity_t;
class viewmodel_t;

// Created with ReClass.NET by KN4CK3R
class CHudTexture
{
public:
    char szShortName[64];    //0x0000
    char szTextureFile[64];  //0x0040
    bool bRenderUsingFont;   //0x0080
    bool bPrecached;         //0x0081
    int8_t cCharacterInFont; //0x0082
    uint8_t pad_0083[1];     //0x0083
    uint32_t hFont;          //0x0084
    int32_t iTextureId;      //0x0088
    float afTexCoords[4];    //0x008C
    uint8_t pad_009C[16];    //0x009C
};

class weapon_info_t { 
public:
	void* vtable;         
	char* szWeaponName;   
	char  pad_0x0008[0xC]; 
	int   iMaxClip1;       
	char  pad_0x0018[0xC]; 
	int   iMaxReserveAmmo; 
	char  pad_0x0028[0x58]; 
	char* szBulletType;     
	char  pad_0x0084[0x44];
	int   iWeaponType;     
	char  pad_0x00CC[32];
	bool bFullAuto;
	char urmumgay[0x3];
	int   m_iDamage;         
	float flArmorRatio;     
	char  pad_0x00F8[0x4]; 
	float flPenetration;    
	char  pad_0x0100[0x8]; 
	float flRange;            
	float flRangeModifier;  
	CSWeaponType WeaponType;
	char  pad_0x0110[0x20]; 
	float flMaxPlayerSpeed;       
	float flMaxSpeedAlt;    
	int iDamage;                      
};

enum CSGO_ACTS
{
	ACT_CSGO_DEFUSE = 958,
	ACT_CSGO_DEFUSE_WITH_KIT = 959,
	ACT_CSGO_FLASHBANG_REACTION = 960,
	ACT_CSGO_FIRE_PRIMARY = 961, // when shooting
	ACT_CSGO_FIRE_PRIMARY_OPT_1 = 962,
	ACT_CSGO_FIRE_PRIMARY_OPT_2 = 963,
	ACT_CSGO_FIRE_SECONDARY = 964, // shooting secondary
	ACT_CSGO_RELOAD = 967,
	ACT_CSGO_RELOAD_START = 968,
	ACT_CSGO_RELOAD_LOOP = 969,
	ACT_CSGO_RELOAD_END = 970,
	ACT_CSGO_PLANT_BOMB = 978,
	ACT_CSGO_IDLE_TURN_BALANCEADJUST = 979,
	ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING = 980,
};

class animation_layer
{
public:
	char  pad_0000[20];
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void *m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Size: 0x0038
typedef short short32; //cuz we're c03l

class C_EconItemView
{
public:
	NETVAR(int32_t, m_bInitialized, "DT_BaseAttributableItem", "m_bInitialized");
	NETVAR(short32, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(int32_t, m_iEntityLevel, "DT_BaseAttributableItem", "m_iEntityLevel");
	NETVAR(int32_t, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID");
	NETVAR(int32_t, m_iItemIDLow, "DT_BaseAttributableItem", "m_iItemIDLow");
	NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality");
};

class entity_t : public IClientEntity
{
public:
	static __forceinline entity_t* get_entity_by_id(int index)
	{
		return static_cast<entity_t*>(g_EntityList->GetClientEntity(index));
	}
	static __forceinline entity_t* get_entity_from_handle(CBaseHandle h)
	{
		return static_cast<entity_t*>(g_EntityList->GetClientEntityFromHandle(h));
	}

	NETVAR(int32_t, m_nModelIndex, "DT_BaseEntity", "m_nModelIndex");
	NETVAR(int32_t, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");
	NETVAR(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");
	NETVAR(bool, m_bShouldGlow, "DT_DynamicProp", "m_bShouldGlow");
	NETVAR(CHandle<player_t>, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");
	NETVAR(int32_t, m_takeDamage, "DT_BaseEntity", "m_takeDamage");
	NETVAR(float, m_simtime, "DT_BaseEntity", "m_flSimulationTime");
	const matrix3x4_t& m_rgflCoordinateFrame()
	{
		static auto _m_rgflCoordinateFrame = NetvarSys::Get().GetOffset("DT_BaseEntity", "m_CollisionGroup") - 0x30;
		return *(matrix3x4_t*)((uintptr_t)this + _m_rgflCoordinateFrame);
	}

	bool is_player();
	bool is_weapon();
	bool is_bomb();
	bool is_defuse();
};

class bomb_t
{
public:
	NETVAR(bool, m_bBombTicking, "DT_PlantedC4", "m_bBombTicking");
	NETVAR(bool, m_bBombDefused, "DT_PlantedC4", "m_bBombDefused");
	NETVAR(float, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(float, m_flTimerLength, "DT_PlantedC4", "m_flTimerLength");
	NETVAR(float, m_flDefuseLength, "DT_PlantedC4", "m_flDefuseLength");
	NETVAR(float, m_flDefuseCountDown, "DT_PlantedC4", "m_flDefuseCountDown");
	NETVAR(CHandle<player_t>, m_hBombDefuser, "DT_PlantedC4", "m_hBombDefuser");
};

class attributableitem_t : public entity_t
{
public:
	NETVAR(uint64_t, m_OriginalOwnerXuid, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidLow, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidHigh, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
	NETVAR(int32_t, m_nFallbackStatTrak, "DT_BaseAttributableItem", "m_nFallbackStatTrak");
	NETVAR(int32_t, m_nFallbackPaintKit, "DT_BaseAttributableItem", "m_nFallbackPaintKit");
	NETVAR(int32_t, m_nFallbackSeed, "DT_BaseAttributableItem", "m_nFallbackSeed");
	NETVAR(float_t, m_flFallbackWear, "DT_BaseAttributableItem", "m_flFallbackWear");

	C_EconItemView& m_Item()
	{
		// Cheating. It should be this + m_Item netvar but then the netvars inside C_EconItemView wont work properly.
		// A real fix for this requires a rewrite of the netvar manager
		return *(C_EconItemView*)this;
	}
};


class weapon_t : public attributableitem_t
{
public:
	NETVAR(float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
	NETVAR(int32_t, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
	NETVAR(int32_t, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2");
	NETVAR(int32_t, m_iViewModelIndex, "DT_BaseCombatWeapon", "m_iViewModelIndex");
	NETVAR(float_t, m_flRecoilIndex, "DT_WeaponCSBase", "m_flRecoilIndex");

	weapon_info_t* get_weapon_data();
	bool has_bullets();
	bool can_fire();
	bool is_grenade();
	bool is_knife();
	bool is_reloading();
	bool is_rifle();
	bool is_pistol();
	bool is_awp();
	float innacuracy();
	float spread();
	void update_accurracy_penalty();

};

class player_t : public entity_t
{
public:
	static __forceinline player_t* get_player_by_uid(int id)
	{
		return static_cast<player_t*>(get_entity_by_id(g_EngineClient->GetPlayerForUserID(id)));
	}
	static __forceinline player_t* get_player_by_id(int i)
	{
		return static_cast<player_t*>(get_entity_by_id(i));
	}

	NETVAR(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
	NETVAR(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
	NETVAR(bool, m_bHasHeavyArmor, "DT_CSPlayer", "m_bHasHeavyArmor");
	NETVAR(int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
	NETVAR(QAngle, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]");
	NETVAR(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
	NETVAR(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
	NETVAR(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");
	NETVAR(float, mlby, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	NETVAR(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");
	NETVAR(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");
	NETVAR(int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState");
	NETVAR(int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags");
	NETVAR(float, m_flFlashMaxAlpha, "DT_CSPlayer", "m_flFlashMaxAlpha");
	NETVAR(int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
	NETVAR(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
	NETVAR(QAngle, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
	NETVAR(QAngle, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
	NETVAR(CHandle<viewmodel_t>, m_hViewModel, "DT_BasePlayer", "m_hViewModel[0]");
	NETVAR(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
	NETVAR(float, m_flMaxspeed, "DT_BasePlayer", "m_flMaxspeed");
	NETVAR(CHandle<player_t>, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
	NETVAR(CHandle<weapon_t>, m_hActiveWeapon, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	PNETVAR(CHandle<weapon_t>, m_hMyWeapons, "DT_BaseCombatCharacter", "m_hMyWeapons");
	PNETVAR(CBaseHandle, m_hMyWearables, "DT_BaseCombatCharacter", "m_hMyWearables");
	int move_type() {
		return *reinterpret_cast<int*>((uintptr_t)this + (uintptr_t)0x258);
	}
	int* get_weapons()
	{
		return reinterpret_cast<int*>(uintptr_t(this) + 0x2DE8);
	}

	CUserCmd*& m_pCurrentCommand();

	Vector        eye_pos();
	player_info_t player_info();
	bool          alive();
	bool          has_bomb();
	animation_layer& get_anim_overlay(int Index);
	int get_sequence_activity(int sequence);
	Vector        hitbox_pos(int hitbox_id);
	bool          hitbox_pos(int hitbox, Vector &output);
	Vector        bone_pos(int bone);
	float_t       m_flSpawnTime();
	bool          player_visible(player_t* player, int hitbox);
	bool          player_visible(player_t* player, const Vector& pos);
};

class viewmodel_t : public entity_t
{
public:
	NETVAR(int32_t, m_nModelIndex, "DT_BaseViewModel", "m_nModelIndex");
	NETVAR(int32_t, m_nViewModelIndex, "DT_BaseViewModel", "m_nViewModelIndex");
	NETVAR(CHandle<weapon_t>, m_hWeapon, "DT_BaseViewModel", "m_hWeapon");
	NETVAR(CHandle<player_t>, m_hOwner, "DT_BaseViewModel", "m_hOwner");
};

