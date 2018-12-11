void CVisuals::Run()
{
	if (vars.visuals.player.deadonly && g_LocalPlayer->IsAlive())
		return;
	if (vars.visuals.player.ammo || vars.visuals.player.armourbar || vars.visuals.player.box
		|| vars.visuals.player.healthbar || vars.visuals.player.information
		|| vars.visuals.player.name || vars.visuals.player.skeleton ||
		vars.visuals.player.snaplines || vars.visuals.player.weapon ||
		vars.visuals.world.weapons || vars.visuals.world.bomb){

		for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
			auto ent = C_BaseEntity::GetEntityByIndex(i);
			if (!ent)
				continue;
			C_BasePlayer* entity = nullptr;
			if (ent->GetClientClass()->m_ClassID == ClassId_CCSPlayer)
			{
				entity = (C_BasePlayer*)ent;
				if (!entity)
					continue;

				if (entity == g_LocalPlayer)
					continue;

				if (i < 65 && !entity->IsDormant() && entity->IsAlive()) {
					Main(entity);
				}
				continue;
			}
			if (strstr(ent->GetClientClass()->m_pNetworkName, "Weapon"))
			{
				C_BaseCombatWeapon* weap = (C_BaseCombatWeapon*)ent;
				if (vars.visuals.world.weapons)
				{
					auto hdr = g_MdlInfo->GetStudioModel(weap->GetModel());

					if (strstr(hdr->szName, "dropped"))
					{
						Vector out;
						if (Math::WorldToScreen(weap->GetRenderOrigin(), out))
						{
							ESPBOX box = GetBox(ent);
							Render::Box(box.x, box.y, box.w, box.h,
								false, Color(255,255,255));
							int ammo = weap->m_iClip1();
							std::string weapn = weap->GetClientClass()->m_pNetworkName;
							if (weapn == "CBaseWeaponWorldModel")
								continue;
							weapn.erase(weapn.begin(), weapn.begin() + 7);
							wchar_t buffer[50];
							swprintf(buffer, 50, L"%s [ammo: %u]", 
								Utils::to_wstring(weapn.c_str()).data(),ammo);
							Render::Text(out.x, out.y, true, false, font, Color::White,
								buffer);
						}
					}
					
					
				}
			}
		}
	}
}
void CVisuals::CreateFonts()
{
	font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(font, "Arial", 13, 700, 0, 0, FONTFLAG_DROPSHADOW);
}
void CVisuals::DestroyFonts(){}
void CVisuals::Box()
{
	Render::Box(data.box.x, data.box.y, data.box.w, data.box.h,
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
void CVisuals::Skeleton()
{
	studiohdr_t* pStudioModel = g_MdlInfo->GetStudioModel(data.pl->GetModel());
	if (pStudioModel)
	{
		static matrix3x4_t pBoneToWorldOut[128];
		if (data.pl->SetupBones(pBoneToWorldOut, 128, 256, g_GlobalVars->curtime))
		{
			for (int i = 0; i < pStudioModel->numbones; i++)
			{
				mstudiobone_t* pBone = pStudioModel->GetBone(i);
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
		font, data.col, Utils::to_wstring(data.pl->GetPlayerInfo().szName).data());
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
	if (health < 100)
	{
		wchar_t buffer[5];
		swprintf(buffer, 5, L"%u", health);
		Render::Text(data.box.x - 3, data.box.y + cutoff, true, false, font, Color::White, buffer);
	}

}
void CVisuals::ArmourBar()
{
	
	int health = data.pl->m_ArmorValue();
	int cutoff = (data.box.h - data.box.y) - ((data.box.h - data.box.y) * health / 100);
	Render::CleanBox(data.box.w + 1, data.box.y, data.box.w + 6, data.box.h, true, Color(0, 0, 0, 150));
	Render::CleanBox(data.box.w + 2, data.box.y + cutoff + 1, data.box.w + 5, data.box.h - 1, true, Color(66, 134, 244));
	if (health < 100)
	{
		wchar_t buffer[5];
		swprintf(buffer, 5, L"%u", health);
		Render::Text(data.box.w + 3, data.box.y + cutoff, true, false, font, Color::White, buffer);
	}
}
void CVisuals::Weapon()
{
	std::wstring w = Utils::to_wstring(data.pl->m_hActiveWeapon()->GetCSWeaponData()->szWeaponName);
	int ammo = data.pl->m_hActiveWeapon()->m_iClip1();
	int maxammo =  data.pl->m_hActiveWeapon()->GetCSWeaponData()->iMaxClip1;
	
	
	if (vars.visuals.player.ammo)
	{
		wchar_t buffer[30];
		swprintf(buffer, 30, L"%s  [%d/%d]", w.data(), ammo, maxammo);
		Render::Text(data.box.x + ((data.box.w - data.box.x) / 2), data.box.h + 10, true, false,
			font, data.col, buffer);
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
void CVisuals::Info()
{
	std::vector<std::wstring> info;

	if (data.pl->HasC4())
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

	if (data.pl->m_hActiveWeapon().Get()->IsReloading())
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
void CLegitBot::move(CUserCmd* cmd2)
{
	cmd = cmd2;
	if (!InputSys::Get().IsKeyDown(VK_LBUTTON))
	{
		locked = false;
		return;
	}
	if (!g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
	{
		locked = false;
		return;
	}
	getconf();
	auto weap = g_LocalPlayer->m_hActiveWeapon();
	if (!config.active || weap->IsKnife() || !weap->CanFire())
	{
		locked = false;
		return;
	}
	viewoffset = g_LocalPlayer->GetRenderOrigin() + g_LocalPlayer->m_vecViewOffset();
	if (locked)
	{
		auto fov = fovtoplayer(viewoffset, target, HITBOX_CHEST, cmd);
		if (config.fov >= fov && target->IsAlive()
			&& !target->IsDormant() && CanSee(target))
		{ //still locked
			Utils::ConsolePrint("hello\n");
			if (!locked)
				hitbox = gethitbox();
			auto hvec = target->GetHitboxPos(hitbox);
			QAngle avec;  // where the enemy is 
			Vector Delta(0, 0, 0);
			VectorSubtract(hvec, viewoffset, Delta);
			Math::VectorAngles(Delta, avec);
			avec.Normalize();
			avec.Clamp();
			double smoothness = (double)(1.05f - (double)(config.smooth + noise()) / 100);
			if (config.smooth == 0 || config.silent)
			{
				QAngle rcs = g_LocalPlayer->m_aimPunchAngle();
				QAngle final = avec - rcs * config.rcs / 50;
				final.Normalize();
				cmd2->viewangles = final;
				if (!config.silent)
					g_EngineClient->SetViewAngles(cmd2->viewangles);
			}
			else
			{
				QAngle rcs = g_LocalPlayer->m_aimPunchAngle();
				QAngle final = avec - rcs * config.rcs / 50;
				QAngle delta = final - cmd2->viewangles;
				if (delta.Length() > smoothness)
				{
					delta *= smoothness;
				}
				QAngle out = cmd2->viewangles  + delta;
				out.Normalize();
				cmd2->viewangles = out;
				if (!config.silent)
					g_EngineClient->SetViewAngles(cmd2->viewangles);
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
inline void CLegitBot::getconf()
{
	switch (vars.legit.mode)
	{
	case 0:
		config = vars.legit.conf;
	case 2:
		config = vars.legit.weapons[g_LocalPlayer->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex()];
	}
	
}
void Autowall::TraceLine(Vector& absStart, Vector& absEnd, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr)
{
	Ray_t ray;
	ray.Init(absStart, absEnd);
	CTraceFilter filter;
	filter.pSkip = ignore;

	g_EngineTrace->TraceRay(ray, mask, &filter, ptr);
}
void Autowall::ClipTraceToPlayers(Vector& absStart, Vector absEnd, unsigned int mask, ITraceFilter* filter, CGameTrace* tr)
{
	trace_t playerTrace;
	Ray_t ray;
	float smallestFraction = tr->fraction;
	const float maxRange = 60.0f;

	ray.Init(absStart, absEnd);

	for (int k = 1; k <= g_GlobalVars->maxClients; ++k)
	{
		C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(k);

		if (!player || !player->IsAlive())
			continue;

#ifdef CLIENT_DLL
		if (player->IsDormant())
			continue;
#endif // CLIENT_DLL

		if (filter && filter->ShouldHitEntity(player, mask) == false)
			continue;

		float range = (absEnd - absStart).Length();
		if (range < 0.0f || range > maxRange)
			continue;

		g_EngineTrace->ClipRayToEntity(ray, mask | CONTENTS_HITBOX, player, &playerTrace);
		if (playerTrace.fraction < smallestFraction)
		{
			// we shortened the ray - save off the trace
			*tr = playerTrace;
			smallestFraction = playerTrace.fraction;
		}
	}
}
void Autowall::GetBulletTypeParameters(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type)
{
	if (sv_penetration_type)
	{
		maxRange = 35.0;
		maxDistance = 3000.0;
	}
	else
	{
		//Play tribune to framerate. Thanks, stringcompare
		//Regardless I doubt anyone will use the old penetration system anyway; so it won't matter much.
		if (!strcmp(bulletType, "BULLET_PLAYER_338MAG"))
		{
			maxRange = 45.0;
			maxDistance = 8000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_762MM"))
		{
			maxRange = 39.0;
			maxDistance = 5000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_556MM") || !strcmp(bulletType, "BULLET_PLAYER_556MM_SMALL") || !strcmp(bulletType, "BULLET_PLAYER_556MM_BOX"))
		{
			maxRange = 35.0;
			maxDistance = 4000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_57MM"))
		{
			maxRange = 30.0;
			maxDistance = 2000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_50AE"))
		{
			maxRange = 30.0;
			maxDistance = 1000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_357SIG") || !strcmp(bulletType, "BULLET_PLAYER_357SIG_SMALL") || !strcmp(bulletType, "BULLET_PLAYER_357SIG_P250") || !strcmp(bulletType, "BULLET_PLAYER_357SIG_MIN"))
		{
			maxRange = 25.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_9MM"))
		{
			maxRange = 21.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_45ACP"))
		{
			maxRange = 15.0;
			maxDistance = 500.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_BUCKSHOT"))
		{
			maxRange = 0.0;
			maxDistance = 0.0;
		}
	}
}
void Autowall::ScaleDamage(CGameTrace &enterTrace, CCSWeaponInfo *weaponData, float& currentDamage)
{
	//Cred. to N0xius for reversing this.
	//TODO: _xAE^; look into reversing this yourself sometime

	bool hasHeavyArmor = ((C_BasePlayer*)enterTrace.hit_entity)->m_bHasHeavyArmor();
	int armorValue = ((C_BasePlayer*)enterTrace.hit_entity)->m_ArmorValue();
	int hitGroup = enterTrace.hitgroup;

	//Fuck making a new function, lambda beste. ~ Does the person have armor on for the hitbox checked?
	auto IsArmored = [&enterTrace]()->bool
	{
		C_BasePlayer* targetEntity = (C_BasePlayer*)enterTrace.hit_entity;
		switch (enterTrace.hitgroup)
		{
		case HITGROUP_HEAD:
			return (C_BasePlayer*)targetEntity->m_bHasHelmet();
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		default:
			return false;
		}
	};

	switch (hitGroup)
	{
	case HITGROUP_HEAD:
		currentDamage *= hasHeavyArmor ? 2.f : 4.f; //Heavy Armor does 1/2 damage
		break;
	case HITGROUP_STOMACH:
		currentDamage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		currentDamage *= 0.75f;
		break;
	default:
		break;
	}

	if (armorValue > 0 && IsArmored())
	{
		float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weaponData->flArmorRatio / 2.f;

		//Damage gets modified for heavy armor users
		if (hasHeavyArmor)
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}

		auto NewDamage = currentDamage * armorRatio;

		if (hasHeavyArmor)
			NewDamage *= 0.85f;

		if (((currentDamage - (currentDamage * armorRatio)) * (bonusValue * armorBonusRatio)) > armorValue)
			NewDamage = currentDamage - (armorValue / armorBonusRatio);

		currentDamage = NewDamage;
	}
}
void Chams::OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, const Color& rgba)
{
    IMaterial* material = nullptr;

    if(flat) {
        if(ignoreZ)
            material = materialFlatIgnoreZ;
        else
            material = materialFlat;
    } else {
        if(ignoreZ)
            material = materialRegularIgnoreZ;
        else
            material = materialRegular;
    }


    if(glass) {
        material = materialFlat;
        material->AlphaModulate(0.45f);
    } else {
        material->AlphaModulate(
            rgba.a() / 255.0f);
    }

    material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
    material->ColorModulate(
        rgba.r() / 255.0f,
        rgba.g() / 255.0f,
        rgba.b() / 255.0f);

    g_MdlRender->ForcedMaterialOverride(material);
}
void Glow::Shutdown()
{
    // Remove glow from all entities
    for(auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++) {
        auto& glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
        auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

        if(glowObject.IsUnused())
            continue;

        if(!entity || entity->IsDormant())
            continue;

        glowObject.m_flAlpha = 0.0f;
    }
}
void InputSys::Initialize()
{
	D3DDEVICE_CREATION_PARAMETERS params;

	if (FAILED(g_D3DDevice9->GetCreationParameters(&params)))
		throw std::runtime_error("[InputSys] GetCreationParameters failed.");

	m_hTargetWindow = params.hFocusWindow;
	m_ulOldWndProc = SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

	if (!m_ulOldWndProc)
		throw std::runtime_error("[InputSys] SetWindowLongPtr failed.");
}
void InputSys::RegisterHotkey(std::uint32_t vk, std::function<void(void)> f)
{
	m_Hotkeys[vk] = f;
}
    void NormalizeAngles(QAngle& angles)
    {
        for(auto i = 0; i < 3; i++) {
            while(angles[i] < -180.0f) angles[i] += 360.0f;
            while(angles[i] >  180.0f) angles[i] -= 360.0f;
        }
    }
    void ClampAngles(QAngle& angles)
    {
        if(angles.pitch > 89.0f) angles.pitch = 89.0f;
        else if(angles.pitch < -89.0f) angles.pitch = -89.0f;

        if(angles.yaw > 180.0f) angles.yaw = 180.0f;
        else if(angles.yaw < -180.0f) angles.yaw = -180.0f;

        angles.roll = 0;
    }
    void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
    {
        out[0] = in1.Dot(in2[0]) + in2[0][3];
        out[1] = in1.Dot(in2[1]) + in2[1][3];
        out[2] = in1.Dot(in2[2]) + in2[2][3];
    }
    void AngleVectors(const QAngle &angles, Vector& forward)
    {
        float	sp, sy, cp, cy;

        DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
        DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

        forward.x = cp*cy;
        forward.y = cp*sy;
        forward.z = -sp;
    }
    void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up)
    {
        float sr, sp, sy, cr, cp, cy;

        DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
        DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
        DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles[2]));

        forward.x = (cp * cy);
        forward.y = (cp * sy);
        forward.z = (-sp);
        right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
        right.y = (-1 * sr * sp * sy + -1 * cr *  cy);
        right.z = (-1 * sr * cp);
        up.x = (cr * sp * cy + -sr*-sy);
        up.y = (cr * sp * sy + -sr*cy);
        up.z = (cr * cp);
    }
    void VectorAngles(const Vector& forward, QAngle& angles)
    {
        float	tmp, yaw, pitch;

        if(forward[1] == 0 && forward[0] == 0) {
            yaw = 0;
            if(forward[2] > 0)
                pitch = 270;
            else
                pitch = 90;
        } else {
            yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
            if(yaw < 0)
                yaw += 360;

            tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
            pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);
            if(pitch < 0)
                pitch += 360;
        }

        angles[0] = pitch;
        angles[1] = yaw;
        angles[2] = 0;
    }
    void AttachConsole()
    {
        _old_out = GetStdHandle(STD_OUTPUT_HANDLE);
        _old_err = GetStdHandle(STD_ERROR_HANDLE);
        _old_in  = GetStdHandle(STD_INPUT_HANDLE);

        ::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

        _out     = GetStdHandle(STD_OUTPUT_HANDLE);
        _err     = GetStdHandle(STD_ERROR_HANDLE);
        _in      = GetStdHandle(STD_INPUT_HANDLE);

        SetConsoleMode(_out,
            ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

        SetConsoleMode(_in,
            ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
            ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
    }
    void DetachConsole()
    {
        if(_out && _err && _in) {
            FreeConsole();

            if(_old_out)
                SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
            if(_old_err)
                SetStdHandle(STD_ERROR_HANDLE, _old_err);
            if(_old_in)
                SetStdHandle(STD_INPUT_HANDLE, _old_in);
        }
    }
    std::uint8_t* PatternScan(void* module, const char* signature)
    {
        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto start = const_cast<char*>(pattern);
            auto end = const_cast<char*>(pattern) + strlen(pattern);

            for(auto current = start; current < end; ++current) {
                if(*current == '?') {
                    ++current;
                    if(*current == '?')
                        ++current;
                    bytes.push_back(-1);
                } else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        auto s = patternBytes.size();
        auto d = patternBytes.data();

        for(auto i = 0ul; i < sizeOfImage - s; ++i) {
            bool found = true;
            for(auto j = 0ul; j < s; ++j) {
                if(scanBytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }
            }
            if(found) {
                return &scanBytes[i];
            }
        }
        return nullptr;
    }
    void SetClantag(const char* tag)
    {
        static auto fnClantagChanged = (int(__fastcall*)(const char*, const char*))PatternScan(GetModuleHandleW(L"engine.dll"), "53 56 57 8B DA 8B F9 FF 15");

        fnClantagChanged(tag, tag);
    }
    void SetName(const char* name)
    {
        static auto nameConvar = g_CVar->FindVar("name");
        nameConvar->m_fnChangeCallbacks.m_Size = 0;

        // Fix so we can change names how many times we want
        // This code will only run once because of `static`
        static auto do_once = (nameConvar->SetValue("\nญญญ"), true);

        nameConvar->SetValue(name);
    }
    void RankRevealAll()
    {
        using ServerRankRevealAll = char(__cdecl*)(int*);

        static auto fnServerRankRevealAll = PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 8B 0D ? ? ? ? 68");

        int v[3] = { 0,0,0 };

        reinterpret_cast<ServerRankRevealAll>(fnServerRankRevealAll)(v);
    }
vfunc_hook::vfunc_hook(void* base)
    : class_base(base), vftbl_len(0), new_vftbl(nullptr), old_vftbl(nullptr)
{
}
bool vfunc_hook::setup(void* base /*= nullptr*/)
{
    if(base != nullptr)
        class_base = base;

    if(class_base == nullptr)
        return false;

    old_vftbl = *(std::uintptr_t**)class_base;
    vftbl_len = estimate_vftbl_length(old_vftbl);

    if(vftbl_len == 0)
        return false;

    new_vftbl = new std::uintptr_t[vftbl_len]();

    std::memcpy(new_vftbl, old_vftbl, vftbl_len * sizeof(std::uintptr_t));

    try {
        auto guard = detail::protect_guard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
        *(std::uintptr_t**)class_base = new_vftbl;
    } catch(...) {
        delete[] new_vftbl;
        return false;
    }

    return true;
}
    void Initialize()
    {
        hlclient_hook .setup(g_CHLClient);
        direct3d_hook .setup(g_D3DDevice9);
        vguipanel_hook.setup(g_VGuiPanel);
        vguisurf_hook .setup(g_VGuiSurface);
        mdlrender_hook.setup(g_MdlRender);
        clientmode_hook.setup(g_ClientMode);

        direct3d_hook.hook_index(index::EndScene, hkEndScene);
        direct3d_hook.hook_index(index::Reset, hkReset);

        hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
        hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);

        vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);

        vguisurf_hook.hook_index(index::PlaySound, hkPlaySound);

        mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);

        clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);

        Visuals.CreateFonts();
    }
	void FixMovement(CUserCmd *usercmd, QAngle &wish_angle)
	{
		Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
		auto viewangles = usercmd->viewangles;
		viewangles.Normalize();

		Math::AngleVectors(wish_angle, view_fwd, view_right, view_up);
		Math::AngleVectors(viewangles, cmd_fwd, cmd_right, cmd_up);

		const float v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
		const float v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
		const float v12 = sqrtf(view_up.z * view_up.z);

		const Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
		const Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
		const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

		const float v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
		const float v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
		const float v18 = sqrtf(cmd_up.z * cmd_up.z);

		const Vector norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
		const Vector norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
		const Vector norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

		const float v22 = norm_view_fwd.x * usercmd->forwardmove;
		const float v26 = norm_view_fwd.y * usercmd->forwardmove;
		const float v28 = norm_view_fwd.z * usercmd->forwardmove;
		const float v24 = norm_view_right.x * usercmd->sidemove;
		const float v23 = norm_view_right.y * usercmd->sidemove;
		const float v25 = norm_view_right.z * usercmd->sidemove;
		const float v30 = norm_view_up.x * usercmd->upmove;
		const float v27 = norm_view_up.z * usercmd->upmove;
		const float v29 = norm_view_up.y * usercmd->upmove;

		usercmd->forwardmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
			+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
			+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
		usercmd->sidemove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
			+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
			+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
		usercmd->upmove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
			+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
			+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

		usercmd->forwardmove = clamp(usercmd->forwardmove, -450.f, 450.f);
		usercmd->sidemove = clamp(usercmd->sidemove, -450.f, 450.f);
		usercmd->upmove = clamp(usercmd->upmove, -320.f, 320.f);
	}
    void Shutdown()
    {
        hlclient_hook.unhook_all();
        direct3d_hook.unhook_all();
        vguipanel_hook.unhook_all();
        vguisurf_hook.unhook_all();
        mdlrender_hook.unhook_all();
        clientmode_hook.unhook_all();

        Glow::Get().Shutdown();

        Visuals.DestroyFonts();
    }
    void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
    {
        auto oCreateMove = hlclient_hook.get_original<CreateMove>(index::CreateMove);

        oCreateMove(g_CHLClient, sequence_number, input_sample_frametime, active);

		try
		{

			auto cmd = g_Input->GetUserCmd(sequence_number);
			auto verified = g_Input->GetVerifiedCmd(sequence_number);

			if (!cmd || !cmd->command_number || !verified)
				return;
			QAngle wish_angle = cmd->viewangles;
			if (g_Options.misc_bhop) {
				BunnyHop::OnCreateMove(cmd);
			}
			if (vars.rage.active)
			{
				rage.Run(cmd, bSendPacket);
			}
			else if (vars.legit.active)
			{
				LegitBot.move(cmd);
			}
			FixMovement(cmd, wish_angle);
			verified->m_cmd = *cmd;
			verified->m_crc = cmd->GetChecksum();
		}
		catch (std::exception ex)
		{
			Utils::ConsolePrint("hkCreateMove error. Please send this code to the developer: %s", ex.what());
		}
    }
    __declspec(naked) void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active)
    {
        __asm
        {
            push ebp
            mov  ebp, esp
            push ebx
            lea  ecx, [esp]
            push ecx
            push dword ptr[active]                
            push dword ptr[input_sample_frametime]
            push dword ptr[sequence_number]       
            call Hooks::hkCreateMove
            pop  ebx
            pop  ebp
            retn 0Ch
        }
    }
    void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
    {
        static auto panelId = vgui::VPANEL{ 0 };
        static auto oPaintTraverse = vguipanel_hook.get_original<PaintTraverse>(index::PaintTraverse);

        oPaintTraverse(g_VGuiPanel, panel, forceRepaint, allowForce);

		try
		{
			if (!panelId) {
				const auto panelName = g_VGuiPanel->GetName(panel);
				if (!strcmp(panelName, "FocusOverlayPanel")) {
					panelId = panel;
				}
			}
			else if (panelId == panel) {
				
				if (g_EngineClient->IsInGame()) {

					if (!g_LocalPlayer)
						return;
					Visuals.Run();

				}
				menu.PaintTraverse();
			}
		}
		catch (std::exception ex)
		{
			Utils::ConsolePrint("hkPaintTraverse error. Please send this code to the developer: %s", ex.what());
		}
    }
    void __stdcall hkPlaySound(const char* name)
    {
        static auto oPlaySound = vguisurf_hook.get_original<PlaySound>(index::PlaySound);

        oPlaySound(g_VGuiSurface, name);

        // Auto Accept
        if(strstr(name, "UI/competitive_accept_beep.wav")) {
            static auto fnAccept =
                (void(*)())Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 83 BE");
        
            fnAccept();

            //This will flash the CSGO window on the taskbar
            //so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
            FLASHWINFO fi;
            fi.cbSize    = sizeof(FLASHWINFO);
            fi.hwnd      = InputSys::Get().GetMainWindow();
            fi.dwFlags   = FLASHW_ALL | FLASHW_TIMERNOFG;
            fi.uCount    = 0;
            fi.dwTimeout = 0;
            FlashWindowEx(&fi);
        }
    }
    void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
    {
        static auto ofunc = hlclient_hook.get_original<FrameStageNotify>(index::FrameStageNotify);
		if (stage == FRAME_RENDER_START)
		{
			if (g_EngineClient->IsInGame())
			{
				if (g_LocalPlayer->IsAlive())
				    if (*(bool*)((DWORD)g_Input + 0xA5)) //Check for thirdperson
					    *(QAngle*)((DWORD)g_LocalPlayer.operator->() + 0x31C8) = globs.real;
			}
		}
        ofunc(g_CHLClient, stage);
    }
    void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
    {
        static auto ofunc = mdlrender_hook.get_original<DrawModelExecute>(index::DrawModelExecute);

        Chams::Get().OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

        ofunc(g_MdlRender, ctx, state, pInfo, pCustomBoneToWorld);

        g_MdlRender->ForcedMaterialOverride(nullptr);
    }
void ImGui_ImplDX9_RenderDrawLists(ImDrawData* draw_data)
{
    // Avoid rendering when minimized
    ImGuiIO& io = ImGui::GetIO();
    if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f)
        return;

    // Create and grow buffers if needed
    if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
        g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
        if (g_pd3dDevice->CreateVertexBuffer(g_VertexBufferSize * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL) < 0)
            return;
    }
    if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
        g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
        if (g_pd3dDevice->CreateIndexBuffer(g_IndexBufferSize * sizeof(ImDrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_pIB, NULL) < 0)
            return;
    }

    // Backup the DX9 state
    IDirect3DStateBlock9* d3d9_state_block = NULL;
    if (g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block) < 0)
        return;

    // Copy and convert all vertices into a single contiguous buffer
    CUSTOMVERTEX* vtx_dst;
    ImDrawIdx* idx_dst;
    if (g_pVB->Lock(0, (UINT)(draw_data->TotalVtxCount * sizeof(CUSTOMVERTEX)), (void**)&vtx_dst, D3DLOCK_DISCARD) < 0)
        return;
    if (g_pIB->Lock(0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (void**)&idx_dst, D3DLOCK_DISCARD) < 0)
        return;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
        for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
        {
            vtx_dst->pos[0] = vtx_src->pos.x;
            vtx_dst->pos[1] = vtx_src->pos.y;
            vtx_dst->pos[2] = 0.0f;
            vtx_dst->col = (vtx_src->col & 0xFF00FF00) | ((vtx_src->col & 0xFF0000)>>16) | ((vtx_src->col & 0xFF) << 16);     // RGBA --> ARGB for DirectX9
            vtx_dst->uv[0] = vtx_src->uv.x;
            vtx_dst->uv[1] = vtx_src->uv.y;
            vtx_dst++;
            vtx_src++;
        }
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        idx_dst += cmd_list->IdxBuffer.Size;
    }
    g_pVB->Unlock();
    g_pIB->Unlock();
    g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
    g_pd3dDevice->SetIndices(g_pIB);
    g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

    // Setup viewport
    D3DVIEWPORT9 vp;
    vp.X = vp.Y = 0;
    vp.Width = (DWORD)io.DisplaySize.x;
    vp.Height = (DWORD)io.DisplaySize.y;
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    g_pd3dDevice->SetViewport(&vp);

    // Setup Render state: fixed-pipeline, alpha-blending, no face culling, no depth testing
    g_pd3dDevice->SetPixelShader(NULL);
    g_pd3dDevice->SetVertexShader(NULL);
    g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF); // without this Menu wasnt displayed while ingame without netgraph/cl_showfps (atleast for me xd)
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    // Setup orthographic projection matrix
    // Being agnostic of whether <d3dx9.h> or <DirectXMath.h> can be used, we aren't relying on D3DXMatrixIdentity()/D3DXMatrixOrthoOffCenterLH() or DirectX::XMMatrixIdentity()/DirectX::XMMatrixOrthographicOffCenterLH()
    {
        const float L = 0.5f, R = io.DisplaySize.x+0.5f, T = 0.5f, B = io.DisplaySize.y+0.5f;
        D3DMATRIX mat_identity = { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } };
        D3DMATRIX mat_projection =
        {
            2.0f/(R-L),   0.0f,         0.0f,  0.0f,
            0.0f,         2.0f/(T-B),   0.0f,  0.0f,
            0.0f,         0.0f,         0.5f,  0.0f,
            (L+R)/(L-R),  (T+B)/(B-T),  0.5f,  1.0f,
        };
        g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat_identity);
        g_pd3dDevice->SetTransform(D3DTS_VIEW, &mat_identity);
        g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);
    }

    // Render command lists
    int vtx_offset = 0;
    int idx_offset = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                const RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
                g_pd3dDevice->SetTexture(0, (LPDIRECT3DTEXTURE9)pcmd->TextureId);
                g_pd3dDevice->SetScissorRect(&r);
                g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vtx_offset, 0, (UINT)cmd_list->VtxBuffer.Size, idx_offset, pcmd->ElemCount/3);
            }
            idx_offset += pcmd->ElemCount;
        }
        vtx_offset += cmd_list->VtxBuffer.Size;
    }

    // Restore the DX9 state
    d3d9_state_block->Apply();
    d3d9_state_block->Release();
}
bool    ImGui_ImplDX9_Init(void* hwnd, IDirect3DDevice9* device)
{
    g_hWnd = (HWND)hwnd;
    g_pd3dDevice = device;

    if (!QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
        return false;
    if (!QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
        return false;

    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
    io.KeyMap[ImGuiKey_Home] = VK_HOME;
    io.KeyMap[ImGuiKey_End] = VK_END;
    io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
    io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    io.RenderDrawListsFn = ImGui_ImplDX9_RenderDrawLists;   // Alternatively you can Set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to Get the same ImDrawData pointer.
    io.ImeWindowHandle = g_hWnd;

    return true;
}
void ImGui_ImplDX9_Shutdown()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    ImGui::Shutdown();
    g_pd3dDevice = NULL;
    g_hWnd = 0;
}
void ImGui_ImplDX9_InvalidateDeviceObjects()
{
    if (!g_pd3dDevice)
        return;
    if (g_pVB)
    {
        g_pVB->Release();
        g_pVB = NULL;
    }
    if (g_pIB)
    {
        g_pIB->Release();
        g_pIB = NULL;
    }
    if (LPDIRECT3DTEXTURE9 tex = (LPDIRECT3DTEXTURE9)ImGui::GetIO().Fonts->TexID)
    {
        tex->Release();
        ImGui::GetIO().Fonts->TexID = 0;
    }
    g_FontTexture = NULL;
}
- 2016/10/15 (1.50) - avoid 'void* user_data' parameter to io.SetClipboardTextFn/io.GetClipboardTextFn pointers. We pass io.ClipboardUserData to it.
- 2016/09/25 (1.50) - style.WindowTitleAlign is now a ImVec2 (ImGuiAlign enum was removed). Set to (0.5f,0.5f) for horizontal+vertical centering, (0.0f,0.0f) for upper-left, etc.
- 2016/07/30 (1.50) - SameLine(x) with x>0.0f is now relative to left of column/group if any, and not always to left of window. This was sort of always the intent and hopefully breakage should be minimal.
- 2016/05/12 (1.49) - title bar (using ImGuiCol_TitleBg/ImGuiCol_TitleBgActive colors) isn't rendered over a window background (ImGuiCol_WindowBg color) anymore.
If your TitleBg/TitleBgActive alpha was 1.0f or you are using the default theme it will not affect you.
However if your TitleBg/TitleBgActive alpha was <1.0f you need to tweak your custom theme to readjust for the fact that we don't draw a WindowBg background behind the title bar.
This helper function will convert an old TitleBg/TitleBgActive color into a new one with the same visual output, given the OLD color and the OLD WindowBg color.
ImVec4 ConvertTitleBgCol(const ImVec4& win_bg_col, const ImVec4& title_bg_col)
{
float new_a = 1.0f - ((1.0f - win_bg_col.w) * (1.0f - title_bg_col.w)), k = title_bg_col.w / new_a;
return ImVec4((win_bg_col.x * win_bg_col.w + title_bg_col.x) * k, (win_bg_col.y * win_bg_col.w + title_bg_col.y) * k, (win_bg_col.z * win_bg_col.w + title_bg_col.z) * k, new_a);
}
- 2015/02/11 (1.32) - changed text input callback ImGuiTextEditCallback return type from void-->int. reserved for future use, return 0 for now.
- 2015/02/10 (1.32) - renamed GetItemWidth() to CalcItemWidth() to clarify its evolving behavior
- 2015/02/08 (1.31) - renamed GetTextLineSpacing() to GetTextLineHeightWithSpacing()
- 2015/02/01 (1.31) - removed IO.MemReallocFn (unused)
- 2015/01/19 (1.30) - renamed ImGuiStorage::GetIntPtr()/GetFloatPtr() to GetIntRef()/GetIntRef() because Ptr was conflicting with actual pointer storage functions.
- 2015/01/11 (1.30) - big font/image API change! now loads TTF file. allow for multiple fonts. no need for a PNG loader.
(1.30) - removed GetDefaultFontData(). uses io.Fonts->GetTextureData*() API to retrieve uncompressed pixels.
this sequence:
const void* png_data;
unsigned int png_size;
ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
// <Copy to GPU>
became:
unsigned char* pixels;
int width, height;
io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
// <Copy to GPU>
io.Fonts->TexID = (your_texture_identifier);
you now have much more flexibility to load multiple TTF fonts and manage the texture buffer for internal needs.
it is now recommended that you sample the font texture with bilinear interpolation.
(1.30) - added texture identifier in ImDrawCmd passed to your Render function (we can now Render images). make sure to Set io.Fonts->TexID.
(1.30) - removed IO.PixelCenterOffset (unnecessary, can be handled in user projection matrix)
(1.30) - removed ImGui::IsItemFocused() in favor of ImGui::IsItemActive() which handles all widgets
- 2014/12/10 (1.18) - removed SetNewWindowDefaultPos() in favor of new generic API SetNextWindowPos(pos, ImGuiSetCondition_FirstUseEver)
- 2014/11/28 (1.17) - moved IO.Font*** options to inside the IO.Font-> structure (FontYOffset, FontTexUvForWhite, FontBaseScale, FontFallbackGlyph)
- 2014/11/26 (1.17) - reworked syntax of IMGUI_ONCE_UPON_A_FRAME helper macro to increase compiler compatibility
- 2014/11/07 (1.15) - renamed IsHovered() to IsItemHovered()
- 2014/10/02 (1.14) - renamed IMGUI_INCLUDE_IMGUI_USER_CPP to IMGUI_INCLUDE_IMGUI_USER_INL and imgui_user.cpp to imgui_user.inl (more IDE friendly)
- 2014/09/25 (1.13) - removed 'text_end' parameter from IO.SetClipboardTextFn (the string is now always zero-terminated for simplicity)
- 2014/09/24 (1.12) - renamed SetFontScale() to SetWindowFontScale()
- 2014/09/24 (1.12) - moved IM_MALLOC/IM_REALLOC/IM_FREE preprocessor defines to IO.MemAllocFn/IO.MemReallocFn/IO.MemFreeFn
- 2014/08/30 (1.09) - removed IO.FontHeight (now computed automatically)
- 2014/08/30 (1.09) - moved IMGUI_FONT_TEX_UV_FOR_WHITE preprocessor define to IO.FontTexUvForWhite
- 2014/08/28 (1.09) - changed the behavior of IO.PixelCenterOffset following various rendering fixes


FREQUENTLY ASKED QUESTIONS (FAQ), TIPS
======================================

Q: How can I help?
A: - If you are experienced enough with ImGui and with C/C++, look at the todo list and see how you want/can help!
- Become a Patron/donate. Convince your company to become a Patron or provide serious funding for development time.

Q: How do I update to a newer version of ImGui?
A: Overwrite the following files:
imgui.cpp
imgui.h
imgui_demo.cpp
imgui_draw.cpp
imgui_internal.h
stb_rect_pack.h
stb_textedit.h
stb_truetype.h
Don't overwrite imconfig.h if you have made modification to your copy.
Check the "API BREAKING CHANGES" sections for a list of occasional API breaking changes. If you have a problem with a function, search for its name
in the code, there will likely be a comment about it. Please report any issue to the GitHub page!

Q: What is ImTextureID and how do I display an image?
A: ImTextureID is a void* used to pass renderer-agnostic texture references around until it hits your Render function.
ImGui knows nothing about what those bits represent, it just passes them around. It is up to you to decide what you want the void* to carry!
It could be an identifier to your OpenGL texture (cast GLuint to void*), a pointer to your custom engine material (cast MyMaterial* to void*), etc.
At the end of the chain, your renderer takes this void* to cast it back into whatever it needs to select a current texture to Render.
Refer to examples applications, where each renderer (in a imgui_impl_xxxx.cpp file) is treating ImTextureID as a different thing.
(c++ tip: OpenGL uses integers to identify textures. You can safely store an integer into a void*, just cast it to void*, don't take it's address!)
To display a custom image/texture within an ImGui window, you may use ImGui::Image(), ImGui::ImageButton(), ImDrawList::AddImage() functions.
ImGui will generate the geometry and draw calls using the ImTextureID that you passed and which your renderer can use.
It is your responsibility to Get textures uploaded to your GPU.

Q: I integrated ImGui in my engine and the text or lines are blurry..
A: In your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f).
Also make sure your orthographic projection matrix and io.DisplaySize matches your actual framebuffer dimension.

Q: I integrated ImGui in my engine and some elements are clipping or disappearing when I move windows around..
A: Most likely you are mishandling the clipping rectangles in your Render function. Rectangles provided by ImGui are defined as (x1=left,y1=top,x2=right,y2=bottom) and NOT as (x1,y1,width,height).

Q: Can I have multiple widgets with the same label? Can I have widget without a label? (Yes)
A: Yes. A primer on the use of labels/IDs in ImGui..

- Elements that are not clickable, such as Text() items don't need an ID.

- Interactive widgets require state to be carried over multiple frames (most typically ImGui often needs to remember what is the "active" widget).
to do so they need a unique ID. unique ID are typically derived from a string label, an integer index or a pointer.

Button("OK");        // Label = "OK",     ID = hash of "OK"
Button("Cancel");    // Label = "Cancel", ID = hash of "Cancel"

- ID are uniquely scoped within windows, tree nodes, etc. so no conflict can happen if you have two buttons called "OK" in two different windows
or in two different locations of a tree.

- If you have a same ID twice in the same location, you'll have a conflict:

Button("OK");
Button("OK");           // ID collision! Both buttons will be treated as the same.

Fear not! this is easy to solve and there are many ways to solve it!

- When passing a label you can optionally specify extra unique ID information within string itself. This helps solving the simpler collision cases.
use "##" to pass a complement to the ID that won't be visible to the end-user:

Button("Play");         // Label = "Play",   ID = hash of "Play"
Button("Play##foo1");   // Label = "Play",   ID = hash of "Play##foo1" (different from above)
Button("Play##foo2");   // Label = "Play",   ID = hash of "Play##foo2" (different from above)

- If you want to completely Hide the label, but still need an ID:

Checkbox("##On", &b);   // Label = "",       ID = hash of "##On" (no label!)

- Occasionally/rarely you might want change a label while preserving a constant ID. This allows you to animate labels.
For example you may want to include varying information in a window title bar (and windows are uniquely identified by their ID.. obviously)
Use "###" to pass a label that isn't part of ID:

Button("Hello###ID";   // Label = "Hello",  ID = hash of "ID"
Button("World###ID";   // Label = "World",  ID = hash of "ID" (same as above)

sprintf(buf, "My game (%f FPS)###MyGame");
Begin(buf);            // Variable label,   ID = hash of "MyGame"

- Use PushID() / PopID() to create scopes and avoid ID conflicts within the same Window.
This is the most convenient way of distinguishing ID if you are iterating and creating many UI elements.
You can push a pointer, a string or an integer value. Remember that ID are formed from the concatenation of everything in the ID stack!

for (int i = 0; i < 100; i++)
{
PushID(i);
Button("Click");   // Label = "Click",  ID = hash of integer + "label" (unique)
PopID();
}
A: You can read the 'io.WantCaptureXXX' flags in the ImGuiIO structure. Preferably read them after calling ImGui::NewFrame() to avoid those flags lagging by one frame, but either should be fine.
When 'io.WantCaptureMouse' or 'io.WantCaptureKeyboard' flags are Set you may want to discard/Hide the inputs from the rest of your application.
When 'io.WantInputsCharacters' is Set to may want to notify your OS to popup an on-screen keyboard, if available.
ImGui is tracking dragging and widget activity that may occur outside the boundary of a window, so 'io.WantCaptureMouse' is a more accurate and complete than testing for ImGui::IsMouseHoveringAnyWindow().
(Advanced note: text input releases focus on Return 'KeyDown', so the following Return 'KeyUp' event that your application receive will typically have 'io.WantcaptureKeyboard=false'.
Depending on your application logic it may or not be inconvenient. You might want to track which key-downs were for ImGui (e.g. with an array of bool) and filter out the corresponding key-ups.)

Q: How can I load a different font than the default? (default is an embedded version of ProggyClean.ttf, rendered at size 13)
A: Use the font atlas to load the TTF file you want:

ImGuiIO& io = ImGui::GetIO();
io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels);
io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()

Q: How can I easily use icons in my application?
A: The most convenient and practical way is to merge an icon font such as FontAwesome inside you main font. Then you can refer to icons within your strings.
Read 'How can I load multiple fonts?' and the file 'extra_fonts/README.txt' for instructions.

Q: How can I load multiple fonts?
A: Use the font atlas to pack them into a single texture:
(Read extra_fonts/README.txt and the code in ImFontAtlas for more details.)

ImGuiIO& io = ImGui::GetIO();
ImFont* font0 = io.Fonts->AddFontDefault();
ImFont* font1 = io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels);
ImFont* font2 = io.Fonts->AddFontFromFileTTF("myfontfile2.ttf", size_in_pixels);
io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()
// the first loaded font gets used by default
// use ImGui::PushFont()/ImGui::PopFont() to change the font at runtime

// Options
ImFontConfig config;
config.OversampleH = 3;
config.OversampleV = 1;
config.GlyphExtraSpacing.x = 1.0f;
io.Fonts->LoadFromFileTTF("myfontfile.ttf", size_pixels, &config);

// Combine multiple fonts into one (e.g. for icon fonts)
ImWchar ranges[] = { 0xf000, 0xf3ff, 0 };
- tree node: add treenode/treepush int variants? not there because (void*) cast from int warns on some platforms/settings?
- tree node: try to apply scrolling at time of TreePop() if node was just opened and end of node is past scrolling limits?
- tree node / selectable Render mismatch which is visible if you use them both next to each other (e.g. cf. property viewer)
- tree node: tweak color scheme to distinguish headers from selected tree node (#581)
- textwrapped: figure out better way to use TextWrapped() in an always auto-resize context (tooltip, etc.) (#249)
- settings: write more decent code to allow saving/loading new fields
- settings: api for per-tool simple persistent data (bool,int,float,columns sizes,etc.) in .ini file
- style: add window shadows.
- style/optimization: store rounded corners in texture to use 1 quad per corner (filled and wireframe) to lower the cost of rounding.
- style: color-box not always square?
- style: a concept of "compact style" that the end-user can easily rely on (e.g. PushStyleCompact()?) that maps to other settings? avoid implementing duplicate helpers such as SmallCheckbox(), etc.
- style: try to make PushStyleVar() more robust to incorrect parameters (to be more friendly to edit & continues situation).
- style: global scale setting.
- style: WindowPadding needs to be EVEN needs the 0.5 multiplier probably have a subtle effect on clip rectangle
- text: simple markup language for color change?
- font: dynamic font atlas to avoid baking huge ranges into bitmap and make scaling easier.
- font: small opt: for monospace font (like the defalt one) we can trim IndexXAdvance as long as trailing value is == FallbackXAdvance
- font: add support for kerning, probably optional. perhaps default to (32..128)^2 matrix ~ 36KB then hash fallback.
- font: add a simpler CalcTextSizeA() api? current one ok but not welcome if user needs to call it directly (without going through ImGui::CalcTextSize)
- font: fix AddRemapChar() to work before font has been built.
- log: LogButtons() options for specifying depth and/or hiding depth slider
- log: have more control over the log scope (e.g. stop logging when leaving current tree node scope)
- log: be able to log anything (e.g. right-click on a window/tree-node, shows context Menu? log into tty/file/clipboard)
- log: let user copy any window content to clipboard easily (CTRL+C on windows? while moving it? context Menu?). code is commented because it fails with multiple Begin/End pairs.
- filters: Set a current filter that tree node can automatically query to Hide themselves
- filters: handle wildcards (with implicit leading/trailing *), regexps
- shortcuts: add a shortcut api, e.g. parse "&Save" and/or "Save (CTRL+S)", pass in to widgets or provide simple ways to use (button=activate, input=focus)
!- keyboard: tooltip & combo boxes are messing up / not honoring keyboard tabbing
- keyboard: full keyboard navigation and focus. (#323)
- focus: preserve ActiveId/focus stack state, e.g. when opening a Menu and close it, previously selected InputText() focus gets restored (#622)
- focus: SetKeyboardFocusHere() on with >= 0 offset could be done on same frame (else latch and modulate on beginning of next frame)
- input: rework IO system to be able to pass actual ordered/timestamped events. (~#335, #71)
- input: allow to decide and pass explicit double-clicks (e.g. for windows by the CS_DBLCLKS style).
- input: support track pad style scrolling & slider edit.
- Misc: provide a way to compile out the entire implementation while providing a dummy API (e.g. #define IMGUI_DUMMY_IMPL)
- Misc: double-clicking on title bar to minimize isn't consistent, perhaps move to single-click on left-most collapse icon?
- Misc: provide HoveredTime and ActivatedTime to ease the creation of animations.
- style editor: have a more global HSV setter (e.g. alter hue on all elements). consider replacing active/hovered by offset in HSV space? (#438)
- style editor: color child window height expressed in multiple of line height.
- remote: make a system like RemoteImGui first-class citizen/project (#75)
- drawlist: move Font, FontSize, FontTexUvWhitePixel inside ImDrawList and make it self-contained (apart from drawing settings?)
- drawlist: end-user probably can't call Clear() directly because we expect a texture to be pushed in the stack.
- examples: directx9: save/restore device state more thoroughly.
- examples: window minimize, maximize (#583)
- optimization: add a flag to disable most of rendering, for the case where the user expect to skip it (#335)
- optimization: use another hash function than crc32, e.g. FNV1a
- optimization/Render: merge command-lists with same clip-rect into one even if they aren't sequential? (as long as in-between clip rectangle don't overlap)?
- optimization: turn some the various stack vectors into statically-sized arrays
- optimization: better clipping for multi-component widgets
*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_PLACEMENT_NEW
#include "imgui_internal.h"

#define NOMINMAX
#include <Windows.h>
#include <ctype.h>      // toupper, isprint
#include <stdlib.h>     // NULL, malloc, free, qsort, atoi
#include <stdio.h>      // vsnprintf, sscanf, printf
#include <limits.h>     // INT_MIN, INT_MAX
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4127) // condition expression is constant
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

// Clang warnings with -Weverything
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"      // warning : format string is not a string literal              // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wexit-time-destructors"  // warning : declaration requires an exit-time destructor       // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. ImGui coding style welcomes static/g_GlobalVars.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference it.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast" // warning : cast to 'void *' from smaller integer type 'int' //
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"      // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat"                   // warning: format '%p' expects argument of type 'void*', but argument 6 has type 'ImGuiWindow*'
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wcast-qual"                // warning: cast from type 'xxxx' to type 'xxxx' casts away qualifiers
#endif

//-------------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------------

static void             PushMultiItemsWidths(int components, float w_full = 0.0f);
static float            GetDraggedColumnOffset(int column_index);

static bool             IsKeyPressedMap(ImGuiKey key, bool repeat = true);

static ImFont*          GetDefaultFont();
static void             SetCurrentFont(ImFont* font);
static void             SetCurrentWindow(ImGuiWindow* window);
static void             SetWindowScrollY(ImGuiWindow* window, float new_scroll_y);
static void             SetWindowPos(ImGuiWindow* window, const ImVec2& pos, ImGuiSetCond cond);
static void             SetWindowSize(ImGuiWindow* window, const ImVec2& size, ImGuiSetCond cond);
static void             SetWindowCollapsed(ImGuiWindow* window, bool collapsed, ImGuiSetCond cond);
static ImGuiWindow*     FindHoveredWindow(ImVec2 pos, bool excluding_childs);
static ImGuiWindow*     CreateNewWindow(const char* name, ImVec2 size, ImGuiWindowFlags flags);
static inline bool      IsWindowContentHoverable(ImGuiWindow* window);
static void             ClearSetNextWindowData();
static void             CheckStacksSize(ImGuiWindow* window, bool write);
static void             Scrollbar(ImGuiWindow* window, bool horizontal);

static void             AddDrawListToRenderList(ImVector<ImDrawList*>& out_render_list, ImDrawList* draw_list);
static void             AddWindowToRenderList(ImVector<ImDrawList*>& out_render_list, ImGuiWindow* window);
static void             AddWindowToSortedBuffer(ImVector<ImGuiWindow*>& out_sorted_windows, ImGuiWindow* window);

static ImGuiIniData*    FindWindowSettings(const char* name);
static ImGuiIniData*    AddWindowSettings(const char* name);
static void             LoadIniSettingsFromDisk(const char* ini_filename);
static void             SaveIniSettingsToDisk(const char* ini_filename);
static void             MarkIniSettingsDirty();

static void             PushColumnClipRect(int column_index = -1);
static ImRect           GetVisibleRect();

static bool             BeginPopupEx(const char* str_id, ImGuiWindowFlags extra_flags);
static void             CloseInactivePopups();
static void             ClosePopupToLevel(int remaining);
static void             ClosePopup(ImGuiID id);
static bool             IsPopupOpen(ImGuiID id);
static ImGuiWindow*     GetFrontMostModalRootWindow();
static ImVec2           FindBestPopupWindowPos(const ImVec2& base_pos, const ImVec2& size, int* last_dir, const ImRect& rect_to_avoid);

static bool             InputTextFilterCharacter(unsigned int* p_char, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data);
static int              InputTextCalcTextLenAndLineCount(const char* text_begin, const char** out_text_end);
static ImVec2           InputTextCalcTextSizeW(const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining = NULL, ImVec2* out_offset = NULL, bool stop_on_new_line = false);

static inline void      DataTypeFormatString(ImGuiDataType data_type, void* data_ptr, const char* display_format, char* buf, int buf_size);
static inline void      DataTypeFormatString(ImGuiDataType data_type, void* data_ptr, int decimal_precision, char* buf, int buf_size);
static void             DataTypeApplyOp(ImGuiDataType data_type, int op, void* value1, const void* value2);
static bool             DataTypeApplyOpFromText(const char* buf, const char* initial_value_buf, ImGuiDataType data_type, void* data_ptr, const char* scalar_format);

//-----------------------------------------------------------------------------
// Platform dependent default implementations
//-----------------------------------------------------------------------------

static const char*      GetClipboardTextFn_DefaultImpl(void* user_data);
static void             SetClipboardTextFn_DefaultImpl(void* user_data, const char* text);
static void             ImeSetInputScreenPosFn_DefaultImpl(int x, int y);

//-----------------------------------------------------------------------------
// Context
//-----------------------------------------------------------------------------

ImVec2 ImVec2::Zero = { 0.0f, 0.0f };
void ImGuiIO::AddInputCharacter(ImWchar c)
{
    const int n = ImStrlenW(InputCharacters);
    if(n + 1 < IM_ARRAYSIZE(InputCharacters)) {
        InputCharacters[n] = c;
        InputCharacters[n + 1] = '\0';
    }
}
void ImGuiIO::AddInputCharactersUTF8(const char* utf8_chars)
{
    // We can't pass more wchars than ImGuiIO::InputCharacters[] can hold so don't convert more
    const int wchars_buf_len = sizeof(ImGuiIO::InputCharacters) / sizeof(ImWchar);
    ImWchar wchars[wchars_buf_len];
    ImTextStrFromUtf8(wchars, wchars_buf_len, utf8_chars, NULL);
    for(int i = 0; i < wchars_buf_len && wchars[i] != 0; i++)
        AddInputCharacter(wchars[i]);
}
void ImStrncpy(char* dst, const char* src, int count)
{
    if(count < 1) return;
    strncpy(dst, src, (size_t)count);
    dst[count - 1] = 0;
}
ImU32 ImHash(const void* data, int data_size, ImU32 seed)
{
    static ImU32 crc32_lut[256] = { 0 };
    if(!crc32_lut[1]) {
        const ImU32 polynomial = 0xEDB88320;
        for(ImU32 i = 0; i < 256; i++) {
            ImU32 crc = i;
            for(ImU32 j = 0; j < 8; j++)
                crc = (crc >> 1) ^ (ImU32(-int(crc & 1)) & polynomial);
            crc32_lut[i] = crc;
        }
    }

    seed = ~seed;
    ImU32 crc = seed;
    const unsigned char* current = (const unsigned char*)data;

    if(data_size > 0) {
        // Known size
        while(data_size--)
            crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ *current++];
    } else {
        // Zero-terminated string
        while(unsigned char c = *current++) {
            // We support a syntax of "label###id" where only "###id" is included in the hash, and only "label" gets displayed.
            // Because this syntax is rarely used we are optimizing for the common case.
            // - If we reach ### in the string we discard the hash so far and reset to the seed.
            // - We don't do 'current += 2; continue;' after handling ### to keep the code smaller.
            if(c == '#' && current[0] == '#' && current[1] == '#')
                crc = seed;
            crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
        }
    }
    return ~crc;
}
void ImGui::ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v)
{
    float K = 0.f;
    if(g < b) {
        const float tmp = g; g = b; b = tmp;
        K = -1.f;
    }
    if(r < g) {
        const float tmp = r; r = g; g = tmp;
        K = -2.f / 6.f - K;
    }

    const float chroma = r - (g < b ? g : b);
    out_h = fabsf(K + (g - b) / (6.f * chroma + 1e-20f));
    out_s = chroma / (r + 1e-20f);
    out_v = r;
}
void ImGui::ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b)
{
    if(s == 0.0f) {
        // gray
        out_r = out_g = out_b = v;
        return;
    }

    h = fmodf(h, 1.0f) / (60.0f / 360.0f);
    int   i = (int)h;
    float f = h - (float)i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch(i) {
        case 0: out_r = v; out_g = t; out_b = p; break;
        case 1: out_r = q; out_g = v; out_b = p; break;
        case 2: out_r = p; out_g = v; out_b = t; break;
        case 3: out_r = p; out_g = q; out_b = v; break;
        case 4: out_r = t; out_g = p; out_b = v; break;
        case 5: default: out_r = v; out_g = p; out_b = q; break;
    }
}
void* ImFileLoadToMemory(const char* filename, const char* file_open_mode, int* out_file_size, int padding_bytes)
{
    IM_ASSERT(filename && file_open_mode);
    if(out_file_size)
        *out_file_size = 0;

    FILE* f;
    if((f = ImFileOpen(filename, file_open_mode)) == NULL)
        return NULL;

    long file_size_signed;
    if(fseek(f, 0, SEEK_END) || (file_size_signed = ftell(f)) == -1 || fseek(f, 0, SEEK_SET)) {
        fclose(f);
        return NULL;
    }

    int file_size = (int)file_size_signed;
    void* file_data = ImGui::MemAlloc(file_size + padding_bytes);
    if(file_data == NULL) {
        fclose(f);
        return NULL;
    }
    if(fread(file_data, 1, (size_t)file_size, f) != (size_t)file_size) {
        fclose(f);
        ImGui::MemFree(file_data);
        return NULL;
    }
    if(padding_bytes > 0)
        memset((void *)(((char*)file_data) + file_size), 0, padding_bytes);

    fclose(f);
    if(out_file_size)
        *out_file_size = file_size;

    return file_data;
}
void ImGuiStorage::Clear()
{
    Data.clear();
}
void* ImGuiStorage::GetVoidPtr(ImGuiID key) const
{
    ImVector<Pair>::iterator it = LowerBound(const_cast<ImVector<ImGuiStorage::Pair>&>(Data), key);
    if(it == Data.end() || it->key != key)
        return NULL;
    return it->val_p;
}
void** ImGuiStorage::GetVoidPtrRef(ImGuiID key, void* default_val)
{
    ImVector<Pair>::iterator it = LowerBound(Data, key);
    if(it == Data.end() || it->key != key)
        it = Data.insert(it, Pair(key, default_val));
    return &it->val_p;
}
void ImGuiStorage::SetInt(ImGuiID key, int val)
{
    ImVector<Pair>::iterator it = LowerBound(Data, key);
    if(it == Data.end() || it->key != key) {
        Data.insert(it, Pair(key, val));
        return;
    }
    it->val_i = val;
}
void ImGuiStorage::SetBool(ImGuiID key, bool val)
{
    SetInt(key, val ? 1 : 0);
}
void ImGuiStorage::SetFloat(ImGuiID key, float val)
{
    ImVector<Pair>::iterator it = LowerBound(Data, key);
    if(it == Data.end() || it->key != key) {
        Data.insert(it, Pair(key, val));
        return;
    }
    it->val_f = val;
}
void ImGuiStorage::SetVoidPtr(ImGuiID key, void* val)
{
    ImVector<Pair>::iterator it = LowerBound(Data, key);
    if(it == Data.end() || it->key != key) {
        Data.insert(it, Pair(key, val));
        return;
    }
    it->val_p = val;
}
void ImGuiStorage::SetAllInt(int v)
{
    for(int i = 0; i < Data.Size; i++)
        Data[i].val_i = v;
}
void ImGuiTextFilter::TextRange::split(char separator, ImVector<TextRange>& out)
{
    out.resize(0);
    const char* wb = b;
    const char* we = wb;
    while(we < e) {
        if(*we == separator) {
            out.push_back(TextRange(wb, we));
            wb = we + 1;
        }
        we++;
    }
    if(wb != we)
        out.push_back(TextRange(wb, we));
}
void ImGuiTextFilter::Build()
{
    Filters.resize(0);
    TextRange input_range(InputBuf, InputBuf + strlen(InputBuf));
    input_range.split(',', Filters);

    CountGrep = 0;
    for(int i = 0; i != Filters.Size; i++) {
        Filters[i].trim_blanks();
        if(Filters[i].empty())
            continue;
        if(Filters[i].front() != '-')
            CountGrep += 1;
    }
}
void ImGuiTextBuffer::appendv(const char* fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);

    int len = vsnprintf(NULL, 0, fmt, args);         // FIXME-OPT: could do a first pass write attempt, likely successful on first pass.
    if(len <= 0)
        return;

    const int write_off = Buf.Size;
    const int needed_sz = write_off + len;
    if(write_off + len >= Buf.Capacity) {
        int double_capacity = Buf.Capacity * 2;
        Buf.reserve(needed_sz > double_capacity ? needed_sz : double_capacity);
    }

    Buf.resize(needed_sz);
    ImFormatStringV(&Buf[write_off] - 1, len + 1, fmt, args_copy);
}
void ImGuiTextBuffer::append(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    appendv(fmt, args);
    va_end(args);
}
void ImGuiSimpleColumns::Update(int count, float spacing, bool clear)
{
    IM_ASSERT(Count <= IM_ARRAYSIZE(Pos));
    Count = count;
    Width = NextWidth = 0.0f;
    Spacing = spacing;
    if(clear) memset(NextWidths, 0, sizeof(NextWidths));
    for(int i = 0; i < Count; i++) {
        if(i > 0 && NextWidths[i] > 0.0f)
            Width += Spacing;
        Pos[i] = (float)(int)Width;
        Width += NextWidths[i];
        NextWidths[i] = 0.0f;
    }
}
static void SetCursorPosYAndSetupDummyPrevLine(float pos_y, float line_height)
{
    // Set cursor position and a few other things so that SetScrollHere() and Columns() can work when seeking cursor. 
    // FIXME: It is problematic that we have to do that here, because custom/equivalent end-user code would stumble on the same issue. Consider moving within SetCursorXXX functions?
    ImGui::SetCursorPosY(pos_y);
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    window->DC.CursorPosPrevLine.y = window->DC.CursorPos.y - line_height;      // Setting those fields so that SetScrollHere() can properly function after the end of our clipper usage.
    window->DC.PrevLineHeight = (line_height - ImGui::GetStyle().ItemSpacing.y);    // If we end up needing more accurate data (to e.g. use SameLine) we may as well make the clipper have a fourth step to let user process and display the last item in their list.
    if(window->DC.ColumnsCount > 1)
        window->DC.ColumnsCellMinY = window->DC.CursorPos.y;                    // Setting this so that cell Y position are Set properly
}
void ImGuiListClipper::Begin(int count, float items_height)
{
    StartPosY = ImGui::GetCursorPosY();
    ItemsHeight = items_height;
    ItemsCount = count;
    StepNo = 0;
    DisplayEnd = DisplayStart = -1;
    if(ItemsHeight > 0.0f) {
        ImGui::CalcListClipping(ItemsCount, ItemsHeight, &DisplayStart, &DisplayEnd); // calculate how many to clip/display
        if(DisplayStart > 0)
            SetCursorPosYAndSetupDummyPrevLine(StartPosY + DisplayStart * ItemsHeight, ItemsHeight); // advance cursor
        StepNo = 2;
    }
}
void ImGuiListClipper::End()
{
    if(ItemsCount < 0)
        return;
    // In theory here we should assert that ImGui::GetCursorPosY() == StartPosY + DisplayEnd * ItemsHeight, but it feels saner to just seek at the end and not assert/crash the user.
    if(ItemsCount < INT_MAX)
        SetCursorPosYAndSetupDummyPrevLine(StartPosY + ItemsCount * ItemsHeight, ItemsHeight); // advance cursor
    ItemsCount = -1;
    StepNo = 3;
}
ImGuiID ImGuiWindow::GetID(const void* ptr)
{
    ImGuiID seed = IDStack.back();
    ImGuiID id = ImHash(&ptr, sizeof(void*), seed);
    ImGui::KeepAliveID(id);
    return id;
}
static void SetCurrentWindow(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    g.CurrentWindow = window;
    if(window)
        g.FontSize = window->CalcFontSize();
}
void ImGui::SetActiveID(ImGuiID id, ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    g.ActiveId = id;
    g.ActiveIdAllowOverlap = false;
    g.ActiveIdIsJustActivated = true;
    if(id)
        g.ActiveIdIsAlive = true;
    g.ActiveIdWindow = window;
}
void ImGui::ClearActiveID()
{
    SetActiveID(0, NULL);
}
void ImGui::SetHoveredID(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    g.HoveredId = id;
    g.HoveredIdAllowOverlap = false;
}
void ImGui::KeepAliveID(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    if(g.ActiveId == id)
        g.ActiveIdIsAlive = true;
}
void ImGui::ItemSize(const ImVec2& size, float text_offset_y)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    // Always align ourselves on pixel boundaries
    ImGuiContext& g = *GImGui;
    const float line_height = ImMax(window->DC.CurrentLineHeight, size.y);
    const float text_base_offset = ImMax(window->DC.CurrentLineTextBaseOffset, text_offset_y);
    window->DC.CursorPosPrevLine = ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y);
    window->DC.CursorPos = ImVec2((float)(int)(window->Pos.x + window->DC.IndentX + window->DC.ColumnsOffsetX), (float)(int)(window->DC.CursorPos.y + line_height + g.Style->ItemSpacing.y));
    window->DC.CursorMaxPos.x = ImMax(window->DC.CursorMaxPos.x, window->DC.CursorPosPrevLine.x);
    window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, window->DC.CursorPos.y);

    //window->DrawList->AddCircle(window->DC.CursorMaxPos, 3.0f, IM_COL32(255,0,0,255), 4); // Debug

    window->DC.PrevLineHeight = line_height;
    window->DC.PrevLineTextBaseOffset = text_base_offset;
    window->DC.CurrentLineHeight = window->DC.CurrentLineTextBaseOffset = 0.0f;
}
void ImGui::ItemSize(const ImRect& bb, float text_offset_y)
{
    ItemSize(bb.GetSize(), text_offset_y);
}
void ImGui::FocusableItemUnregister(ImGuiWindow* window)
{
    window->FocusIdxAllCounter--;
    window->FocusIdxTabCounter--;
}
void* ImGui::MemAlloc(size_t sz)
{
    GImGui->IO.MetricsAllocs++;
    return GImGui->IO.MemAllocFn(sz);
}
void ImGui::MemFree(void* ptr)
{
    if(ptr) GImGui->IO.MetricsAllocs--;
    return GImGui->IO.MemFreeFn(ptr);
}
void ImGui::SetClipboardText(const char* text)
{
    if(GImGui->IO.SetClipboardTextFn)
        GImGui->IO.SetClipboardTextFn(GImGui->IO.ClipboardUserData, text);
}
void ImGui::SetCurrentContext(ImGuiContext* ctx)
{
#ifdef IMGUI_SET_CURRENT_CONTEXT_FUNC
    IMGUI_SET_CURRENT_CONTEXT_FUNC(ctx); // For custom thread-based hackery you may want to have control over this.
#else
    GImGui = ctx;
#endif
}
ImGuiContext* ImGui::CreateContext(void* (*malloc_fn)(size_t), void(*free_fn)(void*))
{
    if(!malloc_fn) malloc_fn = malloc;
    ImGuiContext* ctx = (ImGuiContext*)malloc_fn(sizeof(ImGuiContext));
    IM_PLACEMENT_NEW(ctx) ImGuiContext();
    ctx->IO.MemAllocFn = malloc_fn;
    ctx->IO.MemFreeFn = free_fn ? free_fn : free;
    return ctx;
}
void ImGui::DestroyContext(ImGuiContext* ctx)
{
    void(*free_fn)(void*) = ctx->IO.MemFreeFn;
    ctx->~ImGuiContext();
    free_fn(ctx);
    if(GImGui == ctx)
        SetCurrentContext(NULL);
}
void ImGui::NewFrame()
{
    ImGuiContext& g = *GImGui;

    // Check user data
    IM_ASSERT(g.IO.DeltaTime >= 0.0f);               // Need a positive DeltaTime (zero is tolerated but will cause some timing issues)
    IM_ASSERT(g.IO.DisplaySize.x >= 0.0f && g.IO.DisplaySize.y >= 0.0f);
    IM_ASSERT(g.IO.Fonts->Fonts.Size > 0);           // Font Atlas not created. Did you call io.Fonts->GetTexDataAsRGBA32 / GetTexDataAsAlpha8 ?
    IM_ASSERT(g.IO.Fonts->Fonts[0]->IsLoaded());     // Font Atlas not created. Did you call io.Fonts->GetTexDataAsRGBA32 / GetTexDataAsAlpha8 ?

    if(!g.Initialized) {
        g.IO.FontDefault = g.IO.Fonts->Fonts[0];
        g.StyleStack.push_back(ImGuiStyle());
        g.Style = &(g.StyleStack.back());
        IM_ASSERT(g.Settings.empty());
        LoadIniSettingsFromDisk(g.IO.IniFilename);
        g.Initialized = true;
    }

    IM_ASSERT(g.StyleStack.Size == 1); // Unmatched Push/PopStyleScheme calls
    IM_ASSERT(g.Style->CurveTessellationTol > 0.0f);  // Invalid style setting

    SetCurrentFont(GetDefaultFont());
    IM_ASSERT(g.Font->IsLoaded());

    g.Time += g.IO.DeltaTime;
    g.FrameCount += 1;
    g.Tooltip[0] = '\0';
    g.OverlayDrawList.Clear();
    g.OverlayDrawList.PushTextureID(g.IO.Fonts->TexID);
    g.OverlayDrawList.PushClipRectFullScreen();

    // Mark rendering data as invalid to prevent user who may have a handle on it to use it
    g.RenderDrawData.Valid = false;
    g.RenderDrawData.CmdLists = NULL;
    g.RenderDrawData.CmdListsCount = g.RenderDrawData.TotalVtxCount = g.RenderDrawData.TotalIdxCount = 0;

    // Update inputs state
    if(g.IO.MousePos.x < 0 && g.IO.MousePos.y < 0)
        g.IO.MousePos = ImVec2(-9999.0f, -9999.0f);
    if((g.IO.MousePos.x < 0 && g.IO.MousePos.y < 0) || (g.IO.MousePosPrev.x < 0 && g.IO.MousePosPrev.y < 0))   // if mouse just appeared or disappeared (negative coordinate) we cancel out movement in MouseDelta
        g.IO.MouseDelta = ImVec2(0.0f, 0.0f);
    else
        g.IO.MouseDelta = g.IO.MousePos - g.IO.MousePosPrev;
    g.IO.MousePosPrev = g.IO.MousePos;
    for(int i = 0; i < IM_ARRAYSIZE(g.IO.MouseDown); i++) {
        g.IO.MouseClicked[i] = g.IO.MouseDown[i] && g.IO.MouseDownDuration[i] < 0.0f;
        g.IO.MouseReleased[i] = !g.IO.MouseDown[i] && g.IO.MouseDownDuration[i] >= 0.0f;
        g.IO.MouseDownDurationPrev[i] = g.IO.MouseDownDuration[i];
        g.IO.MouseDownDuration[i] = g.IO.MouseDown[i] ? (g.IO.MouseDownDuration[i] < 0.0f ? 0.0f : g.IO.MouseDownDuration[i] + g.IO.DeltaTime) : -1.0f;
        g.IO.MouseDoubleClicked[i] = false;
        if(g.IO.MouseClicked[i]) {
            if(g.Time - g.IO.MouseClickedTime[i] < g.IO.MouseDoubleClickTime) {
                if(ImLengthSqr(g.IO.MousePos - g.IO.MouseClickedPos[i]) < g.IO.MouseDoubleClickMaxDist * g.IO.MouseDoubleClickMaxDist)
                    g.IO.MouseDoubleClicked[i] = true;
                g.IO.MouseClickedTime[i] = -FLT_MAX;    // so the third click isn't turned into a double-click
            } else {
                g.IO.MouseClickedTime[i] = g.Time;
            }
            g.IO.MouseClickedPos[i] = g.IO.MousePos;
            g.IO.MouseDragMaxDistanceSqr[i] = 0.0f;
        } else if(g.IO.MouseDown[i]) {
            g.IO.MouseDragMaxDistanceSqr[i] = ImMax(g.IO.MouseDragMaxDistanceSqr[i], ImLengthSqr(g.IO.MousePos - g.IO.MouseClickedPos[i]));
        }
    }
    memcpy(g.IO.KeysDownDurationPrev, g.IO.KeysDownDuration, sizeof(g.IO.KeysDownDuration));
    for(int i = 0; i < IM_ARRAYSIZE(g.IO.KeysDown); i++)
        g.IO.KeysDownDuration[i] = g.IO.KeysDown[i] ? (g.IO.KeysDownDuration[i] < 0.0f ? 0.0f : g.IO.KeysDownDuration[i] + g.IO.DeltaTime) : -1.0f;

    // Calculate frame-rate for the user, as a purely luxurious feature
    g.FramerateSecPerFrameAccum += g.IO.DeltaTime - g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx];
    g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx] = g.IO.DeltaTime;
    g.FramerateSecPerFrameIdx = (g.FramerateSecPerFrameIdx + 1) % IM_ARRAYSIZE(g.FramerateSecPerFrame);
    g.IO.Framerate = 1.0f / (g.FramerateSecPerFrameAccum / (float)IM_ARRAYSIZE(g.FramerateSecPerFrame));

    // Clear reference to active widget if the widget isn't alive anymore
    g.HoveredIdPreviousFrame = g.HoveredId;
    g.HoveredId = 0;
    g.HoveredIdAllowOverlap = false;
    if(!g.ActiveIdIsAlive && g.ActiveIdPreviousFrame == g.ActiveId && g.ActiveId != 0)
        ClearActiveID();
    g.ActiveIdPreviousFrame = g.ActiveId;
    g.ActiveIdIsAlive = false;
    g.ActiveIdIsJustActivated = false;

    // Handle user moving window (at the beginning of the frame to avoid input lag or sheering). Only valid for root windows.
    if(g.MovedWindowMoveId && g.MovedWindowMoveId == g.ActiveId) {
        KeepAliveID(g.MovedWindowMoveId);
        IM_ASSERT(g.MovedWindow && g.MovedWindow->RootWindow);
        IM_ASSERT(g.MovedWindow->RootWindow->MoveId == g.MovedWindowMoveId);
        if(g.IO.MouseDown[0]) {
            if(!(g.MovedWindow->Flags & ImGuiWindowFlags_NoMove)) {
                g.MovedWindow->PosFloat += g.IO.MouseDelta;
                if(!(g.MovedWindow->Flags & ImGuiWindowFlags_NoSavedSettings) && (g.IO.MouseDelta.x != 0.0f || g.IO.MouseDelta.y != 0.0f))
                    MarkIniSettingsDirty();
            }
            FocusWindow(g.MovedWindow);
        } else {
            ClearActiveID();
            g.MovedWindow = NULL;
            g.MovedWindowMoveId = 0;
        }
    } else {
        g.MovedWindow = NULL;
        g.MovedWindowMoveId = 0;
    }

    // Delay saving settings so we don't spam disk too much
    if(g.SettingsDirtyTimer > 0.0f) {
        g.SettingsDirtyTimer -= g.IO.DeltaTime;
        if(g.SettingsDirtyTimer <= 0.0f)
            SaveIniSettingsToDisk(g.IO.IniFilename);
    }

    // GetOffset the window we are hovering. Child windows can extend beyond the limit of their parent so we need to derive HoveredRootWindow from HoveredWindow
    g.HoveredWindow = g.MovedWindow ? g.MovedWindow : FindHoveredWindow(g.IO.MousePos, false);
    if(g.HoveredWindow && (g.HoveredWindow->Flags & ImGuiWindowFlags_ChildWindow))
        g.HoveredRootWindow = g.HoveredWindow->RootWindow;
    else
        g.HoveredRootWindow = g.MovedWindow ? g.MovedWindow->RootWindow : FindHoveredWindow(g.IO.MousePos, true);

    if(ImGuiWindow* modal_window = GetFrontMostModalRootWindow()) {
        g.ModalWindowDarkeningRatio = ImMin(g.ModalWindowDarkeningRatio + g.IO.DeltaTime * 6.0f, 1.0f);
        ImGuiWindow* window = g.HoveredRootWindow;
        while(window && window != modal_window)
            window = window->ParentWindow;
        if(!window)
            g.HoveredRootWindow = g.HoveredWindow = NULL;
    } else {
        g.ModalWindowDarkeningRatio = 0.0f;
    }

    // Are we using inputs? Tell user so they can capture/discard the inputs away from the rest of their application.
    // When clicking outside of a window we assume the click is owned by the application and won't request capture. We need to track click ownership.
    int mouse_earliest_button_down = -1;
    bool mouse_any_down = false;
    for(int i = 0; i < IM_ARRAYSIZE(g.IO.MouseDown); i++) {
        if(g.IO.MouseClicked[i])
            g.IO.MouseDownOwned[i] = (g.HoveredWindow != NULL) || (!g.OpenPopupStack.empty());
        mouse_any_down |= g.IO.MouseDown[i];
        if(g.IO.MouseDown[i])
            if(mouse_earliest_button_down == -1 || g.IO.MouseClickedTime[mouse_earliest_button_down] > g.IO.MouseClickedTime[i])
                mouse_earliest_button_down = i;
    }
    bool mouse_avail_to_imgui = (mouse_earliest_button_down == -1) || g.IO.MouseDownOwned[mouse_earliest_button_down];
    if(g.CaptureMouseNextFrame != -1)
        g.IO.WantCaptureMouse = (g.CaptureMouseNextFrame != 0);
    else
        g.IO.WantCaptureMouse = (mouse_avail_to_imgui && (g.HoveredWindow != NULL || mouse_any_down)) || (g.ActiveId != 0) || (!g.OpenPopupStack.empty());
    g.IO.WantCaptureKeyboard = (g.CaptureKeyboardNextFrame != -1) ? (g.CaptureKeyboardNextFrame != 0) : (g.ActiveId != 0);
    g.IO.WantTextInput = (g.ActiveId != 0 && g.InputTextState.Id == g.ActiveId);
    g.MouseCursor = ImGuiMouseCursor_Arrow;
    g.CaptureMouseNextFrame = g.CaptureKeyboardNextFrame = -1;
    g.OsImePosRequest = ImVec2(1.0f, 1.0f); // OS Input Method Editor showing on top-left of our window by default

                                            // If mouse was first clicked outside of ImGui bounds we also cancel out hovering.
    if(!mouse_avail_to_imgui)
        g.HoveredWindow = g.HoveredRootWindow = NULL;

    // Scale & Scrolling
    if(g.HoveredWindow && g.IO.MouseWheel != 0.0f && !g.HoveredWindow->Collapsed) {
        ImGuiWindow* window = g.HoveredWindow;
        if(g.IO.KeyCtrl) {
            if(g.IO.FontAllowUserScaling) {
                // Zoom / Scale window
                float new_font_scale = ImClamp(window->FontWindowScale + g.IO.MouseWheel * 0.10f, 0.50f, 2.50f);
                float scale = new_font_scale / window->FontWindowScale;
                window->FontWindowScale = new_font_scale;

                const ImVec2 offset = window->Size * (1.0f - scale) * (g.IO.MousePos - window->Pos) / window->Size;
                window->Pos += offset;
                window->PosFloat += offset;
                window->Size *= scale;
                window->SizeFull *= scale;
            }
        } else if(!(window->Flags & ImGuiWindowFlags_NoScrollWithMouse)) {
            // Scroll
            const int scroll_lines = (window->Flags & ImGuiWindowFlags_ComboBox) ? 3 : 5;
            SetWindowScrollY(window, window->Scroll.y - g.IO.MouseWheel * window->CalcFontSize() * scroll_lines);
        }
    }

    // Pressing TAB activate widget focus
    // NB: Don't discard FocusedWindow if it isn't active, so that a window that go on/off programatically won't lose its keyboard focus.
    if(g.ActiveId == 0 && g.FocusedWindow != NULL && g.FocusedWindow->Active && IsKeyPressedMap(ImGuiKey_Tab, false))
        g.FocusedWindow->FocusIdxTabRequestNext = 0;

    // Mark all windows as not visible
    for(int i = 0; i != g.Windows.Size; i++) {
        ImGuiWindow* window = g.Windows[i];
        window->WasActive = window->Active;
        window->Active = false;
        window->Accessed = false;
    }

    // Closing the focused window restore focus to the first active root window in descending z-order
    if(g.FocusedWindow && !g.FocusedWindow->WasActive)
        for(int i = g.Windows.Size - 1; i >= 0; i--)
            if(g.Windows[i]->WasActive && !(g.Windows[i]->Flags & ImGuiWindowFlags_ChildWindow)) {
                FocusWindow(g.Windows[i]);
                break;
            }

    // No window should be open at the beginning of the frame.
    // But in order to allow the user to call NewFrame() multiple times without calling Render(), we are doing an explicit clear.
    g.CurrentWindowStack.resize(0);
    g.CurrentPopupStack.resize(0);
    CloseInactivePopups();

    // Create implicit window - we will only Render it if the user has added something to it.
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Debug");
}
void ImGui::Shutdown()
{
    ImGuiContext& g = *GImGui;

    // The fonts atlas can be used prior to calling NewFrame(), so we clear it even if g.Initialized is FALSE (which would happen if we never called NewFrame)
    if(g.IO.Fonts) // Testing for NULL to allow user to NULLify in case of running Shutdown() on multiple contexts. Bit hacky.
        g.IO.Fonts->Clear();

    // Cleanup of other data are conditional on actually having used ImGui.
    if(!g.Initialized)
        return;

    SaveIniSettingsToDisk(g.IO.IniFilename);

    for(int i = 0; i < g.Windows.Size; i++) {
        g.Windows[i]->~ImGuiWindow();
        ImGui::MemFree(g.Windows[i]);
    }
    g.Windows.clear();
    g.WindowsSortBuffer.clear();
    g.CurrentWindow = NULL;
    g.CurrentWindowStack.clear();
    g.FocusedWindow = NULL;
    g.HoveredWindow = NULL;
    g.HoveredRootWindow = NULL;
    g.ActiveIdWindow = NULL;
    g.MovedWindow = NULL;
    for(int i = 0; i < g.Settings.Size; i++)
        ImGui::MemFree(g.Settings[i].Name);
    g.Settings.clear();
    g.ColorModifiers.clear();
    g.StyleModifiers.clear();
    g.FontStack.clear();
    g.OpenPopupStack.clear();
    g.CurrentPopupStack.clear();
    g.SetNextWindowSizeConstraintCallback = NULL;
    g.SetNextWindowSizeConstraintCallbackUserData = NULL;
    for(int i = 0; i < IM_ARRAYSIZE(g.RenderDrawLists); i++)
        g.RenderDrawLists[i].clear();
    g.OverlayDrawList.ClearFreeMemory();
    g.ColorEditModeStorage.Clear();
    if(g.PrivateClipboard) {
        ImGui::MemFree(g.PrivateClipboard);
        g.PrivateClipboard = NULL;
    }
    g.InputTextState.Text.clear();
    g.InputTextState.InitialText.clear();
    g.InputTextState.TempTextBuffer.clear();

    g.Initialized = false;
}
static void LoadIniSettingsFromDisk(const char* ini_filename)
{
    ImGuiContext& g = *GImGui;
    if(!ini_filename)
        return;

    int file_size;
    char* file_data = (char*)ImFileLoadToMemory(ini_filename, "rb", &file_size, 1);
    if(!file_data)
        return;

    ImGuiIniData* settings = NULL;
    const char* buf_end = file_data + file_size;
    for(const char* line_start = file_data; line_start < buf_end; ) {
        const char* line_end = line_start;
        while(line_end < buf_end && *line_end != '\n' && *line_end != '\r')
            line_end++;

        if(line_start[0] == '[' && line_end > line_start && line_end[-1] == ']') {
            char name[64];
            ImFormatString(name, IM_ARRAYSIZE(name), "%.*s", (int)(line_end - line_start - 2), line_start + 1);
            settings = FindWindowSettings(name);
            if(!settings)
                settings = AddWindowSettings(name);
        } else if(settings) {
            float x, y;
            int i;
            if(sscanf(line_start, "Pos=%f,%f", &x, &y) == 2)
                settings->Pos = ImVec2(x, y);
            else if(sscanf(line_start, "Size=%f,%f", &x, &y) == 2)
                settings->Size = ImMax(ImVec2(x, y), g.Style->WindowMinSize);
            else if(sscanf(line_start, "Collapsed=%d", &i) == 1)
                settings->Collapsed = (i != 0);
        }

        line_start = line_end + 1;
    }

    ImGui::MemFree(file_data);
}
static void SaveIniSettingsToDisk(const char* ini_filename)
{
    ImGuiContext& g = *GImGui;
    g.SettingsDirtyTimer = 0.0f;
    if(!ini_filename)
        return;

    // Gather data from windows that were active during this session
    for(int i = 0; i != g.Windows.Size; i++) {
        ImGuiWindow* window = g.Windows[i];
        if(window->Flags & ImGuiWindowFlags_NoSavedSettings)
            continue;
        ImGuiIniData* settings = FindWindowSettings(window->Name);
        settings->Pos = window->Pos;
        settings->Size = window->SizeFull;
        settings->Collapsed = window->Collapsed;
    }

    // Write .ini file
    // If a window wasn't opened in this session we preserve its settings
    FILE* f = ImFileOpen(ini_filename, "wt");
    if(!f)
        return;
    for(int i = 0; i != g.Settings.Size; i++) {
        const ImGuiIniData* settings = &g.Settings[i];
        if(settings->Pos.x == FLT_MAX)
            continue;
        const char* name = settings->Name;
        if(const char* p = strstr(name, "###"))  // Skip to the "###" marker if any. We don't skip past to match the behavior of GetID()
            name = p;
        fprintf(f, "[%s]\n", name);
        fprintf(f, "Pos=%d,%d\n", (int)settings->Pos.x, (int)settings->Pos.y);
        fprintf(f, "Size=%d,%d\n", (int)settings->Size.x, (int)settings->Size.y);
        fprintf(f, "Collapsed=%d\n", settings->Collapsed);
        fprintf(f, "\n");
    }

    fclose(f);
}
static void MarkIniSettingsDirty()
{
    ImGuiContext& g = *GImGui;
    if(g.SettingsDirtyTimer <= 0.0f)
        g.SettingsDirtyTimer = g.IO.IniSavingRate;
}
static int ChildWindowComparer(const void* lhs, const void* rhs)
{
    const ImGuiWindow* a = *(const ImGuiWindow**)lhs;
    const ImGuiWindow* b = *(const ImGuiWindow**)rhs;
    if(int d = (a->Flags & ImGuiWindowFlags_Popup) - (b->Flags & ImGuiWindowFlags_Popup))
        return d;
    if(int d = (a->Flags & ImGuiWindowFlags_Tooltip) - (b->Flags & ImGuiWindowFlags_Tooltip))
        return d;
    if(int d = (a->Flags & ImGuiWindowFlags_ComboBox) - (b->Flags & ImGuiWindowFlags_ComboBox))
        return d;
    return (a->IndexWithinParent - b->IndexWithinParent);
}
static void AddWindowToSortedBuffer(ImVector<ImGuiWindow*>& out_sorted_windows, ImGuiWindow* window)
{
    out_sorted_windows.push_back(window);
    if(window->Active) {
        int count = window->DC.ChildWindows.Size;
        if(count > 1)
            qsort(window->DC.ChildWindows.begin(), (size_t)count, sizeof(ImGuiWindow*), ChildWindowComparer);
        for(int i = 0; i < count; i++) {
            ImGuiWindow* child = window->DC.ChildWindows[i];
            if(child->Active)
                AddWindowToSortedBuffer(out_sorted_windows, child);
        }
    }
}
static void AddDrawListToRenderList(ImVector<ImDrawList*>& out_render_list, ImDrawList* draw_list)
{
    if(draw_list->CmdBuffer.empty())
        return;

    // Remove trailing command if unused
    ImDrawCmd& last_cmd = draw_list->CmdBuffer.back();
    if(last_cmd.ElemCount == 0 && last_cmd.UserCallback == NULL) {
        draw_list->CmdBuffer.pop_back();
        if(draw_list->CmdBuffer.empty())
            return;
    }

    // Draw list sanity check. Detect mismatch between PrimReserve() calls and incrementing _VtxCurrentIdx, _VtxWritePtr etc.
    IM_ASSERT(draw_list->VtxBuffer.Size == 0 || draw_list->_VtxWritePtr == draw_list->VtxBuffer.Data + draw_list->VtxBuffer.Size);
    IM_ASSERT(draw_list->IdxBuffer.Size == 0 || draw_list->_IdxWritePtr == draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size);
    IM_ASSERT((int)draw_list->_VtxCurrentIdx == draw_list->VtxBuffer.Size);

    // Check that draw_list doesn't use more vertices than indexable (default ImDrawIdx = 2 bytes = 64K vertices)
    // If this assert triggers because you are drawing lots of stuff manually, A) workaround by calling BeginChild()/EndChild() to put your draw commands in multiple draw lists, B) #define ImDrawIdx to a 'unsigned int' in imconfig.h and Render accordingly.
    IM_ASSERT((int64_t)draw_list->_VtxCurrentIdx <= ((int64_t)1L << (sizeof(ImDrawIdx) * 8)));  // Too many vertices in same ImDrawList. See comment above.

    out_render_list.push_back(draw_list);
    GImGui->IO.MetricsRenderVertices += draw_list->VtxBuffer.Size;
    GImGui->IO.MetricsRenderIndices += draw_list->IdxBuffer.Size;
}
static void AddWindowToRenderList(ImVector<ImDrawList*>& out_render_list, ImGuiWindow* window)
{
    AddDrawListToRenderList(out_render_list, window->DrawList);
    for(int i = 0; i < window->DC.ChildWindows.Size; i++) {
        ImGuiWindow* child = window->DC.ChildWindows[i];
        if(!child->Active) // clipped children may have been marked not active
            continue;
        if((child->Flags & ImGuiWindowFlags_Popup) && child->HiddenFrames > 0)
            continue;
        AddWindowToRenderList(out_render_list, child);
    }
}
void ImGui::PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DrawList->PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
    window->ClipRect = window->DrawList->_ClipRectStack.back();
}
void ImGui::PopClipRect()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DrawList->PopClipRect();
    window->ClipRect = window->DrawList->_ClipRectStack.back();
}
// This is normally called by Render(). You may want to call it directly if you want to avoid calling Render() but the gain will be very minimal.
void ImGui::EndFrame()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.Initialized);                       // Forgot to call ImGui::NewFrame()
    IM_ASSERT(g.FrameCountEnded != g.FrameCount);   // ImGui::EndFrame() called multiple times, or forgot to call ImGui::NewFrame() again

                                                    // Render tooltip
    if(g.Tooltip[0]) {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(g.Tooltip);
        ImGui::EndTooltip();
    }

    // Notify OS when our Input Method Editor cursor has moved (e.g. CJK inputs using Microsoft IME)
    if(g.IO.ImeSetInputScreenPosFn && ImLengthSqr(g.OsImePosRequest - g.OsImePosSet) > 0.0001f) {
        g.IO.ImeSetInputScreenPosFn((int)g.OsImePosRequest.x, (int)g.OsImePosRequest.y);
        g.OsImePosSet = g.OsImePosRequest;
    }

    // Hide implicit "Debug" window if it hasn't been used
    IM_ASSERT(g.CurrentWindowStack.Size == 1);    // Mismatched Begin()/End() calls
    if(g.CurrentWindow && !g.CurrentWindow->Accessed)
        g.CurrentWindow->Active = false;
    ImGui::End();

    // Click to focus window and start moving (after we're done with all our widgets)
    if(g.ActiveId == 0 && g.HoveredId == 0 && g.IO.MouseClicked[0]) {
        if(!(g.FocusedWindow && !g.FocusedWindow->WasActive && g.FocusedWindow->Active)) // Unless we just made a popup appear
        {
            if(g.HoveredRootWindow != NULL) {
                FocusWindow(g.HoveredWindow);
                if(!(g.HoveredWindow->Flags & ImGuiWindowFlags_NoMove)) {
                    g.MovedWindow = g.HoveredWindow;
                    g.MovedWindowMoveId = g.HoveredRootWindow->MoveId;
                    SetActiveID(g.MovedWindowMoveId, g.HoveredRootWindow);
                }
            } else if(g.FocusedWindow != NULL && GetFrontMostModalRootWindow() == NULL) {
                // Clicking on void disable focus
                FocusWindow(NULL);
            }
        }
    }

    // Sort the window list so that all child windows are after their parent
    // We cannot do that on FocusWindow() because childs may not exist yet
    g.WindowsSortBuffer.resize(0);
    g.WindowsSortBuffer.reserve(g.Windows.Size);
    for(int i = 0; i != g.Windows.Size; i++) {
        ImGuiWindow* window = g.Windows[i];
        if(window->Active && (window->Flags & ImGuiWindowFlags_ChildWindow))       // if a child is active its parent will add it
            continue;
        AddWindowToSortedBuffer(g.WindowsSortBuffer, window);
    }
    IM_ASSERT(g.Windows.Size == g.WindowsSortBuffer.Size);  // we done something wrong
    g.Windows.swap(g.WindowsSortBuffer);

    // Clear Input data for next frame
    g.IO.MouseWheel = 0.0f;
    memset(g.IO.InputCharacters, 0, sizeof(g.IO.InputCharacters));

    g.FrameCountEnded = g.FrameCount;
}
void ImGui::Render()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.Initialized);   // Forgot to call ImGui::NewFrame()

    if(g.FrameCountEnded != g.FrameCount)
        ImGui::EndFrame();
    g.FrameCountRendered = g.FrameCount;

    // Skip Render altogether if alpha is 0.0
    // Note that vertex buffers have been created and are wasted, so it is best practice that you don't create windows in the first place, or consistently respond to Begin() returning false.
    if(g.Style->Alpha > 0.0f) {
        // Gather windows to Render
        g.IO.MetricsRenderVertices = g.IO.MetricsRenderIndices = g.IO.MetricsActiveWindows = 0;
        for(int i = 0; i < IM_ARRAYSIZE(g.RenderDrawLists); i++)
            g.RenderDrawLists[i].resize(0);
        for(int i = 0; i != g.Windows.Size; i++) {
            ImGuiWindow* window = g.Windows[i];
            if(window->Active && window->HiddenFrames <= 0 && (window->Flags & (ImGuiWindowFlags_ChildWindow)) == 0) {
                // FIXME: Generalize this with a proper layering system so e.g. user can draw in specific layers, below text, ..
                g.IO.MetricsActiveWindows++;
                if(window->Flags & ImGuiWindowFlags_Popup)
                    AddWindowToRenderList(g.RenderDrawLists[1], window);
                else if(window->Flags & ImGuiWindowFlags_Tooltip)
                    AddWindowToRenderList(g.RenderDrawLists[2], window);
                else
                    AddWindowToRenderList(g.RenderDrawLists[0], window);
            }
        }

        // Flatten layers
        int n = g.RenderDrawLists[0].Size;
        int flattened_size = n;
        for(int i = 1; i < IM_ARRAYSIZE(g.RenderDrawLists); i++)
            flattened_size += g.RenderDrawLists[i].Size;
        g.RenderDrawLists[0].resize(flattened_size);
        for(int i = 1; i < IM_ARRAYSIZE(g.RenderDrawLists); i++) {
            ImVector<ImDrawList*>& layer = g.RenderDrawLists[i];
            if(layer.empty())
                continue;
            memcpy(&g.RenderDrawLists[0][n], &layer[0], layer.Size * sizeof(ImDrawList*));
            n += layer.Size;
        }

        // Draw software mouse cursor if requested
        if(g.IO.MouseDrawCursor) {
            const ImGuiMouseCursorData& cursor_data = g.MouseCursorData[g.MouseCursor];
            const ImVec2 pos = g.IO.MousePos - cursor_data.HotOffset;
            const ImVec2 size = cursor_data.Size;
            const ImTextureID tex_id = g.IO.Fonts->TexID;
            g.OverlayDrawList.PushTextureID(tex_id);
            g.OverlayDrawList.AddImage(tex_id, pos + ImVec2(1, 0), pos + ImVec2(1, 0) + size, cursor_data.TexUvMin[1], cursor_data.TexUvMax[1], IM_COL32(0, 0, 0, 48));        // Shadow
            g.OverlayDrawList.AddImage(tex_id, pos + ImVec2(2, 0), pos + ImVec2(2, 0) + size, cursor_data.TexUvMin[1], cursor_data.TexUvMax[1], IM_COL32(0, 0, 0, 48));        // Shadow
            g.OverlayDrawList.AddImage(tex_id, pos, pos + size, cursor_data.TexUvMin[1], cursor_data.TexUvMax[1], IM_COL32(0, 0, 0, 255));       // Black border
            g.OverlayDrawList.AddImage(tex_id, pos, pos + size, cursor_data.TexUvMin[0], cursor_data.TexUvMax[0], IM_COL32(255, 255, 255, 255)); // White fill
            g.OverlayDrawList.PopTextureID();
        }
        if(!g.OverlayDrawList.VtxBuffer.empty())
            AddDrawListToRenderList(g.RenderDrawLists[0], &g.OverlayDrawList);

        // Setup draw data
        g.RenderDrawData.Valid = true;
        g.RenderDrawData.CmdLists = (g.RenderDrawLists[0].Size > 0) ? &g.RenderDrawLists[0][0] : NULL;
        g.RenderDrawData.CmdListsCount = g.RenderDrawLists[0].Size;
        g.RenderDrawData.TotalVtxCount = g.IO.MetricsRenderVertices;
        g.RenderDrawData.TotalIdxCount = g.IO.MetricsRenderIndices;

        // Render. If user hasn't Set a callback then they may retrieve the draw data via GetDrawData()
        if(g.RenderDrawData.CmdListsCount > 0 && g.IO.RenderDrawListsFn != NULL)
            g.IO.RenderDrawListsFn(&g.RenderDrawData);
    }
}
void ImGui::RenderText(ImVec2 pos, const char* text, const char* text_end, bool hide_text_after_hash)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    // Hide anything after a '##' string
    const char* text_display_end;
    if(hide_text_after_hash) {
        text_display_end = FindRenderedTextEnd(text, text_end);
    } else {
        if(!text_end)
            text_end = text + strlen(text); // FIXME-OPT
        text_display_end = text_end;
    }

    const int text_len = (int)(text_display_end - text);
    if(text_len > 0) {
        window->DrawList->AddText(g.Font, g.FontSize, pos, GetColorU32(ImGuiCol_Text), text, text_display_end);
    }
}
void ImGui::RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    if(!text_end)
        text_end = text + strlen(text); // FIXME-OPT

    const int text_len = (int)(text_end - text);
    if(text_len > 0) {
        window->DrawList->AddText(g.Font, g.FontSize, pos, GetColorU32(ImGuiCol_Text), text, text_end, wrap_width);
    }
}
void ImGui::RenderTextClipped(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, ImU32 col, const ImVec2& align, const ImRect* clip_rect)
{
    // Hide anything after a '##' string
    const char* text_display_end = FindRenderedTextEnd(text, text_end);
    const int text_len = (int)(text_display_end - text);
    if(text_len == 0)
        return;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    // Perform CPU side clipping for single clipped element to avoid using scissor state
    ImVec2 pos = pos_min;
    const ImVec2 text_size = text_size_if_known ? *text_size_if_known : CalcTextSize(text, text_display_end, false, 0.0f);

    const ImVec2* clip_min = clip_rect ? &clip_rect->Min : &pos_min;
    const ImVec2* clip_max = clip_rect ? &clip_rect->Max : &pos_max;
    bool need_clipping = (pos.x + text_size.x >= clip_max->x) || (pos.y + text_size.y >= clip_max->y);
    if(clip_rect) // If we had no explicit clipping rectangle then pos==clip_min
        need_clipping |= (pos.x < clip_min->x) || (pos.y < clip_min->y);

    // Align whole block. We should defer that to the better rendering function when we'll have support for individual line alignment.
    if(align.x > 0.0f) pos.x = ImMax(pos.x, pos.x + (pos_max.x - pos.x - text_size.x) * align.x);
    if(align.y > 0.0f) pos.y = ImMax(pos.y, pos.y + (pos_max.y - pos.y - text_size.y) * align.y);

    // Render
    if(need_clipping) {
        ImVec4 fine_clip_rect(clip_min->x, clip_min->y, clip_max->x, clip_max->y);
        window->DrawList->AddText(g.Font, g.FontSize, pos, col, text, text_display_end, 0.0f, &fine_clip_rect);
    } else {
        window->DrawList->AddText(g.Font, g.FontSize, pos, col, text, text_display_end, 0.0f, NULL);
    }
}
void ImGui::RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding)
{
    ImGuiWindow* window = GetCurrentWindow();

    window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding);
    if(border && (window->Flags & ImGuiWindowFlags_ShowBorders)) {
        window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow), rounding);
        window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border), rounding);
    }
}
void ImGui::RenderCollapseTriangle(ImVec2 p_min, bool is_open, float scale)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    const float h = g.FontSize * 1.00f;
    const float r = h * 0.40f * scale;
    ImVec2 center = p_min + ImVec2(h*0.50f, h*0.50f*scale);

    ImVec2 a, b, c;
    if(is_open) {
        center.y -= r*0.25f;
        a = center + ImVec2(0, 1)*r;
        b = center + ImVec2(-0.866f, -0.5f)*r;
        c = center + ImVec2(0.866f, -0.5f)*r;
    } else {
        a = center + ImVec2(1, 0)*r;
        b = center + ImVec2(-0.500f, 0.866f)*r;
        c = center + ImVec2(-0.500f, -0.866f)*r;
    }

    window->DrawList->AddTriangleFilled(a, b, c, GetColorU32(ImGuiCol_Text));
}
void ImGui::RenderBullet(ImVec2 pos)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DrawList->AddCircleFilled(pos, GImGui->FontSize*0.20f, GetColorU32(ImGuiCol_Text), 8);
}
void ImGui::RenderCheckMark(ImVec2 pos, ImU32 col)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    ImVec2 a, b, c;
    float start_x = (float)(int)(g.FontSize * 0.307f + 0.5f);
    float rem_third = (float)(int)((g.FontSize - start_x) / 3.0f);
    a.x = pos.x + 0.5f + start_x;
    b.x = a.x + rem_third;
    c.x = a.x + rem_third * 3.0f;
    b.y = pos.y - 1.0f + (float)(int)(g.Font->Ascent * (g.FontSize / g.Font->FontSize) + 0.5f) + (float)(int)(g.Font->DisplayOffset.y);
    a.y = b.y - rem_third;
    c.y = b.y - rem_third * 2.0f;

    window->DrawList->PathLineTo(a);
    window->DrawList->PathLineTo(b);
    window->DrawList->PathLineTo(c);
    window->DrawList->PathStroke(col, false);
}
void ImGui::CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end)
{
    ImGuiWindow* window = GetCurrentWindowRead();

    if(window->SkipItems) {
        *out_items_display_start = *out_items_display_end = 0;
        return;
    }

    const ImVec2 pos = window->DC.CursorPos;
    int start = (int)((window->ClipRect.Min.y - pos.y) / items_height);
    int end = (int)((window->ClipRect.Max.y - pos.y) / items_height);
    start = ImClamp(start, 0, items_count);
    end = ImClamp(end + 1, start, items_count);
    *out_items_display_start = start;
    *out_items_display_end = end;
}
void ImGui::ResetMouseDragDelta(int button)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    // NB: We don't need to reset g.IO.MouseDragMaxDistanceSqr
    g.IO.MouseClickedPos[button] = g.IO.MousePos;
}
void ImGui::SetMouseCursor(ImGuiMouseCursor cursor_type)
{
    GImGui->MouseCursor = cursor_type;
}
void ImGui::CaptureKeyboardFromApp(bool capture)
{
    GImGui->CaptureKeyboardNextFrame = capture ? 1 : 0;
}
void ImGui::CaptureMouseFromApp(bool capture)
{
    GImGui->CaptureMouseNextFrame = capture ? 1 : 0;
}
void ImGui::SetItemAllowOverlap()
{
    ImGuiContext& g = *GImGui;
    if(g.HoveredId == g.CurrentWindow->DC.LastItemId)
        g.HoveredIdAllowOverlap = true;
    if(g.ActiveId == g.CurrentWindow->DC.LastItemId)
        g.ActiveIdAllowOverlap = true;
}
void ImGui::SetTooltipV(const char* fmt, va_list args)
{
    ImGuiContext& g = *GImGui;
    ImFormatStringV(g.Tooltip, IM_ARRAYSIZE(g.Tooltip), fmt, args);
}
void ImGui::SetTooltip(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    SetTooltipV(fmt, args);
    va_end(args);
}
void ImGui::BeginTooltip()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::Begin("##Tooltip", NULL, flags);
}
void ImGui::EndTooltip()
{
    IM_ASSERT(GetCurrentWindowRead()->Flags & ImGuiWindowFlags_Tooltip);   // Mismatched BeginTooltip()/EndTooltip() calls
    ImGui::End();
}
void ImGui::OpenPopupEx(const char* str_id, bool reopen_existing)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID(str_id);
    int current_stack_size = g.CurrentPopupStack.Size;
    ImGuiPopupRef popup_ref = ImGuiPopupRef(id, window, window->GetID("##menus"), g.IO.MousePos); // Tagged as new ref because constructor sets Window to NULL (we are passing the ParentWindow info here)
    if(g.OpenPopupStack.Size < current_stack_size + 1)
        g.OpenPopupStack.push_back(popup_ref);
    else if(reopen_existing || g.OpenPopupStack[current_stack_size].PopupId != id) {
        g.OpenPopupStack.resize(current_stack_size + 1);
        g.OpenPopupStack[current_stack_size] = popup_ref;
    }
}
void ImGui::OpenPopup(const char* str_id)
{
    ImGui::OpenPopupEx(str_id, false);
}
static void CloseInactivePopups()
{
    ImGuiContext& g = *GImGui;
    if(g.OpenPopupStack.empty())
        return;

    // When popups are stacked, clicking on a lower level popups puts focus back to it and close popups above it.
    // Don't close our own child popup windows
    int n = 0;
    if(g.FocusedWindow) {
        for(n = 0; n < g.OpenPopupStack.Size; n++) {
            ImGuiPopupRef& popup = g.OpenPopupStack[n];
            if(!popup.Window)
                continue;
            IM_ASSERT((popup.Window->Flags & ImGuiWindowFlags_Popup) != 0);
            if(popup.Window->Flags & ImGuiWindowFlags_ChildWindow)
                continue;

            bool has_focus = false;
            for(int m = n; m < g.OpenPopupStack.Size && !has_focus; m++)
                has_focus = (g.OpenPopupStack[m].Window && g.OpenPopupStack[m].Window->RootWindow == g.FocusedWindow->RootWindow);
            if(!has_focus)
                break;
        }
    }
    if(n < g.OpenPopupStack.Size)   // This test is not required but it allows to Set a useful breakpoint on the line below
        g.OpenPopupStack.resize(n);
}
static void ClosePopupToLevel(int remaining)
{
    ImGuiContext& g = *GImGui;
    if(remaining > 0)
        ImGui::FocusWindow(g.OpenPopupStack[remaining - 1].Window);
    else
        ImGui::FocusWindow(g.OpenPopupStack[0].ParentWindow);
    g.OpenPopupStack.resize(remaining);
}
static void ClosePopup(ImGuiID id)
{
    if(!IsPopupOpen(id))
        return;
    ImGuiContext& g = *GImGui;
    ClosePopupToLevel(g.OpenPopupStack.Size - 1);
}
void ImGui::CloseCurrentPopup()
{
    ImGuiContext& g = *GImGui;
    int popup_idx = g.CurrentPopupStack.Size - 1;
    if(popup_idx < 0 || popup_idx > g.OpenPopupStack.Size || g.CurrentPopupStack[popup_idx].PopupId != g.OpenPopupStack[popup_idx].PopupId)
        return;
    while(popup_idx > 0 && g.OpenPopupStack[popup_idx].Window && (g.OpenPopupStack[popup_idx].Window->Flags & ImGuiWindowFlags_ChildMenu))
        popup_idx--;
    ClosePopupToLevel(popup_idx);
}
static inline void ClearSetNextWindowData()
{
    ImGuiContext& g = *GImGui;
    g.SetNextWindowPosCond = g.SetNextWindowSizeCond = g.SetNextWindowContentSizeCond = g.SetNextWindowCollapsedCond = 0;
    g.SetNextWindowSizeConstraint = g.SetNextWindowFocus = false;
}
void ImGui::EndPopup()
{
    ImGuiWindow* window = GetCurrentWindow();
    IM_ASSERT(window->Flags & ImGuiWindowFlags_Popup);  // Mismatched BeginPopup()/EndPopup() calls
    IM_ASSERT(GImGui->CurrentPopupStack.Size > 0);
    ImGui::End();
    if(!(window->Flags & ImGuiWindowFlags_Modal))
        ImGui::PopStyleVar();
}
void ImGui::EndGroupBox()
{
    EndChild();
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPosPrevLine.y -= GImGui->FontSize / 2;
}
void ImGui::EndChild()
{
    ImGuiWindow* window = GetCurrentWindow();

    IM_ASSERT(window->Flags & ImGuiWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() callss
    if((window->Flags & ImGuiWindowFlags_ComboBox) || window->BeginCount > 1) {
        ImGui::End();
    } else {
        // When using auto-filling child window, we don't provide full width/height to ItemSize so that it doesn't feed back into automatic size-fitting.
        ImVec2 sz = GetWindowSize();
        if(window->Flags & ImGuiWindowFlags_ChildWindowAutoFitX) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
            sz.x = ImMax(4.0f, sz.x);
        if(window->Flags & ImGuiWindowFlags_ChildWindowAutoFitY)
            sz.y = ImMax(4.0f, sz.y);

        ImGui::End();

        window = GetCurrentWindow();
        ImRect bb(window->DC.CursorPos, window->DC.CursorPos + sz);
        ItemSize(sz);
        ItemAdd(bb, NULL);
    }
}
void ImGui::EndChildFrame()
{
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}
static void CheckStacksSize(ImGuiWindow* window, bool write)
{
    // NOT checking: DC.ItemWidth, DC.AllowKeyboardFocus, DC.ButtonRepeat, DC.TextWrapPos (per window) to allow user to conveniently push once and not pop (they are cleared on Begin)
    ImGuiContext& g = *GImGui;
    int* p_backup = &window->DC.StackSizesBackup[0];
    { int current = window->IDStack.Size;       if(write) *p_backup = current; else IM_ASSERT(*p_backup == current && "PushID/PopID Mismatch!");                   p_backup++; }    // User forgot PopID()
    { int current = window->DC.GroupStack.Size; if(write) *p_backup = current; else IM_ASSERT(*p_backup == current && "BeginGroup/EndGroup Mismatch!");            p_backup++; }    // User forgot EndGroup()
    { int current = g.CurrentPopupStack.Size;   if(write) *p_backup = current; else IM_ASSERT(*p_backup == current && "BeginMenu/EndMenu or BeginPopup/EndPopup Mismatch"); p_backup++; }// User forgot EndPopup()/EndMenu()
    { int current = g.ColorModifiers.Size;      if(write) *p_backup = current; else IM_ASSERT(*p_backup == current && "PushStyleColor/PopStyleColor Mismatch!");   p_backup++; }    // User forgot PopStyleColor()
    { int current = g.StyleModifiers.Size;      if(write) *p_backup = current; else IM_ASSERT(*p_backup == current && "PushStyleVar/PopStyleVar Mismatch!");       p_backup++; }    // User forgot PopStyleVar()
    { int current = g.FontStack.Size;           if(write) *p_backup = current; else IM_ASSERT(*p_backup == current && "PushFont/PopFont Mismatch!");               p_backup++; }    // User forgot PopFont()
    IM_ASSERT(p_backup == window->DC.StackSizesBackup + IM_ARRAYSIZE(window->DC.StackSizesBackup));
}
static void ApplySizeFullWithConstraint(ImGuiWindow* window, ImVec2 new_size)
{
    ImGuiContext& g = *GImGui;
    if(g.SetNextWindowSizeConstraint) {
        // Using -1,-1 on either X/Y axis to preserve the current size.
        ImRect cr = g.SetNextWindowSizeConstraintRect;
        new_size.x = (cr.Min.x >= 0 && cr.Max.x >= 0) ? ImClamp(new_size.x, cr.Min.x, cr.Max.x) : window->SizeFull.x;
        new_size.y = (cr.Min.y >= 0 && cr.Max.y >= 0) ? ImClamp(new_size.y, cr.Min.y, cr.Max.y) : window->SizeFull.y;
        if(g.SetNextWindowSizeConstraintCallback) {
            ImGuiSizeConstraintCallbackData data;
            data.UserData = g.SetNextWindowSizeConstraintCallbackUserData;
            data.Pos = window->Pos;
            data.CurrentSize = window->SizeFull;
            data.DesiredSize = new_size;
            g.SetNextWindowSizeConstraintCallback(&data);
            new_size = data.DesiredSize;
        }
    }
    if(!(window->Flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_AlwaysAutoResize)))
        new_size = ImMax(new_size, g.Style->WindowMinSize);
    window->SizeFull = new_size;
}
            if(!window_pos_set_by_api && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && g.IO.DisplaySize.x > 0.0f && g.IO.DisplaySize.y > 0.0f) // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
            {
                ImVec2 padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
                window->PosFloat = ImMax(window->PosFloat + window->Size, padding) - window->Size;
                window->PosFloat = ImMin(window->PosFloat, g.IO.DisplaySize - padding);
            }
void ImGui::End()
{
static void Scrollbar(ImGuiWindow* window, bool horizontal)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = *g.Style;
    const ImGuiID id = window->GetID(horizontal ? "#SCROLLX" : "#SCROLLY");

    // Render background
    bool other_scrollbar = (horizontal ? window->ScrollbarY : window->ScrollbarX);
    float other_scrollbar_size_w = other_scrollbar ? style.ScrollbarSize : 0.0f;
    const ImRect window_rect = window->Rect();
    const float border_size = window->BorderSize;
    ImRect bb = horizontal
        ? ImRect(window->Pos.x + border_size, window_rect.Max.y - style.ScrollbarSize, window_rect.Max.x - other_scrollbar_size_w - border_size, window_rect.Max.y - border_size)
        : ImRect(window_rect.Max.x - style.ScrollbarSize, window->Pos.y + border_size, window_rect.Max.x - border_size, window_rect.Max.y - other_scrollbar_size_w - border_size);
    if(!horizontal)
        bb.Min.y += window->TitleBarHeight() + ((window->Flags & ImGuiWindowFlags_MenuBar) ? window->MenuBarHeight() : 0.0f);

    float window_rounding = (window->Flags & ImGuiWindowFlags_ChildWindow) ? style.ChildWindowRounding : style.WindowRounding;
    int window_rounding_corners;
    if(horizontal)
        window_rounding_corners = ImGuiCorner_BottomLeft | (other_scrollbar ? 0 : ImGuiCorner_BottomRight);
    else
        window_rounding_corners = (((window->Flags & ImGuiWindowFlags_NoTitleBar) && !(window->Flags & ImGuiWindowFlags_MenuBar)) ? ImGuiCorner_TopRight : 0) | (other_scrollbar ? 0 : ImGuiCorner_BottomRight);
    window->DrawList->AddRectFilled(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_ScrollbarBg), window_rounding, window_rounding_corners);
    bb.Reduce(ImVec2(ImClamp((float)(int)((bb.Max.x - bb.Min.x - 2.0f) * 0.5f), 0.0f, 3.0f), ImClamp((float)(int)((bb.Max.y - bb.Min.y - 2.0f) * 0.5f), 0.0f, 3.0f)));

    // V denote the main axis of the scrollbar
    float scrollbar_size_v = horizontal ? bb.GetWidth() : bb.GetHeight();
    float scroll_v = horizontal ? window->Scroll.x : window->Scroll.y;
    float win_size_avail_v = (horizontal ? window->Size.x : window->Size.y) - other_scrollbar_size_w;
    float win_size_contents_v = horizontal ? window->SizeContents.x : window->SizeContents.y;

    // The grabable box size generally represent the amount visible (vs the total scrollable amount)
    // But we maintain a minimum size in pixel to allow for the user to still aim inside.
    const float grab_h_pixels = ImMin(ImMax(scrollbar_size_v * ImSaturate(win_size_avail_v / ImMax(win_size_contents_v, win_size_avail_v)), style.GrabMinSize), scrollbar_size_v);
    const float grab_h_norm = grab_h_pixels / scrollbar_size_v;

    // Handle input right away. None of the code of Begin() is relying on scrolling position before calling Scrollbar().
    bool held = false;
    bool hovered = false;
    const bool previously_held = (g.ActiveId == id);
    ImGui::ButtonBehavior(bb, id, &hovered, &held);

    float scroll_max = ImMax(1.0f, win_size_contents_v - win_size_avail_v);
    float scroll_ratio = ImSaturate(scroll_v / scroll_max);
    float grab_v_norm = scroll_ratio * (scrollbar_size_v - grab_h_pixels) / scrollbar_size_v;
    if(held && grab_h_norm < 1.0f) {
        float scrollbar_pos_v = horizontal ? bb.Min.x : bb.Min.y;
        float mouse_pos_v = horizontal ? g.IO.MousePos.x : g.IO.MousePos.y;
        float* click_delta_to_grab_center_v = horizontal ? &g.ScrollbarClickDeltaToGrabCenter.x : &g.ScrollbarClickDeltaToGrabCenter.y;

        // Click position in scrollbar normalized space (0.0f->1.0f)
        const float clicked_v_norm = ImSaturate((mouse_pos_v - scrollbar_pos_v) / scrollbar_size_v);
        ImGui::SetHoveredID(id);

        bool seek_absolute = false;
        if(!previously_held) {
            // On initial click calculate the distance between mouse and the center of the grab
            if(clicked_v_norm >= grab_v_norm && clicked_v_norm <= grab_v_norm + grab_h_norm) {
                *click_delta_to_grab_center_v = clicked_v_norm - grab_v_norm - grab_h_norm*0.5f;
            } else {
                seek_absolute = true;
                *click_delta_to_grab_center_v = 0.0f;
            }
        }

        // Apply scroll
        // It is ok to modify Scroll here because we are being called in Begin() after the calculation of SizeContents and before setting up our starting position
        const float scroll_v_norm = ImSaturate((clicked_v_norm - *click_delta_to_grab_center_v - grab_h_norm*0.5f) / (1.0f - grab_h_norm));
        scroll_v = (float)(int)(0.5f + scroll_v_norm * scroll_max);//(win_size_contents_v - win_size_v));
        if(horizontal)
            window->Scroll.x = scroll_v;
        else
            window->Scroll.y = scroll_v;

        // Update values for rendering
        scroll_ratio = ImSaturate(scroll_v / scroll_max);
        grab_v_norm = scroll_ratio * (scrollbar_size_v - grab_h_pixels) / scrollbar_size_v;

        // Update distance to grab now that we have seeked and saturated
        if(seek_absolute)
            *click_delta_to_grab_center_v = clicked_v_norm - grab_v_norm - grab_h_norm*0.5f;
    }

    // Render
    const ImU32 grab_col = ImGui::GetColorU32(held ? ImGuiCol_ScrollbarGrabActive : hovered ? ImGuiCol_ScrollbarGrabHovered : ImGuiCol_ScrollbarGrab);
    if(horizontal)
        window->DrawList->AddRectFilled(ImVec2(ImLerp(bb.Min.x, bb.Max.x, grab_v_norm), bb.Min.y), ImVec2(ImLerp(bb.Min.x, bb.Max.x, grab_v_norm) + grab_h_pixels, bb.Max.y), grab_col, style.ScrollbarRounding);
    else
        window->DrawList->AddRectFilled(ImVec2(bb.Min.x, ImLerp(bb.Min.y, bb.Max.y, grab_v_norm)), ImVec2(bb.Max.x, ImLerp(bb.Min.y, bb.Max.y, grab_v_norm) + grab_h_pixels), grab_col, style.ScrollbarRounding);
}
void ImGui::FocusWindow(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;

    // Always mark the window we passed as focused. This is used for keyboard interactions such as tabbing.
    g.FocusedWindow = window;

    // Passing NULL allow to disable keyboard focus
    if(!window)
        return;

    // And move its root window to the top of the pile
    if(window->RootWindow)
        window = window->RootWindow;

    // Steal focus on active widgets
    if(window->Flags & ImGuiWindowFlags_Popup) // FIXME: This statement should be unnecessary. Need further testing before removing it..
        if(g.ActiveId != 0 && g.ActiveIdWindow && g.ActiveIdWindow->RootWindow != window)
            ClearActiveID();

    // Bring to front
    if((window->Flags & ImGuiWindowFlags_NoBringToFrontOnFocus) || g.Windows.back() == window)
        return;
    for(int i = 0; i < g.Windows.Size; i++)
        if(g.Windows[i] == window) {
            g.Windows.erase(g.Windows.begin() + i);
            break;
        }
    g.Windows.push_back(window);
}
void ImGui::PushItemWidth(float item_width)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ItemWidth = (item_width == 0.0f ? window->ItemWidthDefault : item_width);
    window->DC.ItemWidthStack.push_back(window->DC.ItemWidth);
}
static void PushMultiItemsWidths(int components, float w_full)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiStyle& style = ImGui::GetStyle();
    if(w_full <= 0.0f)
        w_full = ImGui::CalcItemWidth();
    const float w_item_one = ImMax(1.0f, (float)(int)((w_full - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
    const float w_item_last = ImMax(1.0f, (float)(int)(w_full - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));
    window->DC.ItemWidthStack.push_back(w_item_last);
    for(int i = 0; i < components - 1; i++)
        window->DC.ItemWidthStack.push_back(w_item_one);
    window->DC.ItemWidth = window->DC.ItemWidthStack.back();
}
void ImGui::PopItemWidth()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ItemWidthStack.pop_back();
    window->DC.ItemWidth = window->DC.ItemWidthStack.empty() ? window->ItemWidthDefault : window->DC.ItemWidthStack.back();
}
static void SetCurrentFont(ImFont* font)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(font && font->IsLoaded());    // Font Atlas not created. Did you call io.Fonts->GetTexDataAsRGBA32 / GetTexDataAsAlpha8 ?
    IM_ASSERT(font->Scale > 0.0f);
    g.Font = font;
    g.FontBaseSize = g.IO.FontGlobalScale * g.Font->FontSize * g.Font->Scale;
    g.FontSize = g.CurrentWindow ? g.CurrentWindow->CalcFontSize() : 0.0f;
    g.FontTexUvWhitePixel = g.Font->ContainerAtlas->TexUvWhitePixel;
}
void ImGui::PushFont(ImFont* font)
{
    ImGuiContext& g = *GImGui;
    if(!font)
        font = GetDefaultFont();
    SetCurrentFont(font);
    g.FontStack.push_back(font);
    g.CurrentWindow->DrawList->PushTextureID(font->ContainerAtlas->TexID);
}
void  ImGui::PopFont()
{
    ImGuiContext& g = *GImGui;
    g.CurrentWindow->DrawList->PopTextureID();
    g.FontStack.pop_back();
    SetCurrentFont(g.FontStack.empty() ? GetDefaultFont() : g.FontStack.back());
}
void ImGui::PushAllowKeyboardFocus(bool allow_keyboard_focus)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.AllowKeyboardFocus = allow_keyboard_focus;
    window->DC.AllowKeyboardFocusStack.push_back(allow_keyboard_focus);
}
void ImGui::PopAllowKeyboardFocus()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.AllowKeyboardFocusStack.pop_back();
    window->DC.AllowKeyboardFocus = window->DC.AllowKeyboardFocusStack.empty() ? true : window->DC.AllowKeyboardFocusStack.back();
}
void ImGui::PushButtonRepeat(bool repeat)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ButtonRepeat = repeat;
    window->DC.ButtonRepeatStack.push_back(repeat);
}
void ImGui::PopButtonRepeat()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ButtonRepeatStack.pop_back();
    window->DC.ButtonRepeat = window->DC.ButtonRepeatStack.empty() ? false : window->DC.ButtonRepeatStack.back();
}
void ImGui::PushTextWrapPos(float wrap_pos_x)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.TextWrapPos = wrap_pos_x;
    window->DC.TextWrapPosStack.push_back(wrap_pos_x);
}
void ImGui::PopTextWrapPos()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.TextWrapPosStack.pop_back();
    window->DC.TextWrapPos = window->DC.TextWrapPosStack.empty() ? -1.0f : window->DC.TextWrapPosStack.back();
}
void ImGui::PushStyle(const ImGuiStyle& style)
{
    ImGuiContext& g = *GImGui;
    g.StyleStack.push_back(style);
    g.Style = &(g.StyleStack.back());
}
void ImGui::PopStyle()
{
    ImGuiContext& g = *GImGui;
    g.StyleStack.pop_back();

    IM_ASSERT(g.StyleStack.Size > 0);

    g.Style = &(g.StyleStack.back());
}
void ImGui::PushStyleColor(ImGuiCol idx, const ImVec4& col)
{
    ImGuiContext& g = *GImGui;
    ImGuiColMod backup;
    backup.Col = idx;
    backup.BackupValue = g.Style->Colors[idx];
    g.ColorModifiers.push_back(backup);
    g.Style->Colors[idx] = col;
}
void ImGui::PopStyleColor(int count)
{
    ImGuiContext& g = *GImGui;
    while(count > 0) {
        ImGuiColMod& backup = g.ColorModifiers.back();
        g.Style->Colors[backup.Col] = backup.BackupValue;
        g.ColorModifiers.pop_back();
        count--;
    }
}
void ImGui::PushStyleVar(ImGuiStyleVar idx, float val)
{
    const ImGuiStyleVarInfo* var_info = GetStyleVarInfo(idx);
    if(var_info->Type == ImGuiDataType_Float) {
        float* pvar = (float*)var_info->GetVarPtr();
        GImGui->StyleModifiers.push_back(ImGuiStyleMod(idx, *pvar));
        *pvar = val;
        return;
    }
    IM_ASSERT(0); // Called function with wrong-type? Variable is not a float.
}
void ImGui::PushStyleVar(ImGuiStyleVar idx, const ImVec2& val)
{
    const ImGuiStyleVarInfo* var_info = GetStyleVarInfo(idx);
    if(var_info->Type == ImGuiDataType_Float2) {
        ImVec2* pvar = (ImVec2*)var_info->GetVarPtr();
        GImGui->StyleModifiers.push_back(ImGuiStyleMod(idx, *pvar));
        *pvar = val;
        return;
    }
    IM_ASSERT(0); // Called function with wrong-type? Variable is not a ImVec2.
}
void ImGui::PopStyleVar(int count)
{
    ImGuiContext& g = *GImGui;
    while(count > 0) {
        ImGuiStyleMod& backup = g.StyleModifiers.back();
        const ImGuiStyleVarInfo* info = GetStyleVarInfo(backup.VarIdx);
        if(info->Type == ImGuiDataType_Float)          (*(float*)info->GetVarPtr()) = backup.BackupFloat[0];
        else if(info->Type == ImGuiDataType_Float2)    (*(ImVec2*)info->GetVarPtr()) = ImVec2(backup.BackupFloat[0], backup.BackupFloat[1]);
        else if(info->Type == ImGuiDataType_Int)       (*(int*)info->GetVarPtr()) = backup.BackupInt[0];
        g.StyleModifiers.pop_back();
        count--;
    }
}
static void SetWindowScrollY(ImGuiWindow* window, float new_scroll_y)
{
    window->DC.CursorMaxPos.y += window->Scroll.y;
    window->Scroll.y = new_scroll_y;
    window->DC.CursorMaxPos.y -= window->Scroll.y;
}
static void SetWindowPos(ImGuiWindow* window, const ImVec2& pos, ImGuiSetCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if(cond && (window->SetWindowPosAllowFlags & cond) == 0)
        return;
    window->SetWindowPosAllowFlags &= ~(ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing);
    window->SetWindowPosCenterWanted = false;

    // Set
    const ImVec2 old_pos = window->Pos;
    window->PosFloat = pos;
    window->Pos = ImVec2((float)(int)window->PosFloat.x, (float)(int)window->PosFloat.y);
    window->DC.CursorPos += (window->Pos - old_pos);    // As we happen to move the window while it is being appended to (which is a bad idea - will smear) let's at least offset the cursor
    window->DC.CursorMaxPos += (window->Pos - old_pos); // And more importantly we need to adjust this so size calculation doesn't Get affected.
}
void ImGui::SetWindowPos(const ImVec2& pos, ImGuiSetCond cond)
{
    ImGuiWindow* window = GetCurrentWindowRead();
    SetWindowPos(window, pos, cond);
}
void ImGui::SetWindowPos(const char* name, const ImVec2& pos, ImGuiSetCond cond)
{
    if(ImGuiWindow* window = FindWindowByName(name))
        SetWindowPos(window, pos, cond);
}
static void SetWindowSize(ImGuiWindow* window, const ImVec2& size, ImGuiSetCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if(cond && (window->SetWindowSizeAllowFlags & cond) == 0)
        return;
    window->SetWindowSizeAllowFlags &= ~(ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing);

    // Set
    if(size.x > 0.0f) {
        window->AutoFitFramesX = 0;
        window->SizeFull.x = size.x;
    } else {
        window->AutoFitFramesX = 2;
        window->AutoFitOnlyGrows = false;
    }
    if(size.y > 0.0f) {
        window->AutoFitFramesY = 0;
        window->SizeFull.y = size.y;
    } else {
        window->AutoFitFramesY = 2;
        window->AutoFitOnlyGrows = false;
    }
}
void ImGui::SetWindowSize(const ImVec2& size, ImGuiSetCond cond)
{
    SetWindowSize(GImGui->CurrentWindow, size, cond);
}
void ImGui::SetWindowSize(const char* name, const ImVec2& size, ImGuiSetCond cond)
{
    ImGuiWindow* window = FindWindowByName(name);
    if(window)
        SetWindowSize(window, size, cond);
}
static void SetWindowCollapsed(ImGuiWindow* window, bool collapsed, ImGuiSetCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if(cond && (window->SetWindowCollapsedAllowFlags & cond) == 0)
        return;
    window->SetWindowCollapsedAllowFlags &= ~(ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing);

    // Set
    window->Collapsed = collapsed;
}
void ImGui::SetWindowCollapsed(bool collapsed, ImGuiSetCond cond)
{
    SetWindowCollapsed(GImGui->CurrentWindow, collapsed, cond);
}
void ImGui::SetWindowCollapsed(const char* name, bool collapsed, ImGuiSetCond cond)
{
    ImGuiWindow* window = FindWindowByName(name);
    if(window)
        SetWindowCollapsed(window, collapsed, cond);
}
void ImGui::SetWindowFocus()
{
    FocusWindow(GImGui->CurrentWindow);
}
void ImGui::SetWindowFocus(const char* name)
{
    if(name) {
        if(ImGuiWindow* window = FindWindowByName(name))
            FocusWindow(window);
    } else {
        FocusWindow(NULL);
    }
}
void ImGui::SetNextWindowPos(const ImVec2& pos, ImGuiSetCond cond)
{
    ImGuiContext& g = *GImGui;
    g.SetNextWindowPosVal = pos;
    g.SetNextWindowPosCond = cond ? cond : ImGuiSetCond_Always;
}
void ImGui::SetNextWindowPosCenter(ImGuiSetCond cond)
{
    ImGuiContext& g = *GImGui;
    g.SetNextWindowPosVal = ImVec2(-FLT_MAX, -FLT_MAX);
    g.SetNextWindowPosCond = cond ? cond : ImGuiSetCond_Always;
}
void ImGui::SetNextWindowSize(const ImVec2& size, ImGuiSetCond cond)
{
    ImGuiContext& g = *GImGui;
    g.SetNextWindowSizeVal = size;
    g.SetNextWindowSizeCond = cond ? cond : ImGuiSetCond_Always;
}
void ImGui::SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, ImGuiSizeConstraintCallback custom_callback, void* custom_callback_user_data)
{
    ImGuiContext& g = *GImGui;
    g.SetNextWindowSizeConstraint = true;
    g.SetNextWindowSizeConstraintRect = ImRect(size_min, size_max);
    g.SetNextWindowSizeConstraintCallback = custom_callback;
    g.SetNextWindowSizeConstraintCallbackUserData = custom_callback_user_data;
}
void ImGui::SetNextWindowContentSize(const ImVec2& size)
{
    ImGuiContext& g = *GImGui;
    g.SetNextWindowContentSizeVal = size;
    g.SetNextWindowContentSizeCond = ImGuiSetCond_Always;
}
void ImGui::SetNextWindowContentWidth(float width)
{
    ImGuiContext& g = *GImGui;
    g.SetNextWindowContentSizeVal = ImVec2(width, g.SetNextWindowContentSizeCond ? g.SetNextWindowContentSizeVal.y : 0.0f);
    g.SetNextWindowContentSizeCond = ImGuiSetCond_Always;
}
void ImGui::SetNextWindowCollapsed(bool collapsed, ImGuiSetCond cond)
{
    ImGuiContext& g = *GImGui;
    g.SetNextWindowCollapsedVal = collapsed;
    g.SetNextWindowCollapsedCond = cond ? cond : ImGuiSetCond_Always;
}
void ImGui::SetNextWindowFocus()
{
    ImGuiContext& g = *GImGui;
    g.SetNextWindowFocus = true;
}
void ImGui::SetWindowFontScale(float scale)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    window->FontWindowScale = scale;
    g.FontSize = window->CalcFontSize();
}
void ImGui::SetCursorPos(const ImVec2& local_pos)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos = window->Pos - window->Scroll + local_pos;
    window->DC.CursorMaxPos = ImMax(window->DC.CursorMaxPos, window->DC.CursorPos);
}
void ImGui::SetCursorPosX(float x)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + x;
    window->DC.CursorMaxPos.x = ImMax(window->DC.CursorMaxPos.x, window->DC.CursorPos.x);
}
void ImGui::SetCursorPosY(float y)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos.y = window->Pos.y - window->Scroll.y + y;
    window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, window->DC.CursorPos.y);
}
void ImGui::SetCursorScreenPos(const ImVec2& screen_pos)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos = screen_pos;
    window->DC.CursorMaxPos = ImMax(window->DC.CursorMaxPos, window->DC.CursorPos);
}
void ImGui::SetScrollX(float scroll_x)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->ScrollTarget.x = scroll_x;
    window->ScrollTargetCenterRatio.x = 0.0f;
}
void ImGui::SetScrollY(float scroll_y)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->ScrollTarget.y = scroll_y + window->TitleBarHeight() + window->MenuBarHeight(); // title bar height canceled out when using ScrollTargetRelY
    window->ScrollTargetCenterRatio.y = 0.0f;
}
void ImGui::SetScrollFromPosY(float pos_y, float center_y_ratio)
{
    // We store a target position so centering can occur on the next frame when we are guaranteed to have a known window size
    ImGuiWindow* window = GetCurrentWindow();
    IM_ASSERT(center_y_ratio >= 0.0f && center_y_ratio <= 1.0f);
    window->ScrollTarget.y = (float)(int)(pos_y + window->Scroll.y);
    if(center_y_ratio <= 0.0f && window->ScrollTarget.y <= window->WindowPadding.y)    // Minor hack to make "scroll to top" take account of WindowPadding, else it would scroll to (WindowPadding.y - ItemSpacing.y)
        window->ScrollTarget.y = 0.0f;
    window->ScrollTargetCenterRatio.y = center_y_ratio;
}
void ImGui::SetScrollHere(float center_y_ratio)
{
    ImGuiWindow* window = GetCurrentWindow();
    float target_y = window->DC.CursorPosPrevLine.y + (window->DC.PrevLineHeight * center_y_ratio) + (ImGui::GetStyle().ItemSpacing.y * (center_y_ratio - 0.5f) * 2.0f); // Precisely aim above, in the middle or below the last line.
    SetScrollFromPosY(target_y - window->Pos.y, center_y_ratio);
}
void ImGui::SetKeyboardFocusHere(int offset)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->FocusIdxAllRequestNext = window->FocusIdxAllCounter + 1 + offset;
    window->FocusIdxTabRequestNext = INT_MAX;
}
void ImGui::SetStateStorage(ImGuiStorage* tree)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.StateStorage = tree ? tree : &window->StateStorage;
}
void ImGui::TextV(const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const char* text_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    TextUnformatted(g.TempBuffer, text_end);
}
void ImGui::Text(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextV(fmt, args);
    va_end(args);
}
void ImGui::TextColoredV(const ImVec4& col, const char* fmt, va_list args)
{
    PushStyleColor(ImGuiCol_Text, col);
    TextV(fmt, args);
    PopStyleColor();
}
void ImGui::TextColored(const ImVec4& col, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextColoredV(col, fmt, args);
    va_end(args);
}
void ImGui::TextDisabledV(const char* fmt, va_list args)
{
    PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
    TextV(fmt, args);
    PopStyleColor();
}
void ImGui::TextDisabled(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextDisabledV(fmt, args);
    va_end(args);
}
void ImGui::TextWrappedV(const char* fmt, va_list args)
{
    bool need_wrap = (GImGui->CurrentWindow->DC.TextWrapPos < 0.0f);    // Keep existing wrap position is one ia already Set
    if(need_wrap) PushTextWrapPos(0.0f);
    TextV(fmt, args);
    if(need_wrap) PopTextWrapPos();
}
void ImGui::TextWrapped(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextWrappedV(fmt, args);
    va_end(args);
}
void ImGui::TextUnformatted(const char* text, const char* text_end)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    IM_ASSERT(text != NULL);
    const char* text_begin = text;
    if(text_end == NULL)
        text_end = text + strlen(text); // FIXME-OPT

    const float wrap_pos_x = window->DC.TextWrapPos;
    const bool wrap_enabled = wrap_pos_x >= 0.0f;
    if(text_end - text > 2000 && !wrap_enabled) {
        // Long text!
        // Perform manual coarse clipping to optimize for long multi-line text
        // From this point we will only compute the width of lines that are visible. Optimization only available when word-wrapping is disabled.
        // We also don't vertically center the text within the line full height, which is unlikely to matter because we are likely the biggest and only item on the line.
        const char* line = text;
        const float line_height = GetTextLineHeight();
        const ImVec2 text_pos = window->DC.CursorPos + ImVec2(0.0f, window->DC.CurrentLineTextBaseOffset);
        const ImRect clip_rect = window->ClipRect;
        ImVec2 text_size(0, 0);

        if(text_pos.y <= clip_rect.Max.y) {
            ImVec2 pos = text_pos;

            // Lines to skip
            int lines_skippable = (int)((clip_rect.Min.y - text_pos.y) / line_height);
            if(lines_skippable > 0) {
                int lines_skipped = 0;
                while(line < text_end && lines_skipped < lines_skippable) {
                    const char* line_end = strchr(line, '\n');
                    if(!line_end)
                        line_end = text_end;
                    line = line_end + 1;
                    lines_skipped++;
                }
                pos.y += lines_skipped * line_height;
            }

            // Lines to Render
            if(line < text_end) {
                ImRect line_rect(pos, pos + ImVec2(FLT_MAX, line_height));
                while(line < text_end) {
                    const char* line_end = strchr(line, '\n');
                    if(IsClippedEx(line_rect, NULL, false))
                        break;

                    const ImVec2 line_size = CalcTextSize(line, line_end, false);
                    text_size.x = ImMax(text_size.x, line_size.x);
                    RenderText(pos, line, line_end, false);
                    if(!line_end)
                        line_end = text_end;
                    line = line_end + 1;
                    line_rect.Min.y += line_height;
                    line_rect.Max.y += line_height;
                    pos.y += line_height;
                }

                // Count remaining lines
                int lines_skipped = 0;
                while(line < text_end) {
                    const char* line_end = strchr(line, '\n');
                    if(!line_end)
                        line_end = text_end;
                    line = line_end + 1;
                    lines_skipped++;
                }
                pos.y += lines_skipped * line_height;
            }

            text_size.y += (pos - text_pos).y;
        }

        ImRect bb(text_pos, text_pos + text_size);
        ItemSize(bb);
        ItemAdd(bb, NULL);
    } else {
        const float wrap_width = wrap_enabled ? CalcWrapWidthForPos(window->DC.CursorPos, wrap_pos_x) : 0.0f;
        const ImVec2 text_size = CalcTextSize(text_begin, text_end, false, wrap_width);

        // Account of baseline offset
        ImVec2 text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrentLineTextBaseOffset);
        ImRect bb(text_pos, text_pos + text_size);
        ItemSize(text_size);
        if(!ItemAdd(bb, NULL))
            return;

        // Render (we don't Hide text after ## in this end-user function)
        RenderTextWrapped(bb.Min, text_begin, text_end, wrap_width);
    }
}
void ImGui::AlignFirstTextHeightToWidgets()
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    // Declare a dummy item size to that upcoming items that are smaller will center-align on the newly expanded line height.
    ImGuiContext& g = *GImGui;
    ItemSize(ImVec2(0, g.FontSize + g.Style->FramePadding.y * 2), g.Style->FramePadding.y);
    SameLine(0, 0);
}
void ImGui::LabelTextV(const char* label, const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = *g.Style;
    const float w = CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect value_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2));
    const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w + (label_size.x > 0.0f ? style.ItemInnerSpacing.x : 0.0f), style.FramePadding.y * 2) + label_size);
    ItemSize(total_bb, style.FramePadding.y);
    if(!ItemAdd(total_bb, NULL))
        return;

    // Render
    const char* value_text_begin = &g.TempBuffer[0];
    const char* value_text_end = value_text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    RenderTextClipped(value_bb.Min, value_bb.Max, value_text_begin, value_text_end, NULL, GetColorU32(ImGuiCol_Text), ImVec2(0.0f, 0.5f));
    if(label_size.x > 0.0f)
        RenderText(ImVec2(value_bb.Max.x + style.ItemInnerSpacing.x, value_bb.Min.y + style.FramePadding.y), label);
}
void ImGui::LabelText(const char* label, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LabelTextV(label, fmt, args);
    va_end(args);
}
void ImGui::Image(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    if(border_col.w > 0.0f)
        bb.Max += ImVec2(2, 2);
    ItemSize(bb);
    if(!ItemAdd(bb, NULL))
        return;

    if(border_col.w > 0.0f) {
        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(border_col), 0.0f);
        window->DrawList->AddImage(user_texture_id, bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1), uv0, uv1, GetColorU32(tint_col));
    } else {
        window->DrawList->AddImage(user_texture_id, bb.Min, bb.Max, uv0, uv1, GetColorU32(tint_col));
    }
}
        if(CloseButton(window->GetID((void*)(intptr_t)(id + 1)), ImVec2(ImMin(window->DC.LastItemRect.Max.x, window->ClipRect.Max.x) - g.Style->FramePadding.x - button_sz, window->DC.LastItemRect.Min.y + g.Style->FramePadding.y + button_sz), button_sz))
            *p_open = false;
    }

    return is_open;
}

bool ImGui::TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return false;

    return TreeNodeBehavior(window->GetID(label), flags, label, NULL);
}

bool ImGui::TreeNodeExV(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const char* label_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    return TreeNodeBehavior(window->GetID(str_id), flags, g.TempBuffer, label_end);
}

bool ImGui::TreeNodeExV(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const char* label_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    return TreeNodeBehavior(window->GetID(ptr_id), flags, g.TempBuffer, label_end);
}

bool ImGui::TreeNodeV(const char* str_id, const char* fmt, va_list args)
{
    return TreeNodeExV(str_id, 0, fmt, args);
}

bool ImGui::TreeNodeV(const void* ptr_id, const char* fmt, va_list args)
{
    return TreeNodeExV(ptr_id, 0, fmt, args);
}

bool ImGui::TreeNodeEx(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(str_id, flags, fmt, args);
    va_end(args);
    return is_open;
}

bool ImGui::TreeNodeEx(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(ptr_id, flags, fmt, args);
    va_end(args);
    return is_open;
}

bool ImGui::TreeNode(const char* str_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(str_id, 0, fmt, args);
    va_end(args);
    return is_open;
}

bool ImGui::TreeNode(const void* ptr_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(ptr_id, 0, fmt, args);
    va_end(args);
    return is_open;
}

bool ImGui::TreeNode(const char* label)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return false;
    return TreeNodeBehavior(window->GetID(label), 0, label, NULL);
}

void ImGui::TreeAdvanceToLabelPos()
{
    ImGuiContext& g = *GImGui;
    g.CurrentWindow->DC.CursorPos.x += GetTreeNodeToLabelSpacing();
}

// Horizontal distance preceding label when using TreeNode() or Bullet()
float ImGui::GetTreeNodeToLabelSpacing()
{
    ImGuiContext& g = *GImGui;
    return g.FontSize + (g.Style->FramePadding.x * 2.0f);
}

void ImGui::SetNextTreeNodeOpen(bool is_open, ImGuiSetCond cond)
{
    ImGuiContext& g = *GImGui;
    g.SetNextTreeNodeOpenVal = is_open;
    g.SetNextTreeNodeOpenCond = cond ? cond : ImGuiSetCond_Always;
}

void ImGui::PushID(const char* str_id)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->IDStack.push_back(window->GetID(str_id));
}

void ImGui::PushID(const char* str_id_begin, const char* str_id_end)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->IDStack.push_back(window->GetID(str_id_begin, str_id_end));
}

void ImGui::PushID(const void* ptr_id)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->IDStack.push_back(window->GetID(ptr_id));
}

void ImGui::PushID(int int_id)
{
    const void* ptr_id = (void*)(intptr_t)int_id;
    ImGuiWindow* window = GetCurrentWindow();
    window->IDStack.push_back(window->GetID(ptr_id));
}

void ImGui::PopID()
{
    ImGuiWindow* window = GetCurrentWindow();
    window->IDStack.pop_back();
}

ImGuiID ImGui::GetID(const char* str_id)
{
    return GImGui->CurrentWindow->GetID(str_id);
}

ImGuiID ImGui::GetID(const char* str_id_begin, const char* str_id_end)
{
    return GImGui->CurrentWindow->GetID(str_id_begin, str_id_end);
}

ImGuiID ImGui::GetID(const void* ptr_id)
{
    return GImGui->CurrentWindow->GetID(ptr_id);
}

void ImGui::Bullet()
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = *g.Style;
    const float line_height = ImMax(ImMin(window->DC.CurrentLineHeight, g.FontSize + g.Style->FramePadding.y * 2), g.FontSize);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(g.FontSize, line_height));
    ItemSize(bb);
    if(!ItemAdd(bb, NULL)) {
void ImGui::BulletTextV(const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = *g.Style;

    const char* text_begin = g.TempBuffer;
    const char* text_end = text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    const ImVec2 label_size = CalcTextSize(text_begin, text_end, false);
    const float text_base_offset_y = ImMax(0.0f, window->DC.CurrentLineTextBaseOffset); // Latch before ItemSize changes it
    const float line_height = ImMax(ImMin(window->DC.CurrentLineHeight, g.FontSize + g.Style->FramePadding.y * 2), g.FontSize);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(g.FontSize + (label_size.x > 0.0f ? (label_size.x + style.FramePadding.x * 2) : 0.0f), ImMax(line_height, label_size.y)));  // Empty text doesn't add padding
    ItemSize(bb);
    if(!ItemAdd(bb, NULL))
        return;

    // Render
    RenderBullet(bb.Min + ImVec2(style.FramePadding.x + g.FontSize*0.5f, line_height*0.5f));
    RenderText(bb.Min + ImVec2(g.FontSize + style.FramePadding.x * 2, text_base_offset_y), text_begin, text_end, false);
}
void ImGui::BulletText(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    BulletTextV(fmt, args);
    va_end(args);
}
static inline void DataTypeFormatString(ImGuiDataType data_type, void* data_ptr, const char* display_format, char* buf, int buf_size)
{
    if(data_type == ImGuiDataType_Int)
        ImFormatString(buf, buf_size, display_format, *(int*)data_ptr);
    else if(data_type == ImGuiDataType_Float)
        ImFormatString(buf, buf_size, display_format, *(float*)data_ptr);
}
static inline void DataTypeFormatString(ImGuiDataType data_type, void* data_ptr, int decimal_precision, char* buf, int buf_size)
{
    if(data_type == ImGuiDataType_Int) {
        if(decimal_precision < 0)
            ImFormatString(buf, buf_size, "%d", *(int*)data_ptr);
        else
            ImFormatString(buf, buf_size, "%.*d", decimal_precision, *(int*)data_ptr);
    } else if(data_type == ImGuiDataType_Float) {
        if(decimal_precision < 0)
            ImFormatString(buf, buf_size, "%f", *(float*)data_ptr);     // Ideally we'd have a minimum decimal precision of 1 to visually denote that it is a float, while hiding non-significant digits?
        else
            ImFormatString(buf, buf_size, "%.*f", decimal_precision, *(float*)data_ptr);
    }
}
static void DataTypeApplyOp(ImGuiDataType data_type, int op, void* value1, const void* value2)// Store into value1
{
    if(data_type == ImGuiDataType_Int) {
        if(op == '+')
            *(int*)value1 = *(int*)value1 + *(const int*)value2;
        else if(op == '-')
            *(int*)value1 = *(int*)value1 - *(const int*)value2;
    } else if(data_type == ImGuiDataType_Float) {
        if(op == '+')
            *(float*)value1 = *(float*)value1 + *(const float*)value2;
        else if(op == '-')
            *(float*)value1 = *(float*)value1 - *(const float*)value2;
    }
}
static bool DataTypeApplyOpFromText(const char* buf, const char* initial_value_buf, ImGuiDataType data_type, void* data_ptr, const char* scalar_format)
{
    while(ImCharIsSpace(*buf))
        buf++;

    // We don't support '-' op because it would conflict with inputing negative value.
    // Instead you can use +-100 to subtract from an existing value
    char op = buf[0];
    if(op == '+' || op == '*' || op == '/') {
        buf++;
        while(ImCharIsSpace(*buf))
            buf++;
    } else {
        op = 0;
    }
    if(!buf[0])
        return false;

    if(data_type == ImGuiDataType_Int) {
        if(!scalar_format)
            scalar_format = "%d";
        int* v = (int*)data_ptr;
        const int old_v = *v;
        int arg0 = *v;
        if(op && sscanf(initial_value_buf, scalar_format, &arg0) < 1)
            return false;

        // Store operand in a float so we can use fractional value for multipliers (*1.1), but constant always parsed as integer so we can fit big integers (e.g. 2000000003) past float precision
        float arg1 = 0.0f;
        if(op == '+') { if(sscanf(buf, "%f", &arg1) == 1) *v = (int)(arg0 + arg1); }                // Add (use "+-" to subtract)
        else if(op == '*') { if(sscanf(buf, "%f", &arg1) == 1) *v = (int)(arg0 * arg1); }                // Multiply
        else if(op == '/') { if(sscanf(buf, "%f", &arg1) == 1 && arg1 != 0.0f) *v = (int)(arg0 / arg1); }// Divide
        else { if(sscanf(buf, scalar_format, &arg0) == 1) *v = arg0; }                     // Assign constant
        return (old_v != *v);
    } else if(data_type == ImGuiDataType_Float) {
        // For floats we have to ignore format with precision (e.g. "%.2f") because sscanf doesn't take them in
        scalar_format = "%f";
        float* v = (float*)data_ptr;
        const float old_v = *v;
        float arg0 = *v;
        if(op && sscanf(initial_value_buf, scalar_format, &arg0) < 1)
            return false;

        float arg1 = 0.0f;
        if(sscanf(buf, scalar_format, &arg1) < 1)
            return false;
        if(op == '+') { *v = arg0 + arg1; }                    // Add (use "+-" to subtract)
        else if(op == '*') { *v = arg0 * arg1; }                    // Multiply
        else if(op == '/') { if(arg1 != 0.0f) *v = arg0 / arg1; }  // Divide
        else { *v = arg1; }                           // Assign constant
        return (old_v != *v);
    }

    return false;
}
bool ImGui::InputScalarAsWidgetReplacement(const ImRect& aabb, const char* label, ImGuiDataType data_type, void* data_ptr, ImGuiID id, int decimal_precision)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    // Our replacement widget will override the focus ID (registered previously to allow for a TAB focus to happen)
    SetActiveID(g.ScalarAsInputTextId, window);
    SetHoveredID(0);
    FocusableItemUnregister(window);

    char buf[32];
    auto label_size = CalcTextSize(label, NULL, true);
    DataTypeFormatString(data_type, data_ptr, decimal_precision, buf, IM_ARRAYSIZE(buf));
    bool text_value_changed = InputTextEx(label, buf, IM_ARRAYSIZE(buf), aabb.GetSize() + ImVec2{ label_size.x + g.Style->ItemInnerSpacing.x, 0.0f }, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll);
    if(g.ScalarAsInputTextId == 0) {
        // First frame
        //IM_ASSERT(g.ActiveId == id);    // InputText ID expected to match the Slider ID (else we'd need to store them both, which is also possible)
        g.ScalarAsInputTextId = g.ActiveId;
        SetHoveredID(id);
    } else if(g.ActiveId != g.ScalarAsInputTextId) {
        // Release
        g.ScalarAsInputTextId = 0;
    }
    if(text_value_changed)
        return DataTypeApplyOpFromText(buf, GImGui->InputTextState.InitialText.begin(), data_type, data_ptr, NULL);
    return false;
}
void ImGui::PlotEx(ImGuiPlotType plot_type, const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = *g.Style;

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    if(graph_size.x == 0.0f)
        graph_size.x = CalcItemWidth();
    if(graph_size.y == 0.0f)
        graph_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y));
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ItemSize(total_bb, style.FramePadding.y);
    if(!ItemAdd(total_bb, NULL))
        return;

    // Determine scale from values if not specified
    if(scale_min == FLT_MAX || scale_max == FLT_MAX) {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for(int i = 0; i < values_count; i++) {
            const float v = values_getter(data, i);
            v_min = ImMin(v_min, v);
            v_max = ImMax(v_max, v);
        }
        if(scale_min == FLT_MAX)
            scale_min = v_min;
        if(scale_max == FLT_MAX)
            scale_max = v_max;
    }

    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    if(values_count > 0) {
        int res_w = ImMin((int)graph_size.x, values_count) + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);
        int item_count = values_count + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);

        // Tooltip on hover
        int v_hovered = -1;
        if(IsHovered(inner_bb, 0)) {
            const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
            const int v_idx = (int)(t * item_count);
            IM_ASSERT(v_idx >= 0 && v_idx < values_count);

            const float v0 = values_getter(data, (v_idx + values_offset) % values_count);
            const float v1 = values_getter(data, (v_idx + 1 + values_offset) % values_count);
            if(plot_type == ImGuiPlotType_Lines)
                SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx + 1, v1);
            else if(plot_type == ImGuiPlotType_Histogram)
                SetTooltip("%d: %8.4g", v_idx, v0);
            v_hovered = v_idx;
        }

        const float t_step = 1.0f / (float)res_w;

        float v0 = values_getter(data, (0 + values_offset) % values_count);
        float t0 = 0.0f;
        ImVec2 tp0 = ImVec2(t0, 1.0f - ImSaturate((v0 - scale_min) / (scale_max - scale_min)));    // Point in the normalized space of our target rectangle

        const ImU32 col_base = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
        const ImU32 col_hovered = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);

        for(int n = 0; n < res_w; n++) {
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
            const float v1 = values_getter(data, (v1_idx + values_offset + 1) % values_count);
            const ImVec2 tp1 = ImVec2(t1, 1.0f - ImSaturate((v1 - scale_min) / (scale_max - scale_min)));

            // NB: Draw calls are merged together by the DrawList system. Still, we should Render our batch are lower level to save a bit of CPU.
            ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, 1.0f));
            if(plot_type == ImGuiPlotType_Lines) {
                window->DrawList->AddLine(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
            } else if(plot_type == ImGuiPlotType_Histogram) {
                if(pos1.x >= pos0.x + 2.0f)
                    pos1.x -= 1.0f;
                window->DrawList->AddRectFilled(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
            }

            t0 = t1;
            tp0 = tp1;
        }
    }

    // Text overlay
    if(overlay_text)
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, GetColorU32(ImGuiCol_Text), ImVec2(0.5f, 0.0f));

    if(label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
}
static float Plot_ArrayGetter(void* data, int idx)
{
    ImGuiPlotArrayGetterData* plot_data = (ImGuiPlotArrayGetterData*)data;
    const float v = *(float*)(void*)((unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
    return v;
}
void ImGui::PlotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayGetterData data(values, stride);
    PlotEx(ImGuiPlotType_Lines, label, &Plot_ArrayGetter, (void*)&data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}
void ImGui::PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    PlotEx(ImGuiPlotType_Lines, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}
void ImGui::PlotHistogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayGetterData data(values, stride);
    PlotEx(ImGuiPlotType_Histogram, label, &Plot_ArrayGetter, (void*)&data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}
void ImGui::PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    PlotEx(ImGuiPlotType_Histogram, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}
void ImGui::ProgressBar(float fraction, const ImVec2& size_arg, const char* overlay)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = *g.Style;

    ImVec2 pos = window->DC.CursorPos;
    ImRect bb(pos, pos + CalcItemSize(size_arg, CalcItemWidth(), g.FontSize + style.FramePadding.y*2.0f));
    ItemSize(bb, style.FramePadding.y);
    if(!ItemAdd(bb, NULL))
        return;

    // Render
    fraction = ImSaturate(fraction);
    RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
    bb.Reduce(ImVec2(window->BorderSize, window->BorderSize));
    const ImVec2 fill_br = ImVec2(ImLerp(bb.Min.x, bb.Max.x, fraction), bb.Max.y);
    RenderFrame(bb.Min, fill_br, GetColorU32(ImGuiCol_PlotHistogram), false, style.FrameRounding);

    // Default displaying the fraction as percentage string, but user can override it
    char overlay_buf[32];
    if(!overlay) {
        ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%", fraction * 100 + 0.01f);
        overlay = overlay_buf;
    }

    ImVec2 overlay_size = CalcTextSize(overlay, NULL);
    if(overlay_size.x > 0.0f)
        RenderTextClipped(ImVec2(ImClamp(fill_br.x + style.ItemSpacing.x, bb.Min.x, bb.Max.x - overlay_size.x - style.ItemInnerSpacing.x), bb.Min.y), bb.Max, overlay, NULL, &overlay_size, GetColorU32(ImGuiCol_Text), ImVec2(0.0f, 0.5f), &bb);
}
    static void    STB_TEXTEDIT_LAYOUTROW(StbTexteditRow* r, STB_TEXTEDIT_STRING* obj, int line_start_idx)
    {
        const ImWchar* text = obj->Text.Data;
        const ImWchar* text_remaining = NULL;
        const ImVec2 size = InputTextCalcTextSizeW(text + line_start_idx, text + obj->CurLenW, &text_remaining, NULL, true);
        r->x0 = 0.0f;
        r->x1 = size.x;
        r->baseline_y_delta = size.y;
        r->ymin = 0.0f;
        r->ymax = size.y;
        r->num_chars = (int)(text_remaining - (text + line_start_idx));
    }
    static void STB_TEXTEDIT_DELETECHARS(STB_TEXTEDIT_STRING* obj, int pos, int n)
    {
        ImWchar* dst = obj->Text.Data + pos;

        // We maintain our buffer length in both UTF-8 and wchar formats
        obj->CurLenA -= ImTextCountUtf8BytesFromStr(dst, dst + n);
        obj->CurLenW -= n;

        // Offset remaining text
        const ImWchar* src = obj->Text.Data + pos + n;
        while(ImWchar c = *src++)
            *dst++ = c;
        *dst = '\0';
    }
void ImGuiTextEditState::OnKeyPressed(int key)
{
    stb_textedit_key(this, &StbState, key);
    CursorFollow = true;
    CursorAnimReset();
}
void ImGuiTextEditCallbackData::DeleteChars(int pos, int bytes_count)
{
    IM_ASSERT(pos + bytes_count <= BufTextLen);
    char* dst = Buf + pos;
    const char* src = Buf + pos + bytes_count;
    while(char c = *src++)
        *dst++ = c;
    *dst = '\0';

    if(CursorPos + bytes_count >= pos)
        CursorPos -= bytes_count;
    else if(CursorPos >= pos)
        CursorPos = pos;
    SelectionStart = SelectionEnd = CursorPos;
    BufDirty = true;
    BufTextLen -= bytes_count;
}
void ImGuiTextEditCallbackData::InsertChars(int pos, const char* new_text, const char* new_text_end)
{
    const int new_text_len = new_text_end ? (int)(new_text_end - new_text) : (int)strlen(new_text);
    if(new_text_len + BufTextLen + 1 >= BufSize)
        return;

    if(BufTextLen != pos)
        memmove(Buf + pos + new_text_len, Buf + pos, (size_t)(BufTextLen - pos));
    memcpy(Buf + pos, new_text, (size_t)new_text_len * sizeof(char));
    Buf[BufTextLen + new_text_len] = '\0';

    if(CursorPos >= pos)
        CursorPos += new_text_len;
    SelectionStart = SelectionEnd = CursorPos;
    BufDirty = true;
    BufTextLen += new_text_len;
}
static bool InputTextFilterCharacter(unsigned int* p_char, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
{
    unsigned int c = *p_char;

    if(c < 128 && c != ' ' && !isprint((int)(c & 0xFF))) {
        bool pass = false;
        pass |= (c == '\n' && (flags & ImGuiInputTextFlags_Multiline));
        pass |= (c == '\t' && (flags & ImGuiInputTextFlags_AllowTabInput));
        if(!pass)
            return false;
    }

    if(c >= 0xE000 && c <= 0xF8FF) // Filter private Unicode range. I don't imagine anybody would want to input them. GLFW on OSX seems to send private characters for special m_iKeyMap like arrow m_iKeyMap.
        return false;

    if(flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank)) {
        if(flags & ImGuiInputTextFlags_CharsDecimal)
            if(!(c >= '0' && c <= '9') && (c != '.') && (c != '-') && (c != '+') && (c != '*') && (c != '/'))
                return false;

        if(flags & ImGuiInputTextFlags_CharsHexadecimal)
            if(!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
                return false;

        if(flags & ImGuiInputTextFlags_CharsUppercase)
            if(c >= 'a' && c <= 'z')
                *p_char = (c += (unsigned int)('A' - 'a'));

        if(flags & ImGuiInputTextFlags_CharsNoBlank)
            if(ImCharIsSpace(c))
                return false;
    }

    if(flags & ImGuiInputTextFlags_CallbackCharFilter) {
        ImGuiTextEditCallbackData callback_data;
        memset(&callback_data, 0, sizeof(ImGuiTextEditCallbackData));
        callback_data.EventFlag = ImGuiInputTextFlags_CallbackCharFilter;
        callback_data.EventChar = (ImWchar)c;
        callback_data.Flags = flags;
        callback_data.UserData = user_data;
        if(callback(&callback_data) != 0)
            return false;
        *p_char = callback_data.EventChar;
        if(!callback_data.EventChar)
            return false;
    }

    return true;
}
bool ImGui::InputTextEx(const char* label, char* buf, int buf_size, const ImVec2& size_arg, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return false;

    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackHistory) && (flags & ImGuiInputTextFlags_Multiline))); // Can't use both together (they both use up/down m_iKeyMap)
    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackCompletion) && (flags & ImGuiInputTextFlags_AllowTabInput))); // Can't use both together (they both use tab key)

    ImGuiContext& g = *GImGui;
    const ImGuiIO& io = g.IO;
    const ImGuiStyle& style = *g.Style;

    const bool is_multiline = (flags & ImGuiInputTextFlags_Multiline) != 0;
    const bool is_editable = (flags & ImGuiInputTextFlags_ReadOnly) == 0;
    const bool is_password = (flags & ImGuiInputTextFlags_Password) != 0;

    if(is_multiline) // Open group before calling GetID() because groups tracks id created during their spawn
        BeginGroup();
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    ImVec2 size = CalcItemSize(size_arg, CalcItemWidth(), (is_multiline ? GetTextLineHeight() * 8.0f : label_size.y) + style.FramePadding.y*2.0f); // Arbitrary default of 8 lines high for multi-line
    const ImRect frame_bb(window->DC.CursorPos + ImVec2(label_size.x > 0.0f ? label_size.x + style.ItemInnerSpacing.x : 0.0f, 0.0f), window->DC.CursorPos + size);
    const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

    ImGuiWindow* draw_window = window;
    if(is_multiline) {
        if(!BeginChildFrame(id, frame_bb.GetSize())) {
            EndChildFrame();
            EndGroup();
            return false;
        }
        draw_window = GetCurrentWindow();
        size.x -= draw_window->ScrollbarSizes.x;
    } else {
        ItemSize(total_bb, style.FramePadding.y);
        if(!ItemAdd(total_bb, &id))
            return false;
    }

    // Password pushes a temporary font with only a fallback glyph
    if(is_password) {
        const ImFont::Glyph* glyph = g.Font->FindGlyph('*');
        ImFont* password_font = &g.InputTextPasswordFont;
        password_font->FontSize = g.Font->FontSize;
        password_font->Scale = g.Font->Scale;
        password_font->DisplayOffset = g.Font->DisplayOffset;
        password_font->Ascent = g.Font->Ascent;
        password_font->Descent = g.Font->Descent;
        password_font->ContainerAtlas = g.Font->ContainerAtlas;
        password_font->FallbackGlyph = glyph;
        password_font->FallbackXAdvance = glyph->XAdvance;
        IM_ASSERT(password_font->Glyphs.empty() && password_font->IndexXAdvance.empty() && password_font->IndexLookup.empty());
        PushFont(password_font);
    }

    // NB: we are only allowed to access 'edit_state' if we are the active widget.
    ImGuiTextEditState& edit_state = g.InputTextState;

    const bool focus_requested = FocusableItemRegister(window, g.ActiveId == id, (flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_AllowTabInput)) == 0);    // Using completion callback disable keyboard tabbing
    const bool focus_requested_by_code = focus_requested && (window->FocusIdxAllCounter == window->FocusIdxAllRequestCurrent);
    const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

    const bool hovered = IsHovered(frame_bb, id);
    if(hovered) {
        SetHoveredID(id);
        g.MouseCursor = ImGuiMouseCursor_TextInput;
    }
    const bool user_clicked = hovered && io.MouseClicked[0];
    const bool user_scrolled = is_multiline && g.ActiveId == 0 && edit_state.Id == id && g.ActiveIdPreviousFrame == draw_window->GetIDNoKeepAlive("#SCROLLY");

    bool select_all = (g.ActiveId != id) && (flags & ImGuiInputTextFlags_AutoSelectAll) != 0;
    if(focus_requested || user_clicked || user_scrolled) {
        if(g.ActiveId != id) {
            // Start edition
            // Take a copy of the initial buffer value (both in original UTF-8 format and converted to wchar)
            // From the moment we focused we are ignoring the content of 'buf' (unless we are in read-only mode)
            const int prev_len_w = edit_state.CurLenW;
            edit_state.Text.resize(buf_size + 1);        // wchar count <= UTF-8 count. we use +1 to make sure that .Data isn't NULL so it doesn't crash.
            edit_state.InitialText.resize(buf_size + 1); // UTF-8. we use +1 to make sure that .Data isn't NULL so it doesn't crash.
            ImStrncpy(edit_state.InitialText.Data, buf, edit_state.InitialText.Size);
            const char* buf_end = NULL;
            edit_state.CurLenW = ImTextStrFromUtf8(edit_state.Text.Data, edit_state.Text.Size, buf, NULL, &buf_end);
            edit_state.CurLenA = (int)(buf_end - buf); // We can't Get the result from ImFormatString() above because it is not UTF-8 aware. Here we'll cut off malformed UTF-8.
            edit_state.CursorAnimReset();

            // Preserve cursor position and undo/redo stack if we come back to same widget
            // FIXME: We should probably compare the whole buffer to be on the safety side. Comparing buf (utf8) and edit_state.Text (wchar).
            const bool recycle_state = (edit_state.Id == id) && (prev_len_w == edit_state.CurLenW);
            if(recycle_state) {
                // Recycle existing cursor/selection/undo stack but clamp position
                // Note a single mouse click will override the cursor/position immediately by calling stb_textedit_click handler.
                edit_state.CursorClamp();
            } else {
                edit_state.Id = id;
                edit_state.ScrollX = 0.0f;
                stb_textedit_initialize_state(&edit_state.StbState, !is_multiline);
                if(!is_multiline && focus_requested_by_code)
                    select_all = true;
            }
            if(flags & ImGuiInputTextFlags_AlwaysInsertMode)
                edit_state.StbState.insert_mode = true;
            if(!is_multiline && (focus_requested_by_tab || (user_clicked && io.KeyCtrl)))
                select_all = true;
        }
        SetActiveID(id, window);
        FocusWindow(window);
    } else if(io.MouseClicked[0]) {
        // Release focus when we click outside
        if(g.ActiveId == id)
            ClearActiveID();
    }

    bool value_changed = false;
    bool enter_pressed = false;

    if(g.ActiveId == id) {
        if(!is_editable && !g.ActiveIdIsJustActivated) {
            // When read-only we always use the live data passed to the function
            edit_state.Text.resize(buf_size + 1);
            const char* buf_end = NULL;
            edit_state.CurLenW = ImTextStrFromUtf8(edit_state.Text.Data, edit_state.Text.Size, buf, NULL, &buf_end);
            edit_state.CurLenA = (int)(buf_end - buf);
            edit_state.CursorClamp();
        }

        edit_state.BufSizeA = buf_size;

        // Although we are active we don't prevent mouse from hovering other elements unless we are interacting right now with the widget.
        // Down the line we should have a cleaner library-wide concept of Selected vs Active.
        g.ActiveIdAllowOverlap = !io.MouseDown[0];

        // Edit in progress
        const float mouse_x = (io.MousePos.x - frame_bb.Min.x - style.FramePadding.x) + edit_state.ScrollX;
        const float mouse_y = (is_multiline ? (io.MousePos.y - draw_window->DC.CursorPos.y - style.FramePadding.y) : (g.FontSize*0.5f));

        const bool osx_double_click_selects_words = io.OSXBehaviors;      // OS X style: Double click selects by word instead of selecting whole text
        if(select_all || (hovered && !osx_double_click_selects_words && io.MouseDoubleClicked[0])) {
            edit_state.SelectAll();
            edit_state.SelectedAllMouseLock = true;
        } else if(hovered && osx_double_click_selects_words && io.MouseDoubleClicked[0]) {
            // Select a word only, OS X style (by simulating keystrokes)
            edit_state.OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT);
            edit_state.OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
        } else if(io.MouseClicked[0] && !edit_state.SelectedAllMouseLock) {
            stb_textedit_click(&edit_state, &edit_state.StbState, mouse_x, mouse_y);
            edit_state.CursorAnimReset();
        } else if(io.MouseDown[0] && !edit_state.SelectedAllMouseLock && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)) {
            stb_textedit_drag(&edit_state, &edit_state.StbState, mouse_x, mouse_y);
            edit_state.CursorAnimReset();
            edit_state.CursorFollow = true;
        }
        if(edit_state.SelectedAllMouseLock && !io.MouseDown[0])
            edit_state.SelectedAllMouseLock = false;

        if(io.InputCharacters[0]) {
            // Process text input (before we check for Return because using some IME will effectively send a Return?)
            // We ignore CTRL inputs, but need to allow CTRL+ALT as some keyboards (e.g. German) use AltGR - which is Alt+Ctrl - to input certain characters.
            if(!(io.KeyCtrl && !io.KeyAlt) && is_editable) {
                for(int n = 0; n < IM_ARRAYSIZE(io.InputCharacters) && io.InputCharacters[n]; n++)
                    if(unsigned int c = (unsigned int)io.InputCharacters[n]) {
                        // Insert character if they pass filtering
                        if(!InputTextFilterCharacter(&c, flags, callback, user_data))
                            continue;
                        edit_state.OnKeyPressed((int)c);
                    }
            }

            // Consume characters
            memset(g.IO.InputCharacters, 0, sizeof(g.IO.InputCharacters));
        }

        // Handle various key-presses
        bool cancel_edit = false;
        const int k_mask = (io.KeyShift ? STB_TEXTEDIT_K_SHIFT : 0);
        const bool is_shortcut_key_only = (io.OSXBehaviors ? (io.KeySuper && !io.KeyCtrl) : (io.KeyCtrl && !io.KeySuper)) && !io.KeyAlt && !io.KeyShift; // OS X style: Shortcuts using Cmd/Super instead of Ctrl
        const bool is_wordmove_key_down = io.OSXBehaviors ? io.KeyAlt : io.KeyCtrl;                     // OS X style: Text editing cursor movement using Alt instead of Ctrl
        const bool is_startend_key_down = io.OSXBehaviors && io.KeySuper && !io.KeyCtrl && !io.KeyAlt;  // OS X style: Line/Text Start and End using Cmd+Arrows instead of Home/End

        if(IsKeyPressedMap(ImGuiKey_LeftArrow)) { edit_state.OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINESTART : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDLEFT : STB_TEXTEDIT_K_LEFT) | k_mask); } else if(IsKeyPressedMap(ImGuiKey_RightArrow)) { edit_state.OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINEEND : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDRIGHT : STB_TEXTEDIT_K_RIGHT) | k_mask); } else if(IsKeyPressedMap(ImGuiKey_UpArrow) && is_multiline) { if(io.KeyCtrl) SetWindowScrollY(draw_window, ImMax(draw_window->Scroll.y - g.FontSize, 0.0f)); else edit_state.OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTSTART : STB_TEXTEDIT_K_UP) | k_mask); } else if(IsKeyPressedMap(ImGuiKey_DownArrow) && is_multiline) { if(io.KeyCtrl) SetWindowScrollY(draw_window, ImMin(draw_window->Scroll.y + g.FontSize, GetScrollMaxY())); else edit_state.OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTEND : STB_TEXTEDIT_K_DOWN) | k_mask); } else if(IsKeyPressedMap(ImGuiKey_Home)) { edit_state.OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask); } else if(IsKeyPressedMap(ImGuiKey_End)) { edit_state.OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask); } else if(IsKeyPressedMap(ImGuiKey_Delete) && is_editable) { edit_state.OnKeyPressed(STB_TEXTEDIT_K_DELETE | k_mask); } else if(IsKeyPressedMap(ImGuiKey_Backspace) && is_editable) {
            if(!edit_state.HasSelection()) {
                if(is_wordmove_key_down) edit_state.OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT | STB_TEXTEDIT_K_SHIFT);
                else if(io.OSXBehaviors && io.KeySuper && !io.KeyAlt && !io.KeyCtrl) edit_state.OnKeyPressed(STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_SHIFT);
            }
            edit_state.OnKeyPressed(STB_TEXTEDIT_K_BACKSPACE | k_mask);
        } else if(IsKeyPressedMap(ImGuiKey_Enter)) {
            bool ctrl_enter_for_new_line = (flags & ImGuiInputTextFlags_CtrlEnterForNewLine) != 0;
            if(!is_multiline || (ctrl_enter_for_new_line && !io.KeyCtrl) || (!ctrl_enter_for_new_line && io.KeyCtrl)) {
                ClearActiveID();
                enter_pressed = true;
            } else if(is_editable) {
                unsigned int c = '\n'; // Insert new line
                if(InputTextFilterCharacter(&c, flags, callback, user_data))
                    edit_state.OnKeyPressed((int)c);
            }
        } else if((flags & ImGuiInputTextFlags_AllowTabInput) && IsKeyPressedMap(ImGuiKey_Tab) && !io.KeyCtrl && !io.KeyShift && !io.KeyAlt && is_editable) {
            unsigned int c = '\t'; // Insert TAB
            if(InputTextFilterCharacter(&c, flags, callback, user_data))
                edit_state.OnKeyPressed((int)c);
        } else if(IsKeyPressedMap(ImGuiKey_Escape)) { ClearActiveID(); cancel_edit = true; } else if(is_shortcut_key_only && IsKeyPressedMap(ImGuiKey_Z) && is_editable) { edit_state.OnKeyPressed(STB_TEXTEDIT_K_UNDO); edit_state.ClearSelection(); } else if(is_shortcut_key_only && IsKeyPressedMap(ImGuiKey_Y) && is_editable) { edit_state.OnKeyPressed(STB_TEXTEDIT_K_REDO); edit_state.ClearSelection(); } else if(is_shortcut_key_only && IsKeyPressedMap(ImGuiKey_A)) { edit_state.SelectAll(); edit_state.CursorFollow = true; } else if(is_shortcut_key_only && !is_password && ((IsKeyPressedMap(ImGuiKey_X) && is_editable) || IsKeyPressedMap(ImGuiKey_C)) && (!is_multiline || edit_state.HasSelection())) {
            // Cut, Copy
            const bool cut = IsKeyPressedMap(ImGuiKey_X);
            if(cut && !edit_state.HasSelection())
                edit_state.SelectAll();

            if(io.SetClipboardTextFn) {
                const int ib = edit_state.HasSelection() ? ImMin(edit_state.StbState.select_start, edit_state.StbState.select_end) : 0;
                const int ie = edit_state.HasSelection() ? ImMax(edit_state.StbState.select_start, edit_state.StbState.select_end) : edit_state.CurLenW;
                edit_state.TempTextBuffer.resize((ie - ib) * 4 + 1);
                ImTextStrToUtf8(edit_state.TempTextBuffer.Data, edit_state.TempTextBuffer.Size, edit_state.Text.Data + ib, edit_state.Text.Data + ie);
                SetClipboardText(edit_state.TempTextBuffer.Data);
            }

            if(cut) {
                edit_state.CursorFollow = true;
                stb_textedit_cut(&edit_state, &edit_state.StbState);
            }
        } else if(is_shortcut_key_only && IsKeyPressedMap(ImGuiKey_V) && is_editable) {
            // Paste
            if(const char* clipboard = GetClipboardText()) {
                // Filter pasted buffer
                const int clipboard_len = (int)strlen(clipboard);
                ImWchar* clipboard_filtered = (ImWchar*)ImGui::MemAlloc((clipboard_len + 1) * sizeof(ImWchar));
                int clipboard_filtered_len = 0;
                for(const char* s = clipboard; *s; ) {
                    unsigned int c;
                    s += ImTextCharFromUtf8(&c, s, NULL);
                    if(c == 0)
                        break;
                    if(c >= 0x10000 || !InputTextFilterCharacter(&c, flags, callback, user_data))
                        continue;
                    clipboard_filtered[clipboard_filtered_len++] = (ImWchar)c;
                }
                clipboard_filtered[clipboard_filtered_len] = 0;
                if(clipboard_filtered_len > 0) // If everything was filtered, ignore the pasting operation
                {
                    stb_textedit_paste(&edit_state, &edit_state.StbState, clipboard_filtered, clipboard_filtered_len);
                    edit_state.CursorFollow = true;
                }
                ImGui::MemFree(clipboard_filtered);
            }
        }

        if(cancel_edit) {
            // Restore initial value
            if(is_editable) {
                ImStrncpy(buf, edit_state.InitialText.Data, buf_size);
                value_changed = true;
            }
        } else {
            // Apply new value immediately - copy modified buffer back
            // Note that as soon as the input box is active, the in-widget value gets priority over any underlying modification of the input buffer
            // FIXME: We actually always Render 'buf' when calling DrawList->AddText, making the comment above incorrect.
            // FIXME-OPT: CPU waste to do this every time the widget is active, should mark dirty state from the stb_textedit callbacks.
            if(is_editable) {
                edit_state.TempTextBuffer.resize(edit_state.Text.Size * 4);
                ImTextStrToUtf8(edit_state.TempTextBuffer.Data, edit_state.TempTextBuffer.Size, edit_state.Text.Data, NULL);
            }

            // User callback
            if((flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackAlways)) != 0) {
                IM_ASSERT(callback != NULL);

                // The reason we specify the usage semantic (Completion/History) is that Completion needs to disable keyboard TABBING at the moment.
                ImGuiInputTextFlags event_flag = 0;
                ImGuiKey event_key = ImGuiKey_COUNT;
                if((flags & ImGuiInputTextFlags_CallbackCompletion) != 0 && IsKeyPressedMap(ImGuiKey_Tab)) {
                    event_flag = ImGuiInputTextFlags_CallbackCompletion;
                    event_key = ImGuiKey_Tab;
                } else if((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(ImGuiKey_UpArrow)) {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_UpArrow;
                } else if((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(ImGuiKey_DownArrow)) {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_DownArrow;
                } else if(flags & ImGuiInputTextFlags_CallbackAlways)
                    event_flag = ImGuiInputTextFlags_CallbackAlways;

                if(event_flag) {
                    ImGuiTextEditCallbackData callback_data;
                    memset(&callback_data, 0, sizeof(ImGuiTextEditCallbackData));
                    callback_data.EventFlag = event_flag;
                    callback_data.Flags = flags;
                    callback_data.UserData = user_data;
                    callback_data.ReadOnly = !is_editable;

                    callback_data.EventKey = event_key;
                    callback_data.Buf = edit_state.TempTextBuffer.Data;
                    callback_data.BufTextLen = edit_state.CurLenA;
                    callback_data.BufSize = edit_state.BufSizeA;
                    callback_data.BufDirty = false;

                    // We have to convert from wchar-positions to UTF-8-positions, which can be pretty slow (an incentive to ditch the ImWchar buffer, see https://github.com/nothings/stb/issues/188)
                    ImWchar* text = edit_state.Text.Data;
                    const int utf8_cursor_pos = callback_data.CursorPos = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.cursor);
                    const int utf8_selection_start = callback_data.SelectionStart = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.select_start);
                    const int utf8_selection_end = callback_data.SelectionEnd = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.select_end);

                    // Call user code
                    callback(&callback_data);

                    // Read back what user may have modified
                    IM_ASSERT(callback_data.Buf == edit_state.TempTextBuffer.Data);  // Invalid to modify those fields
                    IM_ASSERT(callback_data.BufSize == edit_state.BufSizeA);
                    IM_ASSERT(callback_data.Flags == flags);
                    if(callback_data.CursorPos != utf8_cursor_pos)            edit_state.StbState.cursor = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.CursorPos);
                    if(callback_data.SelectionStart != utf8_selection_start)  edit_state.StbState.select_start = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionStart);
                    if(callback_data.SelectionEnd != utf8_selection_end)      edit_state.StbState.select_end = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionEnd);
                    if(callback_data.BufDirty) {
                        IM_ASSERT(callback_data.BufTextLen == (int)strlen(callback_data.Buf)); // You need to maintain BufTextLen if you change the text!
                        edit_state.CurLenW = ImTextStrFromUtf8(edit_state.Text.Data, edit_state.Text.Size, callback_data.Buf, NULL);
                        edit_state.CurLenA = callback_data.BufTextLen;  // Assume correct length and valid UTF-8 from user, saves us an extra strlen()
                        edit_state.CursorAnimReset();
                    }
                }
            }

            // Copy back to user buffer
            if(is_editable && strcmp(edit_state.TempTextBuffer.Data, buf) != 0) {
                ImStrncpy(buf, edit_state.TempTextBuffer.Data, buf_size);
                value_changed = true;
            }
        }
    }
bool ImGui::InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline()
    return InputTextEx(label, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
}
bool ImGui::InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
{
    return InputTextEx(label, buf, (int)buf_size, size, flags | ImGuiInputTextFlags_Multiline, callback, user_data);
}
bool ImGui::InputScalarEx(const char* label, ImGuiDataType data_type, void* data_ptr, void* step_ptr, void* step_fast_ptr, const char* scalar_format, ImGuiInputTextFlags extra_flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = *g.Style;
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    BeginGroup();
    PushID(label);
    const ImVec2 button_sz = ImVec2(g.FontSize, g.FontSize) + style.FramePadding*2.0f;
    if(step_ptr)
        PushItemWidth(ImMax(1.0f, CalcItemWidth() - (button_sz.x + style.ItemInnerSpacing.x) * 2));

    char buf[64];
    DataTypeFormatString(data_type, data_ptr, scalar_format, buf, IM_ARRAYSIZE(buf));

    bool value_changed = false;
    if(!(extra_flags & ImGuiInputTextFlags_CharsHexadecimal))
        extra_flags |= ImGuiInputTextFlags_CharsDecimal;
    extra_flags |= ImGuiInputTextFlags_AutoSelectAll;
    if(InputText("", buf, IM_ARRAYSIZE(buf), extra_flags)) // PushId(label) + "" gives us the expected ID from outside point of view
        value_changed = DataTypeApplyOpFromText(buf, GImGui->InputTextState.InitialText.begin(), data_type, data_ptr, scalar_format);

    // Step buttons
    if(step_ptr) {
        PopItemWidth();
        SameLine(0, style.ItemInnerSpacing.x);
        if(ButtonEx("-", button_sz, ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups)) {
            DataTypeApplyOp(data_type, '-', data_ptr, g.IO.KeyCtrl && step_fast_ptr ? step_fast_ptr : step_ptr);
            value_changed = true;
        }
        SameLine(0, style.ItemInnerSpacing.x);
        if(ButtonEx("+", button_sz, ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups)) {
            DataTypeApplyOp(data_type, '+', data_ptr, g.IO.KeyCtrl && step_fast_ptr ? step_fast_ptr : step_ptr);
            value_changed = true;
        }
    }
    PopID();

    if(label_size.x > 0) {
        SameLine(0, style.ItemInnerSpacing.x);
        RenderText(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), label);
        ItemSize(label_size, style.FramePadding.y);
    }
    EndGroup();

    return value_changed;
}
static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if(out_text)
        *out_text = items[idx];
    return true;
}
static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
{
    // FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
    const char* items_separated_by_zeros = (const char*)data;
    int items_count = 0;
    const char* p = items_separated_by_zeros;
    while(*p) {
        if(idx == items_count)
            break;
        p += strlen(p) + 1;
        items_count++;
    }
    if(!*p)
        return false;
    if(out_text)
        *out_text = p;
    return true;
}
bool ImGui::Combo(const char* label, int* current_item, bool(*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = *g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    ItemSize(total_bb, style.FramePadding.y);
    if(!ItemAdd(total_bb, &id))
        return false;

    const float arrow_size = (g.FontSize + style.FramePadding.x * 2.0f);
    const bool hovered = IsHovered(frame_bb, id);
    bool popup_open = IsPopupOpen(id);
    bool popup_opened_now = false;

    const ImRect value_bb(frame_bb.Min, frame_bb.Max - ImVec2(arrow_size, 0.0f));
    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
    RenderFrame(ImVec2(frame_bb.Max.x - arrow_size, frame_bb.Min.y), frame_bb.Max, GetColorU32(popup_open || hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button), true, style.FrameRounding); // FIXME-ROUNDING
    RenderCollapseTriangle(ImVec2(frame_bb.Max.x - arrow_size, frame_bb.Min.y) + style.FramePadding, true);

    if(*current_item >= 0 && *current_item < items_count) {
        const char* item_text;
        if(items_getter(data, *current_item, &item_text))
            RenderTextClipped(frame_bb.Min + style.FramePadding, value_bb.Max, item_text, NULL, NULL, GetColorU32(ImGuiCol_Text), ImVec2(0.0f, 0.0f));
    }

    if(label_size.x > 0)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    if(hovered) {
        SetHoveredID(id);
        if(g.IO.MouseClicked[0]) {
            ClearActiveID();
            if(IsPopupOpen(id)) {
                ClosePopup(id);
            } else {
                FocusWindow(window);
                OpenPopup(label);
                popup_open = popup_opened_now = true;
            }
        }
    }

    bool value_changed = false;
    if(IsPopupOpen(id)) {
        // Size default to hold ~7 items
        if(height_in_items < 0)
            height_in_items = 7;

        float popup_height = (label_size.y + style.ItemSpacing.y) * ImMin(items_count, height_in_items) + (style.FramePadding.y * 3);
        float popup_y1 = frame_bb.Max.y;
        float popup_y2 = ImClamp(popup_y1 + popup_height, popup_y1, g.IO.DisplaySize.y - style.DisplaySafeAreaPadding.y);
        if((popup_y2 - popup_y1) < ImMin(popup_height, frame_bb.Min.y - style.DisplaySafeAreaPadding.y)) {
            // Position our combo ABOVE because there's more space to fit! (FIXME: Handle in Begin() or use a shared helper. We have similar code in Begin() for popup placement)
            popup_y1 = ImClamp(frame_bb.Min.y - popup_height, style.DisplaySafeAreaPadding.y, frame_bb.Min.y);
            popup_y2 = frame_bb.Min.y;
        }
        ImRect popup_rect(ImVec2(frame_bb.Min.x, popup_y1), ImVec2(frame_bb.Max.x, popup_y2));
        SetNextWindowPos(popup_rect.Min);
        SetNextWindowSize(popup_rect.GetSize());
        PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding);

        const ImGuiWindowFlags flags = ImGuiWindowFlags_ComboBox | ((window->Flags & ImGuiWindowFlags_ShowBorders) ? ImGuiWindowFlags_ShowBorders : 0);
        if(BeginPopupEx(label, flags)) {
            // Display items
            Spacing();
            for(int i = 0; i < items_count; i++) {
                PushID((void*)(intptr_t)i);
                const bool item_selected = (i == *current_item);
                const char* item_text;
                if(!items_getter(data, i, &item_text))
                    item_text = "*Unknown item*";
                if(Selectable(item_text, item_selected)) {
                    ClearActiveID();
                    value_changed = true;
                    *current_item = i;
                }
                if(item_selected && popup_opened_now)
                    SetScrollHere();
                PopID();
            }
            EndPopup();
        }
        PopStyleVar();
    }
    return value_changed;
}
void ImGui::ListBoxFooter()
{
    ImGuiWindow* parent_window = GetParentWindow();
    const ImRect bb = parent_window->DC.LastItemRect;
    const ImGuiStyle& style = ImGui::GetStyle();

    EndChildFrame();

    // Redeclare item size so that it includes the label (we have stored the full size in LastItemRect)
    // We call SameLine() to restore DC.CurrentLine* data
    SameLine();
    parent_window->DC.CursorPos = bb.Min;
    ItemSize(bb, style.FramePadding.y);
    EndGroup();
}
bool ImGui::ListBox(const char* label, int* current_item, bool(*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items)
{
    if(!ListBoxHeader(label, items_count, height_in_items))
        return false;

    // Assume all items have even height (= 1 line of text). If you need items of different or variable sizes you can create a custom version of ListBox() in your code without using the clipper.
    bool value_changed = false;
    ImGuiListClipper clipper(items_count, GetTextLineHeightWithSpacing());
    while(clipper.Step())
        for(int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
            const bool item_selected = (i == *current_item);
            const char* item_text;
            if(!items_getter(data, i, &item_text))
                item_text = "*Unknown item*";

            PushID(i);
            if(Selectable(item_text, item_selected)) {
                *current_item = i;
                value_changed = true;
            }
            PopID();
        }
    ListBoxFooter();
    return value_changed;
}
void ImGui::EndMainMenuBar()
{
    EndMenuBar();
    End();
    PopStyleVar(2);
}
void ImGui::EndMenuBar()
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    IM_ASSERT(window->Flags & ImGuiWindowFlags_MenuBar);
    IM_ASSERT(window->DC.MenuBarAppending);
    PopClipRect();
    PopID();
    window->DC.MenuBarOffsetX = window->DC.CursorPos.x - window->MenuBarRect().Min.x;
    window->DC.GroupStack.back().AdvanceCursor = false;
    EndGroup();
    window->DC.LayoutType = ImGuiLayoutType_Vertical;
    window->DC.MenuBarAppending = false;
}
void ImGui::EndMenu()
{
    EndPopup();
}
void ImGui::ColorEditMode(ImGuiColorEditMode mode)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.ColorEditMode = mode;
}
void ImGui::Separator()
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    if(window->DC.ColumnsCount > 1)
        PopClipRect();

    float x1 = window->Pos.x;
    float x2 = window->Pos.x + window->Size.x;
    if(!window->DC.GroupStack.empty())
        x1 += window->DC.IndentX;

    const ImRect bb(ImVec2(x1, window->DC.CursorPos.y), ImVec2(x2, window->DC.CursorPos.y + 1.0f));
    ItemSize(ImVec2(0.0f, 0.0f)); // NB: we don't provide our width so that it doesn't Get feed back into AutoFit, we don't provide height to not alter layout.
    if(!ItemAdd(bb, NULL)) {
        if(window->DC.ColumnsCount > 1)
            PushColumnClipRect();
        return;
    }

    window->DrawList->AddLine(bb.Min, ImVec2(bb.Max.x, bb.Min.y), GetColorU32(ImGuiCol_Border));

    if(window->DC.ColumnsCount > 1) {
        PushColumnClipRect();
        window->DC.ColumnsCellMinY = window->DC.CursorPos.y;
    }
}
void ImGui::Spacing()
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;
    ItemSize(ImVec2(0, 0));
}
void ImGui::Dummy(const ImVec2& size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(bb);
    ItemAdd(bb, NULL);
}
void ImGui::BeginGroup()
{
    ImGuiWindow* window = GetCurrentWindow();

    window->DC.GroupStack.resize(window->DC.GroupStack.Size + 1);
    ImGuiGroupData& group_data = window->DC.GroupStack.back();
    group_data.BackupCursorPos = window->DC.CursorPos;
    group_data.BackupCursorMaxPos = window->DC.CursorMaxPos;
    group_data.BackupIndentX = window->DC.IndentX;
    group_data.BackupGroupOffsetX = window->DC.GroupOffsetX;
    group_data.BackupCurrentLineHeight = window->DC.CurrentLineHeight;
    group_data.BackupCurrentLineTextBaseOffset = window->DC.CurrentLineTextBaseOffset;
    group_data.BackupActiveIdIsAlive = GImGui->ActiveIdIsAlive;
    group_data.AdvanceCursor = true;

    window->DC.GroupOffsetX = window->DC.CursorPos.x - window->Pos.x - window->DC.ColumnsOffsetX;
    window->DC.IndentX = window->DC.GroupOffsetX;
    window->DC.CursorMaxPos = window->DC.CursorPos;
    window->DC.CurrentLineHeight = 0.0f;
}
void ImGui::EndGroup()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    IM_ASSERT(!window->DC.GroupStack.empty());	// Mismatched BeginGroup()/EndGroup() calls

    ImGuiGroupData& group_data = window->DC.GroupStack.back();

    ImRect group_bb(group_data.BackupCursorPos, window->DC.CursorMaxPos);
    group_bb.Max.y -= g.Style->ItemSpacing.y;      // Cancel out last vertical spacing because we are adding one ourselves.
    group_bb.Max = ImMax(group_bb.Min, group_bb.Max);

    window->DC.CursorPos = group_data.BackupCursorPos;
    window->DC.CursorMaxPos = ImMax(group_data.BackupCursorMaxPos, window->DC.CursorMaxPos);
    window->DC.CurrentLineHeight = group_data.BackupCurrentLineHeight;
    window->DC.CurrentLineTextBaseOffset = group_data.BackupCurrentLineTextBaseOffset;
    window->DC.IndentX = group_data.BackupIndentX;
    window->DC.GroupOffsetX = group_data.BackupGroupOffsetX;

    if(group_data.AdvanceCursor) {
        window->DC.CurrentLineTextBaseOffset = ImMax(window->DC.PrevLineTextBaseOffset, group_data.BackupCurrentLineTextBaseOffset);      // FIXME: Incorrect, we should grab the base offset from the *first line* of the group but it is hard to obtain now.
        ItemSize(group_bb.GetSize(), group_data.BackupCurrentLineTextBaseOffset);
        ItemAdd(group_bb, NULL);
    }

    // If the current ActiveId was declared within the boundary of our group, we copy it to LastItemId so IsItemActive() will function on the entire group.
    // It would be be neater if we replaced window.DC.LastItemId by e.g. 'bool LastItemIsActive', but if you search for LastItemId you'll notice it is only used in that context.
    const bool active_id_within_group = (!group_data.BackupActiveIdIsAlive && g.ActiveIdIsAlive && g.ActiveId && g.ActiveIdWindow->RootWindow == window->RootWindow);
    if(active_id_within_group)
        window->DC.LastItemId = g.ActiveId;
    if(active_id_within_group && g.HoveredId == g.ActiveId)
        window->DC.LastItemHoveredAndUsable = window->DC.LastItemHoveredRect = true;

    window->DC.GroupStack.pop_back();

    //window->DrawList->AddRect(group_bb.Min, group_bb.Max, IM_COL32(255,0,255,255));   // Debug
}
void ImGui::SameLine(float pos_x, float spacing_w)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    if(pos_x != 0.0f) {
        if(spacing_w < 0.0f) spacing_w = 0.0f;
        window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + pos_x + spacing_w + window->DC.GroupOffsetX + window->DC.ColumnsOffsetX;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    } else {
        if(spacing_w < 0.0f) spacing_w = g.Style->ItemSpacing.x;
        window->DC.CursorPos.x = window->DC.CursorPosPrevLine.x + spacing_w;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }
    window->DC.CurrentLineHeight = window->DC.PrevLineHeight;
    window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
}
void ImGui::NewLine()
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems)
        return;
    if(window->DC.CurrentLineHeight > 0.0f)     // In the event that we are on a line with items that is smaller that FontSize high, we will preserve its height.
        ItemSize(ImVec2(0, 0));
    else
        ItemSize(ImVec2(0.0f, GImGui->FontSize));
}
void ImGui::NextColumn()
{
    ImGuiWindow* window = GetCurrentWindow();
    if(window->SkipItems || window->DC.ColumnsCount <= 1)
        return;

    ImGuiContext& g = *GImGui;
    PopItemWidth();
    PopClipRect();

    window->DC.ColumnsCellMaxY = ImMax(window->DC.ColumnsCellMaxY, window->DC.CursorPos.y);
    if(++window->DC.ColumnsCurrent < window->DC.ColumnsCount) {
        // Columns 1+ cancel out IndentX
        window->DC.ColumnsOffsetX = GetColumnOffset(window->DC.ColumnsCurrent) - window->DC.IndentX + g.Style->ItemSpacing.x;
        window->DrawList->ChannelsSetCurrent(window->DC.ColumnsCurrent);
    } else {
        window->DC.ColumnsCurrent = 0;
        window->DC.ColumnsOffsetX = 0.0f;
        window->DC.ColumnsCellMinY = window->DC.ColumnsCellMaxY;
        window->DrawList->ChannelsSetCurrent(0);
    }
    window->DC.CursorPos.x = (float)(int)(window->Pos.x + window->DC.IndentX + window->DC.ColumnsOffsetX);
    window->DC.CursorPos.y = window->DC.ColumnsCellMinY;
    window->DC.CurrentLineHeight = 0.0f;
    window->DC.CurrentLineTextBaseOffset = 0.0f;

    PushColumnClipRect();
    PushItemWidth(GetColumnWidth() * 0.8f);  // FIXME: Move on columns setup
}
void ImGui::SetColumnOffset(int column_index, float offset)
{
    ImGuiWindow* window = GetCurrentWindow();
    if(column_index < 0)
        column_index = window->DC.ColumnsCurrent;

    IM_ASSERT(column_index < window->DC.ColumnsData.Size);
    const float t = (offset - window->DC.ColumnsMinX) / (window->DC.ColumnsMaxX - window->DC.ColumnsMinX);
    window->DC.ColumnsData[column_index].OffsetNorm = t;

    const ImGuiID column_id = window->DC.ColumnsSetId + ImGuiID(column_index);
    window->DC.StateStorage->SetFloat(column_id, t);
}
static void PushColumnClipRect(int column_index)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if(column_index < 0)
        column_index = window->DC.ColumnsCurrent;

    float x1 = ImFloor(0.5f + window->Pos.x + ImGui::GetColumnOffset(column_index) - 1.0f);
    float x2 = ImFloor(0.5f + window->Pos.x + ImGui::GetColumnOffset(column_index + 1) - 1.0f);
    ImGui::PushClipRect(ImVec2(x1, -FLT_MAX), ImVec2(x2, +FLT_MAX), true);
}
void ImGui::Columns(int columns_count, const char* id, bool border)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    IM_ASSERT(columns_count >= 1);

    if(window->DC.ColumnsCount != 1) {
        if(window->DC.ColumnsCurrent != 0)
            ItemSize(ImVec2(0, 0));   // Advance to column 0
        PopItemWidth();
        PopClipRect();
        window->DrawList->ChannelsMerge();

        window->DC.ColumnsCellMaxY = ImMax(window->DC.ColumnsCellMaxY, window->DC.CursorPos.y);
        window->DC.CursorPos.y = window->DC.ColumnsCellMaxY;
    }

    // Draw columns borders and handle resize at the time of "closing" a columns Set
    if(window->DC.ColumnsCount != columns_count && window->DC.ColumnsCount != 1 && window->DC.ColumnsShowBorders && !window->SkipItems) {
        const float y1 = window->DC.ColumnsStartPosY;
        const float y2 = window->DC.CursorPos.y;
        for(int i = 1; i < window->DC.ColumnsCount; i++) {
            float x = window->Pos.x + GetColumnOffset(i);
            const ImGuiID column_id = window->DC.ColumnsSetId + ImGuiID(i);
            const ImRect column_rect(ImVec2(x - 4, y1), ImVec2(x + 4, y2));
            if(IsClippedEx(column_rect, &column_id, false))
                continue;

            bool hovered, held;
            ButtonBehavior(column_rect, column_id, &hovered, &held);
            if(hovered || held)
                g.MouseCursor = ImGuiMouseCursor_ResizeEW;

            // Draw before resize so our items positioning are in sync with the line being drawn
            const ImU32 col = GetColorU32(held ? ImGuiCol_ColumnActive : hovered ? ImGuiCol_ColumnHovered : ImGuiCol_Column);
            const float xi = (float)(int)x;
            window->DrawList->AddLine(ImVec2(xi, y1 + 1.0f), ImVec2(xi, y2), col);

            if(held) {
                if(g.ActiveIdIsJustActivated)
                    g.ActiveIdClickOffset.x -= 4;   // Store from center of column line (we used a 8 wide rect for columns clicking)
                x = GetDraggedColumnOffset(i);
                SetColumnOffset(i, x);
            }
        }
    }

    // Differentiate column ID with an arbitrary prefix for cases where users name their columns Set the same as another widget.
    // In addition, when an identifier isn't explicitly provided we include the number of columns in the hash to make it uniquer.
    PushID(0x11223347 + (id ? 0 : columns_count));
    window->DC.ColumnsSetId = window->GetID(id ? id : "columns");
    PopID();

    // Set state for first column
    window->DC.ColumnsCurrent = 0;
    window->DC.ColumnsCount = columns_count;
    window->DC.ColumnsShowBorders = border;

    const float content_region_width = (window->SizeContentsExplicit.x != 0.0f) ? window->SizeContentsExplicit.x : window->Size.x;
    window->DC.ColumnsMinX = window->DC.IndentX; // Lock our horizontal range
    window->DC.ColumnsMaxX = content_region_width - window->Scroll.x - ((window->Flags & ImGuiWindowFlags_NoScrollbar) ? 0 : g.Style->ScrollbarSize);// - window->WindowPadding().x;
    window->DC.ColumnsStartPosY = window->DC.CursorPos.y;
    window->DC.ColumnsCellMinY = window->DC.ColumnsCellMaxY = window->DC.CursorPos.y;
    window->DC.ColumnsOffsetX = 0.0f;
    window->DC.CursorPos.x = (float)(int)(window->Pos.x + window->DC.IndentX + window->DC.ColumnsOffsetX);

    if(window->DC.ColumnsCount != 1) {
        // Cache column offsets
        window->DC.ColumnsData.resize(columns_count + 1);
        for(int column_index = 0; column_index < columns_count + 1; column_index++) {
            const ImGuiID column_id = window->DC.ColumnsSetId + ImGuiID(column_index);
            KeepAliveID(column_id);
            const float default_t = column_index / (float)window->DC.ColumnsCount;
            const float t = window->DC.StateStorage->GetFloat(column_id, default_t);      // Cheaply store our floating point value inside the integer (could store a union into the map?)
            window->DC.ColumnsData[column_index].OffsetNorm = t;
        }
        window->DrawList->ChannelsSplit(window->DC.ColumnsCount);
        PushColumnClipRect();
        PushItemWidth(GetColumnWidth() * 0.8f);
    } else {
        window->DC.ColumnsData.resize(0);
    }
}
void ImGui::Indent(float indent_w)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.IndentX += (indent_w > 0.0f) ? indent_w : g.Style->IndentSpacing;
    window->DC.CursorPos.x = window->Pos.x + window->DC.IndentX + window->DC.ColumnsOffsetX;
}
void ImGui::Unindent(float indent_w)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.IndentX -= (indent_w > 0.0f) ? indent_w : g.Style->IndentSpacing;
    window->DC.CursorPos.x = window->Pos.x + window->DC.IndentX + window->DC.ColumnsOffsetX;
}
void ImGui::TreePush(const char* str_id)
{
    ImGuiWindow* window = GetCurrentWindow();
    Indent();
    window->DC.TreeDepth++;
    PushID(str_id ? str_id : "#TreePush");
}
void ImGui::TreePush(const void* ptr_id)
{
    ImGuiWindow* window = GetCurrentWindow();
    Indent();
    window->DC.TreeDepth++;
    PushID(ptr_id ? ptr_id : (const void*)"#TreePush");
}
void ImGui::TreePushRawID(ImGuiID id)
{
    ImGuiWindow* window = GetCurrentWindow();
    Indent();
    window->DC.TreeDepth++;
    window->IDStack.push_back(id);
}
void ImGui::TreePop()
{
    ImGuiWindow* window = GetCurrentWindow();
    Unindent();
    window->DC.TreeDepth--;
    PopID();
}
void ImGui::Value(const char* prefix, bool b)
{
    Text("%s: %s", prefix, (b ? "true" : "false"));
}
void ImGui::Value(const char* prefix, int v)
{
    Text("%s: %d", prefix, v);
}
void ImGui::Value(const char* prefix, unsigned int v)
{
    Text("%s: %d", prefix, v);
}
void ImGui::Value(const char* prefix, float v, const char* float_format)
{
    if(float_format) {
        char fmt[64];
        ImFormatString(fmt, IM_ARRAYSIZE(fmt), "%%s: %s", float_format);
        Text(fmt, prefix, v);
    } else {
        Text("%s: %.3f", prefix, v);
    }
}
void ImGui::ValueColor(const char* prefix, const ImVec4& v)
{
    Text("%s: (%.2f,%.2f,%.2f,%.2f)", prefix, v.x, v.y, v.z, v.w);
    SameLine();
    ColorButton(v, true);
}
void ImGui::ValueColor(const char* prefix, ImU32 v)
{
    Text("%s: %08X", prefix, v);
    SameLine();
    ColorButton(ColorConvertU32ToFloat4(v), true);
}
static const char* GetClipboardTextFn_DefaultImpl(void*)
{
    static ImVector<char> buf_local;
    buf_local.clear();
    if(!OpenClipboard(NULL))
        return NULL;
    HANDLE wbuf_handle = GetClipboardData(CF_UNICODETEXT);
    if(wbuf_handle == NULL)
        return NULL;
    if(ImWchar* wbuf_global = (ImWchar*)GlobalLock(wbuf_handle)) {
        int buf_len = ImTextCountUtf8BytesFromStr(wbuf_global, NULL) + 1;
        buf_local.resize(buf_len);
        ImTextStrToUtf8(buf_local.Data, buf_len, wbuf_global, NULL);
    }
    GlobalUnlock(wbuf_handle);
    CloseClipboard();
    return buf_local.Data;
}
static void SetClipboardTextFn_DefaultImpl(void*, const char* text)
{
    if(!OpenClipboard(NULL))
        return;
    const int wbuf_length = ImTextCountCharsFromUtf8(text, NULL) + 1;
    HGLOBAL wbuf_handle = GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)wbuf_length * sizeof(ImWchar));
    if(wbuf_handle == NULL)
        return;
    ImWchar* wbuf_global = (ImWchar*)GlobalLock(wbuf_handle);
    ImTextStrFromUtf8(wbuf_global, wbuf_length, text, NULL);
    GlobalUnlock(wbuf_handle);
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, wbuf_handle);
    CloseClipboard();
}
static const char* GetClipboardTextFn_DefaultImpl(void*)
{
    return GImGui->PrivateClipboard;
}
static void SetClipboardTextFn_DefaultImpl(void*, const char* text)
{
    ImGuiContext& g = *GImGui;
    if(g.PrivateClipboard) {
        ImGui::MemFree(g.PrivateClipboard);
        g.PrivateClipboard = NULL;
    }
    const char* text_end = text + strlen(text);
    g.PrivateClipboard = (char*)ImGui::MemAlloc((size_t)(text_end - text) + 1);
    memcpy(g.PrivateClipboard, text, (size_t)(text_end - text));
    g.PrivateClipboard[(int)(text_end - text)] = 0;
}
static void ImeSetInputScreenPosFn_DefaultImpl(int x, int y)
{
    // Notify OS Input Method Editor of text input position
    if(HWND hwnd = (HWND)GImGui->IO.ImeWindowHandle)
        if(HIMC himc = ImmGetContext(hwnd)) {
            COMPOSITIONFORM cf;
            cf.ptCurrentPos.x = x;
            cf.ptCurrentPos.y = y;
            cf.dwStyle = CFS_FORCE_POSITION;
            ImmSetCompositionWindow(himc, &cf);
        }
}
static void ImeSetInputScreenPosFn_DefaultImpl(int, int) {}
void ImGui::ShowMetricsWindow(bool* p_open)
{
    if(ImGui::Begin("ImGui Metrics", p_open)) {
        ImGui::Text("ImGui %s", ImGui::GetVersion());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("%d vertices, %d indices (%d triangles)", ImGui::GetIO().MetricsRenderVertices, ImGui::GetIO().MetricsRenderIndices, ImGui::GetIO().MetricsRenderIndices / 3);
        ImGui::Text("%d allocations", ImGui::GetIO().MetricsAllocs);
        static bool show_clip_rects = true;
        ImGui::Checkbox("Show clipping rectangles when hovering a ImDrawCmd", &show_clip_rects);
        ImGui::Separator();

        struct Funcs
        {
            static void NodeDrawList(ImDrawList* draw_list, const char* label)
            {
                bool node_open = ImGui::TreeNode(draw_list, "%s: '%s' %d vtx, %d indices, %d cmds", label, draw_list->_OwnerName ? draw_list->_OwnerName : "", draw_list->VtxBuffer.Size, draw_list->IdxBuffer.Size, draw_list->CmdBuffer.Size);
                if(draw_list == ImGui::GetWindowDrawList()) {
                    ImGui::SameLine();
                    ImGui::TextColored(ImColor(255, 100, 100), "CURRENTLY APPENDING"); // Can't display stats for active draw list! (we don't have the data double-buffered)
                    if(node_open) ImGui::TreePop();
                    return;
                }
                if(!node_open)
                    return;

                ImDrawList* overlay_draw_list = &GImGui->OverlayDrawList;   // Render additional Visuals into the top-most draw list
                overlay_draw_list->PushClipRectFullScreen();
                int elem_offset = 0;
                for(const ImDrawCmd* pcmd = draw_list->CmdBuffer.begin(); pcmd < draw_list->CmdBuffer.end(); elem_offset += pcmd->ElemCount, pcmd++) {
                    if(pcmd->UserCallback) {
                        ImGui::BulletText("Callback %p, user_data %p", pcmd->UserCallback, pcmd->UserCallbackData);
                        continue;
                    }
                    ImDrawIdx* idx_buffer = (draw_list->IdxBuffer.Size > 0) ? draw_list->IdxBuffer.Data : NULL;
                    bool pcmd_node_open = ImGui::TreeNode((void*)(pcmd - draw_list->CmdBuffer.begin()), "Draw %-4d %s vtx, tex = %p, clip_rect = (%.0f,%.0f)..(%.0f,%.0f)", pcmd->ElemCount, draw_list->IdxBuffer.Size > 0 ? "indexed" : "non-indexed", pcmd->TextureId, pcmd->ClipRect.x, pcmd->ClipRect.y, pcmd->ClipRect.z, pcmd->ClipRect.w);
                    if(show_clip_rects && ImGui::IsItemHovered()) {
                        ImRect clip_rect = pcmd->ClipRect;
                        ImRect vtxs_rect;
                        for(int i = elem_offset; i < elem_offset + (int)pcmd->ElemCount; i++)
                            vtxs_rect.Add(draw_list->VtxBuffer[idx_buffer ? idx_buffer[i] : i].pos);
                        clip_rect.Floor(); overlay_draw_list->AddRect(clip_rect.Min, clip_rect.Max, IM_COL32(255, 255, 0, 255));
                        vtxs_rect.Floor(); overlay_draw_list->AddRect(vtxs_rect.Min, vtxs_rect.Max, IM_COL32(255, 0, 255, 255));
                    }
                    if(!pcmd_node_open)
                        continue;
                    ImGuiListClipper clipper(pcmd->ElemCount / 3); // Manually coarse clip our print out of individual vertices to save CPU, only items that may be visible.
                    while(clipper.Step())
                        for(int prim = clipper.DisplayStart, vtx_i = elem_offset + clipper.DisplayStart * 3; prim < clipper.DisplayEnd; prim++) {
                            char buf[300], *buf_p = buf;
                            ImVec2 triangles_pos[3];
                            for(int n = 0; n < 3; n++, vtx_i++) {
                                ImDrawVert& v = draw_list->VtxBuffer[idx_buffer ? idx_buffer[vtx_i] : vtx_i];
                                triangles_pos[n] = v.pos;
                                buf_p += sprintf(buf_p, "%s %04d { pos = (%8.2f,%8.2f), uv = (%.6f,%.6f), col = %08X }\n", (n == 0) ? "vtx" : "   ", vtx_i, v.pos.x, v.pos.y, v.uv.x, v.uv.y, v.col);
                            }
                            ImGui::Selectable(buf, false);
                            if(ImGui::IsItemHovered())
                                overlay_draw_list->AddPolyline(triangles_pos, 3, IM_COL32(255, 255, 0, 255), true, 1.0f, false);  // Add triangle without AA, more readable for large-thin triangle
                        }
                    ImGui::TreePop();
                }
                overlay_draw_list->PopClipRect();
                ImGui::TreePop();
            }

            static void NodeWindows(ImVector<ImGuiWindow*>& windows, const char* label)
            {
                if(!ImGui::TreeNode(label, "%s (%d)", label, windows.Size))
                    return;
                for(int i = 0; i < windows.Size; i++)
                    Funcs::NodeWindow(windows[i], "Window");
                ImGui::TreePop();
            }

            static void NodeWindow(ImGuiWindow* window, const char* label)
            {
                if(!ImGui::TreeNode(window, "%s '%s', %d @ 0x%p", label, window->Name, window->Active || window->WasActive, window))
                    return;
                NodeDrawList(window->DrawList, "DrawList");
                ImGui::BulletText("Pos: (%.1f,%.1f)", window->Pos.x, window->Pos.y);
                ImGui::BulletText("Size: (%.1f,%.1f), SizeContents (%.1f,%.1f)", window->Size.x, window->Size.y, window->SizeContents.x, window->SizeContents.y);
                ImGui::BulletText("Scroll: (%.2f,%.2f)", window->Scroll.x, window->Scroll.y);
                if(window->RootWindow != window) NodeWindow(window->RootWindow, "RootWindow");
                if(window->DC.ChildWindows.Size > 0) NodeWindows(window->DC.ChildWindows, "ChildWindows");
                ImGui::BulletText("Storage: %d bytes", window->StateStorage.Data.Size * (int)sizeof(ImGuiStorage::Pair));
                ImGui::TreePop();
            }
        };

        ImGuiContext& g = *GImGui;                // Access private state
        Funcs::NodeWindows(g.Windows, "Windows");
        if(ImGui::TreeNode("DrawList", "Active DrawLists (%d)", g.RenderDrawLists[0].Size)) {
            for(int i = 0; i < g.RenderDrawLists[0].Size; i++)
                Funcs::NodeDrawList(g.RenderDrawLists[0][i], "DrawList");
            ImGui::TreePop();
        }
        if(ImGui::TreeNode("Popups", "Open Popups Stack (%d)", g.OpenPopupStack.Size)) {
            for(int i = 0; i < g.OpenPopupStack.Size; i++) {
                ImGuiWindow* window = g.OpenPopupStack[i].Window;
                ImGui::BulletText("PopupID: %08x, Window: '%s'%s%s", g.OpenPopupStack[i].PopupId, window ? window->Name : "NULL", window && (window->Flags & ImGuiWindowFlags_ChildWindow) ? " ChildWindow" : "", window && (window->Flags & ImGuiWindowFlags_ChildMenu) ? " ChildMenu" : "");
            }
            ImGui::TreePop();
        }
        if(ImGui::TreeNode("Basic state")) {
            ImGui::Text("FocusedWindow: '%s'", g.FocusedWindow ? g.FocusedWindow->Name : "NULL");
            ImGui::Text("HoveredWindow: '%s'", g.HoveredWindow ? g.HoveredWindow->Name : "NULL");
            ImGui::Text("HoveredRootWindow: '%s'", g.HoveredRootWindow ? g.HoveredRootWindow->Name : "NULL");
            ImGui::Text("HoveredID: 0x%08X/0x%08X", g.HoveredId, g.HoveredIdPreviousFrame); // Data is "in-flight" so depending on when the Metrics window is called we may see current frame information or not
            ImGui::Text("ActiveID: 0x%08X/0x%08X", g.ActiveId, g.ActiveIdPreviousFrame);
            ImGui::TreePop();
        }
    }
    ImGui::End();
}
void ImGui::ShowStyleEditor(ImGuiStyle* ref)
{
    ImGuiStyle& style = ImGui::GetStyle();

    // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to (else it compares to the default style)
    const ImGuiStyle default_style; // Default style
    if(ImGui::Button("Revert Style"))
        style = ref ? *ref : default_style;

    if(ref) {
        ImGui::SameLine();
        if(ImGui::Button("Save Style"))
            *ref = style;
    }

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.55f);

    if(ImGui::TreeNode("Rendering")) {
        ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
        ImGui::Checkbox("Anti-aliased shapes", &style.AntiAliasedShapes);
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, NULL, 2.0f);
        if(style.CurveTessellationTol < 0.0f) style.CurveTessellationTol = 0.10f;
        ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a Toggle to switch between zero and non-zero.
        ImGui::PopItemWidth();
        ImGui::TreePop();
    }

    if(ImGui::TreeNode("Settings")) {
        ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat("ChildWindowRounding", &style.ChildWindowRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
        ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
        ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 16.0f, "%.0f");
        ImGui::Text("Alignment");
        ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
        ImGui::TreePop();
    }

    if(ImGui::TreeNode("Colors")) {
        static int output_dest = 0;
        static bool output_only_modified = false;
        if(ImGui::Button("Copy Colors")) {
            //for(int i = 0; i < ImGuiCol_COUNT; i++) {
            //  const ImVec4& col = style.Colors[i];
            //  const char* name = ImGui::GetStyleColName(i);
            //  if(!output_only_modified || memcmp(&col, (ref ? &ref->Colors[i] : &default_style.Colors[i]), sizeof(ImVec4)) != 0)
            //    ImGui::LogText("style.Colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 22 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
            //}
        }
        ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0"); ImGui::PopItemWidth();
        ImGui::SameLine(); ImGui::Checkbox("Only Modified Fields", &output_only_modified);

        static ImGuiColorEditMode edit_mode = ImGuiColorEditMode_RGB;
        ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditMode_RGB);
        ImGui::SameLine();
        ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditMode_HSV);
        ImGui::SameLine();
        ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditMode_HEX);
        //ImGui::Text("Tip: Click on colored square to change edit mode.");

        static ImGuiTextFilter filter;
        filter.Draw("Filter colors", 200);

        ImGui::BeginChild("#colors", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::PushItemWidth(-160);
        ImGui::ColorEditMode(edit_mode);
        for(int i = 0; i < ImGuiCol_COUNT; i++) {
            const char* name = ImGui::GetStyleColName(i);
            if(!filter.PassFilter(name))
                continue;
            ImGui::PushID(i);
            ImGui::ColorEdit4(name, (float*)&style.Colors[i], true);
            if(memcmp(&style.Colors[i], (ref ? &ref->Colors[i] : &default_style.Colors[i]), sizeof(ImVec4)) != 0) {
                ImGui::SameLine(); if(ImGui::Button("Revert")) style.Colors[i] = ref ? ref->Colors[i] : default_style.Colors[i];
                if(ref) { ImGui::SameLine(); if(ImGui::Button("Save")) ref->Colors[i] = style.Colors[i]; }
            }
            ImGui::PopID();
        }
        ImGui::PopItemWidth();
        ImGui::EndChild();

        ImGui::TreePop();
    }

    if(ImGui::TreeNode("Fonts", "Fonts (%d)", ImGui::GetIO().Fonts->Fonts.Size)) {
        ImGui::SameLine();
        ImFontAtlas* atlas = ImGui::GetIO().Fonts;
        if(ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight)) {
            ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
            ImGui::TreePop();
        }
        ImGui::PushItemWidth(100);
        for(int i = 0; i < atlas->Fonts.Size; i++) {
            ImFont* font = atlas->Fonts[i];
            ImGui::BulletText("Font %d: \'%s\', %.2f px, %d glyphs", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size);
            ImGui::TreePush((void*)(intptr_t)i);
            ImGui::SameLine(); if(ImGui::SmallButton("Set as default")) ImGui::GetIO().FontDefault = font;
            ImGui::PushFont(font);
            ImGui::Text("The quick brown fox jumps over the lazy dog");
            ImGui::PopFont();
            if(ImGui::TreeNode("Details")) {
                ImGui::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
                ImGui::SameLine();
                ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
                ImGui::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
                for(int config_i = 0; config_i < font->ConfigDataCount; config_i++) {
                    ImFontConfig* cfg = &font->ConfigData[config_i];
                    ImGui::BulletText("Input %d: \'%s\'\nOversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
                }
                if(ImGui::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size)) {
                    // Display all glyphs of the fonts in separate pages of 256 characters
                    const ImFont::Glyph* glyph_fallback = font->FallbackGlyph; // Forcefully/dodgily make FindGlyph() return NULL on fallback, which isn't the default behavior.
                    font->FallbackGlyph = NULL;
                    for(int base = 0; base < 0x10000; base += 256) {
                        int count = 0;
                        for(int n = 0; n < 256; n++)
                            count += font->FindGlyph((ImWchar)(base + n)) ? 1 : 0;
                        if(count > 0 && ImGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph")) {
                            float cell_spacing = style.ItemSpacing.y;
                            ImVec2 cell_size(font->FontSize * 1, font->FontSize * 1);
                            ImVec2 base_pos = ImGui::GetCursorScreenPos();
                            ImDrawList* draw_list = ImGui::GetWindowDrawList();
                            for(int n = 0; n < 256; n++) {
                                ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size.x + cell_spacing), base_pos.y + (n / 16) * (cell_size.y + cell_spacing));
                                ImVec2 cell_p2(cell_p1.x + cell_size.x, cell_p1.y + cell_size.y);
                                const ImFont::Glyph* glyph = font->FindGlyph((ImWchar)(base + n));;
                                draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                                font->RenderChar(draw_list, cell_size.x, cell_p1, ImGui::GetColorU32(ImGuiCol_Text), (ImWchar)(base + n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
                                if(glyph && ImGui::IsMouseHoveringRect(cell_p1, cell_p2)) {
                                    ImGui::BeginTooltip();
                                    ImGui::Text("Codepoint: U+%04X", base + n);
                                    ImGui::Separator();
                                    ImGui::Text("XAdvance+1: %.1f", glyph->XAdvance);
                                    ImGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
                                    ImGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
                                    ImGui::EndTooltip();
                                }
                            }
                            ImGui::Dummy(ImVec2((cell_size.x + cell_spacing) * 16, (cell_size.y + cell_spacing) * 16));
                            ImGui::TreePop();
                        }
                    }
                    font->FallbackGlyph = glyph_fallback;
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        static float window_scale = 1.0f;
        ImGui::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.1f");              // scale only this window
        ImGui::DragFloat("global scale", &ImGui::GetIO().FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.1f"); // scale everything
        ImGui::PopItemWidth();
        ImGui::SetWindowFontScale(window_scale);
        ImGui::TreePop();
    }

    ImGui::PopItemWidth();
}
#define STBTT_malloc(x,u)  ((void)(u), ImGui::MemAlloc(x))
#define STBTT_free(x,u)    ((void)(u), ImGui::MemFree(x))
#define STBTT_assert(x)    IM_ASSERT(x)
#ifndef IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#else
#define STBTT_DEF extern
#endif
#include "stb_truetype.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef IMGUI_STB_NAMESPACE
} // namespace ImGuiStb
using namespace IMGUI_STB_NAMESPACE;
#endif

//-----------------------------------------------------------------------------
// ImDrawList
//-----------------------------------------------------------------------------

static const ImVec4 GNullClipRect(-8192.0f, -8192.0f, +8192.0f, +8192.0f); // Large values that are easy to encode in a few bits+shift

void ImDrawList::Clear()
{
void ImDrawList::ClearFreeMemory()
{
    CmdBuffer.clear();
    IdxBuffer.clear();
    VtxBuffer.clear();
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.clear();
    _TextureIdStack.clear();
    _Path.clear();
    _ChannelsCurrent = 0;
    _ChannelsCount = 1;
    for(int i = 0; i < _Channels.Size; i++) {
        if(i == 0) memset(&_Channels[0], 0, sizeof(_Channels[0]));  // channel 0 is a copy of CmdBuffer/IdxBuffer, don't destruct again
        _Channels[i].CmdBuffer.clear();
        _Channels[i].IdxBuffer.clear();
    }
    _Channels.clear();
}
void ImDrawList::AddDrawCmd()
{
    ImDrawCmd draw_cmd;
    draw_cmd.ClipRect = GetCurrentClipRect();
    draw_cmd.TextureId = GetCurrentTextureId();

    IM_ASSERT(draw_cmd.ClipRect.x <= draw_cmd.ClipRect.z && draw_cmd.ClipRect.y <= draw_cmd.ClipRect.w);
    CmdBuffer.push_back(draw_cmd);
}
void ImDrawList::AddCallback(ImDrawCallback callback, void* callback_data)
{
    ImDrawCmd* current_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if(!current_cmd || current_cmd->ElemCount != 0 || current_cmd->UserCallback != NULL) {
        AddDrawCmd();
        current_cmd = &CmdBuffer.back();
    }
    current_cmd->UserCallback = callback;
    current_cmd->UserCallbackData = callback_data;

    AddDrawCmd(); // Force a new command after us (see comment below)
}
void ImDrawList::UpdateClipRect()
{
    // If current command is used with different settings we need to add a new command
    const ImVec4 curr_clip_rect = GetCurrentClipRect();
    ImDrawCmd* curr_cmd = CmdBuffer.Size > 0 ? &CmdBuffer.Data[CmdBuffer.Size - 1] : NULL;
    if(!curr_cmd || (curr_cmd->ElemCount != 0 && memcmp(&curr_cmd->ClipRect, &curr_clip_rect, sizeof(ImVec4)) != 0) || curr_cmd->UserCallback != NULL) {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if(curr_cmd->ElemCount == 0 && prev_cmd && memcmp(&prev_cmd->ClipRect, &curr_clip_rect, sizeof(ImVec4)) == 0 && prev_cmd->TextureId == GetCurrentTextureId() && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->ClipRect = curr_clip_rect;
}
void ImDrawList::UpdateTextureID()
{
    // If current command is used with different settings we need to add a new command
    const ImTextureID curr_texture_id = GetCurrentTextureId();
    ImDrawCmd* curr_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if(!curr_cmd || (curr_cmd->ElemCount != 0 && curr_cmd->TextureId != curr_texture_id) || curr_cmd->UserCallback != NULL) {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if(prev_cmd && prev_cmd->TextureId == curr_texture_id && memcmp(&prev_cmd->ClipRect, &GetCurrentClipRect(), sizeof(ImVec4)) == 0 && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->TextureId = curr_texture_id;
}
void ImDrawList::PushClipRect(ImVec2 cr_min, ImVec2 cr_max, bool intersect_with_current_clip_rect)
{
    ImVec4 cr(cr_min.x, cr_min.y, cr_max.x, cr_max.y);
    if(intersect_with_current_clip_rect && _ClipRectStack.Size) {
        ImVec4 current = _ClipRectStack.Data[_ClipRectStack.Size - 1];
        if(cr.x < current.x) cr.x = current.x;
        if(cr.y < current.y) cr.y = current.y;
        if(cr.z > current.z) cr.z = current.z;
        if(cr.w > current.w) cr.w = current.w;
    }
    cr.z = ImMax(cr.x, cr.z);
    cr.w = ImMax(cr.y, cr.w);

    _ClipRectStack.push_back(cr);
    UpdateClipRect();
}
void ImDrawList::PushClipRectFullScreen()
{
    PushClipRect(ImVec2(GNullClipRect.x, GNullClipRect.y), ImVec2(GNullClipRect.z, GNullClipRect.w));
    //PushClipRect(GetVisibleRect());   // FIXME-OPT: This would be more correct but we're not supposed to access ImGuiContext from here?
}
void ImDrawList::PopClipRect()
{
    IM_ASSERT(_ClipRectStack.Size > 0);
    _ClipRectStack.pop_back();
    UpdateClipRect();
}
void ImDrawList::PushTextureID(const ImTextureID& texture_id)
{
    _TextureIdStack.push_back(texture_id);
    UpdateTextureID();
}
void ImDrawList::PopTextureID()
{
    IM_ASSERT(_TextureIdStack.Size > 0);
    _TextureIdStack.pop_back();
    UpdateTextureID();
}
void ImDrawList::ChannelsSplit(int channels_count)
{
    IM_ASSERT(_ChannelsCurrent == 0 && _ChannelsCount == 1);
    int old_channels_count = _Channels.Size;
    if(old_channels_count < channels_count)
        _Channels.resize(channels_count);
    _ChannelsCount = channels_count;

    // _Channels[] (24 bytes each) hold storage that we'll swap with this->_CmdBuffer/_IdxBuffer
    // The content of _Channels[0] at this point doesn't matter. We clear it to make state tidy in a debugger but we don't strictly need to.
    // When we switch to the next channel, we'll copy _CmdBuffer/_IdxBuffer into _Channels[0] and then _Channels[1] into _CmdBuffer/_IdxBuffer
    memset(&_Channels[0], 0, sizeof(ImDrawChannel));
    for(int i = 1; i < channels_count; i++) {
        if(i >= old_channels_count) {
            IM_PLACEMENT_NEW(&_Channels[i]) ImDrawChannel();
        } else {
            _Channels[i].CmdBuffer.resize(0);
            _Channels[i].IdxBuffer.resize(0);
        }
        if(_Channels[i].CmdBuffer.Size == 0) {
            ImDrawCmd draw_cmd;
            draw_cmd.ClipRect = _ClipRectStack.back();
            draw_cmd.TextureId = _TextureIdStack.back();
            _Channels[i].CmdBuffer.push_back(draw_cmd);
        }
    }
}
void ImDrawList::ChannelsMerge()
{
    // Note that we never use or rely on channels.Size because it is merely a buffer that we never shrink back to 0 to keep all sub-buffers ready for use.
    if(_ChannelsCount <= 1)
        return;

    ChannelsSetCurrent(0);
    if(CmdBuffer.Size && CmdBuffer.back().ElemCount == 0)
        CmdBuffer.pop_back();

    int new_cmd_buffer_count = 0, new_idx_buffer_count = 0;
    for(int i = 1; i < _ChannelsCount; i++) {
        ImDrawChannel& ch = _Channels[i];
        if(ch.CmdBuffer.Size && ch.CmdBuffer.back().ElemCount == 0)
            ch.CmdBuffer.pop_back();
        new_cmd_buffer_count += ch.CmdBuffer.Size;
        new_idx_buffer_count += ch.IdxBuffer.Size;
    }
    CmdBuffer.resize(CmdBuffer.Size + new_cmd_buffer_count);
    IdxBuffer.resize(IdxBuffer.Size + new_idx_buffer_count);

    ImDrawCmd* cmd_write = CmdBuffer.Data + CmdBuffer.Size - new_cmd_buffer_count;
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size - new_idx_buffer_count;
    for(int i = 1; i < _ChannelsCount; i++) {
        ImDrawChannel& ch = _Channels[i];
        if(int sz = ch.CmdBuffer.Size) { memcpy(cmd_write, ch.CmdBuffer.Data, sz * sizeof(ImDrawCmd)); cmd_write += sz; }
        if(int sz = ch.IdxBuffer.Size) { memcpy(_IdxWritePtr, ch.IdxBuffer.Data, sz * sizeof(ImDrawIdx)); _IdxWritePtr += sz; }
    }
    AddDrawCmd();
    _ChannelsCount = 1;
}
void ImDrawList::ChannelsSetCurrent(int idx)
{
    IM_ASSERT(idx < _ChannelsCount);
    if(_ChannelsCurrent == idx) return;
    memcpy(&_Channels.Data[_ChannelsCurrent].CmdBuffer, &CmdBuffer, sizeof(CmdBuffer)); // copy 12 bytes, four times
    memcpy(&_Channels.Data[_ChannelsCurrent].IdxBuffer, &IdxBuffer, sizeof(IdxBuffer));
    _ChannelsCurrent = idx;
    memcpy(&CmdBuffer, &_Channels.Data[_ChannelsCurrent].CmdBuffer, sizeof(CmdBuffer));
    memcpy(&IdxBuffer, &_Channels.Data[_ChannelsCurrent].IdxBuffer, sizeof(IdxBuffer));
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size;
}
void ImDrawList::PrimReserve(int idx_count, int vtx_count)
{
    ImDrawCmd& draw_cmd = CmdBuffer.Data[CmdBuffer.Size - 1];
    draw_cmd.ElemCount += idx_count;

    int vtx_buffer_size = VtxBuffer.Size;
    VtxBuffer.resize(vtx_buffer_size + vtx_count);
    _VtxWritePtr = VtxBuffer.Data + vtx_buffer_size;

    int idx_buffer_size = IdxBuffer.Size;
    IdxBuffer.resize(idx_buffer_size + idx_count);
    _IdxWritePtr = IdxBuffer.Data + idx_buffer_size;
}
void ImDrawList::PrimRect(const ImVec2& a, const ImVec2& c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv(GImGui->FontTexUvWhitePixel);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}
void ImDrawList::PrimRectUV(const ImVec2& a, const ImVec2& c, const ImVec2& uv_a, const ImVec2& uv_c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}
void ImDrawList::PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col)
{
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}
void ImDrawList::AddPolyline(const ImVec2* points, const int points_count, ImU32 col, bool closed, float thickness, bool anti_aliased)
{
    if(points_count < 2)
        return;

    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    anti_aliased &= GImGui->Style->AntiAliasedLines;
    //if (ImGui::GetIO().KeyCtrl) anti_aliased = false; // Debug

    int count = points_count;
    if(!closed)
        count = points_count - 1;

    const bool thick_line = thickness > 1.0f;
    if(anti_aliased) {
        // Anti-aliased stroke
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & IM_COL32(255, 255, 255, 0);

        const int idx_count = thick_line ? count * 18 : count * 12;
        const int vtx_count = thick_line ? points_count * 4 : points_count * 3;
        PrimReserve(idx_count, vtx_count);

        // Temporary buffer
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(ImVec2));
        ImVec2* temp_points = temp_normals + points_count;

        for(int i1 = 0; i1 < count; i1++) {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            ImVec2 diff = points[i2] - points[i1];
            diff *= ImInvLength(diff, 1.0f);
            temp_normals[i1].x = diff.y;
            temp_normals[i1].y = -diff.x;
        }
        if(!closed)
            temp_normals[points_count - 1] = temp_normals[points_count - 2];

        if(!thick_line) {
            if(!closed) {
                temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
                temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
                temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * AA_SIZE;
                temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] - temp_normals[points_count - 1] * AA_SIZE;
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for(int i1 = 0; i1 < count; i1++) {
                const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
                unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 3;

                // Average normals
                ImVec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
                float dmr2 = dm.x*dm.x + dm.y*dm.y;
                if(dmr2 > 0.000001f) {
                    float scale = 1.0f / dmr2;
                    if(scale > 100.0f) scale = 100.0f;
                    dm *= scale;
                }
                dm *= AA_SIZE;
                temp_points[i2 * 2 + 0] = points[i2] + dm;
                temp_points[i2 * 2 + 1] = points[i2] - dm;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[1] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[2] = (ImDrawIdx)(idx1 + 2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[4] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx2 + 0);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[7] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[8] = (ImDrawIdx)(idx1 + 0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[10] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[11] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr += 12;

                idx1 = idx2;
            }

            // Add vertexes
            for(int i = 0; i < points_count; i++) {
                _VtxWritePtr[0].pos = points[i];          _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
                _VtxWritePtr[1].pos = temp_points[i * 2 + 0]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;
                _VtxWritePtr[2].pos = temp_points[i * 2 + 1]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col_trans;
                _VtxWritePtr += 3;
            }
        } else {
            const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
            if(!closed) {
                temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
                temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
                temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count - 1) * 4 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count - 1) * 4 + 1] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness);
                temp_points[(points_count - 1) * 4 + 2] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness);
                temp_points[(points_count - 1) * 4 + 3] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for(int i1 = 0; i1 < count; i1++) {
                const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
                unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 4;

                // Average normals
                ImVec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
                float dmr2 = dm.x*dm.x + dm.y*dm.y;
                if(dmr2 > 0.000001f) {
                    float scale = 1.0f / dmr2;
                    if(scale > 100.0f) scale = 100.0f;
                    dm *= scale;
                }
                ImVec2 dm_out = dm * (half_inner_thickness + AA_SIZE);
                ImVec2 dm_in = dm * half_inner_thickness;
                temp_points[i2 * 4 + 0] = points[i2] + dm_out;
                temp_points[i2 * 4 + 1] = points[i2] + dm_in;
                temp_points[i2 * 4 + 2] = points[i2] - dm_in;
                temp_points[i2 * 4 + 3] = points[i2] - dm_out;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[1] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx1 + 2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[4] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[7] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[8] = (ImDrawIdx)(idx1 + 0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[10] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[11] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr[12] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[13] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[14] = (ImDrawIdx)(idx1 + 3);
                _IdxWritePtr[15] = (ImDrawIdx)(idx1 + 3); _IdxWritePtr[16] = (ImDrawIdx)(idx2 + 3); _IdxWritePtr[17] = (ImDrawIdx)(idx2 + 2);
                _IdxWritePtr += 18;

                idx1 = idx2;
            }

            // Add vertexes
            for(int i = 0; i < points_count; i++) {
                _VtxWritePtr[0].pos = temp_points[i * 4 + 0]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col_trans;
                _VtxWritePtr[1].pos = temp_points[i * 4 + 1]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
                _VtxWritePtr[2].pos = temp_points[i * 4 + 2]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
                _VtxWritePtr[3].pos = temp_points[i * 4 + 3]; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col_trans;
                _VtxWritePtr += 4;
            }
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    } else {
        // Non Anti-aliased Stroke
        const int idx_count = count * 6;
        const int vtx_count = count * 4;      // FIXME-OPT: Not sharing edges
        PrimReserve(idx_count, vtx_count);

        for(int i1 = 0; i1 < count; i1++) {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            const ImVec2& p1 = points[i1];
            const ImVec2& p2 = points[i2];
            ImVec2 diff = p2 - p1;
            diff *= ImInvLength(diff, 1.0f);

            const float dx = diff.x * (thickness * 0.5f);
            const float dy = diff.y * (thickness * 0.5f);
            _VtxWritePtr[0].pos.x = p1.x + dy; _VtxWritePtr[0].pos.y = p1.y - dx; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr[1].pos.x = p2.x + dy; _VtxWritePtr[1].pos.y = p2.y - dx; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
            _VtxWritePtr[2].pos.x = p2.x - dy; _VtxWritePtr[2].pos.y = p2.y + dx; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
            _VtxWritePtr[3].pos.x = p1.x - dy; _VtxWritePtr[3].pos.y = p1.y + dx; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
            _VtxWritePtr += 4;

            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + 1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + 2);
            _IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx + 2); _IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx + 3);
            _IdxWritePtr += 6;
            _VtxCurrentIdx += 4;
        }
    }
}
void ImDrawList::AddConvexPolyFilled(const ImVec2* points, const int points_count, ImU32 col, bool anti_aliased)
{
    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    anti_aliased &= GImGui->Style->AntiAliasedShapes;
    //if (ImGui::GetIO().KeyCtrl) anti_aliased = false; // Debug

    if(anti_aliased) {
        // Anti-aliased Fill
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & IM_COL32(255, 255, 255, 0);
        const int idx_count = (points_count - 2) * 3 + points_count * 6;
        const int vtx_count = (points_count * 2);
        PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = _VtxCurrentIdx;
        unsigned int vtx_outer_idx = _VtxCurrentIdx + 1;
        for(int i = 2; i < points_count; i++) {
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
            _IdxWritePtr += 3;
        }

        // Compute normals
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2));
        for(int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            ImVec2 diff = p1 - p0;
            diff *= ImInvLength(diff, 1.0f);
            temp_normals[i0].x = diff.y;
            temp_normals[i0].y = -diff.x;
        }

        for(int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
            // Average normals
            const ImVec2& n0 = temp_normals[i0];
            const ImVec2& n1 = temp_normals[i1];
            ImVec2 dm = (n0 + n1) * 0.5f;
            float dmr2 = dm.x*dm.x + dm.y*dm.y;
            if(dmr2 > 0.000001f) {
                float scale = 1.0f / dmr2;
                if(scale > 100.0f) scale = 100.0f;
                dm *= scale;
            }
            dm *= AA_SIZE * 0.5f;

            // Add vertices
            _VtxWritePtr[0].pos = (points[i1] - dm); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
            _VtxWritePtr[1].pos = (points[i1] + dm); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
            _VtxWritePtr += 2;

            // Add indexes for fringes
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1)); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + (i0 << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
            _IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1)); _IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx + (i1 << 1)); _IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
            _IdxWritePtr += 6;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    } else {
        // Non Anti-aliased Fill
        const int idx_count = (points_count - 2) * 3;
        const int vtx_count = points_count;
        PrimReserve(idx_count, vtx_count);
        for(int i = 0; i < vtx_count; i++) {
            _VtxWritePtr[0].pos = points[i]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr++;
        }
        for(int i = 2; i < points_count; i++) {
            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + i - 1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + i);
            _IdxWritePtr += 3;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
}
void ImDrawList::PathArcToFast(const ImVec2& centre, float radius, int amin, int amax)
{
    static ImVec2 circle_vtx[12];
    static bool circle_vtx_builds = false;
    const int circle_vtx_count = IM_ARRAYSIZE(circle_vtx);
    if(!circle_vtx_builds) {
        for(int i = 0; i < circle_vtx_count; i++) {
            const float a = ((float)i / (float)circle_vtx_count) * 2 * IM_PI;
            circle_vtx[i].x = cosf(a);
            circle_vtx[i].y = sinf(a);
        }
        circle_vtx_builds = true;
    }

    if(amin > amax) return;
    if(radius == 0.0f) {
        _Path.push_back(centre);
    } else {
        _Path.reserve(_Path.Size + (amax - amin + 1));
        for(int a = amin; a <= amax; a++) {
            const ImVec2& c = circle_vtx[a % circle_vtx_count];
            _Path.push_back(ImVec2(centre.x + c.x * radius, centre.y + c.y * radius));
        }
    }
}
void ImDrawList::PathArcTo(const ImVec2& centre, float radius, float amin, float amax, int num_segments)
{
    if(radius == 0.0f)
        _Path.push_back(centre);
    _Path.reserve(_Path.Size + (num_segments + 1));
    for(int i = 0; i <= num_segments; i++) {
        const float a = amin + ((float)i / (float)num_segments) * (amax - amin);
        _Path.push_back(ImVec2(centre.x + cosf(a) * radius, centre.y + sinf(a) * radius));
    }
}
static void PathBezierToCasteljau(ImVector<ImVec2>* path, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
{
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = ((x2 - x4) * dy - (y2 - y4) * dx);
    float d3 = ((x3 - x4) * dy - (y3 - y4) * dx);
    d2 = (d2 >= 0) ? d2 : -d2;
    d3 = (d3 >= 0) ? d3 : -d3;
    if((d2 + d3) * (d2 + d3) < tess_tol * (dx*dx + dy*dy)) {
        path->push_back(ImVec2(x4, y4));
    } else if(level < 10) {
        float x12 = (x1 + x2)*0.5f, y12 = (y1 + y2)*0.5f;
        float x23 = (x2 + x3)*0.5f, y23 = (y2 + y3)*0.5f;
        float x34 = (x3 + x4)*0.5f, y34 = (y3 + y4)*0.5f;
        float x123 = (x12 + x23)*0.5f, y123 = (y12 + y23)*0.5f;
        float x234 = (x23 + x34)*0.5f, y234 = (y23 + y34)*0.5f;
        float x1234 = (x123 + x234)*0.5f, y1234 = (y123 + y234)*0.5f;

        PathBezierToCasteljau(path, x1, y1, x12, y12, x123, y123, x1234, y1234, tess_tol, level + 1);
        PathBezierToCasteljau(path, x1234, y1234, x234, y234, x34, y34, x4, y4, tess_tol, level + 1);
    }
}
void ImDrawList::PathBezierCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments)
{
    ImVec2 p1 = _Path.back();
    if(num_segments == 0) {
        // Auto-tessellated
        PathBezierToCasteljau(&_Path, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, GImGui->Style->CurveTessellationTol, 0);
    } else {
        float t_step = 1.0f / (float)num_segments;
        for(int i_step = 1; i_step <= num_segments; i_step++) {
            float t = t_step * i_step;
            float u = 1.0f - t;
            float w1 = u*u*u;
            float w2 = 3 * u*u*t;
            float w3 = 3 * u*t*t;
            float w4 = t*t*t;
            _Path.push_back(ImVec2(w1*p1.x + w2*p2.x + w3*p3.x + w4*p4.x, w1*p1.y + w2*p2.y + w3*p3.y + w4*p4.y));
        }
    }
}
void ImDrawList::PathRect(const ImVec2& a, const ImVec2& b, float rounding, int rounding_corners)
{
    float r = rounding;
    r = ImMin(r, fabsf(b.x - a.x) * (((rounding_corners&(1 | 2)) == (1 | 2)) || ((rounding_corners&(4 | 8)) == (4 | 8)) ? 0.5f : 1.0f) - 1.0f);
    r = ImMin(r, fabsf(b.y - a.y) * (((rounding_corners&(1 | 8)) == (1 | 8)) || ((rounding_corners&(2 | 4)) == (2 | 4)) ? 0.5f : 1.0f) - 1.0f);

    if(r <= 0.0f || rounding_corners == 0) {
        PathLineTo(a);
        PathLineTo(ImVec2(b.x, a.y));
        PathLineTo(b);
        PathLineTo(ImVec2(a.x, b.y));
    } else {
        const float r0 = (rounding_corners & 1) ? r : 0.0f;
        const float r1 = (rounding_corners & 2) ? r : 0.0f;
        const float r2 = (rounding_corners & 4) ? r : 0.0f;
        const float r3 = (rounding_corners & 8) ? r : 0.0f;
        PathArcToFast(ImVec2(a.x + r0, a.y + r0), r0, 6, 9);
        PathArcToFast(ImVec2(b.x - r1, a.y + r1), r1, 9, 12);
        PathArcToFast(ImVec2(b.x - r2, b.y - r2), r2, 0, 3);
        PathArcToFast(ImVec2(a.x + r3, b.y - r3), r3, 3, 6);
    }
}
void ImDrawList::AddLine(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(a + ImVec2(0.5f, 0.5f));
    PathLineTo(b + ImVec2(0.5f, 0.5f));
    PathStroke(col, false, thickness);
}
void ImDrawList::AddRect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners_flags, float thickness)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;
    PathRect(a + ImVec2(0.5f, 0.5f), b - ImVec2(0.5f, 0.5f), rounding, rounding_corners_flags);
    PathStroke(col, true, thickness);
}
void ImDrawList::AddRectFilled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners_flags)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;
    if(rounding > 0.0f) {
        PathRect(a, b, rounding, rounding_corners_flags);
        PathFill(col);
    } else {
        PrimReserve(6, 4);
        PrimRect(a, b, col);
    }
}
void ImDrawList::AddRectFilledMultiColor(const ImVec2& a, const ImVec2& c, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{
    if(((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    PrimReserve(6, 4);
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 1)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 2));
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 2)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 3));
    PrimWriteVtx(a, uv, col_upr_left);
    PrimWriteVtx(ImVec2(c.x, a.y), uv, col_upr_right);
    PrimWriteVtx(c, uv, col_bot_right);
    PrimWriteVtx(ImVec2(a.x, c.y), uv, col_bot_left);
}
void ImDrawList::AddQuad(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col, float thickness)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathLineTo(d);
    PathStroke(col, true, thickness);
}
void ImDrawList::AddQuadFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathLineTo(d);
    PathFill(col);
}
void ImDrawList::AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col, float thickness)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathStroke(col, true, thickness);
}
void ImDrawList::AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathFill(col);
}
void ImDrawList::AddCircle(const ImVec2& centre, float radius, ImU32 col, int num_segments, float thickness)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;

    const float a_max = IM_PI*2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius - 0.5f, 0.0f, a_max, num_segments);
    PathStroke(col, true, thickness);
}
void ImDrawList::AddCircleFilled(const ImVec2& centre, float radius, ImU32 col, int num_segments)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;

    const float a_max = IM_PI*2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius, 0.0f, a_max, num_segments);
    PathFill(col);
}
void ImDrawList::AddBezierCurve(const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, ImU32 col, float thickness, int num_segments)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(pos0);
    PathBezierCurveTo(cp0, cp1, pos1, num_segments);
    PathStroke(col, false, thickness);
}
void ImDrawList::AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;

    if(text_end == NULL)
        text_end = text_begin + strlen(text_begin);
    if(text_begin == text_end)
        return;

    // Note: This is one of the few instance of breaking the encapsulation of ImDrawList, as we pull this from ImGui state, but it is just SO useful.
    // Might just move Font/FontSize to ImDrawList?
    if(font == NULL)
        font = GImGui->Font;
    if(font_size == 0.0f)
        font_size = GImGui->FontSize;

    IM_ASSERT(font->ContainerAtlas->TexID == _TextureIdStack.back());  // Use high-level ImGui::PushFont() or low-level ImDrawList::PushTextureId() to change font.

    ImVec4 clip_rect = _ClipRectStack.back();
    if(cpu_fine_clip_rect) {
        clip_rect.x = ImMax(clip_rect.x, cpu_fine_clip_rect->x);
        clip_rect.y = ImMax(clip_rect.y, cpu_fine_clip_rect->y);
        clip_rect.z = ImMin(clip_rect.z, cpu_fine_clip_rect->z);
        clip_rect.w = ImMin(clip_rect.w, cpu_fine_clip_rect->w);
    }
    font->RenderText(this, font_size, pos, col, clip_rect, text_begin, text_end, wrap_width, cpu_fine_clip_rect != NULL);
}
void ImDrawList::AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end)
{
    AddText(GImGui->Font, GImGui->FontSize, pos, col, text_begin, text_end);
}
void ImDrawList::AddImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv0, const ImVec2& uv1, ImU32 col)
{
    if((col & IM_COL32_A_MASK) == 0)
        return;

    // FIXME-OPT: This is wasting draw calls.
    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if(push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimRectUV(a, b, uv0, uv1, col);

    if(push_texture_id)
        PopTextureID();
}
void ImDrawData::DeIndexAllBuffers()
{
    ImVector<ImDrawVert> new_vtx_buffer;
    TotalVtxCount = TotalIdxCount = 0;
    for(int i = 0; i < CmdListsCount; i++) {
        ImDrawList* cmd_list = CmdLists[i];
        if(cmd_list->IdxBuffer.empty())
            continue;
        new_vtx_buffer.resize(cmd_list->IdxBuffer.Size);
        for(int j = 0; j < cmd_list->IdxBuffer.Size; j++)
            new_vtx_buffer[j] = cmd_list->VtxBuffer[cmd_list->IdxBuffer[j]];
        cmd_list->VtxBuffer.swap(new_vtx_buffer);
        cmd_list->IdxBuffer.resize(0);
        TotalVtxCount += cmd_list->VtxBuffer.Size;
    }
}
void ImDrawData::ScaleClipRects(const ImVec2& scale)
{
    for(int i = 0; i < CmdListsCount; i++) {
        ImDrawList* cmd_list = CmdLists[i];
        for(int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            ImDrawCmd* cmd = &cmd_list->CmdBuffer[cmd_i];
            cmd->ClipRect = ImVec4(cmd->ClipRect.x * scale.x, cmd->ClipRect.y * scale.y, cmd->ClipRect.z * scale.x, cmd->ClipRect.w * scale.y);
        }
    }
}
void    ImFontAtlas::ClearInputData()
{
    for(int i = 0; i < ConfigData.Size; i++)
        if(ConfigData[i].FontData && ConfigData[i].FontDataOwnedByAtlas) {
            ImGui::MemFree(ConfigData[i].FontData);
            ConfigData[i].FontData = NULL;
        }

    // When clearing this we lose access to the font name and other information used to build the font.
    for(int i = 0; i < Fonts.Size; i++)
        if(Fonts[i]->ConfigData >= ConfigData.Data && Fonts[i]->ConfigData < ConfigData.Data + ConfigData.Size) {
            Fonts[i]->ConfigData = NULL;
            Fonts[i]->ConfigDataCount = 0;
        }
    ConfigData.clear();
}
void    ImFontAtlas::ClearTexData()
{
    if(TexPixelsAlpha8)
        ImGui::MemFree(TexPixelsAlpha8);
    if(TexPixelsRGBA32)
        ImGui::MemFree(TexPixelsRGBA32);
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
}
void    ImFontAtlas::ClearFonts()
{
    for(int i = 0; i < Fonts.Size; i++) {
        Fonts[i]->~ImFont();
        ImGui::MemFree(Fonts[i]);
    }
    Fonts.clear();
}
void    ImFontAtlas::Clear()
{
    ClearInputData();
    ClearTexData();
    ClearFonts();
}
void    ImFontAtlas::GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Build atlas on demand
    if(TexPixelsAlpha8 == NULL) {
        if(ConfigData.empty())
            AddFontDefault();
        Build();
    }

    *out_pixels = TexPixelsAlpha8;
    if(out_width) *out_width = TexWidth;
    if(out_height) *out_height = TexHeight;
    if(out_bytes_per_pixel) *out_bytes_per_pixel = 1;
}
void    ImFontAtlas::GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Convert to RGBA32 format on demand
    // Although it is likely to be the most commonly used format, our font rendering is 1 channel / 8 bpp
    if(!TexPixelsRGBA32) {
        unsigned char* pixels;
        GetTexDataAsAlpha8(&pixels, NULL, NULL);
        TexPixelsRGBA32 = (unsigned int*)ImGui::MemAlloc((size_t)(TexWidth * TexHeight * 4));
        const unsigned char* src = pixels;
        unsigned int* dst = TexPixelsRGBA32;
        for(int n = TexWidth * TexHeight; n > 0; n--)
            *dst++ = IM_COL32(255, 255, 255, (unsigned int)(*src++));
    }

    *out_pixels = (unsigned char*)TexPixelsRGBA32;
    if(out_width) *out_width = TexWidth;
    if(out_height) *out_height = TexHeight;
    if(out_bytes_per_pixel) *out_bytes_per_pixel = 4;
}
static void         Decode85(const unsigned char* src, unsigned char* dst)
{
    while(*src) {
        unsigned int tmp = Decode85Byte(src[0]) + 85 * (Decode85Byte(src[1]) + 85 * (Decode85Byte(src[2]) + 85 * (Decode85Byte(src[3]) + 85 * Decode85Byte(src[4]))));
        dst[0] = ((tmp >> 0) & 0xFF); dst[1] = ((tmp >> 8) & 0xFF); dst[2] = ((tmp >> 16) & 0xFF); dst[3] = ((tmp >> 24) & 0xFF);   // We can't assume little-endianness.
        src += 5;
        dst += 4;
    }
}
ImFont* ImFontAtlas::AddFontFromMemoryTTF(void* ttf_data, int ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontData = ttf_data;
    font_cfg.FontDataSize = ttf_size;
    font_cfg.SizePixels = size_pixels;
    if(glyph_ranges)
        font_cfg.GlyphRanges = glyph_ranges;
    return AddFont(&font_cfg);
}
ImFont* ImFontAtlas::AddFontFromMemoryCompressedTTF(const void* compressed_ttf_data, int compressed_ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    const unsigned int buf_decompressed_size = stb_decompress_length((unsigned char*)compressed_ttf_data);
    unsigned char* buf_decompressed_data = (unsigned char *)ImGui::MemAlloc(buf_decompressed_size);
    stb_decompress(buf_decompressed_data, (unsigned char*)compressed_ttf_data, (unsigned int)compressed_ttf_size);

    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontDataOwnedByAtlas = true;
    return AddFontFromMemoryTTF(buf_decompressed_data, (int)buf_decompressed_size, size_pixels, &font_cfg, glyph_ranges);
}
void ImFontAtlas::RenderCustomTexData(int pass, void* p_rects)
{
    // A work of art lies ahead! (. = white layer, X = black layer, others are blank)
    // The white texels on the top left are the ones we'll use everywhere in ImGui to Render filled shapes.
    const int TEX_DATA_W = 90;
    const int TEX_DATA_H = 27;
    const char texture_data[TEX_DATA_W*TEX_DATA_H + 1] =
    {
        "..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX"
        "..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X"
        "---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X"
        "X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X"
        "XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X"
        "X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X"
        "X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX"
        "X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      "
        "X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       "
        "X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        "
        "X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         "
        "X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          "
        "X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           "
        "X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            "
        "X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           "
        "X......XXXXX-XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          "
        "X...X..X    ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          "
        "X..X X..X   -       -XXXX.XXXX-       XXXX.XXXX       ------------------------------------"
        "X.X  X..X   -       -X.......X-       X.......X       -    XX           XX    -           "
        "XX    X..X  -       - X.....X -        X.....X        -   X.X           X.X   -           "
        "      X..X          -  X...X  -         X...X         -  X..X           X..X  -           "
        "       XX           -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           "
        "------------        -    X    -           X           -X.....................X-           "
        "                    ----------------------------------- X...XXXXXXXXXXXXX...X -           "
        "                                                      -  X..X           X..X  -           "
        "                                                      -   X.X           X.X   -           "
        "                                                      -    XX           XX    -           "
    };

    ImVector<stbrp_rect>& rects = *(ImVector<stbrp_rect>*)p_rects;
    if(pass == 0) {
        // Request rectangles
        stbrp_rect r;
        memset(&r, 0, sizeof(r));
        r.w = (TEX_DATA_W * 2) + 1;
        r.h = TEX_DATA_H + 1;
        rects.push_back(r);
    } else if(pass == 1) {
        // Render/copy pixels
        const stbrp_rect& r = rects[0];
        for(int y = 0, n = 0; y < TEX_DATA_H; y++)
            for(int x = 0; x < TEX_DATA_W; x++, n++) {
                const int offset0 = (int)(r.x + x) + (int)(r.y + y) * TexWidth;
                const int offset1 = offset0 + 1 + TEX_DATA_W;
                TexPixelsAlpha8[offset0] = texture_data[n] == '.' ? 0xFF : 0x00;
                TexPixelsAlpha8[offset1] = texture_data[n] == 'X' ? 0xFF : 0x00;
            }
        const ImVec2 tex_uv_scale(1.0f / TexWidth, 1.0f / TexHeight);
        TexUvWhitePixel = ImVec2((r.x + 0.5f) * tex_uv_scale.x, (r.y + 0.5f) * tex_uv_scale.y);

        // Setup mouse cursors
        const ImVec2 cursor_datas[ImGuiMouseCursor_Count_][3] =
        {
            // Pos ........ Size ......... Offset ......
            { ImVec2(0,3),  ImVec2(12,19), ImVec2(0, 0) }, // ImGuiMouseCursor_Arrow
            { ImVec2(13,0), ImVec2(7,16),  ImVec2(4, 8) }, // ImGuiMouseCursor_TextInput
            { ImVec2(31,0), ImVec2(23,23), ImVec2(11,11) }, // ImGuiMouseCursor_Move
            { ImVec2(21,0), ImVec2(9,23), ImVec2(5,11) }, // ImGuiMouseCursor_ResizeNS
            { ImVec2(55,18),ImVec2(23, 9), ImVec2(11, 5) }, // ImGuiMouseCursor_ResizeEW
            { ImVec2(73,0), ImVec2(17,17), ImVec2(9, 9) }, // ImGuiMouseCursor_ResizeNESW
            { ImVec2(55,0), ImVec2(17,17), ImVec2(9, 9) }, // ImGuiMouseCursor_ResizeNWSE
        };

        for(int type = 0; type < ImGuiMouseCursor_Count_; type++) {
            ImGuiMouseCursorData& cursor_data = GImGui->MouseCursorData[type];
            ImVec2 pos = cursor_datas[type][0] + ImVec2((float)r.x, (float)r.y);
            const ImVec2 size = cursor_datas[type][1];
            cursor_data.Type = type;
            cursor_data.Size = size;
            cursor_data.HotOffset = cursor_datas[type][2];
            cursor_data.TexUvMin[0] = (pos)* tex_uv_scale;
            cursor_data.TexUvMax[0] = (pos + size) * tex_uv_scale;
            pos.x += TEX_DATA_W + 1;
            cursor_data.TexUvMin[1] = (pos)* tex_uv_scale;
            cursor_data.TexUvMax[1] = (pos + size) * tex_uv_scale;
        }
    }
}
void    ImFont::Clear()
{
    FontSize = 0.0f;
    DisplayOffset = ImVec2(0.0f, 1.0f);
    ConfigData = NULL;
    ConfigDataCount = 0;
    Ascent = Descent = 0.0f;
    ContainerAtlas = NULL;
    Glyphs.clear();
    FallbackGlyph = NULL;
    FallbackXAdvance = 0.0f;
    IndexXAdvance.clear();
    IndexLookup.clear();
}
void ImFont::BuildLookupTable()
{
    int max_codepoint = 0;
    for(int i = 0; i != Glyphs.Size; i++)
        max_codepoint = ImMax(max_codepoint, (int)Glyphs[i].Codepoint);

    IM_ASSERT(Glyphs.Size < 0xFFFF); // -1 is reserved
    IndexXAdvance.clear();
    IndexLookup.clear();
    GrowIndex(max_codepoint + 1);
    for(int i = 0; i < Glyphs.Size; i++) {
        int codepoint = (int)Glyphs[i].Codepoint;
        IndexXAdvance[codepoint] = Glyphs[i].XAdvance;
        IndexLookup[codepoint] = (unsigned short)i;
    }

    // Create a glyph to handle TAB
    // FIXME: Needs proper TAB handling but it needs to be contextualized (or we could arbitrary say that each string starts at "column 0" ?)
    if(FindGlyph((unsigned short)' ')) {
        if(Glyphs.back().Codepoint != '\t')   // So we can call this function multiple times
            Glyphs.resize(Glyphs.Size + 1);
        ImFont::Glyph& tab_glyph = Glyphs.back();
        tab_glyph = *FindGlyph((unsigned short)' ');
        tab_glyph.Codepoint = '\t';
        tab_glyph.XAdvance *= 4;
        IndexXAdvance[(int)tab_glyph.Codepoint] = (float)tab_glyph.XAdvance;
        IndexLookup[(int)tab_glyph.Codepoint] = (unsigned short)(Glyphs.Size - 1);
    }

    FallbackGlyph = NULL;
    FallbackGlyph = FindGlyph(FallbackChar);
    FallbackXAdvance = FallbackGlyph ? FallbackGlyph->XAdvance : 0.0f;
    for(int i = 0; i < max_codepoint + 1; i++)
        if(IndexXAdvance[i] < 0.0f)
            IndexXAdvance[i] = FallbackXAdvance;
}
void ImFont::SetFallbackChar(ImWchar c)
{
    FallbackChar = c;
    BuildLookupTable();
}
void ImFont::GrowIndex(int new_size)
{
    IM_ASSERT(IndexXAdvance.Size == IndexLookup.Size);
    int old_size = IndexLookup.Size;
    if(new_size <= old_size)
        return;
    IndexXAdvance.resize(new_size);
    IndexLookup.resize(new_size);
    for(int i = old_size; i < new_size; i++) {
        IndexXAdvance[i] = -1.0f;
        IndexLookup[i] = (unsigned short)-1;
    }
}
void ImFont::AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst)
{
    IM_ASSERT(IndexLookup.Size > 0);    // Currently this can only be called AFTER the font has been built, aka after calling ImFontAtlas::GetTexDataAs*() function.
    int index_size = IndexLookup.Size;

    if(dst < index_size && IndexLookup.Data[dst] == (unsigned short)-1 && !overwrite_dst) // 'dst' already exists
        return;
    if(src >= index_size && dst >= index_size) // both 'dst' and 'src' don't exist -> no-op
        return;

    GrowIndex(dst + 1);
    IndexLookup[dst] = (src < index_size) ? IndexLookup.Data[src] : (unsigned short)-1;
    IndexXAdvance[dst] = (src < index_size) ? IndexXAdvance.Data[src] : 1.0f;
}
void ImFont::RenderChar(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, unsigned short c) const
{
    if(c == ' ' || c == '\t' || c == '\n' || c == '\r') // Match behavior of RenderText(), those 4 codepoints are hard-coded.
        return;
    if(const Glyph* glyph = FindGlyph(c)) {
        float scale = (size >= 0.0f) ? (size / FontSize) : 1.0f;
        pos.x = (float)(int)pos.x + DisplayOffset.x;
        pos.y = (float)(int)pos.y + DisplayOffset.y;
        ImVec2 pos_tl(pos.x + glyph->X0 * scale, pos.y + glyph->Y0 * scale);
        ImVec2 pos_br(pos.x + glyph->X1 * scale, pos.y + glyph->Y1 * scale);
        draw_list->PrimReserve(6, 4);
        draw_list->PrimRectUV(pos_tl, pos_br, ImVec2(glyph->U0, glyph->V0), ImVec2(glyph->U1, glyph->V1), col);
    }
}
void ImFont::RenderText(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip) const
{
    if(!text_end)
        text_end = text_begin + strlen(text_begin); // ImGui functions generally already provides a valid text_end, so this is merely to handle direct calls.

                                                    // Align to be pixel perfect
    pos.x = (float)(int)pos.x + DisplayOffset.x;
    pos.y = (float)(int)pos.y + DisplayOffset.y;
    float x = pos.x;
    float y = pos.y;
    if(y > clip_rect.w)
        return;

    const float scale = size / FontSize;
    const float line_height = FontSize * scale;
    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    // Skip non-visible lines
    const char* s = text_begin;
    if(!word_wrap_enabled && y + line_height < clip_rect.y)
        while(s < text_end && *s != '\n')  // Fast-forward to next line
            s++;

    // Reserve vertices for remaining worse case (over-reserving is useful and easily amortized)
    const int vtx_count_max = (int)(text_end - s) * 4;
    const int idx_count_max = (int)(text_end - s) * 6;
    const int idx_expected_size = draw_list->IdxBuffer.Size + idx_count_max;
    draw_list->PrimReserve(idx_count_max, vtx_count_max);

    ImDrawVert* vtx_write = draw_list->_VtxWritePtr;
    ImDrawIdx* idx_write = draw_list->_IdxWritePtr;
    unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;

    while(s < text_end) {
        if(word_wrap_enabled) {
            // Calculate how far we can Render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if(!word_wrap_eol) {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos.x));
                if(word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if(s >= word_wrap_eol) {
                x = pos.x;
                y += line_height;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while(s < text_end) {
                    const char c = *s;
                    if(ImCharIsSpace(c)) { s++; } else if(c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        unsigned int c = (unsigned int)*s;
        if(c < 0x80) {
            s += 1;
        } else {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if(c == 0) // Malformed UTF-8?
                break;
        }

        if(c < 32) {
            if(c == '\n') {
                x = pos.x;
                y += line_height;

                if(y > clip_rect.w)
                    break;
                if(!word_wrap_enabled && y + line_height < clip_rect.y)
                    while(s < text_end && *s != '\n')  // Fast-forward to next line
                        s++;
                continue;
            }
            if(c == '\r')
                continue;
        }

        float char_width = 0.0f;
        if(const Glyph* glyph = FindGlyph((unsigned short)c)) {
            char_width = glyph->XAdvance * scale;

            // Arbitrarily assume that both space and tabs are empty glyphs as an optimization
            if(c != ' ' && c != '\t') {
                // We don't do a second finer clipping test on the Y axis as we've already skipped anything before clip_rect.y and exit once we pass clip_rect.w
                float x1 = x + glyph->X0 * scale;
                float x2 = x + glyph->X1 * scale;
                float y1 = y + glyph->Y0 * scale;
                float y2 = y + glyph->Y1 * scale;
                if(x1 <= clip_rect.z && x2 >= clip_rect.x) {
                    // Render a character
                    float u1 = glyph->U0;
                    float v1 = glyph->V0;
                    float u2 = glyph->U1;
                    float v2 = glyph->V1;

                    // CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads.
                    if(cpu_fine_clip) {
                        if(x1 < clip_rect.x) {
                            u1 = u1 + (1.0f - (x2 - clip_rect.x) / (x2 - x1)) * (u2 - u1);
                            x1 = clip_rect.x;
                        }
                        if(y1 < clip_rect.y) {
                            v1 = v1 + (1.0f - (y2 - clip_rect.y) / (y2 - y1)) * (v2 - v1);
                            y1 = clip_rect.y;
                        }
                        if(x2 > clip_rect.z) {
                            u2 = u1 + ((clip_rect.z - x1) / (x2 - x1)) * (u2 - u1);
                            x2 = clip_rect.z;
                        }
                        if(y2 > clip_rect.w) {
                            v2 = v1 + ((clip_rect.w - y1) / (y2 - y1)) * (v2 - v1);
                            y2 = clip_rect.w;
                        }
                        if(y1 >= y2) {
                            x += char_width;
                            continue;
                        }
                    }

                    // We are NOT calling PrimRectUV() here because non-inlined causes too much overhead in a debug build.
                    // Inlined here:
                    {
                        idx_write[0] = (ImDrawIdx)(vtx_current_idx); idx_write[1] = (ImDrawIdx)(vtx_current_idx + 1); idx_write[2] = (ImDrawIdx)(vtx_current_idx + 2);
                        idx_write[3] = (ImDrawIdx)(vtx_current_idx); idx_write[4] = (ImDrawIdx)(vtx_current_idx + 2); idx_write[5] = (ImDrawIdx)(vtx_current_idx + 3);
                        vtx_write[0].pos.x = x1; vtx_write[0].pos.y = y1; vtx_write[0].col = col; vtx_write[0].uv.x = u1; vtx_write[0].uv.y = v1;
                        vtx_write[1].pos.x = x2; vtx_write[1].pos.y = y1; vtx_write[1].col = col; vtx_write[1].uv.x = u2; vtx_write[1].uv.y = v1;
                        vtx_write[2].pos.x = x2; vtx_write[2].pos.y = y2; vtx_write[2].col = col; vtx_write[2].uv.x = u2; vtx_write[2].uv.y = v2;
                        vtx_write[3].pos.x = x1; vtx_write[3].pos.y = y2; vtx_write[3].col = col; vtx_write[3].uv.x = u1; vtx_write[3].uv.y = v2;
                        vtx_write += 4;
                        vtx_current_idx += 4;
                        idx_write += 6;
                    }
                }
            }
        }

        x += char_width;
    }

    // Give back unused vertices
    draw_list->VtxBuffer.resize((int)(vtx_write - draw_list->VtxBuffer.Data));
    draw_list->IdxBuffer.resize((int)(idx_write - draw_list->IdxBuffer.Data));
    draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 1].ElemCount -= (idx_expected_size - draw_list->IdxBuffer.Size);
    draw_list->_VtxWritePtr = vtx_write;
    draw_list->_IdxWritePtr = idx_write;
    draw_list->_VtxCurrentIdx = (unsigned int)draw_list->VtxBuffer.Size;
}
static void stb__match(unsigned char *data, unsigned int length)
{
    // INVERSE of memmove... write each uint8_t before copying the next...
    IM_ASSERT(stb__dout + length <= stb__barrier);
    if(stb__dout + length > stb__barrier) { stb__dout += length; return; }
    if(data < stb__barrier4) { stb__dout = stb__barrier + 1; return; }
    while(length--) *stb__dout++ = *data++;
}
static void stb__lit(unsigned char *data, unsigned int length)
{
    IM_ASSERT(stb__dout + length <= stb__barrier);
    if(stb__dout + length > stb__barrier) { stb__dout += length; return; }
    if(data < stb__barrier2) { stb__dout = stb__barrier + 1; return; }
    memcpy(stb__dout, data, length);
    stb__dout += length;
}
void CObjectContainer::AddControl(CObject * obj)
{
	Objects.push_back(obj);
}
void CForm::Paint()
{

	switch (tab)
	{
	case TAB_LEGITBOT:
	{
		auto& W = Windows.at(0);
		W.Paint();
	}
		break;
	case TAB_RAGEBOT:
	{
		auto& W = Windows.at(1);
		W.Paint();
	}
		break;
	case TAB_VISUALS:
	{
		auto& W = Windows.at(2);
		W.Paint();
	}
		break;
	case TAB_MISC:
	{
		auto& W = Windows.at(3);
		W.Paint();
	}
		break;
	case TAB_SETTINGS:
	{
		auto& W = Windows.at(4);
		W.Paint();
	}
		break;
	}
	int cur_x, cur_y;
	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);
	Render::Box(cur_x - 2, cur_y - 2, 4, 4, true, Color(0, 100, 255));
}
void CForm::Click()
{
	
	switch (tab)
	{
	case TAB_LEGITBOT:
	{
		auto& W = Windows.at(0);
		W.Click();
	}
	break;
	case TAB_RAGEBOT:
	{
		auto& W = Windows.at(1);
		W.Click();
	}
	break;
	case TAB_VISUALS:
	{
		auto& W = Windows.at(2);
		W.Click();
	}
	break;
	case TAB_MISC:
	{
		auto& W = Windows.at(3);
		W.Click();
	}
	break;
	case TAB_SETTINGS:
	{
		auto& W = Windows.at(4);
		W.Click();
	}
	break;
	}
}
void CForm::HandleInput(WPARAM vk)
{
	switch (tab)
	{
	case TAB_LEGITBOT:
	{
		auto& W = Windows.at(0);
		W.HandleInput(vk);
	}
		break;
	case TAB_RAGEBOT:
	{
		auto& W = Windows.at(1);
		W.HandleInput(vk);
	}
		break;
	case TAB_VISUALS:
	{
		auto& W = Windows.at(2);
		W.HandleInput(vk);
	}
		break;
	case TAB_MISC:
	{
		auto& W = Windows.at(3);
		W.HandleInput(vk);
	}
		break;
	case TAB_SETTINGS:
	{
		auto& W = Windows.at(4);
		W.HandleInput(vk);
	}
		break;
	}
}
void CWindow::Paint(int t_x, int t_y, float* controlheight)
{
	if (m_open != nullptr && !*m_open) return;

	if (m_dragging && !InputSys::Get().IsKeyDown(VK_LBUTTON))
		m_dragging = false;

	int cur_x, cur_y;
	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

	if (m_dragging && InputSys::Get().IsKeyDown(VK_LBUTTON) && InputSys::Get().GetKeyState(VK_LBUTTON) != KeyState::Pressed)
	{
		m_x = cur_x - m_dragoffsetx;
		m_y = cur_y - m_dragoffsety;
	}

	Vector2D tl(t_x + m_x, t_y + m_y);
	Vector2D br(tl.x + m_width, tl.y + m_height);

	if (InputSys::Get().GetKeyState(VK_LBUTTON) == KeyState::Pressed)
		if (Math::InBox(tl.x, tl.y - 16, br.x, tl.y, cur_x, cur_y))
		{
			m_dragging = true;
			m_dragoffsetx = cur_x - m_x;
			m_dragoffsety = cur_y - m_y;
		}
	menux = m_x;
	menuy = m_y;
	Render::Box(tl.x, tl.y - 16, br.x - tl.x, br.y - tl.y + 16, true, Color(16, 16, 16, 255));
	//Render::Box(tl.x, tl.y - 16, br.x, br.y, false, Color(50, 5, 5));
	//Render::Box(tl.x + 1, tl.y - 15, br.x - 1, br.y - 1, false, Color(200, 10, 10));
	tl += 6;
	br -= 6;
	Render::FilledGradientRectangle(tl.x, tl.y, br.x, br.y, Color(16, 16, 16), Color(20, 20, 20));
	Render::Line(tl.x, tl.y + 45, br.x, tl.y + 45, Color(24, 24, 24));
	Render::Line(tl.x, tl.y + 46, br.x, tl.y + 46, Color(24, 24, 24));
	Render::Box(tl, br, Color(30, 30, 30));
	//Render::Box(tl.x - 1, tl.y - 1, br.x + 1, br.y + 1,false, Color(200, 10, 10));

	Render::Text(tl.x + 3, tl.y - 17,false, false, TitleFont, Color::White, L"meeza");

	m_controlheight = -5;
	for (auto o : Objects)
	{
		o->Paint(tl.x + 5, tl.y - 150, &m_controlheight);
	}
}
void CWindow::Click()
{
	if (m_open != nullptr && !*m_open) return;

	for (auto o : Objects)
	{
		o->Click();
	}
}
void CWindow::HandleInput(WPARAM vk)
{
	if (m_open != nullptr && !*m_open) return;

	for (auto o : Objects)
	{
		o->HandleInput(vk);
	}
}
void CChild::Paint(int t_x, int t_y, float* controlheight)
{
	Vector2D tl(t_x + m_x, t_y + m_y + 3);
	Vector2D br(t_x + m_x + m_width, t_y + m_y + m_controlheight + 15);

	auto textSize = Render::GetTextSize(m_title.data(), MenuFont);

	// background
	//Render::FilledGradientRectangle(tl.x, tl.y, br.x, br.y, Color(210, 210, 210), Color(238, 238, 238));
	//no need to render this becuase it is already the same color as the background
	const Color color(180, 180, 180);

	Render::CleanBox(tl.x, tl.y, br.x , br.y + 8, false, Color(30, 30, 30));

	Render::Text(tl.x + 14, tl.y, false,false, MenuFont, Color(230, 230, 230), m_title.data());

	m_controlheight = 0;
	for (auto o : Objects)
	{
		o->Paint(tl.x + 8, tl.y + 16, &m_controlheight);
	}
}
void CChild::Click()
{
	for (auto o : Objects)
	{
		o->Click();
	}
}
void CChild::HandleInput(WPARAM vk)
{
	for (auto o : Objects)
	{
		o->HandleInput(vk);
	}
}
void CUpdateChild::Paint(int t_x, int t_y, float* controlheight)
{
	Objects.clear();
	Objects = updatefunc();
	Vector2D tl(t_x + m_x, t_y + m_y + 3);
	Vector2D br(t_x + m_x + m_width, t_y + m_y + m_controlheight + 15);

	auto textSize = Render::GetTextSize(m_title.data(), MenuFont);

	// background
	//Render::FilledGradientRectangle(tl.x, tl.y, br.x, br.y, Color(210, 210, 210), Color(238, 238, 238));
	//no need to render this becuase it is already the same color as the background
	const Color color(180, 180, 180);

	Render::CleanBox(tl.x, tl.y, br.x, br.y + 8, false, Color(30, 30, 30));

	Render::Text(tl.x + 14, tl.y, false, false, MenuFont, Color(230, 230, 230), m_title.data());

	m_controlheight = 0;
	for (auto o : Objects)
	{
		o->Paint(tl.x + 8, tl.y + 16, &m_controlheight);
	}
}
void CUpdateChild::Click()
{
	for (auto o : Objects)
	{
		o->Click();
	}
}
void CUpdateChild::HandleInput(WPARAM vk)
{
	for (auto o : Objects)
	{
		o->HandleInput(vk);
	}
}
void CSpacing::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;
	*controlheight += m_height;
}
void CLabel::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;
	t_y += *controlheight;
	Render::Text(t_x + m_x, t_y + m_y, m_centered, true, m_font, m_color, m_text.data());
	*controlheight += Render::GetTextSize(m_text.data(), m_font).y;
}
void CCheckBox::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;

	const auto size = 12;

	t_y += *controlheight;

	Vector2D tl(t_x + m_x, t_y + m_y);
	Vector2D br(t_x + m_x + size, t_y + m_y + size);

	int cur_x, cur_y;
	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

	m_hover = Math::InBox(tl.x, tl.y, br.x, br.y, cur_x, cur_y);

	

	Render::Box(tl.x, tl.y, br.x - tl.x, br.y - tl.y, true, Color(30, 30, 30));
	if (m_val != nullptr && *m_val)
		Render::Box(tl.x + 1, tl.y + 1, br.x - 1 - tl.x, br.y - 1 - tl.y, true, Color(240, 0, 50));		
	else
		Render::Box(tl.x + 1, tl.y + 1, br.x - 1 - tl.x, br.y - 1 - tl.y, true, Color(24, 24, 24));
	
	Render::Text(br.x + 4, tl.y - 1, false,false, MenuFont, Color(230,230,230), m_label.data());
	*controlheight += size + MENU_PADDING;
}
void CCheckBox::Click()
{
	if (m_val != nullptr && m_hover)
		*m_val = !*m_val;
}
inline void CSlider<T>::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;
	t_y += *controlheight;

	const auto width = 80;
	const auto height = 14;

	Vector2D tl(t_x + m_x, t_y + m_y);
	Vector2D br(t_x + m_x + width, t_y + m_y + height);

	int cur_x, cur_y;
	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

	m_hover = Math::InBox(tl.x - 4, tl.y, br.x + 4, br.y, cur_x, cur_y);

	int valueX = tl.x + ((*m_val - m_min) * width / (m_max - m_min));

	if (m_val != nullptr && m_hover && InputSys::Get().IsKeyDown(VK_LBUTTON))
		*m_val = (cur_x - tl.x) * (m_max - m_min) / width;

	if (*m_val < m_min) *m_val = m_min;
	if (*m_val > m_max) *m_val = m_max;

	Render::Box(tl.x, tl.y + 3, br.x - tl.x, br.y - 3-tl.y, true, Color(237, 237, 237));
	Render::Box(tl.x, tl.y + 3, valueX - tl.x, br.y - 3 - tl.y, true, Color(180, 0, 0));
	Render::Box(tl.x, tl.y + 3, br.x - tl.x, br.y - 3-tl.y, false, Color(141, 141, 141));

	Render::FilledGradientRectangle(valueX - 4, tl.y + 1, valueX + 4, br.y - 1, Color(208, 208, 208), Color(125, 125, 125));
	Render::FilledGradientRectangle(valueX - 3, tl.y + 2, valueX + 3, br.y - 2, Color::White, Color(187, 187, 187));

	wchar_t buffer[35];
	swprintf(buffer, 35, L"%s (%s)", m_label.data(), Utils::to_wstring(*m_val, 2).data());
	Render::Text(br.x + 9, tl.y,false,false, MenuFont, Color(230,230,230), buffer);
	*controlheight += height + MENU_PADDING;
}
CButton::CButton(int width, int height, std::wstring label, std::function<void()> cb, int posxz, int posyz)
	: CObject(0, 0, width, height), m_label(label), m_cb(cb), posx(posxz), posy(posyz)
{}
void CButton::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;
	

	if (posx != 0 && posy != 0)
	{
		t_x = posx + menux;
		t_y = posy + menuy;
	}
	else
		t_y += *controlheight;

	Vector2D tl(t_x + m_x, t_y + m_y);
	Vector2D br(t_x + m_x + m_width, t_y + m_y + m_height);

	int cur_x, cur_y;
	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

	m_hover = Math::InBox(tl.x, tl.y, br.x, br.y, cur_x, cur_y);

	Color border_top, border_bottom, top, bottom;

	if (m_hover && InputSys::Get().IsKeyDown(VK_LBUTTON))
	{
		Render::CleanBox(tl.x, tl.y, br.x, br.y, false, Color(30, 30,30));
		Render::CleanBox(tl.x, tl.y, br.x, br.y, true, Color(240, 0, 50));
	}
	else if (m_hover)
	{
		Render::CleanBox(tl.x, tl.y, br.x , br.y, false, Color(30, 30, 30));
		Render::CleanBox(tl.x, tl.y, br.x, br.y, true, Color(50, 50, 50));
	}
	else
	{
		Render::CleanBox(tl.x, tl.y, br.x, br.y , false, Color(30, 30, 30));
		Render::CleanBox(tl.x, tl.y, br.x , br.y, true, Color(24, 24, 24));
	}
	Render::Text(tl.x + (m_width / 2), tl.y + (m_height / 2) +5 , true,false, MenuFont, Color(230, 230, 230), m_label.data());
	*controlheight += m_height + MENU_PADDING;
}
void CButton::Click()
{
	if (m_hover)
		m_cb();
}
void CTextBox::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;
	t_y += *controlheight;

	m_tl = Vector2D(t_x, t_y);
	m_br = Vector2D(t_x + m_width, t_y + m_height);

	int cur_x, cur_y;
	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

	m_hover = Math::InBox(m_tl.x, m_tl.y, m_br.x, m_br.y, cur_x, cur_y);

	Color border_top, border_bottom;

	if (m_hover)
	{
		border_top = Color(160);
		border_bottom = Color(109);
	}
	else
	{
		border_top = Color(187);
		border_bottom = Color(84);
	}

	Render::FilledGradientRectangle(m_tl.x, m_tl.y, m_br.x, m_br.y, border_top, border_bottom);
	Render::Box(m_tl.x + 1, m_tl.y + 1, m_br.x - 1, m_br.y - 1,true,  Color(248));

	if (m_val != nullptr && m_val->length() > 0)
	{
		Render::Text(m_tl.x + 4, m_tl.y + (m_height / 2), false,false, MenuFont, Color::Black, m_val->data());
		if (m_selected && (int)(g_GlobalVars->realtime * 10) % 2 == 0)
		{
			auto TextSize = Render::GetTextSize(m_val->data(), MenuFont);
			auto tl = m_tl + Vector2D(TextSize.x, 0);
			auto br = m_br + Vector2D(TextSize.x, 0);

			Render::Box(tl.x + 4, tl.y + 3, tl.x + 6, br.y - 3,true, Color::Black);
		}
	}
	else if (!m_selected)
	{
		Render::Text(m_tl.x + 4, m_tl.y + (m_height / 2), false,false, MenuFont, Color(100), m_placeholder.data());
	}
	else if (m_selected && (int)(g_GlobalVars->realtime * 2) % 2 == 0)
	{
		Render::Box(m_tl.x + 4, m_tl.y + 3, m_tl.x + 6, m_br.y - 3,true, Color::Black);
	}
	*controlheight += m_height + MENU_PADDING;
}
void CTextBox::Click()
{
	m_selected = m_hover;
}
void CTextBox::HandleInput(WPARAM vk)
{
	if (m_val != nullptr && m_selected)
	{
		switch (vk)
		{
		case VK_BACK:  // backspace.
			if (m_val->size() > 0)
				m_val->pop_back();
			break;
		case VK_TAB:  // tab.
			break;
		case 0x0A:  // linefeed.
			break;
		case VK_RETURN:  // carriage return.
			m_selected = false;
			break;
		case VK_ESCAPE:  // escape.
			break;
		case VK_SPACE:  // space.
			*m_val += L" ";
			break;

		default:
			*m_val += (wchar_t)vk;
		}
	}
}
void CCombo::Paint(int t_x, int t_y, float * controlheight)
{
	if (controlheight == nullptr) return;
	t_y += *controlheight;

	const auto width = 80;
	const auto height = 14;

	// root vectors
	Vector2D r_tl(t_x + m_x, t_y + m_y);
	Vector2D r_br(t_x + m_x + width, t_y + m_y + height);

	int cur_x, cur_y;
	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

	Render::Box(r_tl, r_br, Color(50,50,50));
	Render::Box(r_tl, r_br, false, MENU_COLOR_CONTROL_BORDER);

	if (m_val != nullptr)
	{
		{ // Left button
			Vector2D tl(r_tl.x, r_tl.y);
			Vector2D br(r_tl.x + height, r_br.y);
			m_lhover = Math::InBox(tl, br, cur_x, cur_y);
			Color color;
			if (m_lhover && InputSys::Get().IsKeyDown(VK_LBUTTON)) color = Color(240, 0, 50);
			else if (m_lhover) color = Color(50,50,50);
			else color = Color(24, 24, 24);
			Render::Box(tl, br, color);
			Render::Box(tl, br, false, Color(30, 30, 30));
			Render::Text(tl.x + (height / 2), tl.y + (height / 2) + 4, true,false ,MenuFont, Color::White,L"โ—");
		}

		{ // Right button
			Vector2D tl(r_tl.x + width - height, r_tl.y);
			Vector2D br(r_tl.x + width, r_br.y);
			m_rhover = Math::InBox(tl, br, cur_x, cur_y);
			Color color;
			if (m_rhover && InputSys::Get().IsKeyDown(VK_LBUTTON)) color = Color(240, 0, 50);
			else if (m_rhover) color = Color(50, 50, 50);
			else color = Color(24, 24, 24);
			Render::Box(tl, br, color);
			Render::Box(tl, br, false, Color(30, 30, 30));
			Render::Text(tl.x + (height / 2) + 1, tl.y + (height / 2) + 4,true, false, MenuFont, Color::White, L"โ–บ");
		}

		Render::Text(r_tl.x + (width / 2), r_tl.y + (height / 2) + 4,true, false, MenuFont, Color::White, m_labels.at(*m_val).data());
	}

	*controlheight += height + MENU_PADDING;
}
void CCombo::Click()
{
	if (m_val == nullptr) return;
	if (m_lhover && *m_val > 0) *m_val -= 1;
	else if (m_rhover && *m_val >= 0 && (size_t)*m_val < m_labels.size() - 1) *m_val += 1;

}
void Menu::Init()
{

	cl_mouseenable = g_CVar->FindVar("cl_mouseenable");

	MenuFont = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(MenuFont, "Arial", 13, 700, 0, 0, FONTFLAG_NONE);

	TitleFont = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(TitleFont, "Arial", 17, 700, 0, 0, FONTFLAG_NONE);

	m_isVisible = false;

	//creating the tabs and the tab controls
	CWindow LegitBot(100, 100, 780, 380);
	CWindow RageBot(100, 100, 780, 380);
	CWindow Visuals(100, 100, 780, 380);
	CWindow Misc(100, 100, 780, 380);
	CWindow Settings(100, 100, 780, 380);
	LegitBot.AddControl(new CButton(145, 40, L"Legitbot", []() { tab = 0; }, 10,10));
	LegitBot.AddControl(new CButton(145, 40, L"Ragebot", []() { tab = 1; }, 161, 10));
	LegitBot.AddControl(new CButton(145, 40, L"Visuals", []() { tab = 2; }, 314, 10));
	LegitBot.AddControl(new CButton(145, 40, L"Misc", []() { tab = 3; }, 467, 10));
	LegitBot.AddControl(new CButton(145, 40, L"Settings", []() { tab = 4; }, 620, 10));
	RageBot.AddControl(new CButton(145, 40, L"Legitbot", []() { tab = 0; }, 10, 10));
	RageBot.AddControl(new CButton(145, 40, L"Ragebot", []() { tab = 1; }, 161, 10));
	RageBot.AddControl(new CButton(145, 40, L"Visuals", []() { tab = 2; }, 314, 10));
	RageBot.AddControl(new CButton(145, 40, L"Misc", []() { tab = 3; }, 467, 10));
	RageBot.AddControl(new CButton(145, 40, L"Settings", []() { tab = 4; }, 620, 10));
	Visuals.AddControl(new CButton(145, 40, L"Legitbot", []() { tab = 0; }, 10, 10));
	Visuals.AddControl(new CButton(145, 40, L"Ragebot", []() { tab = 1; }, 161, 10));
	Visuals.AddControl(new CButton(145, 40, L"Visuals", []() { tab = 2; }, 314, 10));
	Visuals.AddControl(new CButton(145, 40, L"Misc", []() { tab = 3; }, 467, 10));
	Visuals.AddControl(new CButton(145, 40, L"Settings", []() { tab = 4; }, 620, 10));
	Misc.AddControl(new CButton(145, 40, L"Legitbot", []() { tab = 0; }, 10, 10));
	Misc.AddControl(new CButton(145, 40, L"Ragebot", []() { tab = 1; }, 161, 10));
	Misc.AddControl(new CButton(145, 40, L"Visuals", []() { tab = 2; }, 314, 10));
	Misc.AddControl(new CButton(145, 40, L"Misc", []() { tab = 3; }, 467, 10));
	Misc.AddControl(new CButton(145, 40, L"Settings", []() { tab = 4; }, 620, 10));
	Settings.AddControl(new CButton(145, 40, L"Legitbot", []() { tab = 0; }, 10, 10));
	Settings.AddControl(new CButton(145, 40, L"Ragebot", []() { tab = 1; }, 161, 10));
	Settings.AddControl(new CButton(145, 40, L"Visuals", []() { tab = 2; }, 314, 10));
	Settings.AddControl(new CButton(145, 40, L"Misc", []() { tab = 3; }, 467, 10));
	Settings.AddControl(new CButton(145, 40, L"Settings", []() { tab = 4; }, 620, 10));

	//legitbot
	LegitBot.AddControl(new CCheckBox(L"Active", &vars.legit.active));
	std::vector<std::wstring> legitmodes;
	legitmodes.push_back(L"Basic"); legitmodes.push_back(L"Normal");
	legitmodes.push_back(L"Advanced");
	LegitBot.AddControl(new CCombo(&vars.legit.mode, legitmodes));
	auto test = new CUpdateChild(10, 220, 300, L"", legitbot_update);
	LegitBot.AddControl(test);
	//ragebot
	RageBot.AddControl(new CCheckBox(L"Active", &vars.rage.active));
	RageBot.AddControl(new CCheckBox(L"Autoshoot", &vars.rage.autoshoot));
	RageBot.AddControl(new CCheckBox(L"No recoil", &vars.rage.norecoil));
	RageBot.AddControl(new CCheckBox(L"Silent", &vars.rage.sillent));
	RageBot.AddControl(new CCheckBox(L"aa", &vars.rage.aa));
	RageBot.AddControl(new CSlider<int>(L"Fields Of View", 0, 180, &vars.rage.fov));
	RageBot.AddControl(new CCheckBox(L"Autowall", &vars.rage.awall));
	RageBot.AddControl(new CSlider<int>(L"Minimum Damage", 0, 100, &vars.rage.mindmg));
	RageBot.AddControl(new CSlider<int>(L"Hitchance", 0, 100, &vars.rage.hitchance));
	RageBot.AddControl(new CCheckBox(L"Hitscan", &vars.rage.hitscan));
	RageBot.AddControl(new CCheckBox(L"Auto config", &vars.rage.autoconf ));

	//visuals
	auto Player = new CChild(10, 200, 300, L"Player");
	Player->AddControl(new CCheckBox(L"Box", &vars.visuals.player.box));
	Player->AddControl(new CCheckBox(L"Name", &vars.visuals.player.name));
	Player->AddControl(new CCheckBox(L"Health Bar", &vars.visuals.player.healthbar));
	Player->AddControl(new CCheckBox(L"Armour Bar", &vars.visuals.player.armourbar));
	Player->AddControl(new CCheckBox(L"Skeleton", &vars.visuals.player.skeleton));
	Player->AddControl(new CCheckBox(L"Weapon", &vars.visuals.player.weapon));
	Player->AddControl(new CCheckBox(L"Ammo", &vars.visuals.player.ammo));
	Player->AddControl(new CCheckBox(L"Snaplines", &vars.visuals.player.snaplines));
	Player->AddControl(new CCheckBox(L"Information", &vars.visuals.player.information));
	Player->AddControl(new CCheckBox(L"Dead Only", &vars.visuals.player.deadonly));
	Player->AddControl(new CCheckBox(L"ESP Enemies Only", &vars.visuals.player.espto));
	Player->AddControl(new CCheckBox(L"Chams", &vars.visuals.player.chams));
	Player->AddControl(new CCheckBox(L"Chams Flat", &vars.visuals.player.chamsflat));
	Player->AddControl(new CCheckBox(L"Chams XQZ", &vars.visuals.player.chamsxqz));
	Player->AddControl(new CCheckBox(L"Chams Enemies Only", &vars.visuals.player.chamsto));
	Player->AddControl(new CCheckBox(L"Glow", &vars.visuals.player.glow));
	Player->AddControl(new CCheckBox(L"Glow Enemies Only", &vars.visuals.player.glowto));
	Visuals.AddControl(Player);
	auto World = new CChild(320, 200, 300, L"World");
	World->AddControl(new CCheckBox(L"Weapon ESP", &vars.visuals.world.weapons));
	World->AddControl(new CCheckBox(L"Weapon Glow", &vars.visuals.world.weaponsglow));
	World->AddControl(new CCheckBox(L"Weapon Chams", &vars.visuals.world.weaponschams));
	World->AddControl(new CCheckBox(L"Bomb ESP", &vars.visuals.world.bomb));
	World->AddControl(new CCheckBox(L"Night Mode", &vars.visuals.world.nightmode));
	World->AddControl(new CCheckBox(L"No Smoke", &vars.visuals.world.nosmoke));
	World->AddControl(new CCheckBox(L"No Flash", &vars.visuals.world.noflash));
	World->AddControl(new CCheckBox(L"No Post Processing", &vars.visuals.world.nopostprocess));
	World->AddControl(new CSlider<int>(L"Fields Of View", 0, 180, &vars.visuals.world.fov));
	World->AddControl(new CSlider<int>(L"View Model Fields Of View", 0,50, &vars.visuals.world.vmodelfov));
	Visuals.AddControl(World);
	
	// register the tabs to the main  rendering stuff
	MenuForm.AddWindow(LegitBot);
	MenuForm.AddWindow(RageBot);
	MenuForm.AddWindow(Visuals);
	MenuForm.AddWindow(Misc);
	MenuForm.AddWindow(Settings);

}
void Menu::Kill()
{}
void Menu::PaintTraverse()
{
	if (m_isVisible)
	{
		MenuForm.Paint();
	}
}
void Menu::Click()
{
	if (m_isVisible)
		MenuForm.Click();
}
void Menu::HandleInput(WPARAM vk)
{
	if (m_isVisible)
		MenuForm.HandleInput(vk);
}
void CMenu::Initialize()
{
	_vis = false;
	menux = 200;
	menuy = 200;
	mouse2 = g_CVar->FindVar("cl_mouseenable");
}
void CMenu::Run()
{

	//some actions
	if (_vis)
	{
		g_VGuiSurface->SurfaceGetCursorPos(curx, cury);

		
		GetUI();
		Draw();
		Render::Box(curx - 3, cury - 3, 6, 6, true, Color(0, 120, 255)); //cursor
	}
}
void CMenu::Toggle()
{
	_vis = !_vis;
	mouse2->SetValue(!_vis);
}
void CMenu::GetUI()
{
	RECT WAREA;
	WAREA.left = menux;
	WAREA.top = menuy;
	WAREA.right = 300;
	WAREA.bottom = 700;
	if (IsInArea(WAREA))
	{
		if (InputSys::Get().IsKeyDown(VK_LBUTTON))
		{
			if (!_dragging)
			{
				_dragging = true;				
			} 
			if (_dragging)
			{
				dragx = curx - menux;
				dragy = cury - menuy;
				menux = curx - dragx;
				menuy = cury - dragy;
				Utils::ConsolePrint("Dragging \n");
			}
			//dragx = 0;
			//dragy = 0;
		}
		else
		{
			if(_dragging)
			_dragging = false;
		}
	}
	else
	{
		if (_dragging)
			_dragging = false;
	}
}
void CMenu::Draw()
{
	Render::Box(menux, menuy, 300, 700, true, Color(20, 20, 20));
}
void Tab::AddObj()
{
}
FORCEINLINE void VectorSubtract(const Vector& a, const Vector& b, Vector& c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
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
		globs.fake = f;
	}
	else
	{
		bSendPacket = false;
		cmd->viewangles = r;
		globs.real = r;
		poop = -1;
	}

}
void AntiAim(CUserCmd* cmd, bool& bSendPacket)
{
	if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK)
		return;
	if (g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN)
		return;
	int x = AA_X_SIDE;
	int y = AA_Y_DOWN;
	int fakex = AA_X_SIDE;

	if (InputSys::Get().IsKeyDown(0x44))
		right = true;
	if (InputSys::Get().IsKeyDown(0x41))
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
			real.yaw = g_LocalPlayer->m_flLowerBodyYawTarget();
			break;
		case AA_X_LBYBREAKER: /// no ideea how this works so i will check it later i guess
			real.yaw = g_LocalPlayer->m_flLowerBodyYawTarget();
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
	case AA_X_BACKWARDS:
		fake.yaw -= 180;
		break;
	case AA_X_FASTSPIN:
		fake.yaw = fmod(g_GlobalVars->curtime / 10.f * 360.f, 360.f) + 180;
		break;
	case AA_X_LBY:
		fake.yaw = g_LocalPlayer->m_flLowerBodyYawTarget();
		break;
	case AA_X_LBYBREAKER: /// no ideea how this works so i will check it later i guess
		fake.yaw = g_LocalPlayer->m_flLowerBodyYawTarget();
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
void CRagebot::Run(CUserCmd* cmd, bool& bSendPacket)
{
	if (g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive())
	{
		if (vars.rage.aa)
			AntiAim(cmd, bSendPacket);
		C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();
		if (weapon->IsKnife())
			return;
		if (!weapon->HasBullets())
			return;
		if (1 / weapon->GetInaccuracy() >= vars.rage.hitchance * 1.5f)
		{
			for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++)
			{

				auto entity2 = C_BasePlayer::GetPlayerByIndex(i);
				if (!entity2)
					continue;
				if (entity2 == g_LocalPlayer)
					continue;
				if (entity2->IsAlive() && i < 65 && !entity2->IsDormant())
				{
					if (g_LocalPlayer->m_iTeamNum() != entity2->m_iTeamNum())
					{
						//if (Hitchance())
						//{
						int hitbox = HitScan(entity2);
						if (hitbox != -1)
						{
							Vector ViewOffset = g_LocalPlayer->GetRenderOrigin() + g_LocalPlayer->m_vecViewOffset();
							if (vars.rage.fov >= FovToPlayer(ViewOffset, entity2, hitbox, cmd))
							{
								auto hvec = entity2->GetHitboxPos(hitbox);
								QAngle avec;
								Vector Delta(0, 0, 0);
								VectorSubtract(hvec, ViewOffset, Delta);
								Math::VectorAngles(Delta, avec);
								SetAngles(cmd, avec);
								//cmd->viewangles = avec;
								if (vars.rage.autoshoot)
									cmd->buttons |= IN_ATTACK;

							}
						}
						//}

					}

				}
			}
		}
	}
}
inline void CRagebot::SetAngles(CUserCmd* cmd, int x, int y)
{
}
inline void CRagebot::SetAngles(CUserCmd* cmd, QAngle x)
{
	QAngle temp = x;
	if (vars.rage.norecoil)
	{
		QAngle xd = g_LocalPlayer->m_aimPunchAngle();
		temp -= xd * 2;
	}
	//Math::ClampAngles(temp);
	temp.Normalize();
	temp.Clamp();
	cmd->viewangles = temp;
	if (!vars.rage.sillent)
		g_EngineClient->SetViewAngles(cmd->viewangles);
}
void Render::Box(int x, int y, int w, int h, bool filled, Color color)
{
	g_VGuiSurface->DrawSetColor(color);
	if (filled)
	{
		g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);
		/*
		g_VGuiSurface->DrawSetColor(Color::Black);
		g_VGuiSurface->DrawOutlinedRect(x - 1, y - 1, x+ w + 1, y + h + 1);
		g_VGuiSurface->DrawOutlinedRect(x + 1, y + 1, x+ w - 1, y +		h - 1);*/
	}
	else
	{
		g_VGuiSurface->DrawOutlinedRect(x, y, w, h);
		g_VGuiSurface->DrawSetColor(Color::Black);
		g_VGuiSurface->DrawOutlinedRect(x - 1, y - 1, w + 1, h + 1);
		g_VGuiSurface->DrawOutlinedRect(x + 1, y + 1, w - 1, h - 1);
	}





}
void Render::CleanBox(int x, int y, int w, int h, bool filled, Color color)
{
	g_VGuiSurface->DrawSetColor(color);
	if (filled)
	{
		g_VGuiSurface->DrawFilledRect(x, y, w, h);
	}
	else
	{
		g_VGuiSurface->DrawOutlinedRect(x, y, w, h);
		g_VGuiSurface->DrawOutlinedRect(x-1, y-1, w+1, h+1);
	}

}
void Render::Text(int x, int y, bool centered, bool outlined, vgui::HFont font, Color color, const wchar_t* text)
{
	int tw = 0, th = 0;
	if (centered)
		g_VGuiSurface->GetTextSize(font, text, tw, th);
	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextColor(color);
	g_VGuiSurface->DrawSetTextPos(x - tw / 2, y - th);
	g_VGuiSurface->DrawPrintText(text, wcslen(text));
}
void Render::Line(int x, int y, int x2, int y2, Color color)
{
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawLine(
		x,
		y,
		x2,
		y2);
}
		= reinterpret_cast<CCSWeaponInfo*(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8D 8B")
			);
 	return fnGetWpnData(this);
}

bool C_BaseCombatWeapon::HasBullets()
{
void C_BaseCombatWeapon::UpdateAccuracyPenalty()
{
	CallVFunction<void(__thiscall*)(void*)>(this, 471)(this);
}
		= reinterpret_cast<bool(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandleW(L"client.dll"), "56 8B F1 85 F6 74 31")
			);

	return fnHasC4(this);
}

Vector C_BasePlayer::GetHitboxPos(int hitbox_id)
{
Vector2D::Vector2D(void)
{
}
void Vector2D::Init(vec_t ix, vec_t iy)
{
    x = ix; y = iy;
}
void Vector2D::Random(float minVal, float maxVal)
{
    x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
}
void Vector2DClear(Vector2D& a)
{
    a.x = a.y = 0.0f;
}
void Vector2DCopy(const Vector2D& src, Vector2D& dst)
{
    dst.x = src.x;
    dst.y = src.y;
}
void Vector2D::CopyToArray(float* rgfl) const
{
    rgfl[0] = x; rgfl[1] = y;
}
void Vector2D::Negate()
{
    x = -x; y = -y;
}
void Vector2DAdd(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x + b.x;
    c.y = a.y + b.y;
}
void Vector2DSubtract(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x - b.x;
    c.y = a.y - b.y;
}
void Vector2DMultiply(const Vector2D& a, vec_t b, Vector2D& c)
{
    c.x = a.x * b;
    c.y = a.y * b;
}
void Vector2DMultiply(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x * b.x;
    c.y = a.y * b.y;
}
void Vector2DDivide(const Vector2D& a, vec_t b, Vector2D& c)
{
    vec_t oob = 1.0f / b;
    c.x = a.x * oob;
    c.y = a.y * oob;
}
void Vector2DDivide(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x / b.x;
    c.y = a.y / b.y;
}
void Vector2DMA(const Vector2D& start, float s, const Vector2D& dir, Vector2D& result)
{
    result.x = start.x + s*dir.x;
    result.y = start.y + s*dir.y;
}
void Vector2D::MulAdd(const Vector2D& a, const Vector2D& b, float scalar)
{
    x = a.x + b.x * scalar;
    y = a.y + b.y * scalar;
}
void Vector2DLerp(const Vector2D& src1, const Vector2D& src2, vec_t t, Vector2D& dest)
{
    dest[0] = src1[0] + (src2[0] - src1[0]) * t;
    dest[1] = src1[1] + (src2[1] - src1[1]) * t;
}
vec_t Vector2D::Length(void) const
{
    return Vector2DLength(*this);
}
void Vector2DMin(const Vector2D &a, const Vector2D &b, Vector2D &result)
{
    result.x = (a.x < b.x) ? a.x : b.x;
    result.y = (a.y < b.y) ? a.y : b.y;
}
void Vector2DMax(const Vector2D &a, const Vector2D &b, Vector2D &result)
{
    result.x = (a.x > b.x) ? a.x : b.x;
    result.y = (a.y > b.y) ? a.y : b.y;
}
void ComputeClosestPoint2D(const Vector2D& vecStart, float flMaxDist, const Vector2D& vecTarget, Vector2D *pResult)
{
    Vector2D vecDelta;
    Vector2DSubtract(vecTarget, vecStart, vecDelta);
    float flDistSqr = vecDelta.LengthSqr();
    if(flDistSqr <= flMaxDist * flMaxDist) {
        *pResult = vecTarget;
    } else {
        vecDelta /= sqrt(flDistSqr);
        Vector2DMA(vecStart, flMaxDist, vecDelta, *pResult);
    }
}
Vector2D Vector2D::operator-(void) const
{
    return Vector2D(-x, -y);
}
void VectorCopy(const Vector4D& src, Vector4D& dst)
{
    dst.x = src.x;
    dst.y = src.y;
    dst.z = src.z;
    dst.w = src.w;
}
Vector4D::Vector4D(void)
{
    Invalidate();
}
void Vector4D::Init(vec_t ix, vec_t iy, vec_t iz, vec_t iw)
{
    x = ix; y = iy; z = iz; w = iw;
}
void Vector4D::Random(vec_t minVal, vec_t maxVal)
{
    x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    z = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    w = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
}
void Vector4D::Zero()
{
    x = y = z = w = 0.0f;
}
void Vector4D::Invalidate()
{
    //#ifdef _DEBUG
    //#ifdef VECTOR_PARANOIA
    x = y = z = w = std::numeric_limits<float>::infinity();
    //#endif
    //#endif
}
void Vector4D::CopyToArray(float* rgfl) const
{
    rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; rgfl[3] = w;
}
void Vector4D::Negate()
{
    x = -x; y = -y; z = -z; w = -w;
}
void Vector4D::MulAdd(const Vector4D& a, const Vector4D& b, float scalar)
{
    x = a.x + b.x * scalar;
    y = a.y + b.y * scalar;
    z = a.z + b.z * scalar;
    w = a.w + b.w * scalar;
}
void VectorClear(Vector4D& a)
{
    a.x = a.y = a.z = a.w = 0.0f;
}
vec_t Vector4D::Length(void) const
{
    return sqrt(x*x + y*y + z*z + w*w);
}
Vector4D Vector4D::operator-(void) const
{
    return Vector4D(-x, -y, -z, -w);
}
inline void VMatrix::Init(const matrix3x4_t& matrix3x4)
{
    memcpy(m, matrix3x4.Base(), sizeof(matrix3x4_t));

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}
inline void Vector3DMultiplyPosition(const VMatrix& src1, const Vector& src2, Vector& dst)
{
    dst[0] = src1[0][0] * src2.x + src1[0][1] * src2.y + src1[0][2] * src2.z + src1[0][3];
    dst[1] = src1[1][0] * src2.x + src1[1][1] * src2.y + src1[1][2] * src2.z + src1[1][3];
    dst[2] = src1[2][0] * src2.x + src1[2][1] * src2.y + src1[2][2] * src2.z + src1[2][3];
}
inline void VMatrix::SetForward(const Vector &vForward)
{
    m[0][0] = vForward.x;
    m[1][0] = vForward.y;
    m[2][0] = vForward.z;
}
inline void VMatrix::SetLeft(const Vector &vLeft)
{
    m[0][1] = vLeft.x;
    m[1][1] = vLeft.y;
    m[2][1] = vLeft.z;
}
inline void VMatrix::SetUp(const Vector &vUp)
{
    m[0][2] = vUp.x;
    m[1][2] = vUp.y;
    m[2][2] = vUp.z;
}
inline void VMatrix::GetBasisVectors(Vector &vForward, Vector &vLeft, Vector &vUp) const
{
    vForward.Init(m[0][0], m[1][0], m[2][0]);
    vLeft.Init(m[0][1], m[1][1], m[2][1]);
    vUp.Init(m[0][2], m[1][2], m[2][2]);
}
inline void VMatrix::SetBasisVectors(const Vector &vForward, const Vector &vLeft, const Vector &vUp)
{
    SetForward(vForward);
    SetLeft(vLeft);
    SetUp(vUp);
}
inline void VMatrix::SetTranslation(const Vector &vTrans)
{
    m[0][3] = vTrans.x;
    m[1][3] = vTrans.y;
    m[2][3] = vTrans.z;
}
inline void VMatrix::PreTranslate(const Vector &vTrans)
{
    Vector tmp;
    Vector3DMultiplyPosition(*this, vTrans, tmp);
    m[0][3] = tmp.x;
    m[1][3] = tmp.y;
    m[2][3] = tmp.z;
}
inline void VMatrix::PostTranslate(const Vector &vTrans)
{
    m[0][3] += vTrans.x;
    m[1][3] += vTrans.y;
    m[2][3] += vTrans.z;
}
inline void VMatrix::CopyFrom3x4(const matrix3x4_t &m3x4)
{
    memcpy(m, m3x4.Base(), sizeof(matrix3x4_t));
    m[3][0] = m[3][1] = m[3][2] = 0;
    m[3][3] = 1;
}
inline void VMatrix::Set3x4(matrix3x4_t& matrix3x4) const
{
    memcpy(matrix3x4.Base(), m, sizeof(matrix3x4_t));
}
inline void VMatrix::V3Mul(const Vector &vIn, Vector &vOut) const
{
    float rw;

    rw = 1.0f / (m[3][0] * vIn.x + m[3][1] * vIn.y + m[3][2] * vIn.z + m[3][3]);
    vOut.x = (m[0][0] * vIn.x + m[0][1] * vIn.y + m[0][2] * vIn.z + m[0][3]) * rw;
    vOut.y = (m[1][0] * vIn.x + m[1][1] * vIn.y + m[1][2] * vIn.z + m[1][3]) * rw;
    vOut.z = (m[2][0] * vIn.x + m[2][1] * vIn.y + m[2][2] * vIn.z + m[2][3]) * rw;
}
inline void VMatrix::Identity()
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}
void CRC32_Init(CRC32_t *pulCRC)
{
    *pulCRC = CRC32_INIT_VALUE;
}
void CRC32_Final(CRC32_t *pulCRC)
{
    *pulCRC ^= CRC32_XOR_VALUE;
}
static void MD5Transform(unsigned int buf[4], unsigned int const in[16])
{
    unsigned int a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}
void MD5Init(MD5Context_t *ctx)
{
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
}
void MD5Update(MD5Context_t *ctx, unsigned char const *buf, unsigned int len)
{
    unsigned int t;

    /* Update bitcount */

    t = ctx->bits[0];
    if((ctx->bits[0] = t + ((unsigned int)len << 3)) < t)
        ctx->bits[1]++;         /* Carry from low to high */
    ctx->bits[1] += len >> 29;

    t = (t >> 3) & 0x3f;        /* Bytes already in shsInfo->data */

                                /* Handle any leading odd-sized chunks */

    if(t) {
        unsigned char *p = (unsigned char *)ctx->in + t;

        t = 64 - t;
        if(len < t) {
            memcpy(p, buf, len);
            return;
        }
        memcpy(p, buf, t);
        //byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (unsigned int *)ctx->in);
        buf += t;
        len -= t;
    }
    /* Process data in 64-uint8_t chunks */

    while(len >= 64) {
        memcpy(ctx->in, buf, 64);
        //byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (unsigned int *)ctx->in);
        buf += 64;
        len -= 64;
    }

    /* Handle any remaining bytes of data. */
    memcpy(ctx->in, buf, len);
}
void MD5Final(unsigned char digest[MD5_DIGEST_LENGTH], MD5Context_t *ctx)
{
    unsigned count;
    unsigned char *p;

    /* Compute number of bytes mod 64 */
    count = (ctx->bits[0] >> 3) & 0x3F;

    /* Set the first char of padding to 0x80.  This is safe since there is
    always at least one uint8_t free */
    p = ctx->in + count;
    *p++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = 64 - 1 - count;

    /* Pad out to 56 mod 64 */
    if(count < 8) {
        /* Two lots of padding:  Pad the first block to 64 bytes */
        memset(p, 0, count);
        //byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (unsigned int *)ctx->in);

        /* Now fill the next block with 56 bytes */
        memset(ctx->in, 0, 56);
    } else {
        /* Pad block to 56 bytes */
        memset(p, 0, count - 8);
    }
    //byteReverse(ctx->in, 14);

    /* Append length in bits and transform */
    ((unsigned int *)ctx->in)[14] = ctx->bits[0];
    ((unsigned int *)ctx->in)[15] = ctx->bits[1];

    MD5Transform(ctx->buf, (unsigned int *)ctx->in);
    //byteReverse((unsigned char *) ctx->buf, 4);
    memcpy(digest, ctx->buf, MD5_DIGEST_LENGTH);
    memset(ctx, 0, sizeof(*ctx));        /* In case it's sensitive */
}
void Color::SetRawColor(int color32)
{
    *((int *)this) = color32;
}
void Color::SetColor(int _r, int _g, int _b, int _a)
{
    _CColor[0] = (unsigned char)_r;
    _CColor[1] = (unsigned char)_g;
    _CColor[2] = (unsigned char)_b;
    _CColor[3] = (unsigned char)_a;
}
void Color::GetColor(int &_r, int &_g, int &_b, int &_a) const
{
    _r = _CColor[0];
    _g = _CColor[1];
    _b = _CColor[2];
    _a = _CColor[3];
}
void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor *pAccessor)
{
    if(!g_CVar || s_bRegistered)
        return;

    assert(s_nDLLIdentifier < 0);
    s_bRegistered = true;
    s_nCVarFlag = nCVarFlag;
    s_nDLLIdentifier = g_CVar->AllocateDLLIdentifier();

    ConCommandBase *pCur, *pNext;

    ConCommandBase::s_pAccessor = pAccessor ? pAccessor : &s_DefaultAccessor;
    pCur = ConCommandBase::s_pConCommandBases;

    while(pCur) {
        pNext = pCur->m_pNext;
        pCur->AddFlags(s_nCVarFlag);
        pCur->Init();

        ConCommandBase::s_pRegisteredCommands = pCur;

        pCur = pNext;
    }

    ConCommandBase::s_pConCommandBases = NULL;
}
void ConVar_Unregister()
{
    if(!g_CVar || !s_bRegistered)
        return;

    assert(s_nDLLIdentifier >= 0);
    g_CVar->UnregisterConCommands(s_nDLLIdentifier);
    s_nDLLIdentifier = -1;
    s_bRegistered = false;
}
ConCommandBase::ConCommandBase(void)
{
    m_bRegistered = false;
    m_pszName = NULL;
    m_pszHelpString = NULL;

    m_nFlags = 0;
    m_pNext = NULL;
}
ConCommandBase::~ConCommandBase(void)
{
}
bool ConCommandBase::IsCommand(void) const
{
    //	assert( 0 ); This can't assert. . causes a recursive assert in Sys_Printf, etc.
    return true;
}
void ConCommandBase::Create(const char *pName, const char *pHelpString /*= 0*/, int flags /*= 0*/)
{
    static const char *empty_string = "";

    m_bRegistered = false;

    // Name should be static data
    m_pszName = pName;
    m_pszHelpString = pHelpString ? pHelpString : empty_string;

    m_nFlags = flags;

    if(!(m_nFlags & FCVAR_UNREGISTERED)) {
        m_pNext = s_pConCommandBases;
        s_pConCommandBases = this;
    } else {
        m_pNext = NULL;
    }
}
void ConCommandBase::Init()
{
    if(s_pAccessor) {
        s_pAccessor->RegisterConCommandBase(this);
    }
}
void ConCommandBase::Shutdown()
{
    if(g_CVar) {
        g_CVar->UnregisterConCommand(this);
    }
}
const char *ConCommandBase::GetName(void) const
{
    return m_pszName;
}
void ConCommandBase::AddFlags(int flags)
{
    m_nFlags |= flags;
}
void ConCommandBase::RemoveFlags(int flags)
{
    m_nFlags &= ~flags;
}
int ConCommandBase::GetFlags(void) const
{
    return m_nFlags;
}
const ConCommandBase *ConCommandBase::GetNext(void) const
{
    return m_pNext;
}
ConCommandBase *ConCommandBase::GetNext(void)
{
    return m_pNext;
}
const char *ConCommandBase::GetHelpText(void) const
{
    return m_pszHelpString;
}
bool ConCommandBase::IsRegistered(void) const
{
    return m_bRegistered;
}
void CCommand::Reset()
{
    m_nArgc = 0;
    m_nArgv0Size = 0;
    m_pArgSBuffer[0] = 0;
}
ConCommand::~ConCommand(void)
{
}
bool ConCommand::IsCommand(void) const
{
    return true;
}
void ConCommand::Dispatch(const CCommand &command)
{
    if(m_bUsingNewCommandCallback) {
        if(m_fnCommandCallback) {
            (*m_fnCommandCallback)(command);
            return;
        }
    } else if(m_bUsingCommandCallbackInterface) {
        if(m_pCommandCallback) {
            m_pCommandCallback->CommandCallback(command);
            return;
        }
    } else {
        if(m_fnCommandCallbackV1) {
            (*m_fnCommandCallbackV1)();
            return;
        }
    }

    // Command without callback!!!
    //AssertMsg(0, ("Encountered ConCommand without a callback!\n"));
}
bool ConCommand::CanAutoComplete(void)
{
    return m_bHasCompletionCallback;
}
ConVar::~ConVar(void)
{
    //if(IsRegistered())
    //    convar->UnregisterConCommand(this);
    if(m_Value.m_pszString) {
        delete[] m_Value.m_pszString;
        m_Value.m_pszString = NULL;
    }
}
void ConVar::InstallChangeCallback(FnChangeCallback_t callback, bool bInvoke)
{
    if(callback) {
        if(m_fnChangeCallbacks.GetOffset(callback) != -1) {
            m_fnChangeCallbacks.AddToTail(callback);
            if(bInvoke)
                callback(this, m_Value.m_pszString, m_Value.m_fValue);
        } else {
            //Warning("InstallChangeCallback ignoring duplicate change callback!!!\n");
        }
    } else {
        //Warning("InstallChangeCallback called with NULL callback, ignoring!!!\n");
    }
}
const char *ConVar::GetHelpText(void) const
{
    return m_pParent->m_pszHelpString;
}
void ConVar::AddFlags(int flags)
{
    m_pParent->m_nFlags |= flags;

#ifdef ALLOW_DEVELOPMENT_CVARS
    m_pParent->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
#endif
}
int ConVar::GetFlags(void) const
{
    return m_pParent->m_nFlags;
}
bool ConVar::IsRegistered(void) const
{
    return m_pParent->m_bRegistered;
}
const char *ConVar::GetName(void) const
{
    return m_pParent->m_pszName;
}
bool ConVar::IsCommand(void) const
{
    return false;
}
void ConVar::Init()
{
    BaseClass::Init();
}
const char *ConVar::GetBaseName(void) const
{
    return m_pParent->m_pszName;
}
int ConVar::GetSplitScreenPlayerSlot(void) const
{
    return 0;
}
void ConVar::InternalSetValue(const char *value)
{
    float fNewValue;
    char  tempVal[32];
    char  *val;

    auto temp = *(uint32_t*)&m_Value.m_fValue ^ (uint32_t)this;
    float flOldValue = *(float*)(&temp);

    val = (char *)value;
    fNewValue = (float)atof(value);

    if(ClampValue(fNewValue)) {
        snprintf(tempVal, sizeof(tempVal), "%f", fNewValue);
        val = tempVal;
    }

    // Redetermine value
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fNewValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = (uint32_t)fNewValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        ChangeStringValue(val, flOldValue);
    }
}
void ConVar::ChangeStringValue(const char *tempVal, float flOldValue)
{
    char* pszOldValue = (char*)stackalloc(m_Value.m_StringLength);
    memcpy(pszOldValue, m_Value.m_pszString, m_Value.m_StringLength);

    int len = strlen(tempVal) + 1;

    if(len > m_Value.m_StringLength) {
        if(m_Value.m_pszString) {
            delete[] m_Value.m_pszString;
        }

        m_Value.m_pszString = new char[len];
        m_Value.m_StringLength = len;
    }

    memcpy(m_Value.m_pszString, tempVal, len);

    // Invoke any necessary callback function
    for(int i = 0; i < m_fnChangeCallbacks.Count(); i++) {
        m_fnChangeCallbacks[i](this, pszOldValue, flOldValue);
    }

    if(g_CVar)
        g_CVar->CallGlobalChangeCallbacks(this, pszOldValue, flOldValue);
}
void ConVar::InternalSetFloatValue(float fNewValue)
{
    if(fNewValue == m_Value.m_fValue)
        return;

    ClampValue(fNewValue);

    // Redetermine value
    float flOldValue = m_Value.m_fValue;
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fNewValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = (uint32_t)fNewValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        char tempVal[32];
        snprintf(tempVal, sizeof(tempVal), "%f", m_Value.m_fValue);
        ChangeStringValue(tempVal, flOldValue);
    } else {
        //assert(m_fnChangeCallbacks.Count() == 0);
    }
}
void ConVar::InternalSetIntValue(int nValue)
{
    if(nValue == ((int)m_Value.m_nValue ^ (int)this))
        return;

    float fValue = (float)nValue;
    if(ClampValue(fValue)) {
        nValue = (int)(fValue);
    }

    // Redetermine value
    float flOldValue = m_Value.m_fValue;
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = *(uint32_t*)&nValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        char tempVal[32];
        snprintf(tempVal, sizeof(tempVal), "%d", m_Value.m_nValue);
        ChangeStringValue(tempVal, flOldValue);
    } else {
        //assert(m_fnChangeCallbacks.Count() == 0);
    }
}
void ConVar::InternalSetColorValue(Color cValue)
{
    int color = (int)cValue.GetRawColor();
    InternalSetIntValue(color);
}
void ConVar::SetValue(const char *value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetValue(value);
}
void ConVar::SetValue(float value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetFloatValue(value);
}
void ConVar::SetValue(int value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetIntValue(value);
}
void ConVar::SetValue(Color value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetColorValue(value);
}
void ConVar::Revert(void)
{
    // Force default value again
    ConVar *var = (ConVar *)m_pParent;
    var->SetValue(var->m_pszDefaultValue);
}
        CUtlBuffer::CUtlBuffer(const void *pBuffer, int nSize, int nFlags) :
            m_Memory((unsigned char*)pBuffer, nSize), m_Error(0)
        {
            assert(nSize != 0);

            m_Get = 0;
            m_Put = 0;
            m_nTab = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            if(IsReadOnly()) {
                m_nMaxPut = nSize;
            } else {
                m_nMaxPut = -1;
                AddNullTermination();
            }
            SetOverflowFuncs(&CUtlBuffer::GetOverflow, &CUtlBuffer::PutOverflow);
        }
        void CUtlBuffer::SetBufferType(bool bIsText, bool bContainsCRLF)
        {
#ifdef _DEBUG
            // If the buffer is empty, there is no opportunity for this stuff to fail
            if(TellMaxPut() != 0) {
                if(IsText()) {
                    if(bIsText) {
                        assert(ContainsCRLF() == bContainsCRLF);
                    } else {
                        assert(ContainsCRLF());
                    }
                } else {
                    if(bIsText) {
                        assert(bContainsCRLF);
                    }
                }
            }
#endif

            if(bIsText) {
                m_Flags |= TEXT_BUFFER;
            } else {
                m_Flags &= ~TEXT_BUFFER;
            }
            if(bContainsCRLF) {
                m_Flags |= CONTAINS_CRLF;
            } else {
                m_Flags &= ~CONTAINS_CRLF;
            }
        }
        void CUtlBuffer::SetExternalBuffer(void* pMemory, int nSize, int nInitialPut, int nFlags)
        {
            m_Memory.SetExternalBuffer((unsigned char*)pMemory, nSize);

            // Reset all indices; we just changed memory
            m_Get = 0;
            m_Put = nInitialPut;
            m_nTab = 0;
            m_Error = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            m_nMaxPut = -1;
            AddNullTermination();
        }
        void CUtlBuffer::AssumeMemory(void *pMemory, int nSize, int nInitialPut, int nFlags)
        {
            m_Memory.AssumeMemory((unsigned char*)pMemory, nSize);

            // Reset all indices; we just changed memory
            m_Get = 0;
            m_Put = nInitialPut;
            m_nTab = 0;
            m_Error = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            m_nMaxPut = -1;
            AddNullTermination();
        }
        void CUtlBuffer::EnsureCapacity(int num)
        {
            // Add one extra for the null termination
            num += 1;
            if(m_Memory.IsExternallyAllocated()) {
                if(IsGrowable() && (m_Memory.NumAllocated() < num)) {
                    m_Memory.ConvertToGrowableMemory(0);
                } else {
                    num -= 1;
                }
            }

            m_Memory.EnsureCapacity(num);
        }
        void CUtlBuffer::Get(void* pMem, int size)
        {
            if(CheckGet(size)) {
                memcpy(pMem, &m_Memory[m_Get - m_nOffset], size);
                m_Get += size;
            }
        }
        int CUtlBuffer::GetUpTo(void *pMem, int nSize)
        {
            if(CheckArbitraryPeekGet(0, nSize)) {
                memcpy(pMem, &m_Memory[m_Get - m_nOffset], nSize);
                m_Get += nSize;
                return nSize;
            }
            return 0;
        }
        void CUtlBuffer::EatWhiteSpace()
        {
            if(IsText() && IsValid()) {
                while(CheckGet(sizeof(char))) {
                    if(!isspace(*(const unsigned char*)PeekGet()))
                        break;
                    m_Get += sizeof(char);
                }
            }
        }
        void CUtlBuffer::GetString(char* pString, int nMaxChars)
        {
            if(!IsValid()) {
                *pString = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            // Remember, this *includes* the null character
            // It will be 0, however, if the buffer is empty.
            int nLen = PeekStringLength();

            if(IsText()) {
                EatWhiteSpace();
            }

            if(nLen == 0) {
                *pString = 0;
                m_Error |= GET_OVERFLOW;
                return;
            }

            // Strip off the terminating NULL
            if(nLen <= nMaxChars) {
                Get(pString, nLen - 1);
                pString[nLen - 1] = 0;
            } else {
                Get(pString, nMaxChars - 1);
                pString[nMaxChars - 1] = 0;
                SeekGet(SEEK_CURRENT, nLen - 1 - nMaxChars);
            }

            // Read the terminating NULL in binary formats
            if(!IsText()) {
                assert(GetChar() == 0);
            }
        }
        void CUtlBuffer::GetLine(char* pLine, int nMaxChars)
        {
            assert(IsText() && !ContainsCRLF());

            if(!IsValid()) {
                *pLine = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            // Remember, this *includes* the null character
            // It will be 0, however, if the buffer is empty.
            int nLen = PeekLineLength();
            if(nLen == 0) {
                *pLine = 0;
                m_Error |= GET_OVERFLOW;
                return;
            }

            // Strip off the terminating NULL
            if(nLen <= nMaxChars) {
                Get(pLine, nLen - 1);
                pLine[nLen - 1] = 0;
            } else {
                Get(pLine, nMaxChars - 1);
                pLine[nMaxChars - 1] = 0;
                SeekGet(SEEK_CURRENT, nLen - 1 - nMaxChars);
            }
        }
        void CUtlBuffer::GetDelimitedString(CUtlCharConversion *pConv, char *pString, int nMaxChars)
        {
            if(!IsText() || !pConv) {
                GetString(pString, nMaxChars);
                return;
            }

            if(!IsValid()) {
                *pString = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            EatWhiteSpace();
            if(!PeekStringMatch(0, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                return;

            // Pull off the starting delimiter
            SeekGet(SEEK_CURRENT, pConv->GetDelimiterLength());

            int nRead = 0;
            while(IsValid()) {
                if(PeekStringMatch(0, pConv->GetDelimiter(), pConv->GetDelimiterLength())) {
                    SeekGet(SEEK_CURRENT, pConv->GetDelimiterLength());
                    break;
                }

                char c = GetDelimitedCharInternal(pConv);

                if(nRead < nMaxChars) {
                    pString[nRead] = c;
                    ++nRead;
                }
            }

            if(nRead >= nMaxChars) {
                nRead = nMaxChars - 1;
            }
            pString[nRead] = '\0';
        }
        const void* CUtlBuffer::PeekGet(int nMaxSize, int nOffset)
        {
            if(!CheckPeekGet(nOffset, nMaxSize))
                return NULL;
            return &m_Memory[m_Get + nOffset - m_nOffset];
        }
        void CUtlBuffer::SeekGet(SeekType_t type, int offset)
        {
            switch(type) {
                case SEEK_HEAD:
                    m_Get = offset;
                    break;

                case SEEK_CURRENT:
                    m_Get += offset;
                    break;

                case SEEK_TAIL:
                    m_Get = m_nMaxPut - offset;
                    break;
            }

            if(m_Get > m_nMaxPut) {
                m_Error |= GET_OVERFLOW;
            } else {
                m_Error &= ~GET_OVERFLOW;
                if(m_Get < m_nOffset || m_Get >= m_nOffset + Size()) {
                    OnGetOverflow(-1);
                }
            }
        }
        void CUtlBuffer::Put(const void *pMem, int size)
        {
            if(size && CheckPut(size)) {
                memcpy(&m_Memory[m_Put - m_nOffset], pMem, size);
                m_Put += size;

                AddNullTermination();
            }
        }
        void CUtlBuffer::PutString(const char* pString)
        {
            if(!IsText()) {
                if(pString) {
                    // Not text? append a null at the end.
                    size_t nLen = strlen(pString) + 1;
                    Put(pString, nLen * sizeof(char));
                    return;
                } else {
                    PutTypeBin<char>(0);
                }
            } else if(pString) {
                int nTabCount = (m_Flags & AUTO_TABS_DISABLED) ? 0 : m_nTab;
                if(nTabCount > 0) {
                    if(WasLastCharacterCR()) {
                        PutTabs();
                    }

                    const char* pEndl = strchr(pString, '\n');
                    while(pEndl) {
                        size_t nSize = (size_t)pEndl - (size_t)pString + sizeof(char);
                        Put(pString, nSize);
                        pString = pEndl + 1;
                        if(*pString) {
                            PutTabs();
                            pEndl = strchr(pString, '\n');
                        } else {
                            pEndl = NULL;
                        }
                    }
                }
                size_t nLen = strlen(pString);
                if(nLen) {
                    Put(pString, nLen * sizeof(char));
                }
            }
        }
        inline void CUtlBuffer::PutDelimitedCharInternal(CUtlCharConversion *pConv, char c)
        {
            int l = pConv->GetConversionLength(c);
            if(l == 0) {
                PutChar(c);
            } else {
                PutChar(pConv->GetEscapeChar());
                Put(pConv->GetConversionString(c), l);
            }
        }
        void CUtlBuffer::PutDelimitedChar(CUtlCharConversion *pConv, char c)
        {
            if(!IsText() || !pConv) {
                PutChar(c);
                return;
            }

            PutDelimitedCharInternal(pConv, c);
        }
        void CUtlBuffer::PutDelimitedString(CUtlCharConversion *pConv, const char *pString)
        {
            if(!IsText() || !pConv) {
                PutString(pString);
                return;
            }

            if(WasLastCharacterCR()) {
                PutTabs();
            }
            Put(pConv->GetDelimiter(), pConv->GetDelimiterLength());

            int nLen = pString ? strlen(pString) : 0;
            for(int i = 0; i < nLen; ++i) {
                PutDelimitedCharInternal(pConv, pString[i]);
            }

            if(WasLastCharacterCR()) {
                PutTabs();
            }
            Put(pConv->GetDelimiter(), pConv->GetDelimiterLength());
        }
        void CUtlBuffer::VaPrintf(const char* pFmt, va_list list)
        {
            char temp[2048];
            int nLen = vsnprintf(temp, sizeof(temp), pFmt, list);
            assert(nLen < 2048);
            PutString(temp);
        }
        void CUtlBuffer::Printf(const char* pFmt, ...)
        {
            va_list args;

            va_start(args, pFmt);
            VaPrintf(pFmt, args);
            va_end(args);
        }
        void CUtlBuffer::SetOverflowFuncs(UtlBufferOverflowFunc_t getFunc, UtlBufferOverflowFunc_t putFunc)
        {
            m_GetOverflowFunc = getFunc;
            m_PutOverflowFunc = putFunc;
        }
        void CUtlBuffer::SeekPut(SeekType_t type, int offset)
        {
            int nNextPut = m_Put;
            switch(type) {
                case SEEK_HEAD:
                    nNextPut = offset;
                    break;

                case SEEK_CURRENT:
                    nNextPut += offset;
                    break;

                case SEEK_TAIL:
                    nNextPut = m_nMaxPut - offset;
                    break;
            }

            // Force a write of the data
            // FIXME: We could make this more optimal potentially by writing out
            // the entire buffer if you seek outside the current range

            // NOTE: This call will write and will also seek the file to nNextPut.
            OnPutOverflow(-nNextPut - 1);
            m_Put = nNextPut;

            AddNullTermination();
        }
        void CUtlBuffer::ActivateByteSwapping(bool bActivate)
        {
            m_Byteswap.ActivateByteSwapping(bActivate);
        }
        void CUtlBuffer::SetBigEndian(bool bigEndian)
        {
            m_Byteswap.SetTargetBigEndian(bigEndian);
        }
        bool CUtlBuffer::IsBigEndian(void)
        {
            return m_Byteswap.IsTargetBigEndian();
        }
        void CUtlBuffer::AddNullTermination(void)
        {
            if(m_Put > m_nMaxPut) {
                if(!IsReadOnly() && ((m_Error & PUT_OVERFLOW) == 0)) {
                    // Add null termination value
                    if(CheckPut(1)) {
                        m_Memory[m_Put - m_nOffset] = 0;
                    } else {
                        // Restore the overflow state, it was valid before...
                        m_Error &= ~PUT_OVERFLOW;
                    }
                }
                m_nMaxPut = m_Put;
            }
        }
CUtlBinaryBlock::CUtlBinaryBlock(void* pMemory, int nSizeInBytes, int nInitialLength) : m_Memory((unsigned char*)pMemory, nSizeInBytes)
{
    m_nActualLength = nInitialLength;
}
CUtlBinaryBlock::CUtlBinaryBlock(const void* pMemory, int nSizeInBytes) : m_Memory((const unsigned char*)pMemory, nSizeInBytes)
{
    m_nActualLength = nSizeInBytes;
}
void CUtlBinaryBlock::Get(void *pValue, int nLen) const
{
    assert(nLen > 0);
    if(m_nActualLength < nLen) {
        nLen = m_nActualLength;
    }

    if(nLen > 0) {
        memcpy(pValue, m_Memory.Base(), nLen);
    }
}
void CUtlBinaryBlock::SetLength(int nLength)
{
    assert(!m_Memory.IsReadOnly());

    m_nActualLength = nLength;
    if(nLength > m_Memory.NumAllocated()) {
        int nOverFlow = nLength - m_Memory.NumAllocated();
        m_Memory.Grow(nOverFlow);

        // If the reallocation failed, clamp length
        if(nLength > m_Memory.NumAllocated()) {
            m_nActualLength = m_Memory.NumAllocated();
        }
    }

#ifdef _DEBUG
    if(m_Memory.NumAllocated() > m_nActualLength) {
        memset(((char *)m_Memory.Base()) + m_nActualLength, 0xEB, m_Memory.NumAllocated() - m_nActualLength);
    }
#endif
}
void CUtlBinaryBlock::Set(const void *pValue, int nLen)
{
    assert(!m_Memory.IsReadOnly());

    if(!pValue) {
        nLen = 0;
    }

    SetLength(nLen);

    if(m_nActualLength) {
        if(((const char *)m_Memory.Base()) >= ((const char *)pValue) + nLen ||
            ((const char *)m_Memory.Base()) + m_nActualLength <= ((const char *)pValue)) {
            memcpy(m_Memory.Base(), pValue, m_nActualLength);
        } else {
            memmove(m_Memory.Base(), pValue, m_nActualLength);
        }
    }
}
CUtlString::CUtlString(void* pMemory, int nSizeInBytes, int nInitialLength) : m_Storage(pMemory, nSizeInBytes, nInitialLength)
{
}
CUtlString::CUtlString(const void* pMemory, int nSizeInBytes) : m_Storage(pMemory, nSizeInBytes)
{
}
void CUtlString::Set(const char *pValue)
{
    assert(!m_Storage.IsReadOnly());
    int nLen = pValue ? strlen(pValue) + 1 : 0;
    m_Storage.Set(pValue, nLen);
}
void CUtlString::SetLength(int nLen)
{
    assert(!m_Storage.IsReadOnly());

    // Add 1 to account for the NULL
    m_Storage.SetLength(nLen > 0 ? nLen + 1 : 0);
}
void NetvarSys::Initialize()
{
    database.clear();

    for(auto clientclass = g_CHLClient->GetAllClasses();
        clientclass != nullptr;
        clientclass = clientclass->m_pNext) {
        if(clientclass->m_pRecvTable) {
            database.emplace_back(LoadTable(clientclass->m_pRecvTable));
        }
    }
}
void NetvarSys::Dump()
{
    auto outfile = std::ofstream("netvar_dump.txt");

    Dump(outfile);
}
void NetvarSys::Dump(std::ostream& stream)
{
    for(const auto& table : database) {
        if(table.child_props.empty() && table.child_tables.empty())
            continue;
        stream << table.name << '\n';
        DumpTable(stream, table, 1);
        stream << '\n';
    }

    stream << std::endl;
}
void NetvarSys::DumpTable(std::ostream& stream, const netvar_table& table, uint32_t indentation)
{
    char line_buffer[1024];

    for(const auto& prop : table.child_props) {
        sprintf_s(line_buffer, "%*c%*s: 0x%08X", indentation * 4, ' ', -(50 - (int)indentation * 4), prop->m_pVarName, table.offset + prop->m_Offset);
        stream << line_buffer << '\n';
    }
    for(const auto& child : table.child_tables) {
        sprintf_s(line_buffer, "%*c%*s: 0x%08X", indentation * 4, ' ', -(50 - (int)indentation * 4), child.prop->m_pVarName, table.offset + child.offset);
        stream << line_buffer << '\n';
        DumpTable(stream, child, indentation + 1);
    }
}
    void Initialize()
    {
        auto engineFactory    = get_module_factory(GetModuleHandleW(L"engine.dll"));
        auto clientFactory    = get_module_factory(GetModuleHandleW(L"client.dll"));
        auto valveStdFactory  = get_module_factory(GetModuleHandleW(L"vstdlib.dll"));
        auto vguiFactory      = get_module_factory(GetModuleHandleW(L"vguimatsurface.dll"));
        auto vgui2Factory     = get_module_factory(GetModuleHandleW(L"vgui2.dll"));
        auto matSysFactory    = get_module_factory(GetModuleHandleW(L"materialsystem.dll"));
        auto dataCacheFactory = get_module_factory(GetModuleHandleW(L"datacache.dll"));
        auto vphysicsFactory  = get_module_factory(GetModuleHandleW(L"vphysics.dll"));
        
        g_CHLClient           = get_interface<IBaseClientDLL>      (clientFactory   , "VClient018");
        g_EntityList          = get_interface<IClientEntityList>   (clientFactory   , "VClientEntityList003");
        g_Prediction          = get_interface<IPrediction>         (clientFactory   , "VClientPrediction001");
        g_GameMovement        = get_interface<CGameMovement>       (clientFactory   , "GameMovement001");
        g_MdlCache            = get_interface<IMDLCache>           (dataCacheFactory, "MDLCache004");
        g_EngineClient        = get_interface<IVEngineClient>      (engineFactory   , "VEngineClient014");
        g_MdlInfo             = get_interface<IVModelInfoClient>   (engineFactory   , "VModelInfoClient004");
        g_MdlRender           = get_interface<IVModelRender>       (engineFactory   , "VEngineModel016");
        g_RenderView          = get_interface<IVRenderView>        (engineFactory   , "VEngineRenderView014");
        g_EngineTrace         = get_interface<IEngineTrace>        (engineFactory   , "EngineTraceClient004");
        g_DebugOverlay        = get_interface<IVDebugOverlay>      (engineFactory   , "VDebugOverlay004");
        g_GameEvents          = get_interface<IGameEventManager2>  (engineFactory   , "GAMEEVENTSMANAGER002");
        g_EngineSound         = get_interface<IEngineSound>        (engineFactory   , "IEngineSoundClient003");
        g_MatSystem           = get_interface<IMaterialSystem>     (matSysFactory   , "VMaterialSystem080");
        g_CVar                = get_interface<ICvar>               (valveStdFactory , "VEngineCvar007");
        g_VGuiPanel           = get_interface<IPanel>              (vgui2Factory    , "VGUI_Panel009");
        g_VGuiSurface         = get_interface<ISurface>            (vguiFactory     , "VGUI_Surface031");
        g_PhysSurface         = get_interface<IPhysicsSurfaceProps>(vphysicsFactory , "VPhysicsSurfaceProps001");

        auto client = GetModuleHandleW(L"client.dll");
        auto engine = GetModuleHandleW(L"engine.dll");
        auto dx9api = GetModuleHandleW(L"shaderapidx9.dll");

        g_GlobalVars      =  **(CGlobalVarsBase***)(Utils::PatternScan(client, "A1 ? ? ? ? 5E 8B 40 10") + 1);
        g_ClientMode      =        *(IClientMode**)(Utils::PatternScan(client, "A1 ? ? ? ? 8B 80 ? ? ? ? 5D") + 1);
        g_Input           =             *(CInput**)(Utils::PatternScan(client, "B9 ? ? ? ? 8B 40 38 FF D0 84 C0 0F 85") + 1);
        g_MoveHelper      =      **(IMoveHelper***)(Utils::PatternScan(client, "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01") + 2);
        g_GlowObjManager  = *(CGlowObjectManager**)(Utils::PatternScan(client, "0F 11 05 ? ? ? ? 83 C8 01") + 3);
        g_ViewRender      =        *(IViewRender**)(Utils::PatternScan(client, "A1 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? FF 10") + 1);
        g_D3DDevice9      = **(IDirect3DDevice9***)(Utils::PatternScan(dx9api, "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
        g_ClientState     =     **(CClientState***)(Utils::PatternScan(engine, "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1);

        g_LocalPlayer     = *(C_LocalPlayer*)(Utils::PatternScan(client, "8B 0D ? ? ? ? 83 FF FF 74 07") + 2);
    }
    void Dump()
    {
        // Ugly macros ugh
        #define STRINGIFY_IMPL(s) #s
        #define STRINGIFY(s)      STRINGIFY_IMPL(s)
        #define PRINT_INTERFACE(name) Utils::ConsolePrint("%-20s: %p\n", STRINGIFY(name), name)

        PRINT_INTERFACE(g_CHLClient   );
        PRINT_INTERFACE(g_EntityList  );
        PRINT_INTERFACE(g_Prediction  );
        PRINT_INTERFACE(g_GameMovement);
        PRINT_INTERFACE(g_MdlCache    );
        PRINT_INTERFACE(g_EngineClient);
        PRINT_INTERFACE(g_MdlInfo     );
        PRINT_INTERFACE(g_MdlRender   );
        PRINT_INTERFACE(g_RenderView  );
        PRINT_INTERFACE(g_EngineTrace );
        PRINT_INTERFACE(g_DebugOverlay);
        PRINT_INTERFACE(g_GameEvents  );
        PRINT_INTERFACE(g_EngineSound );
        PRINT_INTERFACE(g_MatSystem   );
        PRINT_INTERFACE(g_CVar        );
        PRINT_INTERFACE(g_VGuiPanel   );
        PRINT_INTERFACE(g_VGuiSurface );
        PRINT_INTERFACE(g_PhysSurface );
    }
void CVisuals::Run()
{
	if (vars.visuals.player.deadonly && g_LocalPlayer->IsAlive())
		return;
	if (vars.visuals.player.ammo || vars.visuals.player.armourbar || vars.visuals.player.box
		|| vars.visuals.player.healthbar || vars.visuals.player.information
		|| vars.visuals.player.name || vars.visuals.player.skeleton ||
		vars.visuals.player.snaplines || vars.visuals.player.weapon ||
		vars.visuals.world.weapons || vars.visuals.world.bomb){

		for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
			auto ent = C_BaseEntity::GetEntityByIndex(i);
			if (!ent)
				continue;
			C_BasePlayer* entity = nullptr;
			if (ent->GetClientClass()->m_ClassID == ClassId_CCSPlayer)
			{
				entity = (C_BasePlayer*)ent;
				if (!entity)
					continue;

				if (entity == g_LocalPlayer)
					continue;

				if (i < 65 && !entity->IsDormant() && entity->IsAlive()) {
					Main(entity);
				}
				continue;
			}
			if (strstr(ent->GetClientClass()->m_pNetworkName, "Weapon"))
			{
				C_BaseCombatWeapon* weap = (C_BaseCombatWeapon*)ent;
				if (vars.visuals.world.weapons)
				{
					auto hdr = g_MdlInfo->GetStudioModel(weap->GetModel());

					if (strstr(hdr->szName, "dropped"))
					{
						Vector out;
						if (Math::WorldToScreen(weap->GetRenderOrigin(), out))
						{
							ESPBOX box = GetBox(ent);
							Render::Box(box.x, box.y, box.w, box.h,
								false, Color(255,255,255));
							int ammo = weap->m_iClip1();
							std::string weapn = weap->GetClientClass()->m_pNetworkName;
							if (weapn == "CBaseWeaponWorldModel")
								continue;
							weapn.erase(weapn.begin(), weapn.begin() + 7);
							wchar_t buffer[50];
							swprintf(buffer, 50, L"%s [ammo: %u]", 
								Utils::to_wstring(weapn.c_str()).data(),ammo);
							Render::Text(out.x, out.y, true, false, font, Color::White,
								buffer);
						}
					}
					
					
				}
			}
		}
	}
}
void CVisuals::CreateFonts()
{
	font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(font, "Arial", 13, 700, 0, 0, FONTFLAG_DROPSHADOW);
}
void CVisuals::DestroyFonts(){}
void CVisuals::Box()
{
	Render::Box(data.box.x, data.box.y, data.box.w, data.box.h,
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
void CVisuals::Skeleton()
{
	studiohdr_t* pStudioModel = g_MdlInfo->GetStudioModel(data.pl->GetModel());
	if (pStudioModel)
	{
		static matrix3x4_t pBoneToWorldOut[128];
		if (data.pl->SetupBones(pBoneToWorldOut, 128, 256, g_GlobalVars->curtime))
		{
			for (int i = 0; i < pStudioModel->numbones; i++)
			{
				mstudiobone_t* pBone = pStudioModel->GetBone(i);
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
		font, data.col, Utils::to_wstring(data.pl->GetPlayerInfo().szName).data());
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
	if (health < 100)
	{
		wchar_t buffer[5];
		swprintf(buffer, 5, L"%u", health);
		Render::Text(data.box.x - 3, data.box.y + cutoff, true, false, font, Color::White, buffer);
	}

}
void CVisuals::ArmourBar()
{
	
	int health = data.pl->m_ArmorValue();
	int cutoff = (data.box.h - data.box.y) - ((data.box.h - data.box.y) * health / 100);
	Render::CleanBox(data.box.w + 1, data.box.y, data.box.w + 6, data.box.h, true, Color(0, 0, 0, 150));
	Render::CleanBox(data.box.w + 2, data.box.y + cutoff + 1, data.box.w + 5, data.box.h - 1, true, Color(66, 134, 244));
	if (health < 100)
	{
		wchar_t buffer[5];
		swprintf(buffer, 5, L"%u", health);
		Render::Text(data.box.w + 3, data.box.y + cutoff, true, false, font, Color::White, buffer);
	}
}
void CVisuals::Weapon()
{
	std::wstring w = Utils::to_wstring(data.pl->m_hActiveWeapon()->GetCSWeaponData()->szWeaponName);
	int ammo = data.pl->m_hActiveWeapon()->m_iClip1();
	int maxammo =  data.pl->m_hActiveWeapon()->GetCSWeaponData()->iMaxClip1;
	
	
	if (vars.visuals.player.ammo)
	{
		wchar_t buffer[30];
		swprintf(buffer, 30, L"%s  [%d/%d]", w.data(), ammo, maxammo);
		Render::Text(data.box.x + ((data.box.w - data.box.x) / 2), data.box.h + 10, true, false,
			font, data.col, buffer);
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
void CVisuals::Info()
{
	std::vector<std::wstring> info;

	if (data.pl->HasC4())
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

	if (data.pl->m_hActiveWeapon().Get()->IsReloading())
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
