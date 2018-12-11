#include "config.h"
using namespace std;

#define o(name)     !name ? out << 0 << "\n" :out << name << "\n"
#define i(name)     {if(!shit) {typeid(name) == typeid(bool) ? name = bstr(nextline()) : x = y; typeid(name) == typeid(int) ? name = atoi(nextline().c_str()) : x = y;typeid(name) == typeid(float) ? name = atof(nextline().c_str()) : x = y;}}
static vector<string> coll;
static int linecount = -1;
static bool shit = false;
static std::string nextline()
{
	linecount++;
	try
	{
		string x = coll.at(linecount);
		return x;
	}// i needed to use this so i can actually catch the error
	catch (std::exception ex)
	{
		shit = true;
		return "0";
	}
}
bool bstr(std::string v)
{
	int z = atoi(v.c_str());
	return z != 0;
}

void wconfo(std::ofstream &out, int id)
{
	out << vars.legit.weapons[id].active << "\n";
	out << vars.legit.weapons[id].backtrack << "\n";
	out << vars.legit.weapons[id].fov << "\n";
	out << vars.legit.weapons[id].hitbox << "\n";
	out << vars.legit.weapons[id].hitscan << "\n";
	out << vars.legit.weapons[id].humanize << "\n";
	out << vars.legit.weapons[id].key << "\n";
	out << vars.legit.weapons[id].legitaa << "\n";
	out << vars.legit.weapons[id].legitaaresolver << "\n";
	out << vars.legit.weapons[id].rcs << "\n";
	out << vars.legit.weapons[id].silent << "\n";
	out << vars.legit.weapons[id].smooth << "\n";
}

void wconfo2(std::ofstream &out, int id)
{
	out << vars.legit.normal[id].active << "\n";
	out << vars.legit.normal[id].backtrack << "\n";
	out << vars.legit.normal[id].fov << "\n";
	out << vars.legit.normal[id].hitbox << "\n";
	out << vars.legit.normal[id].hitscan << "\n";
	out << vars.legit.normal[id].humanize << "\n";
	out << vars.legit.normal[id].key << "\n";
	out << vars.legit.normal[id].legitaa << "\n";
	out << vars.legit.normal[id].legitaaresolver << "\n";
	out << vars.legit.normal[id].rcs << "\n";
	out << vars.legit.normal[id].silent << "\n";
	out << vars.legit.normal[id].smooth << "\n";
}
void wconfi(std::ifstream &out, int id);
void wconfi2(std::ifstream &out, int id);

void basic(std::ofstream &out)
{
	out << vars.legit.conf.active << "\n";
	out << vars.legit.conf.backtrack << "\n";
	out << vars.legit.conf.fov << "\n";
	out << vars.legit.conf.hitbox << "\n";
	out << vars.legit.conf.hitscan << "\n";
	out << vars.legit.conf.humanize << "\n";
	out << vars.legit.conf.key << "\n";
	out << vars.legit.conf.legitaa << "\n";
	out << vars.legit.conf.legitaaresolver << "\n";
	out << vars.legit.conf.rcs << "\n";
	out << vars.legit.conf.silent << "\n";
	out << vars.legit.conf.smooth << "\n";
}

void basici(std::ifstream &out)
{
	vars.legit.conf.active = bstr(nextline());
	vars.legit.conf.backtrack = atoi(nextline().c_str());
	vars.legit.conf.fov = atof(nextline().c_str());
	vars.legit.conf.hitbox = atoi(nextline().c_str());
	vars.legit.conf.hitscan = bstr(nextline());
	vars.legit.conf.humanize = atof(nextline().c_str());
	vars.legit.conf.key = atoi(nextline().c_str());
	vars.legit.conf.legitaa = bstr(nextline());
	vars.legit.conf.legitaaresolver = bstr(nextline());
	vars.legit.conf.rcs = atof(nextline().c_str());
	vars.legit.conf.silent = bstr(nextline());
	vars.legit.conf.smooth = atof(nextline().c_str());
}




