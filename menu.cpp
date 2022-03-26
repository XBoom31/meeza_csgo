#include "menu.h"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/directx9/imgui_impl_dx9.h"
#include "helpers\utils.hpp"
#include "config.h"
#include "vguimenu.h"
Menu menu;


static ConVar* cl_mouseenable = nullptr;//no man ill update it then p

constexpr static float get_sidebar_item_width() { return 150.0f; }
constexpr static float get_sidebar_item_height() { return  50.0f; }

namespace ImGuiEx
{
	inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
	{
		auto clr = ImVec4{
			v->r() / 255.0f,
			v->g() / 255.0f,
			v->b() / 255.0f,
			v->a() / 255.0f
		};

		if (ImGui::ColorEdit4(label, &clr.x, show_alpha)) {
			v->SetColor(clr.x, clr.y, clr.z, clr.w);
			return true;
		}
		return false;
	}
	inline bool ColorEdit3(const char* label, Color* v)
	{
		return ColorEdit4(label, v, false);
	}
}

template<size_t N>
void render_tabs(const char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
	bool values[N] = { false };

	values[activetab] = true;

	for (auto i = 0u; i < N; ++i) {
		if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h })) {
			activetab = i;
		}
		if (sameline && i < N - 1)
			ImGui::SameLine();
	}
}

int get_fps()
{
	using namespace std::chrono;
	static int count = 0;
	static auto last = high_resolution_clock::now();
	auto now = high_resolution_clock::now();
	static int fps = 0;

	count++;

	if (duration_cast<milliseconds>(now - last).count() > 1000) {
		fps = count;
		count = 0;
		last = now;
	}

	return fps;
}

void Menu::Initialize()
{
	_visible = true;

	cl_mouseenable = g_CVar->FindVar("cl_mouseenable");

	ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), g_D3DDevice9);

	CreateStyle();
}

void Menu::Shutdown()
{
	ImGui_ImplDX9_Shutdown();
	cl_mouseenable->SetValue(true);
}

void Menu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void renderwconf(wconf* conf) {
	conf->hitscan = true;
	ImGui::Columns(2);
	ImGui::Checkbox("active", &conf->active);
	ImGui::Checkbox("silent", &conf->silent);
	ImGui::SliderFloat("fields of view", &conf->fov, 0, 40);
	ImGui::SliderFloat("rcs strenght", &conf->rcs, 0, 100);
	ImGui::SliderFloat("humanize strenght", &conf->humanize, 0, 100);
	ImGui::SliderFloat("smoothing", &conf->smooth, 0, 100);
	ImGui::SliderInt("backtrack", &conf->backtrack, 0, 12);
	ImGui::Checkbox("triggerbot", &conf->trigger);
	ImGui::NextColumn();
	ImGui::Hotkey("triggerbot key", &conf->triggerkey);
	ImGui::SliderInt("triggerbot hitchance", &conf->thitchance, 0, 100);
	ImGui::Checkbox("one tap mode", &conf->tap);
	ImGui::Checkbox("one tap mode safe", &conf->tapsafe);
	ImGui::SliderFloat("one tap mode snap", &conf->tapsnap, 0, 3);
}

