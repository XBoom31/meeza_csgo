#pragma once

#include <string>
#include "valve_sdk/misc/Color.hpp"
#include "valve_sdk\csgostructs.hpp"
#include "wconf.h"
#include "menu.h"
#include "skins.h" 
#define OPTION(type, var, val) type var = val

#define RESOLVER_NOT_BREAKING_LBY 0
#define RESOLVER_MOVING           1
#define RESOLVER_BRUTEFORCE       2
#define RESOLVER_LBY_UPDATE       3
#define RESOLVER_LBY_FLICK        4
#define RESOLVER_FREESTANDING     5
#define RESOLVER_LBY_120          6
#define RESOLVER_LBY_180          7
#define RESOLVER_MEME             8 
#define RESOLVER_LAST_MOVING_LBY  9
#define color_enemy     0
#define color_ally      1
#define color_enemynvis 2
#define color_allynvis  3
#define color_chamsen   4
#define color_chamsev   5
#define color_chamsan   6
#define color_chamsav   7
#define color_glowe     8
#define color_glowa     9
#define color_gloww     10
#define color_glowbomb  11
#define color_glowkit   12
#define color_weapon    13
#define color_bomb      14
#define color_kit       15
#define color_max       16

static const char* color_names[] = { "Enemy Visible", "Ally Visible", "Enemy Not Visible", "Ally Not Visible",
"Chams Enemy Not Visible", "Chams Enemy Visible", "Chams Ally Not Visible", "Chams Ally Visible",
"Glow Enemy", "Glow Ally", "Glow Weapon", "Glow Bomb", "Glow Defuse Kit", "Weapons",
"Bomb", "Kit" };

struct Globs
{
	QAngle real;
	QAngle fake;
	bool aim;
	vgui::HFont ufont;
	vgui::HFont lbyfont;
	QAngle lby;
	bool blby;
	int skins[100];
	bool killed_somebody;
	int missed_shots[64][20];
	int resolver[64];
	int brute_id;
	DWORD gsa;
	int brute_ent;
	Vector left1, left2;
	Vector right1, right2;
}; extern Globs globs;

struct Vars
{

	struct 
	{
		bool active;
		int mode;
		wconf conf;
		wconf normal[3];
		wconf weapons[65];
		
	}legit;
	struct 
	{
		bool active;
		int fov;
		bool awall;
		bool autoshoot;
		bool norecoil;
		bool sillent;
		bool autostop;
		bool aa;
		int mindmg;
		bool hitscan;
		int hitbox;
		int hitchance;
		int backupaaswitch;
		bool autorevolver;
		int backupaa;
		bool baa;
		bool baaside;
		bool autoconf;
		bool resolver;
	}rage;
	struct 
	{
		struct 
		{
			int box;
			bool name;
			int healthbar;
			int armourbar;
			bool skeleton;
			int weapon;
			bool ammo;
			bool snaplines;
			int information;
			bool deadonly;
			bool chams;
			bool chamsflat;
			bool chamsxqz;
			bool chamsto;
			bool espto;
			bool glow;
			bool glowto;

		}player;
		struct 
		{
			bool weapons;
			bool weaponsglow;
			bool weaponschams;
			bool bomb;
			bool bombglow;
			bool defuse;
			bool defuseglow;
			bool nightmode;
			bool nosmoke;
			bool noflash;
			bool update;
			bool nopostprocess;
			int fov;
			int vmodelfov;
		}world;
	}visuals;
	struct 
	{
		bool skinchanger;
		bool thirdperson;
		int thirdpersonrange;
		int thirdpersonkey;
		bool aalines;
		bool bhop;
		bool hitmarker;
		bool autostrafe;
		bool engine_pred;
		bool fakelag;
	}misc;
	struct 
	{
		std::wstring configname;
		Color colors[color_max];
		SkinInfo_t skins[5035];
	}settings;
	bool menuvisible = false;
}; extern Vars vars;
 