#define se(name)     out << name << "\n"
#define col(name)    out << name.GetRawColor() << "\n"
void viso(std::ofstream &out)
{
	se(vars.visuals.player.ammo);
	se(vars.visuals.player.armourbar);
	se(vars.visuals.player.box);
	se(vars.visuals.player.chams);
	se(vars.visuals.player.chamsflat);
	se(vars.visuals.player.chamsto);
	se(vars.visuals.player.chamsxqz);
	se(vars.visuals.player.deadonly);
	se(vars.visuals.player.espto);
	se(vars.visuals.player.glow);
	se(vars.visuals.player.glowto);
	se(vars.visuals.player.healthbar);
	se(vars.visuals.player.information);
	se(vars.visuals.player.name);
	se(vars.visuals.player.skeleton);
	se(vars.visuals.player.snaplines);
	se(vars.visuals.player.weapon);

	se(vars.visuals.world.bomb);
	se(vars.visuals.world.fov);
	se(vars.visuals.world.nightmode);
	se(vars.visuals.world.noflash);
	se(vars.visuals.world.nopostprocess);
	se(vars.visuals.world.nosmoke);
	se(vars.visuals.world.vmodelfov);
	se(vars.visuals.world.weapons);
	se(vars.visuals.world.weaponschams);
	//se(vars.visuals.world.weaponscolor);
	se(vars.visuals.world.weaponsglow);


	se(vars.rage.aa);
	se(vars.rage.active);
	se(vars.rage.autoconf);
	se(vars.rage.autoshoot);
	se(vars.rage.awall);
	se(vars.rage.fov);
	se(vars.rage.hitbox);
	se(vars.rage.hitchance);
	se(vars.rage.hitscan);
	se(vars.rage.mindmg);
	se(vars.rage.norecoil);
	se(vars.rage.sillent);

	for (int x = 0; x < color_max; x++)
		col(vars.settings.colors[x]);
}
int x = 0;
int y = 1;
#undef se
#define se(name)     {if(!shit) {typeid(name) == typeid(bool) ? name = bstr(nextline()) : x = y; typeid(name) == typeid(int) ? name = atoi(nextline().c_str()) : x = y;typeid(name) == typeid(float) ? name = atof(nextline().c_str()) : x = y;}}
#undef col
#define col(name)    {if(!shit) {name.SetRawColor(atoi(nextline().c_str()));}}
void visi(std::ifstream &out)
{
	se(vars.visuals.player.ammo);
	se(vars.visuals.player.armourbar);
	se(vars.visuals.player.box);
	se(vars.visuals.player.chams);
	se(vars.visuals.player.chamsflat);
	se(vars.visuals.player.chamsto);
	se(vars.visuals.player.chamsxqz);
	se(vars.visuals.player.deadonly);
	se(vars.visuals.player.espto);
	se(vars.visuals.player.glow);
	se(vars.visuals.player.glowto);
	se(vars.visuals.player.healthbar);
	se(vars.visuals.player.information);
	se(vars.visuals.player.name);
	se(vars.visuals.player.skeleton);
	se(vars.visuals.player.snaplines);
	se(vars.visuals.player.weapon);

	se(vars.visuals.world.bomb);
	se(vars.visuals.world.fov);
	se(vars.visuals.world.nightmode);
	se(vars.visuals.world.noflash);
	se(vars.visuals.world.nopostprocess);
	se(vars.visuals.world.nosmoke);
	se(vars.visuals.world.vmodelfov);
	se(vars.visuals.world.weapons);
	se(vars.visuals.world.weaponschams);
	//se(vars.visuals.world.weaponscolor);
	se(vars.visuals.world.weaponsglow);


	se(vars.rage.aa);
	se(vars.rage.active);
	se(vars.rage.autoconf);
	se(vars.rage.autoshoot);
	se(vars.rage.awall);
	se(vars.rage.fov);
	se(vars.rage.hitbox);
	se(vars.rage.hitchance);
	se(vars.rage.hitscan);
	se(vars.rage.mindmg);
	se(vars.rage.norecoil);
	se(vars.rage.sillent);

	for (int x = 0; x < color_max; x++)
		col(vars.settings.colors[x]);

}
void skino(std::ofstream &out, int id)
{
	o(vars.settings.skins[id].weapon_seed);
	o(vars.settings.skins[id].weapon_skin_id);
	o(vars.settings.skins[id].weapon_stat_trak);
	//o(vars.settings.skins[id].weapon_wear);
}
void skini(std::ifstream &out, int id)
{
	i(vars.settings.skins[id].weapon_seed);
	i(vars.settings.skins[id].weapon_skin_id);
	i(vars.settings.skins[id].weapon_stat_trak);
	//i(vars.settings.skins[id].weapon_wear);
}
void configs::load(std::string filename)
{
	shit = false;
	std::ifstream out;
	out.open((filename + ".meeza").c_str());
	string str;
	while (getline(out, str))
		coll.push_back(str);
	//rebuilding the vars struct

	//legit
	vars.legit.active = bstr(nextline());
	//weapon configs
	for (int x = 0; x < 65; x++)
		wconfi(out, x);
	for (int x = 0; x < 3; x++)
		wconfi2(out, x);
	basici(out);

	visi(out);

	for (int x = 0; x < 100; x++)
		globs.skins[x] = atoi(nextline().c_str());

	for (int x = 0; x < 65; x++) {
		vars.legit.weapons[x].trigger = bstr(nextline());
		vars.legit.weapons[x].triggerkey = atoi(nextline().c_str());
		vars.legit.weapons[x].trigger = atoi(nextline().c_str());
	}
	//	for (int x = 0; x < 5035 ;x++)
	//	skini(out, x);
	//if (shit == true)
		//MessageBoxA(NULL, "The config isn't loaded corectly. The version of the cheat which was used to save it may be different. Please save the config again.", "meeza", MB_ICONEXCLAMATION);
}
void configs::save(std::string filename)
{
	std::ofstream out;
	out.open((filename + ".meeza").c_str());

	//rebuilding the vars struct

	//legit
	out << vars.legit.active << "\n";
	//weapon configs
	for (int x = 0; x < 65; x++)
		wconfo(out, x);
	for (int x = 0; x < 3; x++)
		wconfo2(out, x);
	basic(out);

	viso(out);

	for (int x = 0; x < 100; x++)
		out << globs.skins[x] << "\n";

	for (int x = 0; x < 65; x++) {
		out << vars.legit.weapons[x].trigger << "\n";
		out << vars.legit.weapons[x].triggerkey << "\n";
		out << vars.legit.weapons[x].thitchance << "\n";
	}
	
	 


}