void Menu::Render() //i can dont worry but where did u initialize your old menu class?
{
	if (!_visible)
		return;

	ImGui_ImplDX9_NewFrame();

	ImGui::GetIO().MouseDrawCursor = _visible;
	static int tabid;

	ImGui::PushStyle(_style);

	ImGui::SetNextWindowPos(ImVec2{ 100, 100 }, ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2{ 700, 350 }, ImGuiSetCond_Once);

	if (ImGui::Begin("meeza", 
		&_visible,
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoResize)) {

		if (ImGui::Button("legitbot", ImVec2(132, 30))) { tabid = 0; };
		ImGui::SameLine();
		if (ImGui::Button("ragebot", ImVec2(132, 30))) { tabid = 1; };
		ImGui::SameLine();
		if (ImGui::Button("visuals", ImVec2(132, 30))) { tabid = 2; };
		ImGui::SameLine();
		if (ImGui::Button("misc", ImVec2(132, 30))) { tabid = 3; };
		ImGui::SameLine();
		if (ImGui::Button("settings", ImVec2(132, 30))) { tabid = 4; };
		ImGui::Separator();
		switch (tabid)
		{
		case 0:
			ImGui::Checkbox("legitbot master switch  ", &vars.legit.active);
			ImGui::SameLine();
			ImGui::PushItemWidth(200);
			ImGui::Combo("legitbot mode", &vars.legit.mode, "simple\0normal\0advanced");	
			ImGui::Separator();
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(300);
			if (vars.legit.mode == 0)
				renderwconf(&vars.legit.conf);
			else if (vars.legit.mode == 2) {
				if (g_EngineClient->IsInGame() || local)
				{
					if (local->alive())
					{
						auto w = local->m_hActiveWeapon();
						if (w)
						{
							int wid = w->m_Item().m_iItemDefinitionIndex();
							ImGui::Text(w->GetClientClass()->m_pNetworkName);
							renderwconf(&vars.legit.weapons[wid]);
						}
					}
				}
				else
				{
					ImGui::Text("not in game");
				}
			}
			ImGui::PopItemWidth();
			break;
		case 1:
			ImGui::PushItemWidth(300);
			ImGui::Columns(2);
			ImGui::Checkbox("active", &vars.rage.active);
			ImGui::SliderInt("fields of view", &vars.rage.fov, 0, 180);
			ImGui::Checkbox("autowall", &vars.rage.awall);
			ImGui::Checkbox("autoshoot", &vars.rage.autoshoot);
			ImGui::Checkbox("no recoil", &vars.rage.norecoil);
			ImGui::Checkbox("silent", &vars.rage.sillent);
			ImGui::Checkbox("htiscan", &vars.rage.hitscan);
			ImGui::Checkbox("resolver", &vars.rage.resolver);
			ImGui::SliderInt("minimum damage", &vars.rage.mindmg, 0, 100);
			ImGui::NextColumn();
			ImGui::SliderInt("hitchance", &vars.rage.hitchance, 0, 100);
			ImGui::Checkbox("anti aim", &vars.rage.aa);
			ImGui::Hotkey("backup anti aim", &vars.rage.backupaa);
			ImGui::Hotkey("backup anti aim switch", &vars.rage.backupaaswitch);
			ImGui::Checkbox("auto revolver", &vars.rage.autorevolver);
			ImGui::Checkbox("auto stop", &vars.rage.autostop);
			ImGui::Columns(1);
			ImGui::PopItemWidth();
			break;
		case 2:	
			ImGui::Columns(2);
			ImGui::PushItemWidth(200);
			ImGui::Text("player"); 
			ImGui::Combo("box", &vars.visuals.player.box, "off\0full\0full clean\0corners");
			ImGui::Checkbox("name", &vars.visuals.player.name);
			ImGui::Combo("healthbar", &vars.visuals.player.healthbar, "off\0side\0side + text");
			ImGui::Combo("armour", &vars.visuals.player.armourbar, "off\0side\0side + text");
			ImGui::Checkbox("skeleton", &vars.visuals.player.skeleton);
			ImGui::Combo("weapon", &vars.visuals.player.weapon, "off\0normal\0without 'weapon_'");
			ImGui::Checkbox("ammo", &vars.visuals.player.ammo);
			ImGui::Checkbox("snaplines", &vars.visuals.player.snaplines);
			ImGui::Combo("information", &vars.visuals.player.information, "off\0normal\0normal + resolver mode");
			ImGui::Checkbox("dead only", &vars.visuals.player.deadonly);
			ImGui::Checkbox("enemies only", &vars.visuals.player.espto);
			ImGui::Checkbox("chams", &vars.visuals.player.chams);
			ImGui::Checkbox("chams flat", &vars.visuals.player.chamsflat);
			ImGui::Checkbox("chamsxqz", &vars.visuals.player.chamsxqz);
			ImGui::Checkbox("chams enemies only", &vars.visuals.player.chamsto);
			ImGui::Checkbox("glow", &vars.visuals.player.glow);
			ImGui::Checkbox("glow enemies only", &vars.visuals.player.glowto);
			ImGui::NextColumn();
			ImGui::Text("world");
			ImGui::Checkbox("weapon esp", &vars.visuals.world.weapons);
			ImGui::Checkbox("weapon glow", &vars.visuals.world.weaponsglow);
			ImGui::Checkbox("bomb esp", &vars.visuals.world.bomb);
			ImGui::Checkbox("bomb glow", &vars.visuals.world.bombglow);
			ImGui::Checkbox("defuse kit esp", &vars.visuals.world.defuse);
			ImGui::Checkbox("defuse kit glow", &vars.visuals.world.defuseglow);
			ImGui::Checkbox("nightmode", &vars.visuals.world.nightmode); ImGui::SameLine();
			if (ImGui::Button("update", ImVec2(60, 20))) { vars.visuals.world.update = true; };
			ImGui::Checkbox("no smoke", &vars.visuals.world.nosmoke);
			ImGui::Checkbox("no flash", &vars.visuals.world.noflash);
			ImGui::Checkbox("no post processing", &vars.visuals.world.nopostprocess);
			ImGui::SliderInt("fields of view", &vars.visuals.world.fov, 0, 180);
			ImGui::SliderInt("view model fields of view", &vars.visuals.world.vmodelfov, 68, 180);
			ImGui::Columns(1);
			ImGui::PopItemWidth();
			break;
		case 3:
			if (ImGui::Button("anti report")) //spicy memes by @publiccode
			{
				static ConVar* name = g_CVar->FindVar("name");
				*(int*)((DWORD)&name->m_fnChangeCallbacks + 0xC) = NULL;
				Utils::SetClantag(local->player_info().szName);
				name->SetValue("\n");
				name->SetValue(antireport);
			}
			ImGui::Checkbox("thirdperson", &vars.misc.thirdperson);
			ImGui::SliderInt("thirderson range", &vars.misc.thirdpersonrange, 50,350);
			ImGui::Hotkey("thirdperson key", &vars.misc.thirdpersonkey);
			ImGui::Checkbox("bhop", &vars.misc.bhop);
			ImGui::Checkbox("autostrafe", &vars.misc.autostrafe);
			ImGui::Checkbox("hitmarker", &vars.misc.hitmarker);
			ImGui::Checkbox("fakelag", &vars.misc.fakelag);
			ImGui::Checkbox("aa lines", &vars.misc.aalines);
			break;
		case 4:
			ImGui::BeginGroup();
			static char configname[512] = {0};
			ImGui::InputText("config name", configname, 512);
			ImGui::Text(configname);
			if(ImGui::Button("save", ImVec2(60, 20))) {
				configs::save(configname);
			}
			if(ImGui::Button("load", ImVec2(60, 20))) {
				configs::load(configname);
			}
			ImGui::EndGroup();
			static int selectedcolor;
			ImGui::ListBox("Colors", &selectedcolor, color_names, ARRAYSIZE(color_names));

			ImGuiEx::ColorEdit4("", &vars.settings.colors[selectedcolor]);

			if (ImGui::Button("vguimenu init", ImVec2(60, 20))) {
				oldmenu.Init();
			}
			break;

		
		}

		ImGui::SameLine();
		ImGui::End();
	}
	ImGui::PopStyle();

	ImGui::Render();
}

