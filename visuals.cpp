#include "Visuals.h"
#include "backtrake.h"

CVisuals Visuals;
vgui::HFont font;

inline bool CanSee(player_t* target)  //fucking p120 coded function
{
	for (int x = 1; x < 20; x++) //x representing the hitbox  1 - head etc
	{
		if (local->player_visible(target, x))
		{
			return true;
		}
	}
	return false;
}


void CVisuals::Run()
{
	if (vars.visuals.player.deadonly && local->alive())
		return;
	if (vars.visuals.player.ammo || vars.visuals.player.armourbar || vars.visuals.player.box
		|| vars.visuals.player.healthbar || vars.visuals.player.information
		|| vars.visuals.player.name || vars.visuals.player.skeleton ||
		vars.visuals.player.snaplines || vars.visuals.player.weapon ||
		vars.visuals.world.weapons || vars.visuals.world.bomb){

		for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
			auto ent = entity_t::get_entity_by_id(i);
			if (!ent)
				continue;
			player_t* entity = nullptr;

			auto asd = ent->GetClientClass()->m_ClassID;

			if (asd == 40) // if entity is  a player
			{
				entity = (player_t*)ent;
				if (!entity)
					continue;

				if (entity == local)
					continue;

				auto alive = entity->alive();
				if (alive && i < 65 ) {
					Main(entity);  // i did this just to not make this function 200 lines long
				}
				continue;
			}
			else if (ent->is_bomb() && vars.visuals.world.bomb)
			{
				if (!ent)
					continue;
				bomb_t* bomb = (bomb_t*)ent;
				int w, h;
				g_VGuiSurface->GetScreenSize(w, h);
				//Utils::ConsolePrint("%f\n", bomb->);
				
				Vector pos;
				if (Math::WorldToScreen(ent->m_vecOrigin(), pos))
					Render::Text(pos.x, pos.y, true, true, font, Color(255, 255, 255), L"die Bombe");
			}
			if (strstr(ent->GetClientClass()->m_pNetworkName, "Weapon"))
			{
				weapon_t* weap = (weapon_t*)ent;
				if (vars.visuals.world.weapons)
				{
					auto hdr = g_MdlInfo->GetStudiomodel(weap->GetModel());

					if (strstr(hdr->szName, "dropped"))
					{
						Vector out;
						if (Math::WorldToScreen(weap->GetRenderOrigin(), out))
						{
							ESPBOX box = GetBox(ent);
							Render::Box(box.x, box.y, box.w, box.h,
								false, vars.settings.colors[color_weapon]);
							int ammo = weap->m_iClip1();
							std::string weapn = weap->GetClientClass()->m_pNetworkName;
							if (weapn != "CBaseWeaponWorldModel")
							{
								weapn.erase(weapn.begin(), weapn.begin() + 7);
								wchar_t buffer[50];
								swprintf(buffer, 50, L"%s [ammo: %u]",
									Utils::to_wstring(weapn.c_str()).data(), ammo);
								Render::Text(out.x, out.y, true, false, font, vars.settings.colors[color_weapon],
									buffer);
							}
						}
					}
					
					
				}
			}
		}
	}
}
void CVisuals::Main(player_t* entity) // 'main' means the player esp but naming things right is hard
{
	static ESPDATA backups[64];
	if (!entity->IsDormant()) {
		if (entity->m_iTeamNum() == local->m_iTeamNum() && vars.visuals.player.espto)
		{
			Clear();
			return;
		}
		data.vis = CanSee(entity);
		data.pl = entity;

		auto head = data.pl->hitbox_pos(HITBOX_HEAD);
		auto origin = data.pl->m_vecOrigin();


		head.z += 15;

		if (!Math::WorldToScreen(head, data.headpos) ||
			!Math::WorldToScreen(origin, data.feetpos))
		{
			Clear();
			return;
		}
		data.box = GetBox(data.pl);

		data.curtime = g_GlobalVars->curtime;

		GetColor();

		backups[data.pl->EntIndex()] = data;
	}
	else {
		data = backups[entity->EntIndex()];
		data.pl = entity;
		data.box = GetBox(data.pl);
		GetColor();
	}
	if (g_GlobalVars->curtime - data.curtime > 7)
		return;
	if (vars.visuals.player.box != 0)
		Box();
	if (vars.visuals.player.name)
		Name();
	if (vars.visuals.player.healthbar != 0)
		HealthBar();
	if (vars.visuals.player.armourbar != 0)
		ArmourBar();
	if (vars.visuals.player.skeleton)
		Skeleton();
	if (vars.visuals.player.weapon != 0)
		Weapon();
	if (vars.visuals.player.snaplines)
		Snaplines();
	if (vars.visuals.player.information != 0)
		Info();
}

void CVisuals::CreateFonts()
{
	font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(font, "Arial", 13, 700, 0, 0, FONTFLAG_DROPSHADOW);
	globs.lbyfont = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(globs.lbyfont, "Arial", 18, 700, 0, 0, FONTFLAG_DROPSHADOW);
}

void CVisuals::DestroyFonts(){}