void wconfi(std::ifstream &out, int id)
{
	vars.legit.weapons[id].active = bstr(nextline());
	vars.legit.weapons[id].backtrack = atoi(nextline().c_str());
	vars.legit.weapons[id].fov = atof(nextline().c_str());
	vars.legit.weapons[id].hitbox = atoi(nextline().c_str());
	vars.legit.weapons[id].hitscan = bstr(nextline());
	vars.legit.weapons[id].humanize = atof(nextline().c_str());
	vars.legit.weapons[id].key = atoi(nextline().c_str());
	vars.legit.weapons[id].legitaa = bstr(nextline());
	vars.legit.weapons[id].legitaaresolver = bstr(nextline());
	vars.legit.weapons[id].rcs = atof(nextline().c_str());
	vars.legit.weapons[id].silent = bstr(nextline());
	vars.legit.weapons[id].smooth = atof(nextline().c_str());
}

void wconfi2(std::ifstream &out, int id)
{
	vars.legit.normal[id].active = bstr(nextline());
	vars.legit.normal[id].backtrack = atoi(nextline().c_str());
	vars.legit.normal[id].fov = atof(nextline().c_str());
	vars.legit.normal[id].hitbox = atoi(nextline().c_str());
	vars.legit.normal[id].hitscan = bstr(nextline());
	vars.legit.normal[id].humanize = atof(nextline().c_str());
	vars.legit.normal[id].key = atoi(nextline().c_str());
	vars.legit.normal[id].legitaa = bstr(nextline());
	vars.legit.normal[id].legitaaresolver = bstr(nextline());
	vars.legit.normal[id].rcs = atof(nextline().c_str());
	vars.legit.normal[id].silent = bstr(nextline());
	vars.legit.normal[id].smooth = atof(nextline().c_str());
}

#include "valve_sdk\csgostructs.hpp"

static int st_data[65];

void configs::killed()
{
	auto w = local->m_hActiveWeapon();
	if (!w)
		return;
	int wid = w->m_Item().m_iItemDefinitionIndex();
	st_data[wid] += 1;
	save_st();
}
int configs::get_st(int weapon_index)
{
	load_st();
	return st_data[weapon_index];
}
void configs::save_st()
{
	ofstream out;
	out.open("stdata.meeza_data");
	for (int x = 0; x < 65; x++)
		out << st_data[x] << "\n";
}
void configs::load_st()
{
	ifstream out;
	out.open("stdata.meeza_data");
	string str;
	vector<string> data;
	while (getline(out, str))
		data.push_back(str);
	for (int x = 0; x < 65; x++) {
		string str;
		try {
			str = data.at(x);
		}
		catch (int d) {
			str = "0";
		}
		st_data[x] = atoi(str.c_str());
	}

}