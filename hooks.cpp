#include "hooks.hpp"

#include "menu.h"
#include "options.hpp"
#include "helpers/input.hpp"
#include "backtrake.h"
#include "helpers/utils.hpp"
#include "features/bhop.hpp"
#include "features/chams.hpp"
#include "Visuals.h"
#include "aim_assistance.h"
#include "features/glow.hpp"
#include "Rage.h"
#include "engine_prediction.h"
#include "ItemDefinitions.h"
#include "chrome.h"
#include "triggerbot.h"
#include "skinchanger.h"
#include "notificationsystem.h"
#include "game_events.h"

std::vector<const char*> smoke_materials = {
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
	};
std::unique_ptr<C_EventListener>          g_EventListener = nullptr;

#define uffyaa$ resolver.run()
static QAngle real;
namespace Hooks
{
	void thirdperson();
	vfunc_hook hlclient_hook;
	vfunc_hook direct3d_hook;
	vfunc_hook vguipanel_hook;
	vfunc_hook vguisurf_hook;
	vfunc_hook mdlrender_hook;
	vfunc_hook clientmode_hook;

	void Initialize()
	{
		hlclient_hook.setup(g_CHLClient);
		direct3d_hook.setup(g_D3DDevice9);
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		mdlrender_hook.setup(g_MdlRender);
		clientmode_hook.setup(g_ClientMode);

		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);

		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);

		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);

		vguisurf_hook.hook_index(index::PlaySound, hkPlaySound);
		vguisurf_hook.hook_index(67, hkLockCursor);

		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);

		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(18, hkOverrideView);
		g_EventListener = std::make_unique<C_EventListener>();
		Visuals.CreateFonts();
		menu.Initialize();
	}
	template<class T, class U>
	T clamp(T in, U low, U high)
	{
		if (in <= low)// yes $ idk how 2 hook endscene tho
			return low;

		if (in >= high)
			return high;

		return in;
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
	//--------------------------------------------------------------------------------
	void Shutdown()
	{
		//g_EventListener  ~C_EventListener();=
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();

		Glow::Get().Shutdown();

		Visuals.DestroyFonts();
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* device)
	{
		auto oEndScene = direct3d_hook.get_original<EndScene>(index::EndScene);

		menu.Render();

		static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");
		static auto mat_ambient_light_r = g_CVar->FindVar("mat_ambient_light_r");
		static auto mat_ambient_light_g = g_CVar->FindVar("mat_ambient_light_g");
		static auto mat_ambient_light_b = g_CVar->FindVar("mat_ambient_light_b");

		viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
		viewmodel_fov->SetValue(vars.visuals.world.vmodelfov);
		static auto linegoesthrusmoke = Utils::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0", "xxxxxxxx????xxx");
		static auto smokecout = *(DWORD*)(linegoesthrusmoke + 0x8);
		if (vars.visuals.world.nosmoke) *(int*)(smokecout) = 0;
		if (vars.visuals.world.noflash && local) local->m_flFlashMaxAlpha() = 0;

		if (vars.visuals.world.update)
		{

			static auto fog_enableskybox = g_CVar->FindVar("fog_enableskybox");
			static auto fog_override = g_CVar->FindVar("fog_override");
			static auto fog_enable = g_CVar->FindVar("fog_enable");
			static auto draw_specific_static_prop = g_CVar->FindVar("r_DrawSpecificStaticProp");

			fog_enableskybox->m_nFlags &= ~FCVAR_CHEAT;
			fog_override->m_nFlags &= ~FCVAR_CHEAT;
			fog_enable->m_nFlags &= ~FCVAR_CHEAT;
			draw_specific_static_prop->m_nFlags &= ~FCVAR_CHEAT;

			// set values
			fog_enable->SetValue(0.0f);
			fog_enableskybox->SetValue(0.0f);
			fog_override->SetValue(1.0f);
			draw_specific_static_prop->SetValue(1.0f);

			for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
			{
				IMaterial *pMaterial = g_MatSystem->GetMaterial(i);
				if (!pMaterial || pMaterial->IsErrorMaterial())
					continue;
				else
				{
					if (!vars.visuals.world.nightmode) {
						if (strstr(pMaterial->GetTextureGroupName(), TEXTURE_GROUP_SKYBOX))
						{
							pMaterial->ColorModulate(1, 1, 1);
						}
					}
				}
				if (vars.visuals.world.nightmode)
				{


					if (strstr(pMaterial->GetTextureGroupName(), "World textures"))
					{
						pMaterial->ColorModulate(0.1, 0.1, 0.1);
					}
					if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
					{
						pMaterial->ColorModulate(0.1, 0.1, 0.1);
					}
					if (strstr(pMaterial->GetTextureGroupName(), "models/props/de_dust/palace_bigdome"))
					{
						pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
					}
					if (strstr(pMaterial->GetTextureGroupName(), "models/props/de_dust/palace_pillars"))
					{
						pMaterial->ColorModulate(0.1, 0.1, 0.1);
					}

					if (strstr(pMaterial->GetTextureGroupName(), "Particle textures"))
					{
						pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
					}

				}
				else
				{

					if (strstr(pMaterial->GetTextureGroupName(), "World textures"))
					{
						pMaterial->ColorModulate(1, 1, 1);
					}
					if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
					{
						pMaterial->ColorModulate(1, 1, 1);
					}
					if (strstr(pMaterial->GetTextureGroupName(), "models/props/de_dust/palace_bigdome"))
					{
						pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
					}
					if (strstr(pMaterial->GetTextureGroupName(), "models/props/de_dust/palace_pillars"))
					{
						pMaterial->ColorModulate(1, 1, 1);
					}

					if (strstr(pMaterial->GetTextureGroupName(), "Particle textures"))
					{
						pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
					}
				}

			}
			vars.visuals.world.update = false;
		}

		return oEndScene(device);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		auto oReset = direct3d_hook.get_original<Reset>(index::Reset);

		Visuals.DestroyFonts();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0) {
			Visuals.CreateFonts();
		}

		return hr;
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		auto oCreateMove = hlclient_hook.get_original<CreateMove>(index::CreateMove);

		oCreateMove(g_CHLClient, sequence_number, input_sample_frametime, active);

		/*if (g_EngineClient->IsConnected() && g_EngineClient->IsInGame() && g_ClientState &&
			oSendDatagram != g_Hooks.pNetChannelHook.GetOriginal<SendDatagram_t>(VT::NetChannel::SendDatagram)
		{
			INetChannel* netchan = (INetChannel*)(g_pClientState->m_NetChannel);
			if (netchan)
			{
				static bool bOnce = false;
				if (!bOnce)
				{
					g_Hooks.pNetChannelHook.Init(netchan);
					g_Hooks.pNetChannelHook.Hook(vtable_indexes::SendDatagram, Hooks::SendDatagram);
					oSendDatagram = g_Hooks.pNetChannelHook.GetOriginal<SendDatagram_t>(vtable_indexes::SendDatagram);
					bOnce = true;
				}
				else
				{
					g_Hooks.pNetChannelHook.Unhook(vtable_indexes::SendDatagram);
					g_Hooks.pNetChannelHook.Hook(vtable_indexes::SendDatagram, Hooks::SendDatagram);
					oSendDatagram = g_Hooks.pNetChannelHook.GetOriginal<SendDatagram_t>(vtable_indexes::SendDatagram);
				}
			}
		}
		*/
		try
		{
			if (InputSys::Get().WasKeyPressed(vars.rage.backupaa))
				vars.rage.baa = !vars.rage.baa;
			if (InputSys::Get().WasKeyPressed(vars.rage.backupaaswitch))
				vars.rage.baaside = !vars.rage.baaside;
			auto cmd = g_Input->GetUserCmd(sequence_number);
			auto verified = g_Input->GetVerifiedCmd(sequence_number);

			if (!cmd || !cmd->command_number || !verified)
				return;
			QAngle wish_angle = cmd->viewangles;
			static int fakelag;
			if (vars.misc.fakelag) {
				if (fakelag < 13) {
					bSendPacket = false;
					fakelag++;
				}
				else
					fakelag = 0;
			}
			if (vars.misc.bhop) {
				BunnyHop::OnCreateMove(cmd);
			}
			engine_prediction::run(cmd);
			triggerbot::createmove(cmd);

			if (vars.rage.active) {
				rage.Run(cmd, bSendPacket);

			}
			else if (vars.legit.active)
			{
				LegitBot.move(cmd, bSendPacket);
				static bool last_tick_ghetto_fix_meme = false;
				if (!last_tick_ghetto_fix_meme && (cmd->buttons & IN_ATTACK))
				{
					cmd->buttons &= ~IN_ATTACK;
					last_tick_ghetto_fix_meme = true;
				}
				else {
					if (!(cmd->buttons & IN_ATTACK))
						last_tick_ghetto_fix_meme = false;
				}
			}
			LegitBot.legitaa(cmd, bSendPacket);
			//C_LagComp::Get().LegitBacktrack(cmd);
			//backtrack.CreateMove(cmd);
			cmd->viewangles.Normalize();
			cmd->viewangles.Clamp();
			//game.paint();
			FixMovement(cmd, wish_angle);
			engine_prediction::end();
			verified->m_cmd = *cmd;
			verified->m_crc = cmd->GetChecksum();
		}
		catch (std::exception ex)
		{
			Utils::ConsolePrint("hkCreateMove error. Please send this code to the developer: %s", ex.what());
		}
	}
	//--------------------------------------------------------------------------------
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
	//--------------------------------------------------------------------------------
	void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<PaintTraverse>(index::PaintTraverse);

		oPaintTraverse(g_VGuiPanel, panel, forceRepaint, allowForce);


		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel) {
			static bool init;
			if (!init) {
				globs.ufont = g_VGuiSurface->CreateFont_();
				g_VGuiSurface->SetFontGlyphSet(globs.ufont, "Arial", 13, 700, 0, 0, FONTFLAG_NONE);
				init = true;
			}
			Render::Text(10, 10, false, false, globs.ufont, Color(200, 200, 200), L"meeza | welcome control, xboom, bby");
			if (g_EngineClient->IsInGame()) {

				if (!local)
					return;
				if (vars.rage.aa && vars.rage.active)
				{
					if (vars.rage.baa) {
						Render::Text(10, 20, false, false, globs.ufont, Color(200, 0, 0), L"WARNING BACKUP AA ON");
						int w, h;
						g_EngineClient->GetScreenSize(w, h);
						if (vars.rage.baaside) {
							Render::Text(w / 2, h / 2 + 20, true, false, globs.ufont, Color(0, 100, 200), L"<left>");
						}
						else {
							Render::Text(w / 2, h / 2 + 20, true, false, globs.ufont, Color(0, 100, 200), L"<right>");
						}
					}
					int w, h;
					g_EngineClient->GetScreenSize(w, h);
					float sz = Render::GetTextSize(L"MEEZA APOCALYPSE PASTE", globs.lbyfont).y;
					if (fabsf(globs.real.yaw - local->mlby()) > 35)
						Render::Text(0, h - sz - 100, false, true, globs.lbyfont, Color(0, 255, 0), L"LBY");
					else
						Render::Text(0, h - sz - 100, false, true, globs.lbyfont, Color(255, 0, 0), L"LBY");
				}
				Visuals.Run();
				if (vars.misc.aalines)
				{
					{
						Vector forward;
						Math::AngleVectors(QAngle(0, globs.fake.yaw, 0), forward);
						Vector end = local->m_vecOrigin() + forward * 40;

						Vector start2d, end2d;
						if (Math::WorldToScreen(local->m_vecOrigin(), start2d) && Math::WorldToScreen(end, end2d)) {
							Render::Line(start2d.x, start2d.y, end2d.x, end2d.y, Color(200, 200, 0));
							Render::Text(end2d.x, end2d.y, true, false, globs.ufont, Color(200, 200, 0), L"Fake");
						}
					}
					{
						Vector forward;
						Math::AngleVectors(QAngle(0, globs.real.yaw, 0), forward);
						Vector end = local->m_vecOrigin() + forward * 40;

						Vector start2d, end2d;
						if (Math::WorldToScreen(local->m_vecOrigin(), start2d) && Math::WorldToScreen(end, end2d)) {
							Render::Line(start2d.x, start2d.y, end2d.x, end2d.y, Color(200, 0, 0));
							Render::Text(end2d.x, end2d.y, true, false, globs.ufont, Color(200, 0, 0), L"Real");
						}
					}
					{
						Vector forward;
						Math::AngleVectors(QAngle(0, local->mlby(), 0), forward);
						Vector end = local->m_vecOrigin() + forward * 40;

						Vector start2d, end2d;
						if (Math::WorldToScreen(local->m_vecOrigin(), start2d) && Math::WorldToScreen(end, end2d)) {
							Render::Line(start2d.x, start2d.y, end2d.x, end2d.y, Color(0, 200, 0));
							Render::Text(end2d.x, end2d.y, true, false, globs.ufont, Color(0, 200, 0), L"LBY");
						}
					}
				}

			}
			//menu.PaintTraverse();
			NotificatonSys.painttransverse(); //always... always.. run this after the menu
			if (vars.misc.hitmarker)
				hitmarker.paint();
		}

	}
	//--------------------------------------------------------------------------------
	void __stdcall hkPlaySound(const char* name)
	{
		static auto oPlaySound = vguisurf_hook.get_original<PlaySound>(index::PlaySound);

		oPlaySound(g_VGuiSurface, name);

		// Auto Accept
		if (strstr(name, "UI/competitive_accept_beep.wav")) {
			static auto fnAccept =
				(void(*)())Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 83 BE");

			fnAccept();

			//This will flash the CSGO window on the taskbar
			//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
			FLASHWINFO fi;
			fi.cbSize = sizeof(FLASHWINFO);
			fi.hwnd = InputSys::Get().GetMainWindow();
			fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
			fi.uCount = 0;
			fi.dwTimeout = 0;
			FlashWindowEx(&fi);
		}
	}
	void __stdcall hkOverrideView(CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<OverrideView>(18);

		if (g_EngineClient->IsInGame() && vsView && vars.misc.thirdperson) {
			//thirdperson();

			//will paste this in the future
			/*if (g_LocalPlayer && g_Options.esp_grenadeprediction)
				C_GrenadePrediction::Get().View(vsView);*/

		}

		ofunc(g_ClientMode, vsView);
	}
	//--------------------------------------------------------------------------------
	int __stdcall hkDoPostScreenEffects(int a1)
	{
		auto oDoPostScreenEffects = clientmode_hook.get_original<DoPostScreenEffects>(index::DoPostScreenSpaceEffects);

		if (local && vars.visuals.player.glow)
			Glow::Get().Run();

		return oDoPostScreenEffects(g_ClientMode, a1);
	}

	void ChangeSkins() {
		if (!local || !local->alive())
			return;
		int* hweapons = local->get_weapons();
		if (!hweapons)
			return;
		for (int i = 0; hweapons[i] != INVALID_EHANDLE_INDEX; i++)
		{
			auto weapon = (attributableitem_t*)g_EntityList->GetClientEntity(hweapons[i] & 0xFFF);
			if (!weapon)
				continue;

			int definition_index = weapon->m_Item().m_iItemDefinitionIndex();
			if (definition_index == 0 || definition_index == 31/*zeus*/)
				continue;
			auto w = (weapon_t*)weapon;
			if (w->is_knife())
			{
				w->m_Item().m_iEntityQuality() = 3;
				w->m_Item().m_iItemDefinitionIndex() = 508;
				int mdl = g_MdlInfo->GetModelIndex(ItemDefinitions::WeaponInfo.at(WEAPON_KNIFE_M9_BAYONET).model);
				w->m_nModelIndex() = mdl;
				w->m_iViewModelIndex() = mdl;
				w->m_nFallbackPaintKit() = vars.settings.skins[42].weapon_skin_id;
				auto pViewModel = local->m_hViewModel();
				if (!pViewModel) continue;

				auto hViewModelWeapon = pViewModel->m_hWeapon();
				if (!hViewModelWeapon) continue;
				auto pViewModelWeapon = static_cast<attributableitem_t*>(g_EntityList->GetClientEntityFromHandle(hViewModelWeapon));


				int nViewModelIndex = pViewModel->m_nModelIndex();

				const auto nOriginalKnifeCT = g_MdlInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
				const auto nOriginalKnifeT = g_MdlInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");




				if (nViewModelIndex == nOriginalKnifeCT || nViewModelIndex == nOriginalKnifeT)
					pViewModel->m_nModelIndex() = g_MdlInfo->GetModelIndex(ItemDefinitions::WeaponInfo.at(WEAPON_KNIFE_M9_BAYONET).model);

			}
			//else
			//	weapon->m_nFallbackPaintKit() = vars.settings.skins[definition_index].weapon_skin_id;
			//weapon->m_nFallbackPaintKit() = 446;


			//weapon->m_nFallbackSeed() = 32;
		//	weapon->m_nFallbackStatTrak() = -1;
			//weapon->m_flFallbackWear() = 0.001f;
			//weapon->m_nFallbackStatTrak() = config::get_st(definition_index);
			//weapon->m_Item().m_iItemIDHigh() = -1;
			//weapon->m_Item().m_iAccountID() = local->player_info().xuid_low;

		}

	}
	typedef void(__thiscall* LockCursor)(void*);
	LockCursor oLockCursor;

	void __stdcall hkLockCursor()
	{
		oLockCursor = Hooks::vguisurf_hook.get_original<LockCursor>(67);

		if (menu.IsVisible()) {
			g_VGuiSurface->UnlockCursor();
			return;
		}

		oLockCursor(g_VGuiSurface);
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<FrameStageNotify>(index::FrameStageNotify);
		if (InputSys::Get().WasKeyPressed(vars.misc.thirdpersonkey))
			vars.misc.thirdperson = !vars.misc.thirdperson;
		if (stage == FRAME_RENDER_START) //b1g pasta
		{
			thirdperson();
			/*if((g_EngineClient->IsInGame() && local &&g_Input))
			{
				static QAngle vecAngles;
				if (vars.rage.active && vars.rage.aa)
					vecAngles = globs.real;
				else
				g_EngineClient->GetViewAngles(vecAngles);
				local->m_angEyeAngles() = globs.real;
				if (vars.misc.thirdperson && local->alive())
				{
					if (!g_Input->m_fCameraInThirdPerson)
					{
						g_Input->m_fCameraInThirdPerson = true;
						g_EngineClient->ClientCmd_Unrestricted("thirdperson");
						g_Input->m_vecCameraOffset = Vector(vecAngles.pitch, vecAngles.yaw, vars.misc.thirdpersonrange);
					}
				}
				else
				{
					g_Input->m_fCameraInThirdPerson = false;
					g_EngineClient->ClientCmd_Unrestricted("firstperson");
					g_Input->m_vecCameraOffset = Vector(vecAngles.pitch, vecAngles.yaw, 0);
				}
				if (vars.misc.thirdperson)
				{
					*(bool*)((DWORD)g_Input + 0xA5) = true;
					*(float*)((DWORD)g_Input + 0xA8 + 0x8) = vars.misc.thirdpersonrange;
					*(QAngle*)((DWORD)local.operator->() + 0x31C8) = globs.real;
				}
				else
				{
					*(bool*)((DWORD)g_Input + 0xA5) = false;
					*(float*)((DWORD)g_Input + 0xA8 + 0x8);
				}
			}
			else
			{
				*(bool*)((DWORD)g_Input + 0xA5) = false;
				*(float*)((DWORD)g_Input + 0xA8 + 0x8);
			}*/
		}
		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
			skinchanger.frame_stage_notify();
			//if (vars.rage.resolver)
				//newresolver::run();
		}


		ofunc(g_CHLClient, stage);
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<DrawModelExecute>(index::DrawModelExecute);
		if (!(pInfo.flags & (1 << 28)))
		Chams::Get().OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

		ofunc(g_MdlRender, ctx, state, pInfo, pCustomBoneToWorld);

		g_MdlRender->ForcedMaterialOverride(nullptr);
	}

	void thirdperson() {
		if (!local)
			return;
		if (vars.misc.thirdperson) {
			if (local->alive())
			{
				if (!g_Input->m_fCameraInThirdPerson)
				{
					g_Input->m_fCameraInThirdPerson = true;
				}

				float dist = vars.misc.thirdpersonrange;

				QAngle* view = &local->m_angEyeAngles();
				trace_t tr;
				Ray_t ray;

				Vector desiredCamOffset = Vector(cos(DEG2RAD(view->yaw)) * dist,
					sin(DEG2RAD(view->yaw)) * dist,
					sin(DEG2RAD(-view->pitch)) * dist
				);

				//cast a ray from the Current camera Origin to the Desired 3rd person Camera origin
				ray.Init(local->eye_pos(), (local->eye_pos() - desiredCamOffset));
				CTraceFilter traceFilter;
				traceFilter.pSkip = local;
				g_EngineTrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

				Vector diff = local->eye_pos() - tr.endpos;

				float distance2D = sqrt(abs(diff.x * diff.x) + abs(diff.y * diff.y));// Pythagorean

				bool horOK = distance2D > (dist - 2.0f);
				bool vertOK = (abs(diff.z) - abs(desiredCamOffset.z) < 3.0f);

				float cameraDistance;

				if (horOK && vertOK)  // If we are clear of obstacles
				{
					cameraDistance = dist; // go ahead and set the distance to the setting
				}
				else
				{
					if (vertOK) // if the Vertical Axis is OK
					{
						cameraDistance = distance2D * 0.95f;
					}
					else// otherwise we need to move closer to not go into the floor/ceiling
					{
						cameraDistance = abs(diff.z) * 0.95f;
					}
				}
				g_Input->m_fCameraInThirdPerson = true;
				*(QAngle*)((DWORD)local.operator->() + 0x31C8) = globs.real;
				g_Input->m_vecCameraOffset.z = cameraDistance;
			}
			else
			{
				g_Input->m_fCameraInThirdPerson = false;
			}
		}
		else
			g_Input->m_fCameraInThirdPerson = false;
	}
}