void CVisuals::Box()
{
	Vector left1, left2, right1, right2;
	if (Math::WorldToScreen(globs.left1, left1) && Math::WorldToScreen(globs.left2, left2))
		Render::Line(left1.x, left1.y, left2.x, left2.y, Color(255, 255, 255));
	if (Math::WorldToScreen(globs.right1, right1) && Math::WorldToScreen(globs.right2, right2))
		Render::Line(right1.x, right1.y, right2.x, right2.y, Color(255, 255, 255));
	for (int x = 0; x < 13; x++)
	{
		Vector pos;
		if (Math::WorldToScreen(TimeWarp::Get().TimeWarpData[data.pl->EntIndex()][x].hitboxPos, pos))
		{
			Render::CleanBox(pos.x - 2, pos.y - 2, pos.x + 2, pos.y + 2, true, Color(0, 255, 0));
		}
	}
	switch (vars.visuals.player.box)
	{
	case 1:
		Render::Box(data.box.x, data.box.y, data.box.w, data.box.h,
			false, data.col);
		break;
	case 2:
		Render::CleanBox(data.box.x, data.box.y, data.box.w, data.box.h,
			false, data.col);
		break;
	case 3:
		const int meme = 6;
		int wd = data.box.w - data.box.x;
		int hd = data.box.h - data.box.y;
		Render::Line(data.box.x, data.box.y, data.box.x + wd / meme, data.box.y, data.col);
		Render::Line(data.box.w - wd / meme, data.box.y, data.box.w, data.box.y, data.col);
		Render::Line(data.box.w, data.box.y, data.box.w, data.box.y + hd / meme, data.col);
		Render::Line(data.box.w, data.box.h - hd / meme, data.box.w, data.box.h, data.col);

		Render::Line(data.box.x, data.box.h, data.box.x + wd / meme, data.box.h, data.col);
		Render::Line(data.box.w - wd / meme, data.box.h, data.box.w, data.box.h, data.col);

		Render::Line(data.box.x, data.box.y, data.box.x, data.box.y + hd / meme, data.col);
		Render::Line(data.box.x, data.box.h - hd / meme, data.box.x, data.box.h, data.col);
		break;

	}
	Render::CleanBox(data.box.x, data.box.y, data.box.w, data.box.h,
		false, data.col);
}

void CVisuals::Clear()
{
	data.box = ESPBOX();
	data.col = Color();
	data.feetpos = Vector();
	data.headpos = Vector();
	data.pl = nullptr;
	data.team = 0;
	data.vis = false;
}


ESPBOX CVisuals::GetBox(entity_t* entity)
{
	ESPBOX rect{};
	auto collideable = entity->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = entity->m_rgflCoordinateFrame();

	Vector points[] =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++)
	{
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++)
	{
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++)
	{
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (bottom < screen_points[i].y)
			bottom = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (top > screen_points[i].y)
			top = screen_points[i].y;
	}

	return ESPBOX{ (long)left, (long)top, (long)right, (long)bottom };
}