void Menu::Show()
{
	_visible = true;
	cl_mouseenable->SetValue(false);
}

void Menu::Hide()
{
	_visible = false;
	cl_mouseenable->SetValue(true);
}

void Menu::Toggle()
{
	_visible = !_visible;
	cl_mouseenable->SetValue(!_visible);
}

void Menu::CreateStyle()
{
	_style.Alpha = 1.0f;                                // Global alpha applies to everything in ImGui
	_style.WindowPadding = ImVec2(10, 10);                      // Padding within a window
	_style.WindowMinSize = ImVec2(100, 100);                    // Minimum window size
	_style.WindowRounding = 0.0f;                                // Radius of window corners rounding. Set to 0.0f to have rectangular windows
	_style.WindowTitleAlign = ImVec2(0.0f, 0.5f);                  // Alignment for title bar text
	_style.ChildWindowRounding = 0.0f;                                // Radius of child window corners rounding. Set to 0.0f to have rectangular child windows
	_style.FramePadding = ImVec2(5, 5);                        // Padding within a framed rectangle (used by most widgets)
	_style.FrameRounding = 0.0f;                                // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
	_style.ItemSpacing = ImVec2(5, 5);                        // Horizontal and vertical spacing between widgets/lines
	_style.ItemInnerSpacing = ImVec2(4, 4);                        // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
	_style.TouchExtraPadding = ImVec2(0, 0);                        // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
	_style.IndentSpacing = 21.0f;                               // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
	_style.ColumnsMinSpacing = 6.0f;                                // Minimum horizontal spacing between two columns
	_style.ScrollbarSize = 16.0f;                               // Width of the vertical scrollbar, Height of the horizontal scrollbar
	_style.ScrollbarRounding = 9.0f;                                // Radius of grab corners rounding for scrollbar
	_style.GrabMinSize = 10.0f;                               // Minimum width/height of a grab box for slider/scrollbar
	_style.GrabRounding = 0.0f;                                // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
	_style.ButtonTextAlign = ImVec2(0.5f, 0.5f);                  // Alignment of button text when button is larger than text.
	_style.DisplayWindowPadding = ImVec2(22, 22);                      // Window positions are clamped to be IsVisible within the display area by at least this amount. Only covers regular windows.
	_style.DisplaySafeAreaPadding = ImVec2(4, 4);                        // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
	_style.AntiAliasedLines = true;                                // Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
	_style.AntiAliasedShapes = true;        // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
	_style.CurveTessellationTol = 1.25f;                               // Tessellation tolerance. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.

	_style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	_style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	_style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
	_style.Colors[ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.f, 1.00f);
	_style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.00f);
	_style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	_style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
	_style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	_style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	_style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
	_style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
	_style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	_style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	_style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	_style.Colors[ImGuiCol_TitleText] = ImVec4(0.80f, 0.80f, 1.00f, 1.00f);
	_style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
	_style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
	_style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
	_style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
	_style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 0.40f);
	_style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
	_style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.60f, 0.90f, 0.50f);
	_style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	_style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
	_style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	_style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	_style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.83f);
	_style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.10f, 0.10f, 0.45f);
	_style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.15f, 0.15f, 0.80f);
	_style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.80f);
	_style.Colors[ImGuiCol_Column] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	_style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
	_style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
	_style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	_style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	_style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	_style.Colors[ImGuiCol_CloseButton] = ImVec4(0.10f, 0.10f, 0.10f, 0.50f);
	_style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.00f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.20f, 0.00f, 0.83f);
	_style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	_style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 0.70f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}