class Config
{
public:
    // 
    // ESP
    // 

	

    OPTION(bool, esp_enabled, false);
    OPTION(bool, esp_enemies_only, false);
    OPTION(bool, esp_player_boxes, false);
    OPTION(bool, esp_player_names, false);
    OPTION(bool, esp_player_health, false);
    OPTION(bool, esp_player_armour, false);
    OPTION(bool, esp_player_weapons, false);
    OPTION(bool, esp_player_snaplines, false);
    OPTION(bool, esp_crosshair, false);
    OPTION(bool, esp_dropped_weapons, false);
    OPTION(bool, esp_defuse_kit, false);
    OPTION(bool, esp_planted_c4, false);

    // 
    // GLOW
    // 
    OPTION(bool, glow_enabled, false);
    OPTION(bool, glow_enemies_only, false);
    OPTION(bool, glow_players, false);
    OPTION(bool, glow_chickens, false);
    OPTION(bool, glow_c4_carrier, false);
    OPTION(bool, glow_planted_c4, false);
    OPTION(bool, glow_defuse_kits, false);
    OPTION(bool, glow_weapons, false);

    //
    // CHAMS
    //
    OPTION(bool, chams_player_enabled, false);
    OPTION(bool, chams_player_enemies_only, false);
    OPTION(bool, chams_player_wireframe, false);
    OPTION(bool, chams_player_flat, false);
    OPTION(bool, chams_player_ignorez, false);
    OPTION(bool, chams_player_glass, false);
    OPTION(bool, chams_arms_enabled, false);
    OPTION(bool, chams_arms_wireframe, false);
    OPTION(bool, chams_arms_flat, false);
    OPTION(bool, chams_arms_ignorez, false);
    OPTION(bool, chams_arms_glass, false);

    //
    // MISC
    //
    OPTION(bool, misc_bhop, false);
    OPTION(bool, misc_no_hands, false);
    OPTION(int, viewmodel_fov, 68);
    OPTION(float, mat_ambient_light_r, 0.0f);
    OPTION(float, mat_ambient_light_g, 0.0f);
    OPTION(float, mat_ambient_light_b, 0.0f);

    // 
    // COLORS
    // 
    OPTION(Color, color_esp_ally_visible, Color(0, 128, 255));
    OPTION(Color, color_esp_enemy_visible, Color(255, 0, 0));
    OPTION(Color, color_esp_ally_occluded, Color(0, 128, 255));
    OPTION(Color, color_esp_enemy_occluded, Color(255, 0, 0));
    OPTION(Color, color_esp_crosshair, Color(255, 255, 255));
    OPTION(Color, color_esp_weapons, Color(128, 0, 128));
    OPTION(Color, color_esp_defuse, Color(0, 128, 255));
    OPTION(Color, color_esp_c4, Color(255, 255, 0));

    OPTION(Color, color_glow_ally, Color(0, 128, 255));
    OPTION(Color, color_glow_enemy, Color(255, 0, 0));
    OPTION(Color, color_glow_chickens, Color(0, 128, 0));
    OPTION(Color, color_glow_c4_carrier, Color(255, 255, 0));
    OPTION(Color, color_glow_planted_c4, Color(128, 0, 128));
    OPTION(Color, color_glow_defuse, Color(255, 255, 255));
    OPTION(Color, color_glow_weapons, Color(255, 128, 0));

    OPTION(Color, color_chams_player_ally_visible, Color(0, 128, 255));
    OPTION(Color, color_chams_player_ally_occluded, Color(0, 255, 128));
    OPTION(Color, color_chams_player_enemy_visible, Color(255, 0, 0));
    OPTION(Color, color_chams_player_enemy_occluded, Color(255, 128, 0));
    OPTION(Color, color_chams_arms_visible, Color(0, 128, 255));
    OPTION(Color, color_chams_arms_occluded, Color(0, 128, 255));
};

extern Config g_Options;
extern bool   g_Unload;