void CVisuals::Skeleton()
{
	studiohdr_t* pStudioModel = g_MdlInfo->GetStudiomodel(data.pl->GetModel());
	if (pStudioModel)
	{
		static matrix3x4_t pBoneToWorldOut[128];
		if (data.pl->SetupBones(pBoneToWorldOut, 128, 256, g_GlobalVars->curtime))
		{
			for (int i = 0; i < pStudioModel->numbones; i++)
			{
				mstudiobone_t* pBone = pStudioModel->pBone(i);
				if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
					continue;

				Vector vBonePos1;
				if (!Math::WorldToScreen(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
					continue;

				Vector vBonePos2;
				if (!Math::WorldToScreen(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
					continue;

				Render::Line((int)vBonePos1.x, (int)vBonePos1.y, (int)vBonePos2.x, (int)vBonePos2.y, data.col);
			}
		}
	}
}

void CVisuals::Name()
{
	Render::Text(data.box.x + ((data.box.w - data.box.x) / 2), data.box.y + 2, true, false,
		font, data.col, Utils::to_wstring(data.pl->player_info().szName).data());
}

void CVisuals::HealthBar()
{
	int health = data.pl->m_iHealth();

	int g = health * 2.55;
	int r = 230 - g;
	if (r < 0)
		r = 0;
	Color col = Color(r, g, 0);
	int cutoff = (data.box.h - data.box.y) - ((data.box.h - data.box.y) * health / 100);
	Render::CleanBox(data.box.x - 6, data.box.y, data.box.x - 1, data.box.h, true, Color(0,0,0, 150));
	Render::CleanBox(data.box.x - 5, data.box.y + cutoff + 1, data.box.x - 2, data.box.h -1, true, col);
	if (health < 100 && vars.visuals.player.healthbar == 2)
	{
		wchar_t buffer[5];
		swprintf(buffer, 5, L"%u", health);
		Render::Text(data.box.x - 3, data.box.y + cutoff + 1, true, false, font, Color::White, buffer);
	}

}

void CVisuals::ArmourBar()
{
	
	int health = data.pl->m_ArmorValue();
	int cutoff = (data.box.h - data.box.y) - ((data.box.h - data.box.y) * health / 100);
	Render::CleanBox(data.box.w + 1, data.box.y, data.box.w + 6, data.box.h, true, Color(0, 0, 0, 150));
	Render::CleanBox(data.box.w + 2, data.box.y + cutoff + 1, data.box.w + 5, data.box.h - 1, true, Color(66, 134, 244));
	if (health < 100 && vars.visuals.player.armourbar == 2)
	{
		wchar_t buffer[5];
		swprintf(buffer, 5, L"%u", health);
		Render::Text(data.box.w + 3, data.box.y + cutoff, true, false, font, Color::White, buffer);
	}
}

void CVisuals::Weapon()
{
	auto weapon = data.pl->m_hActiveWeapon();
	if (!weapon)
		return;
	std::wstring w = Utils::to_wstring(weapon->get_weapon_data()->szWeaponName);
	int ammo = data.pl->m_hActiveWeapon()->m_iClip1();
	int maxammo =  data.pl->m_hActiveWeapon()->get_weapon_data()->iMaxClip1;
	
	
	if (vars.visuals.player.ammo)
	{
		wchar_t buffer[30];
		swprintf(buffer, 30, L"%s  [%d/%d]", w.data(), ammo, maxammo);
		std::wstring meme = buffer;
		if(vars.visuals.player.weapon == 2)
		meme.erase(meme.begin(), meme.begin() + 7);
		Render::Text(data.box.x + ((data.box.w - data.box.x) / 2), data.box.h + 10, true, false,
			font, data.col, meme.data());
	}
	else
	{
		Render::Text(data.box.x + ((data.box.w - data.box.x) / 2), data.box.h + 10, true, false,
			font, data.col, w.data());
	}
	
}

void CVisuals::Snaplines()
{
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	Render::Line(w / 2, h, data.box.x + ((data.box.w - data.box.x) / 2), data.box.h,
		data.col);
}

std::wstring get_resolver(int ent) {
	int r = globs.resolver[ent];
	switch (r)
	{
	case RESOLVER_NOT_BREAKING_LBY:
		return L"not breaking lby";
	case RESOLVER_BRUTEFORCE:
		return L"bruteforcing";
	case RESOLVER_FREESTANDING:
		return L"freestanding";
	case RESOLVER_LBY_120:
		return L"breaking bly under 120";
	case RESOLVER_LBY_180:
		return L"breaking bly over 120";
	case RESOLVER_LBY_UPDATE:
		return L"lby updated";
	case RESOLVER_LBY_FLICK:
		return L"lby flicked";
	case RESOLVER_MOVING:
		return L"moving";
	case RESOLVER_MEME:
		return L"meme";
	case RESOLVER_LAST_MOVING_LBY:
		return L"last moving lby";
	}
	return L"Meme";
}
void CVisuals::Info()
{
	std::vector<std::wstring> info;

	if (data.pl->has_bomb())
		info.push_back(L"c4");
	else
		if (data.pl->m_bHasDefuser())
			info.push_back(L"defuse kit");
	if (data.pl->m_bHasHelmet())
		info.push_back(L"helmet");

	if (data.pl->m_bIsScoped())
		info.push_back(L"scoped");

	if (*(float*)((int)data.pl + 0xA2EC) > 250.0)
		info.push_back(L"flashed");

	//if (resolver.used_fake[data.pl->EntIndex()])
	//	info.push_back(L"fake");

	//if (resolver.breaking_lby[data.pl->EntIndex()])
	//	info.push_back(L"breaking lby");
	if(vars.visuals.player.information == 2)
	info.push_back(get_resolver(data.pl->EntIndex()));

	auto weap2 = data.pl->m_hActiveWeapon();
	if (!weap2)
	{
	}
	else
		if (weap2->is_reloading())
			info.push_back(L"reloading");

	int index = 0;
	int textsize;
	int r;
	g_VGuiSurface->GetTextSize(font, L"meeza", r, textsize);
	for (auto s : info)
	{
		if (vars.visuals.player.armourbar)
		{
			Render::Text(data.box.w + 7, data.box.y + (textsize * index), false, false,
				font, data.col, s.data());
		}
		else
		{
			Render::Text(data.box.w + 1, data.box.y + (textsize * index), false, false,
				font, data.col, s.data());
		}
		index++;
	}
}


void CVisuals::GetColor()
{
	if (data.pl->m_iTeamNum() != local->m_iTeamNum())
	{
		
		if (data.vis)
			data.col = vars.settings.colors[color_enemy];
		else
			data.col = vars.settings.colors[color_enemynvis];
	}
	else
	{
		if(data.vis)
		data.col = vars.settings.colors[color_ally];
		else
			data.col = vars.settings.colors[color_allynvis];
	}
	if (data.curtime != g_GlobalVars->curtime) {
		float delta = g_GlobalVars->curtime - data.curtime;
		data.col._CColor[4] = 255 - (36 * delta);
	}
}
