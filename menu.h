#pragma once

#include <string>

#include "imgui/imgui.h"

struct IDirect3DDevice9;

class Menu
{
public:
	void Initialize();
	void Shutdown();

	void OnDeviceLost();
	void OnDeviceReset();

	void Render();

	void Show();
	void Hide();
	void Toggle();
	bool IsVisible() const { return _visible; }

private:
	void CreateStyle();

	ImGuiStyle        _style;
	bool              _visible;
}; extern Menu menu;





static const char* antireport = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

/*
#pragma region shit

std::vector<std::pair<int, const char*>> legitbot_guns =
{
	{ WEAPON_AK47,			"AK-47" },
{ WEAPON_AUG,			"AUG" },
{ WEAPON_AWP,			"AWP" },
{ WEAPON_CZ75,			"CZ75-A" },
{ WEAPON_DEAGLE,		"Desert Eagle" },
{ WEAPON_DUALS,			"Dual Barettas" },
{ WEAPON_FAMAS,			"Famas" },
{ WEAPON_FIVE7, 	"Five-Seven" },
{ WEAPON_GALIL,		"Galil-AR" },
{ WEAPON_G3SG1,			"G3SG1" },
{ WEAPON_GLOCK,			"Glock-18" },
{ WEAPON_M249,			"M249" },
{ WEAPON_M4A1S, "M4A1-S" },
{ WEAPON_M4A1,			"M4A4" },
{ WEAPON_MAC10,			"MAC-10" },
{ WEAPON_MAG7,			"Mag-7" },
{ WEAPON_MP7,			"MP7" },
{ WEAPON_MP9,			"MP9" },
{ WEAPON_NEGEV,			"Negev" },
{ WEAPON_NOVA,			"Nova" },
{ WEAPON_P2000,		"P2000" },
{ WEAPON_P250,			"P250" },
{ WEAPON_P90,			"P90" },
{ WEAPON_BIZON,			"PP-Bizon" },
{ WEAPON_REVOLVER,		"R8 Revolver" },
{ WEAPON_SAWEDOFF,		"Sawed-Off" },
{ WEAPON_SCAR20,		"SCAR-20" },
{ WEAPON_SG553,			"SG-556" },
{ WEAPON_SCOUT,			"Scout" },
{ WEAPON_TEC9,			"Tec-9" },
{ WEAPON_UMP45,			"UMP-45" },
{ WEAPON_USPS,	"USP-s" },
{ WEAPON_XM1014, "XM1014" },
};

std::vector<std::pair<int, const char*>> guns =
{
	{ WEAPON_AK47,			"AK-47" },
{ WEAPON_AUG,			"AUG" },
{ WEAPON_AWP,			"AWP" },
{ WEAPON_CZ75,			"CZ75-A" },
{ WEAPON_DEAGLE,		"Desert Eagle" },
{ WEAPON_DUALS,			"Dual Barettas" },
{ WEAPON_FAMAS,			"Famas" },
{ WEAPON_FIVE7, 	"Five-Seven" },
{ WEAPON_GALIL,		"Galil-AR" },
{ WEAPON_G3SG1,			"G3SG1" },
{ WEAPON_GLOCK,			"Glock-18" },
{ WEAPON_M249,			"M249" },
{ WEAPON_M4A1S, "M4A1-S" },
{ WEAPON_M4A1,			"M4A4" },
{ WEAPON_MAC10,			"MAC-10" },
{ WEAPON_MAG7,			"Mag-7" },
{ WEAPON_MP7,			"MP7" },
{ WEAPON_MP9,			"MP9" },
{ WEAPON_NEGEV,			"Negev" },
{ WEAPON_NOVA,			"Nova" },
{ WEAPON_P2000,		"P2000" },
{ WEAPON_P250,			"P250" },
{ WEAPON_P90,			"P90" },
{ WEAPON_BIZON,			"PP-Bizon" },
{ WEAPON_REVOLVER,		"R8 Revolver" },
{ WEAPON_SAWEDOFF,		"Sawed-Off" },
{ WEAPON_SCAR20,		"SCAR-20" },
{ WEAPON_SG553,			"SG-556" },
{ WEAPON_SCOUT,			"Scout" },
{ WEAPON_TEC9,			"Tec-9" },
{ WEAPON_UMP45,			"UMP-45" },
{ WEAPON_USPS,	"USP-s" },
{ WEAPON_XM1014, "XM1014" },
{ WEAPON_KNIFE_BAYONET,					"Bayonet Knife" },
{ WEAPON_KNIFE_FLIP,					"Flip Knife" },
{ WEAPON_KNIFE_GUT,					"Gut Knife" },
{ WEAPON_KNIFE_KARAMBIT,					"Karambit Knife" },
{ WEAPON_KNIFE_M9BAYONET,					"M9 Bayonet Knife" },
{ WEAPON_KNIFE_HUNTSMAN,					"Hunstman Knife" },
{ WEAPON_KNIFE_FALCHION,					"Falchion Knife" },
{ WEAPON_KNIFE_BOWIE,					"Bowie Knife" },
{ WEAPON_KNIFE_BUTTERFLY,					"Butterfly Knife" },
{ WEAPON_KNIFE_DAGGER, "Shadow Daggers" },
{ weapon_knife_gypsy_jackknife, "Navaja Knife" },
{ weapon_knife_stiletto, "Stiletto Knife" },
{ weapon_knife_widowmaker, "Talon Knife" },
{ weapon_knife_ursus, "Ursos Knife" },
};

std::vector<std::pair<int, const char*>> famas_skins =
{
	{ 723, "Eye of Athena" },
{ 22, "Contrast Spray" },
{ 47 , "Colony" },
{ 92 , "Cyanospatter" },
{ 429 , "Djinn" },
{ 154 , "Afterimage" },
{ 178 , "Doomkitty" },
{ 194 , "Spitfire" },
{ 244 , "Teardown" },
{ 218 , "Hexane" },
{ 260 , "Pulse" },
{ 288 , "Sergeant" },
{ 371 , "Styx" },
{ 477 ," Neural Net" }
};

std::vector<std::pair<int, const char*>> mag7_skins =
{
	{ 462 , "Counter Terrace" },
{ 34 , "Metallic DDPAT" },
{ 32 , "Silver" },
{ 100 , "Storm" },
{ 39 , "Bulldozer" },
{ 431 , "Heat" },
{ 99 ,"Sand Dune" },
{ 171 , "Irradiated Alert" },
{ 177 , "Memento" },
{ 198 , "Hazard" },
{ 291 , "Heaven Guard" },
{ 385 , "Firestarter" },
{ 473 , "Seabird" }
};

std::vector<std::pair<int, const char*>> g3sg1_skins =
{
	{ 712, "High Seas" },
{ 8 , "Desert Storm" },
{ 6 , "Arctic Camo" },
{ 27 , "Bone Mask" },
{ 46 , "Contractor" },
{ 72 , "Safari Mesh" },
{ 74 , "Polar Camo" },
{ 147 , "Jungle Dashed" },
{ 235 , "VariCamo" },
{ 170 , "Predator" },
{ 195 , "Demeter" },
{ 229 , "Azure Zebra" },
{ 294 , "Green Apple" },
{ 465 , "Orange Kimono" },
{ 464 , "Neon Kimono" },
{ 382 , "Murky" },
{ 438 , "Chronos" }
};

std::vector<std::pair<int, const char*>> m4a1_skins =
{
	{ 8 , "Desert Storm" },
{ 101 , "Tornado" },
{ 5 , "Forest DDPAT" },
{ 167 , "Radiation Hazard" },
{ 164 , "Modern Hunter" },
{ 16 , "Jungle Tiger" },
{ 17 , "Urban DDPAT" },
{ 155 , "Bullet Rain" },
{ 170 , "Predator" },
{ 176 , "Faded Zebra" },
{ 187 , "Zirka" },
{ 255 , "Asiimov" },
{ 309 , "Howl" },
{ 215 , "X - Ray" },
{ 336 , "Desert - Strike" },
{ 384 , "Griffin" },
{ 471 , "Daybreak" },
{ 449 , "Poseidon" },
{ 480 , "Evil Daimyo" }
};

std::vector<std::pair<int, const char*>> knife_skins =
{
	{ 5 , "Forest DDPAT" },
{ 418 , "Doppler" },
{ 12 , "Crimson Web" },
{ 419 , "Doppler##1" },
{ 59 , "Slaughter" },
{ 27 , "Bone Mask" },
{ 38 , "Fade" },
{ 40 , "Night" },
{ 42 , "Blue Steel" },
{ 43 , "Stained " },
{ 44 , "Case Hardened" },
{ 72 , "Safari Mesh" },
{ 77 , "Boreal Forest " },
{ 98 , "Ultraviolet " },
{ 143 , "Urban Masked" },
{ 420 ,"Doppler##2" },
{ 421 , "Doppler##3" },
{ 410 , "Damascus Steel" },
{ 175 , "Scorched" },
{ 409 , "Tiger Tooth" },
{ 414 , "Rust Coat" },
{ 415 , "Doppler##4" },
{ 416 , "Doppler##6" },
{ 417 , "Doppler##5" },
{ 413 , "Marble Fade" },
{ 323 , "Rust Coat" }
};

std::vector<std::pair<int, const char*>> deagle_skins =
{
	{ 711, "Code Red" },
{ 37 , "Blaze" },
{ 347 , "Pilot" },
{ 468 , "Midnight Storm" },
{ 469 , "Sunset Storm" },
{ 5 , "Forest DDPAT" },
{ 12 , "Crimson Web" },
{ 17 , "Urban DDPAT" },
{ 40 , "Night" },
{ 61 , "Hypnotic" },
{ 90 , "Mudder" },
{ 235 , "VariCamo" },
{ 185 , "Golden Koi" },
{ 248 , "Red Quartz" },
{ 231 , "Cobalt Disruption" },
{ 232 , "Crimson Web" },
{ 237 , "Urban Rubble" },
{ 397 , "Naga" },
{ 328 , "Hand Cannon" },
{ 273 , "Heirloom" },
{ 296 , "Meteorite" },
{ 351 , "Conspiracy" },
{ 425 , "Bronze Deco" },
{ 470 , "Sunset Storm" }
};

std::vector<std::pair<int, const char*>> awp_skins =
{
	{ 718, "Paw" },
{ 174 , "BOOM" },
{ 344 , "Dragon Lore" },
{ 5 , "Forest DDPAT" },
{ 84 , "Pink DDPAT" },
{ 30 , "Snake Camo" },
{ 51 , "Lightning Strike" },
{ 72 , "Safari Mesh" },
{ 181 , "Corticera" },
{ 259 , "Redline" },
{ 395 , "Man , o ,war" },
{ 212 , "Graphite" },
{ 227 , "Electric Hive" },
{ 251 , "Pit Viper" },
{ 279 , "Asiimov" },
{ 424 , "Worm God" },
{ 446 , "Medusa" },
{ 451 , "Sun in Leoa" },
{ 475 , "Hyper Beast" }
};

std::vector<std::pair<int, const char*>> m249_skins =
{
	{ 5 , "Forest DDPAT" },
{ 22 , "Contrast Spray" },
{ 75 , "Blizzard Marbleized" },
{ 202 , "Jungle DDPAT" },
{ 243 , "Gator Mesh" },
{ 266 , "Magma" },
{ 401 , "System Lock" },
{ 452 , "Shipping Forecast" },
{ 472 , "Impact Drill" }
};

std::vector<std::pair<int, const char*>> cz_skins =
{
	{ 709, "Eco" },
{ 435 , "Pole Position" },
{ 12 , "Crimson Web" },
{ 254 , "Nitro" },
{ 218 , "Hexane" },
{ 268 , "Tread Plate" },
{ 269 , "The Fuschia Is Now" },
{ 270 , "Victoria" },
{ 297 , "Tuxedo" },
{ 298 , "Army Sheen" },
{ 315 , "Poison Dart" },
{ 322 , "Nitro" },
{ 325 , "Chalice" },
{ 334 , "Twist" },
{ 350 , "Tigris" },
{ 366 , "Green Plaid" },
{ 453 , "Emerald" },
{ 476 , "Yellow Jacket" }
};

std::vector<std::pair<int, const char*>> aug_skins =
{
	{ 708, "Amber Slipstream" },
{ 73 , "Wings" },
{ 10 , "Copperhead" },
{ 9 , "Bengal Tiger" },
{ 28 , "Anodized Navy" },
{ 167 , "Radiation Hazard" },
{ 110 , "Condemned" },
{ 33 , "Hot Rod" },
{ 100 , "Storm" },
{ 46 , "Contractor" },
{ 47 , "Colony" },
{ 197 , "Anodized Navy" },
{ 280 , "Chameleon" },
{ 305 , "Torque" },
{ 375 , "Radiation Hazard" },
{ 442 , "Asterion" },
{ 444 , "Daedalus" },
{ 455 , "Akihabara Accept" }
};

std::vector<std::pair<int, const char*>> ump_skins =
{
	{ 37 , "Blaze" },
{ 5 , "Forest DDPAT" },
{ 15 , "Gunsmoke" },
{ 17 , "Urban DDPAT" },
{ 436 , "Grand Prix" },
{ 70 , "Carbon Fiber" },
{ 93 , "Caramel" },
{ 169 , "Fallout Warning" },
{ 175 , "Scorched" },
{ 193 , "Bone Pile" },
{ 392 , "Delusion" },
{ 281 , "Corporal" },
{ 333 , "Indigo" },
{ 362 , "Labyrinth" },
{ 488 , "Riot" }
};

std::vector<std::pair<int, const char*>> mac10_skins =
{
	{ 101 , "Tornado" },
{ 3 , "Candy Apple" },
{ 32 , "Silver" },
{ 5 , "Forest DDPAT" },
{ 17 , "Urban DDPAT" },
{ 38 , "Fade" },
{ 433 , "Neon Rider" },
{ 98 , "Ultraviolet" },
{ 157 , "Palm" },
{ 188 , "Graven" },
{ 337 , "Tatter" },
{ 246 , "Amber Fade" },
{ 284 , "Heat" },
{ 310 , "Curse" },
{ 333 , "Indigo" },
{ 343 , "Commuter" },
{ 372 , "Nuclear Garden" },
{ 402 , "Malachite" }
};

std::vector<std::pair<int, const char*>> tec9_skins =
{
	{ 722, "Snek-9" },
{ 101 , "Tornado" },
{ 2 , "Groundwater" },
{ 5 , "Forest DDPAT" },
{ 463 , "Terrace" },
{ 17 , "Urban DDPAT" },
{ 36 , "Ossified" },
{ 439 , "Hades" },
{ 159 , "Brass" },
{ 168 , "Nuclear Threat" },
{ 235 , "VariCamo" },
{ 179 , "Nuclear Threat" },
{ 248 , "Red Quartz" },
{ 206 , "Tornado" },
{ 216 , "Blue Titanium" },
{ 242 , "Army Mesh" },
{ 272 , "Titanium Bit" },
{ 289 , "Sandstorm" },
{ 303 , "Isaac" },
{ 374 , "Toxic" },
{ 459 , "Bamboo Forest" }
};

std::vector<std::pair<int, const char*>> sg556_skins =
{
	{ 101 , "Tornado" },
{ 28 , "Anodized Navy" },
{ 22 , "Contrast Spray" },
{ 27 , "Bone Mask" },
{ 39 , "Bulldozer" },
{ 98 , "Ultraviolet" },
{ 136 , "Waves Perforated" },
{ 410 , "Damascus Steel" },
{ 169 , "Fallout Warning" },
{ 186 , "Wave Spray" },
{ 243 , "Gator Mesh" },
{ 247 , "Damascus Steel" },
{ 287 , "Pulse" },
{ 298 , "Army Sheen" },
{ 363 , "Traveler" },
{ 378 , "Fallout Warning" },
{ 487 , "Cyrex" }
};

std::vector<std::pair<int, const char*>> p2000_skins =
{
	{ 104 , "Grassland Leaves" },
{ 32 , "Silver" },
{ 21 , "Granite Marbleized" },
{ 25 , "Forest Leaves" },
{ 36 , "Ossified" },
{ 485 , "Handgun" },
{ 38 , "Fade" },
{ 71 , "Scorpion" },
{ 95 , "Grassland" },
{ 184 , "Corticera" },
{ 211 , "Ocean Foam" },
{ 338 , "Pulse" },
{ 246 , "Amber Fade" },
{ 275 , "Red FragCam" },
{ 327 , "Chainmail" },
{ 346 , "Coach Class" },
{ 357 , "Ivory" },
{ 389 , "Fire Elemental" },
{ 442 , "Asterion" },
{ 443 , "Pathfinder" }
};

std::vector<std::pair<int, const char*>> sawedoff_skins =
{
	{ 345 , "First Class" },
{ 5 , "Forest DDPAT" },
{ 22 , "Contrast Spray" },
{ 30 , "Snake Camo" },
{ 83 , "Orange DDPAT" },
{ 38 , "Fade" },
{ 41 , "Copper" },
{ 434 , "Origami" },
{ 119 , "Sage Spray" },
{ 235 , "VariCamo" },
{ 171 , "Irradiated Alert" },
{ 204 , "Mosaico" },
{ 405 , "Serenity" },
{ 246 , "Amber Fade" },
{ 250 , "Full Stop" },
{ 390 , "Highwayman" },
{ 256 , "The Kraken" },
{ 323 , "Rust Coat" },
{ 458 , "Bamboo Shadow" },
{ 459 , "Bamboo Forest" }
};

std::vector<std::pair<int, const char*>> glock_skins =
{
	{ 713, "Warhawk" },
{ 2 , "Groundwater" },
{ 3 , "Candy Apple" },
{ 38 , "Fade" },
{ 40 , "Night" },
{ 48 , "Dragon Tattoo" },
{ 437 , "Twilight Galaxy" },
{ 99 , "Sand Dune" },
{ 159 , "Brass" },
{ 399 , "Catacombs" },
{ 208 , "Sand Dune" },
{ 230 , "Steel Disruption" },
{ 278 , "Blue Fissure" },
{ 293 , "Death Rattle" },
{ 353 , "Water Elemental" },
{ 367 , "Reactor" },
{ 381 , "Grinder" },
{ 479 , "Bunsen Burner" }
};

std::vector<std::pair<int, const char*>> galil_skins =
{
	{ 5 , "Forest DDPAT" },
{ 22 , "Contrast Spray" },
{ 83 , "Orange DDPAT" },
{ 428 , "Eco" },
{ 76 , "Winter Forest" },
{ 119 , "Sage Spray" },
{ 235 , "VariCamo" },
{ 398 , "Chatterbox" },
{ 192 , "Shattered" },
{ 308 , "Kami" },
{ 216 , "Blue Titanium" },
{ 237 , "Urban Rubble" },
{ 241 , "Hunting Blind" },
{ 264 , "Sandstorm" },
{ 297 , "Tuxedo" },
{ 379 , "Cerberus" },
{ 460 , "Aqua Terrace" },
{ 478 , "Rocket Pop" }
};

std::vector<std::pair<int, const char*>> mp7_skins =
{
	{ 719, "Powercore" },
{ 2 , "Groundwater" },
{ 102 , "Whiteout" },
{ 5 , "Forest DDPAT" },
{ 28 , "Anodized Navy" },
{ 11 , "Skulls" },
{ 15 , "Gunsmoke" },
{ 22 , "Contrast Spray" },
{ 27 , "Bone Mask" },
{ 36 , "Ossified" },
{ 141 , "Orange Peel" },
{ 235 , "VariCamo" },
{ 245 , "Army Recon" },
{ 209 , "Groundwater" },
{ 213 , "Ocean Foam" },
{ 250 , "Full Stop" },
{ 354 , "Urban Hazard" },
{ 365 , "Olive Plaid" },
{ 423 , "Armor Core" },
{ 442 , "Asterion" },
{ 481 , "Nemesis" }
};

std::vector<std::pair<int, const char*>> p250_skins =
{
	{ 102 , "Whiteout" },
{ 34 , "Metallic DDPAT" },
{ 162 , "Splash" },
{ 15 , "Gunsmoke" },
{ 164 , "Modern Hunter" },
{ 27 , "Bone Mask" },
{ 77 , "Boreal Forest" },
{ 99 , "Sand Dune" },
{ 168 , "Nuclear Threat" },
{ 258 , "Mehndi" },
{ 207 , "Facets" },
{ 219 , "Hive" },
{ 404 , "Muertos" },
{ 230 , "Steel Disruption" },
{ 271 , "Undertow" },
{ 295 , "Franklin" },
{ 464 , "Neon Kimono" },
{ 358 , "Supernova" },
{ 373 , "Contamination" },
{ 388 , "Cartel" },
{ 426 , "Valence" },
{ 466 , "Crimson Kimono" },
{ 467 , "Mint Kimono" }
};

std::vector<std::pair<int, const char*>> five7_skins =
{
	{ 3 , "Candy Apple" },
{ 27 , "Bone Mask" },
{ 44 , "Case Hardened" },
{ 46 , "Contractor" },
{ 78 , "Forest Night" },
{ 141 , "Orange Peel" },
{ 151 , "Jungle" },
{ 254 , "Nitro" },
{ 248 , "Red Quartz" },
{ 210 , "Anodized Gunmetal" },
{ 223 , "Nightshade" },
{ 252 , "Silver Quartz " },
{ 265 , "Kami" },
{ 274 , "Copper Galaxy" },
{ 464 , "Neon Kimono" },
{ 352 , "Fowl Play" },
{ 377 , "Hot Shot" },
{ 387 , "Urban Hazard" },
{ 427 , "Monkey Business" }
};

std::vector<std::pair<int, const char*>> nova_skins =
{
	{ 716, "Toy Soldier" },
{ 3 , "Candy Apple" },
{ 166 , "Blaze Orange" },
{ 164 , "Modern Hunter" },
{ 25 , "Forest Leaves" },
{ 62 , "Bloomstick" },
{ 99 , "Sand Dune" },
{ 107 , "Polar Mesh" },
{ 158 , "Walnut" },
{ 170 , "Predator" },
{ 191 , "Tempest" },
{ 214 , "Graphite" },
{ 225 , "Ghost Camo" },
{ 263 , "Rising Skull" },
{ 286 , "Antique" },
{ 294 , "Green Apple" },
{ 299 , "Caged Steel" },
{ 356 , "Koi" },
{ 450 , "Moon in Libra" },
{ 484 , "Ranger" }
};

std::vector<std::pair<int, const char*>> negev_skins =
{
	{ 28 , "Anodized Navy" },
{ 432 ," Man -o- war" },
{ 157 , "Palm" },
{ 235 , "VariCamo" },
{ 201 , "Palm" },
{ 240 , "CaliCamo" },
{ 285 , "Terrain" },
{ 298 , "Army Sheen" },
{ 317 , "Bratatat" },
{ 355 , "Desert - Strike" },
{ 369 , "Nuclear Waste" },
{ 483 , "Loudmouth" }
};

std::vector<std::pair<int, const char*>> p90_skins =
{
	{ 717, "Traction" },
{ 342 , "Leather" },
{ 20 , "Virus" },
{ 22 , "Contrast Spray" },
{ 100 , "Storm" },
{ 67 , "Cold Blooded" },
{ 111 , "Glacier Mesh" },
{ 124 , "Sand Spray" },
{ 156 , "Death by Kitty" },
{ 234 , "Ash Wood" },
{ 169 , "Fallout Warning" },
{ 175 , "Scorched" },
{ 182 , "Emerald Dragon" },
{ 244 , "Teardown" },
{ 228 , "Blind Spot" },
{ 283 , "Trigon" },
{ 311 , "Desert Warfare" },
{ 335 , "Module" },
{ 359 , "Asiimov" },
{ 486 , "Elite Build" }
};

std::vector<std::pair<int, const char*>> scar_skins =
{
	{ 12 , "Crimson Web" },
{ 165 , "Splash Jam" },
{ 100 , "Storm" },
{ 46 , "Contractor" },
{ 70 , "Carbon Fiber" },
{ 116 , "Sand Mesh" },
{ 157 , "Palm" },
{ 196 , "Emerald" },
{ 232 , "Crimson Web" },
{ 391 , "Cardiac" },
{ 298 , "Army Sheen" },
{ 312 , "Cyrex" },
{ 406 , "Grotto" },
{ 453 , "Emerald" }
};

std::vector<std::pair<int, const char*>> mp9_skins =
{
	{ 715, "Capillary" },
{ 482 , "Ruby Poison Dart" },
{ 27 , "Bone Mask" },
{ 33 , "Hot Rod" },
{ 100 , "Storm" },
{ 39 , "Bulldozer" },
{ 61 , "Hypnotic" },
{ 148 , "Sand Dashed" },
{ 141 , "Orange Peel" },
{ 199 , "Dry Season" },
{ 329 , "Dark Age" },
{ 262 , "Rose Iron" },
{ 366 , "Green Plaid" },
{ 368 , "Setting Sun" },
{ 386 , "Dart" },
{ 403 , "Deadly Poison" },
{ 448 , "Pandora's Box" }
};

std::vector<std::pair<int, const char*>> ak47_skins =
{
	{ 707, "Neon Rider" },
{ 341 , "First Class" },
{ 14 , "Red Laminate" },
{ 22 , "Contrast Spray" },
{ 44 , "Case Hardened" },
{ 72 , "Safari Mesh" },
{ 122 , "Jungle Spray" },
{ 170 , "Predator" },
{ 172 , "Black Laminate" },
{ 180 , "Fire Serpent" },
{ 394 , "Cartel" },
{ 300 , "Emerald Pinstripe" },
{ 226 , "Blue Laminate" },
{ 282 , "Redline" },
{ 302 , "Vulcan" },
{ 316 , "Jaguar" },
{ 340 , "Jet Set" },
{ 380 , "Wasteland Rebel" },
{ 422 , "Elite Build" },
{ 456 , "Hydroponic" },
{ 474 , "Aquamarine Revenge" }
};

std::vector<std::pair<int, const char*>> m4a1s_skins =
{
	{ 714, "Nightmare" },
{ 33 , "Hot Rod" },
{ 60 , "Dark Water" },
{ 430 , "Hyper Beast" },
{ 77 , "Boreal Forest" },
{ 235 , "VariCamo" },
{ 254 , "Nitro" },
{ 189 , "Bright Water" },
{ 301 , "Atomic Alloy" },
{ 217 , "Blood Tiger" },
{ 257 , "Guardian" },
{ 321 , "Master Piece" },
{ 326 , "Knight" },
{ 360 , "Cyrex" },
{ 383 , "Basilisk" },
{ 440 , "Icarus Fell" },
{ 445 , "Hot Rod" }
};

std::vector<std::pair<int, const char*>> elite_skins =
{
	{ 730, "Shred" },
{ 28 , "Anodized Navy" },
{ 36 , "Ossified" },
{ 43 , "Stained" },
{ 46 , "Contractor" },
{ 47 , "Colony" },
{ 153 , "Demolition" },
{ 190 , "Black Limba" },
{ 248 , "Red Quartz" },
{ 249 , "Cobalt Quartz" },
{ 220 , "Hemoglobin" },
{ 396 , "Urban Shock" },
{ 261 , "Marina" },
{ 276 , "Panther" },
{ 307 , "Retribution" },
{ 330 , "Briar" },
{ 447 , "Duelist" },
{ 450 , "Moon in Libra" }
};

std::vector<std::pair<int, const char*>> bizon_skins =
{
	{ 13 , "Blue Streak" },
{ 164 , "Modern Hunter" },
{ 25 , "Forest Leaves" },
{ 27 , "Bone Mask" },
{ 70 , "Carbon Fiber" },
{ 148 , "Sand Dashed" },
{ 149 , "Urban Dashed" },
{ 159 , "Brass" },
{ 235 , "VariCamo" },
{ 171 , "Irradiated Alert" },
{ 203 , "Rust Coat" },
{ 224 , "Water Sigil" },
{ 236 , "Night Ops" },
{ 267 , "Cobalt Halftone" },
{ 306 , "Antique" },
{ 323 , "Rust Coat" },
{ 349 , "Osiris" },
{ 376 , "Chemical Green" },
{ 457 , "Bamboo Print" },
{ 459 , "Bamboo Forest" }
};

std::vector<std::pair<int, const char*>> xm1014_skins =
{
	{ 166 , "Blaze Orange" },
{ 238 , "VariCamo Blue" },
{ 27 , "Bone Mask" },
{ 42 , "Blue Steel" },
{ 96 , "Blue Spruce" },
{ 95 , "Grassland" },
{ 135 , "Urban Perforated" },
{ 151 , "Jungle" },
{ 235 , "VariCamo" },
{ 169 , "Fallout Warning" },
{ 205 , "Jungle" },
{ 240 , "CaliCamo" },
{ 251 , "Pit Viper" },
{ 393 , "Tranquility" },
{ 320 , "Red Python" },
{ 314 , "Heaven Guard" },
{ 348 , "Red Leather" },
{ 370 , "Bone Machine" },
{ 407 , "Quicksilver" }
};

std::vector<std::pair<int, const char*>> usp_skins =
{
	{ 25 , "Forest Leaves" },
{ 60 , "Dark Water" },
{ 235 , "VariCamo" },
{ 183 , "Overgrowth" },
{ 339 , "Caiman" },
{ 217 , "Blood Tiger" },
{ 221 , "Serum" },
{ 236 , "Night Ops" },
{ 277 , "Stainless" },
{ 290 , "Guardian" },
{ 313 , "Orion" },
{ 318 , "Road Rash" },
{ 332 , "Royal Blue" },
{ 364 , "Business Class" },
{ 454 , "Para Green" },
{ 489 , "Torque" }
};

std::vector<std::pair<int, const char*>> scout_skins =
{
	{ 26 , "Lichen Dashed" },
{ 38 , "Fade" },
{ 60 , "Dark Water" },
{ 96 , "Blue Spruce" },
{ 99 , "Sand Dune" },
{ 157 , "Palm" },
{ 200 , "Mayan Dreams" },
{ 222 , "Blood in the Water" },
{ 233 , "Tropical Storm" },
{ 253 , "Acid Fade" },
{ 304 , "Slashed" },
{ 319 , "Detour" },
{ 361 , "Abyss" }
};

std::vector<std::pair<int, const char*>> null_skins =
{

};

#pragma endregion

*/