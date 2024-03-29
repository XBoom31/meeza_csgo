﻿#include "vguimenu.h"

#include <array>
#include <comdef.h> 
#include "config.h"
#include <chrono>
#include <cctype>
#include <algorithm>
#include "KitParser.h"
#include <vector>
#include "skins.h"
#include "notificationsystem.h"
#include "ItemDefinitions.h"

//std::vector<paint_kit> k_skins;
std::vector<std::string> skins_cc;
static std::vector<std::wstring> wnames;
static std::vector<std::wstring> wskins;
static std::vector<std::wstring> colornames;
static int ban_input;
static int tab = 0;
vgui::HFont VGUIMenuFont;
vgui::HFont TitleFont;
VGUIMenu oldmenu;
std::vector<paint_kit> k_skins;
static bool shouldupdate = false;
static int VGUIMenux = 0;
static int VGUIMenuy = 0;
static ConVar* cl_mouseenable = nullptr;

template <class T>
std::vector<std::wstring> makevec(T element, T element2)
{
	std::vector<std::wstring> temp;
	temp.push_back(element); 
	temp.push_back(element2);
	return temp;
}
template <class T>
std::vector<std::wstring> makevec(T element, T element2, T element3)
{
	std::vector<std::wstring> temp;
	temp.push_back(element);
	temp.push_back(element2);
	temp.push_back(element3);
	return temp;
}
template <class T>
std::vector<std::wstring> makevec(T element, T element2, T element3, T element4)
{
	std::vector<std::wstring> temp;
	temp.push_back(element);
	temp.push_back(element2);
	temp.push_back(element3);
	temp.push_back(element4);
	return temp;
}

CObject::CObject(int x, int y, int width, int height, int layer)
	: m_x(x), m_y(y), m_width(width), m_height(height), m_layer(layer)
{}

//--------------------------------------------------------------------------------

void CObjectContainer::AddControl(CObject * obj)
{
	Objects.push_back(obj);
}


void ApplySkins()
{

	g_ClientState->ForceFullUpdate();
	//g_EngineClient->ClientCmd_Unrestricted("cl_fullupdate");
}

std::vector<std::pair<int, const wchar_t*>> weaponcomboname = {
	{ WEAPON_DEAGLE, L"Desert Eagle" },
{ WEAPON_ELITE, L"Dual Berettas" },
{ WEAPON_FIVESEVEN, L"Five-SeveN" },
{ WEAPON_GLOCK, L"Glock-18" },
{ WEAPON_AK47, L"AK-47" },
{ WEAPON_AUG, L"AUG" },
{ WEAPON_AWP, L"AWP" },
{ WEAPON_FAMAS, L"FAMAS" },
{ WEAPON_G3SG1, L"G3SG1" },
{ WEAPON_GALILAR, L"Galil AR" },
{ WEAPON_M249, L"M249" },
{ WEAPON_M4A1, L"M4A4" },
{ WEAPON_MAC10, L"MAC-10" },
{ WEAPON_P90, L"P90" },
{ WEAPON_UMP45, L"UMP-45" },
{ WEAPON_XM1014, L"XM1014" },
{ WEAPON_BIZON, L"PP-Bizon" },
{ WEAPON_MAG7, L"MAG-7" },
{ WEAPON_NEGEV, L"Negev" },
{ WEAPON_SAWEDOFF, L"Sawed.Off" },
{ WEAPON_TEC9, L"Tec-9" },
{ WEAPON_HKP2000, L"P2000" },
{ WEAPON_MP7, L"MP7" },
{ WEAPON_MP9, L"MP9" },
{ WEAPON_NOVA, L"Nova" },
{ WEAPON_P250, L"P250" },
{ WEAPON_SCAR20, L"SCAR-20" },
{ WEAPON_SG556, L"SG 553" },
{ WEAPON_SSG08, L"SSG 08" },
{ WEAPON_M4A1_SILENCER, L"M4A1-S" },
{ WEAPON_USP_SILENCER, L"USP-S" },
{ WEAPON_CZ75A, L"CZ75-Auto" },
{ WEAPON_REVOLVER, L"R8 Revolver" },
{ 42, L"Knife" }
};

#include <xlocbuf>
#include <codecvt>
void InitializePaintKits() {
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	const auto sig_address = Utils::FindPattern("client.dll", (PBYTE)"\xE8\x00\x00\x00\x00\xFF\x76\x0C\x8D\x48\x04\xE8", "x????xxxxxxx");
	
	const auto item_system_offset = *reinterpret_cast<std::int32_t*>(sig_address + 1);
	
	const auto item_system_fn = reinterpret_cast<CCStrike15ItemSystem* (*)()>(sig_address + 5 + item_system_offset);
	
	const auto item_schema = reinterpret_cast<CCStrike15ItemSchema*>(std::uintptr_t(item_system_fn()) + sizeof(void*));
	
	const auto get_paint_kit_definition_offset = *reinterpret_cast<std::int32_t*>(sig_address + 11 + 1);
	
	const auto get_paint_kit_definition_fn = reinterpret_cast<CPaintKit*(__thiscall*)(CCStrike15ItemSchema*, int)>(sig_address + 11 + 5 + get_paint_kit_definition_offset);
	
	const auto start_element_offset = *reinterpret_cast<std::intptr_t*>(std::uintptr_t(get_paint_kit_definition_fn) + 8 + 2);
	
	const auto head_offset = start_element_offset - 12;
	
	const auto map_head = reinterpret_cast<Head_t<int, CPaintKit*>*>(std::uintptr_t(item_schema) + head_offset);
	
	for (auto i = 0; i <= map_head->last_element; ++i) {
		try {
			
			const auto paint_kit = map_head->memory[i].value;
			
			if (paint_kit->id == 9001)
				continue;
			
			const wchar_t* wide_name = g_Localize->Find(paint_kit->item_name.buffer);
			
			_bstr_t b(wide_name);
			std::string pcode = b;
			
			if (paint_kit->id < 10000)
				k_skins.push_back({ paint_kit->id, pcode });
		}
		catch (int e)
		{

		}
	}
	
	//std::sort(k_skins.begin(), k_skins.end());
	//k_skins.insert(k_skins.begin(), { 0, "None" });
}
void initialize()
{
	
	static bool once = false;
	
	if (!once)
	{
		
		InitializePaintKits();
		
		for (unsigned int i = 0; i < k_skins.size(); i++) {
			//std::ofstream ofstr;
			//ofstr.open("skindump", std::ofstream::out | std::ofstream::app);
			//ofstr << k_skins[i].name << " : " << k_skins[i].id << "\n";

			skins_cc.push_back(k_skins[i].name);
		}
		
		for (auto s : weaponcomboname)
			wnames.push_back(Utils::to_wstring(s.second));
		
		for (auto s : skins_cc)
			wskins.push_back(Utils::to_wstring(s.c_str()));
		
		once = true;
	}
}


//--------------------------------------------------------------------------------

void CForm::Paint()
{
	ban_input = false;
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
void CForm::Clear()
{
	Windows.clear();
}


void CForm::Click()
{
	//if (ban_input)
	//	return;
	if (!oldmenu.IsVisible())
		return;
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
	if (!oldmenu.IsVisible())
		return;
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

//--------------------------------------------------------------------------------
static int m_x = 100, m_y = 100;
CWindow::CWindow(int x, int y, int width, int height, bool* open)
	: CObject(x, y, width, height), m_open(open)
{}

void CWindow::Paint(int t_x, int t_y, float* controlheight)
{
	if (m_open != nullptr && !*m_open) return;

	if (m_dragging && !InputSys::Get().IsKeyDown(VK_LBUTTON))
		m_dragging = false;

	if (shouldupdate)
	{
		m_x = VGUIMenux;
		m_y = VGUIMenuy;
		shouldupdate = false;
	}

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
	VGUIMenux = m_x;
	VGUIMenuy = m_y;
	Render::Box(tl.x, tl.y - 16, br.x - tl.x, br.y - tl.y + 16, true, Color(16, 16, 16, 255));
	//Render::Box(tl.x, tl.y - 16, br.x, br.y, false, Color(50, 5, 5));
	//Render::Box(tl.x + 1, tl.y - 15, br.x - 1, br.y - 1, false, Color(200, 10, 10));
	tl += 6;
	br -= 6;
	//	Render::Box(tl.x, tl.y, br.x, br.y, true, Color(16, 16, 16));
	Render::Line(tl.x, tl.y + 45, br.x, tl.y + 45, Color(24, 24, 24));
	Render::Line(tl.x, tl.y + 46, br.x, tl.y + 46, Color(24, 24, 24));
	Render::Box(tl, br, Color(30, 30, 30));
	//Render::Box(tl.x - 1, tl.y - 1, br.x + 1, br.y + 1,false, Color(200, 10, 10));

	Render::Text(tl.x + 3, tl.y - 17, false, false, TitleFont, Color::White, L"meeza");

	m_controlheight = -5;
	std::vector<std::pair<CCombo, int>> combos;
	for (auto o : Objects)
	{

		if (typeid(*o) == typeid(CCombo))
		{
			CCombo x = *(CCombo*)o;
			std::pair<CCombo, int> mypair(x, (int)m_controlheight);
			combos.push_back(mypair);
			m_controlheight += 20;
		}
		else
		    o->Paint(tl.x + 5, tl.y - 170, &m_controlheight);
	}
	for (auto o : combos)
	{
		float meme = o.second;
		o.first.Paint(tl.x + 5, tl.y - 170, &meme);
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

//--------------------------------------------------------------------------------

CChild::CChild(int x, int y, int width, std::wstring title, int mes, bool meme)
	: CObject(x, y, width), m_title(title), me(mes), mem(meme)
{}

void CChild::Paint(int t_x, int t_y, float* controlheight)
{
	Vector2D tl(t_x + m_x, t_y + m_y + 3);
	Vector2D br(t_x + m_x + m_width, t_y + m_y + m_controlheight + 15);

	auto textSize = Render::GetTextSize(m_title.data(), VGUIMenuFont);

	// background
	//Render::FilledGradientRectangle(tl.x, tl.y, br.x, br.y, Color(210, 210, 210), Color(238, 238, 238));
	//no need to render this becuase it is already the same color as the background
	const Color color(180, 180, 180);

	Render::CleanBox(tl.x, tl.y, br.x, br.y + 8, false, Color(30, 30, 30));

	Render::Text(tl.x + 14, tl.y, false, false, VGUIMenuFont, Color(230, 230, 230), m_title.data());

	m_controlheight = 0;
	float m_controlheight2 = 0;
	int elements = 0;
	int comboz = 0;
	if (!mem)
	{
		bool nervinervosi[30];
		std::vector<std::pair<CCombo, int>> combos;
		for (auto o : Objects)
		{

			if (typeid(*o) == typeid(CCombo))
			{
				CCombo x = *(CCombo*)o;
				std::pair<CCombo, int> mypair(x, (int)m_controlheight);
				combos.push_back(mypair);
				m_controlheight += 20;
				nervinervosi[comboz] = elements >= me;
				comboz++;
				
			}
			else
				if(elements>=me)
					o->Paint(tl.x + 158, tl.y - 170, &m_controlheight);
				else
					o->Paint(tl.x + 8, tl.y - 170, &m_controlheight);
		}
		for (int index = 0; index < combos.size(); index++) {
			auto obj = combos[index];
			float meme = obj.second;
			if(nervinervosi[index])
				obj.first.Paint(tl.x + 158, tl.y - 170, &meme);
			else
				obj.first.Paint(tl.x + 8, tl.y - 170, &meme);
		}
	}
	else
	{
		for (auto o : Objects)
		{
			if (elements >= me)
				o->Paint(tl.x + 230, tl.y + 16, &m_controlheight2);
			else
				o->Paint(tl.x + 8, tl.y + 16, &m_controlheight);
			elements++;
		}
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


CUpdateChild::CUpdateChild(int x, int y, int width, std::wstring title, std::function<std::vector<CObject*>()> update, int mes)
	: CObject(x, y, width), m_title(title), updatefunc(update), me(mes)
{}

void CUpdateChild::Paint(int t_x, int t_y, float* controlheight)
{
	Objects.clear();
	Objects = updatefunc();
	Vector2D tl(t_x + m_x, t_y + m_y + 3);
	Vector2D br(t_x + m_x + m_width, t_y + m_y + m_controlheight + 15);

	auto textSize = Render::GetTextSize(m_title.data(), VGUIMenuFont);
	// background
	//Render::FilledGradientRectangle(tl.x, tl.y, br.x, br.y, Color(210, 210, 210), Color(238, 238, 238));
	//no need to render this becuase it is already the same color as the background
	const Color color(180, 180, 180);

	Render::CleanBox(tl.x, tl.y, br.x, br.y + 8, false, Color(30, 30, 30));

	Render::Text(tl.x + 14, tl.y, false, false, VGUIMenuFont, Color(230, 230, 230), m_title.data());
	m_controlheight = 0;
	float m_controlheight2 = 0;
	int elements = 0;
	for (auto o : Objects)
	{
		if (elements >= me)
			o->Paint(tl.x + 300, tl.y + 16, &m_controlheight2);
		else
			o->Paint(tl.x + 8, tl.y + 16, &m_controlheight);
		elements++;
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
//--------------------------------------------------------------------------------

CSpacing::CSpacing(int height)
	: m_height(height)
{}

void CSpacing::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;
	*controlheight += m_height;
}


//--------------------------------------------------------------------------------

CLabel::CLabel(std::wstring text, vgui::HFont font, Color color, bool centered)
	: m_text(text), m_font(font), m_color(color), m_centered(centered)
{}

void CLabel::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;
	t_y += *controlheight;
	Render::Text(t_x + m_x, t_y + m_y, m_centered, true, m_font, m_color, m_text.data());
	*controlheight += Render::GetTextSize(m_text.data(), m_font).y;
}

//--------------------------------------------------------------------------------

CCheckBox::CCheckBox(std::wstring label, bool* value)
	: m_label(label), m_val(value)
{}

void CCheckBox::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;

	const auto size = 20;

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

	Render::Text(br.x + 4, tl.y + 6, false, false, VGUIMenuFont, Color(230, 230, 230), m_label.data());
	*controlheight += size + VGUIMenu_PADDING;
}

void CCheckBox::Click()
{
	if (m_val != nullptr && m_hover)
		*m_val = !*m_val;

}

//--------------------------------------------------------------------------------

template<typename T>
inline CSlider<T>::CSlider(std::wstring label, T min, T max, T * value)
	: m_label(label), m_min(min), m_max(max), m_val(value)
{}

template<typename T>
inline void CSlider<T>::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;
	t_y += *controlheight;

	const auto width = 150;
	const auto height = 20;

	Vector2D tl(t_x + m_x + 7, t_y + m_y);
	Vector2D br(t_x + m_x + width + 7, t_y + m_y + height);

	int cur_x, cur_y;
	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

	m_hover = Math::InBox(tl.x - 4, tl.y, br.x + 4, br.y, cur_x, cur_y);

	int valueX = tl.x + ((*m_val - m_min) * width / (m_max - m_min));

	if (m_val != nullptr && m_hover && InputSys::Get().IsKeyDown(VK_LBUTTON))
		*m_val = (cur_x - tl.x) * (m_max - m_min) / width;

	if (*m_val < m_min) *m_val = m_min;
	if (*m_val > m_max) *m_val = m_max;

	Render::Box(tl.x - 7, tl.y, br.x + 7, br.y, false, Color(30, 30, 30));
	Render::CleanBox(tl.x - 6, tl.y + 1, br.x + 6, br.y - 1, true, Color(24, 24, 24));
	Render::CleanBox(valueX - 5, tl.y + 2, valueX + 5, br.y - 2, true, Color(50, 50, 50));
	wchar_t buffer[35];
	swprintf(buffer, 35, L"%s (%s)", m_label.data(), Utils::to_wstring(*m_val, 4).data());
	Render::Text(br.x + 9, tl.y, false, false, VGUIMenuFont, Color(230, 230, 230), buffer);
	*controlheight += height + VGUIMenu_PADDING;
}

template class CSlider<int>;
template class CSlider<float>;
template class CSlider<double>;

CColorPicker::CColorPicker(std::wstring label, Color* col)
{
	m_label = label; m_col = col;
}

void CColorPicker::Paint(int t_x, int xt_y, float* controlheight)
{
	int r, g, b, a;
	m_col->GetColor(r, g, b, a);
	if (controlheight == nullptr) return;
	*controlheight += 10;
	//xt_y += *controlheight;
	int zt_y = xt_y;

	zt_y += *controlheight;
	Render::Text(t_x + m_x, zt_y + m_y - 7, false, true, VGUIMenuFont, Color(200, 200, 200), m_label.data());
	int meme = t_x + m_x + Render::GetTextSize(m_label.data(), VGUIMenuFont).x;
	Render::Box(meme + 5, zt_y + m_y - 8, 50, 15, true, *m_col);
	*controlheight += Render::GetTextSize(m_label.data(), VGUIMenuFont).y;
	{
		int t_y = xt_y;
		t_y += *controlheight;
		const auto width = 150;
		const auto height = 20;

		Vector2D tl(t_x + m_x + 7, t_y + m_y);
		Vector2D br(t_x + m_x + width + 7, t_y + m_y + height);

		int cur_x, cur_y;
		g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

		m_hover = Math::InBox(tl.x - 4, tl.y, br.x + 4, br.y, cur_x, cur_y);

		int valueX = tl.x + ((r - 0) * width / (254));

		if (r != 0 && m_hover && InputSys::Get().IsKeyDown(VK_LBUTTON))
			r = (cur_x - tl.x) * (254) / width;

		if (r < 1) r = 1;
		if (r > 254) r = 254;

		Render::Box(tl.x - 7, tl.y, br.x + 7, br.y, false, Color(30, 30, 30));
		Render::CleanBox(tl.x - 6, tl.y + 1, br.x + 6, br.y - 1, true, Color(24, 24, 24));
		Render::CleanBox(valueX - 5, tl.y + 2, valueX + 5, br.y - 2, true, Color(50, 50, 50));
		wchar_t buffer[35];
		swprintf(buffer, 35, L"%s (%s)", L"Red", Utils::to_wstring(r, 4).data());
		Render::Text(br.x + 9, tl.y, false, false, VGUIMenuFont, Color(230, 230, 230), buffer);
		*controlheight += height + VGUIMenu_PADDING;
	}
	{
		int t_y = xt_y;
		t_y += *controlheight;
		const auto width = 150;
		const auto height = 20;

		Vector2D tl(t_x + m_x + 7, t_y + m_y);
		Vector2D br(t_x + m_x + width + 7, t_y + m_y + height);

		int cur_x, cur_y;
		g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

		m_hover = Math::InBox(tl.x - 4, tl.y, br.x + 4, br.y, cur_x, cur_y);

		int valueX = tl.x + ((g - 0) * width / (254));

		if (g != 0 && m_hover && InputSys::Get().IsKeyDown(VK_LBUTTON))
			g = (cur_x - tl.x) * (255 - 0) / width;

		if (g < 1) g = 1;
		if (g > 254) g = 254;

		Render::Box(tl.x - 7, tl.y, br.x + 7, br.y, false, Color(30, 30, 30));
		Render::CleanBox(tl.x - 6, tl.y + 1, br.x + 6, br.y - 1, true, Color(24, 24, 24));
		Render::CleanBox(valueX - 5, tl.y + 2, valueX + 5, br.y - 2, true, Color(50, 50, 50));
		wchar_t buffer[35];
		swprintf(buffer, 35, L"%s (%s)", L"Green", Utils::to_wstring(g, 4).data());
		Render::Text(br.x + 9, tl.y, false, false, VGUIMenuFont, Color(230, 230, 230), buffer);
		*controlheight += height + VGUIMenu_PADDING;
	}
	{
		int t_y = xt_y;
		t_y += *controlheight;
		const auto width = 150;
		const auto height = 20;

		Vector2D tl(t_x + m_x + 7, t_y + m_y);
		Vector2D br(t_x + m_x + width + 7, t_y + m_y + height);

		int cur_x, cur_y;
		g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

		m_hover = Math::InBox(tl.x - 4, tl.y, br.x + 4, br.y, cur_x, cur_y);

		int valueX = tl.x + ((b - 0) * width / (254));

		if (b != 0 && m_hover && InputSys::Get().IsKeyDown(VK_LBUTTON))
			b = (cur_x - tl.x) * (255 - 0) / width;

		if (b < 1)b = 1;
		if (b > 254)b = 254;

		Render::Box(tl.x - 7, tl.y, br.x + 7, br.y, false, Color(30, 30, 30));
		Render::CleanBox(tl.x - 6, tl.y + 1, br.x + 6, br.y - 1, true, Color(24, 24, 24));
		Render::CleanBox(valueX - 5, tl.y + 2, valueX + 5, br.y - 2, true, Color(50, 50, 50));
		wchar_t buffer[35];
		swprintf(buffer, 35, L"%s (%s)", L"Blue", Utils::to_wstring(b, 4).data());
		Render::Text(br.x + 9, tl.y, false, false, VGUIMenuFont, Color(230, 230, 230), buffer);
		*controlheight += height + VGUIMenu_PADDING;
	}
	m_col->SetColor(r, g, b, 255);
	//*controlheight += 10;
}
//--------------------------------------------------------------------------------

CButton::CButton(int width, int height, std::wstring label, std::function<void()> cb, int posxz, int posyz)
	: CObject(0, 0, width, height), m_label(label), m_cb(cb), posx(posxz), posy(posyz)
{}

void CButton::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;


	if (posx != 0 && posy != 0)
	{
		t_x = posx + VGUIMenux;
		t_y = posy + VGUIMenuy;
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
		Render::CleanBox(tl.x, tl.y, br.x, br.y, false, Color(30, 30, 30));
		Render::CleanBox(tl.x, tl.y, br.x, br.y, true, Color(240, 0, 50));
	}
	else if (m_hover)
	{
		Render::CleanBox(tl.x, tl.y, br.x, br.y, false, Color(30, 30, 30));
		Render::CleanBox(tl.x, tl.y, br.x, br.y, true, Color(50, 50, 50));
	}
	else
	{
		Render::CleanBox(tl.x, tl.y, br.x, br.y, false, Color(30, 30, 30));
		Render::CleanBox(tl.x, tl.y, br.x, br.y, true, Color(24, 24, 24));
	}
	Render::Text(tl.x + (m_width / 2), tl.y + (m_height / 2) + 5, true, false, VGUIMenuFont, Color(230, 230, 230), m_label.data());
	*controlheight += m_height + VGUIMenu_PADDING + 3;
}

void CButton::Click()
{
	if (m_hover)
	{
		shouldupdate = true;
		m_cb();
	}
}

//--------------------------------------------------------------------------------

CTextBox::CTextBox(int width, int height, std::wstring placeholder, std::wstring* value)
	: CObject(0, 0, width, height), m_placeholder(placeholder), m_val(value)
{}

void CTextBox::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;
	t_y += *controlheight;

	m_tl = Vector2D(t_x, t_y);
	m_br = Vector2D(t_x + m_width, t_y + m_height);

	int cur_x, cur_y;
	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

	m_hover = Math::InBox(m_tl.x, m_tl.y, m_br.x, m_br.y, cur_x, cur_y);


	Render::Box(m_tl, m_br, Color(30, 30, 30));
	Render::CleanBox(m_tl.x + 1, m_tl.y + 1, m_br.x - 1, m_br.y - 1, true, Color(24, 24, 24));
	//Render::Box(m_tl.x + 1, m_tl.y + 1, m_br.x - 1 - m_tl.x, m_br.y - 1 - m_tl.y,true,  Color(248, 248, 248));

	if (m_val != nullptr && m_val->length() > 0)
	{
		Render::Text(m_tl.x + 4, m_tl.y + (m_height / 2) - 6, false, false, VGUIMenuFont, Color(230, 230, 230), m_val->data());
		if (m_selected && (int)(g_GlobalVars->realtime * 2) % 2 == 0)
		{
			auto TextSize = Render::GetTextSize(m_val->data(), VGUIMenuFont);
			Render::Line(m_tl.x + TextSize.x + 4, m_tl.y + 4, m_tl.x + TextSize.x + 4, m_br.y - 4, Color(230, 230, 230));
		}
	}
	else if (!m_selected)
	{
		Render::Text(m_tl.x + 4, m_tl.y + (m_height / 2) - 6, false, false, VGUIMenuFont, Color(100, 100, 100), m_placeholder.data());
	}
	else if (m_selected && (int)(g_GlobalVars->realtime * 2) % 2 == 0)
	{
		Render::Line(m_tl.x + 0 + 4, m_tl.y + 4, m_tl.x + 0 + 4, m_br.y - 4, Color(230, 230, 230));
	}
	*controlheight += m_height + VGUIMenu_PADDING;
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

CGetKey::CGetKey(std::wstring placeholder, int* val)
	: m_placeholder(placeholder), m_val(val)
{}
void CGetKey::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;
	t_y += *controlheight;

	m_tl = Vector2D(t_x, t_y);
	m_br = Vector2D(t_x + 150, t_y + 20);

	int cur_x, cur_y;
	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);

	m_hover = Math::InBox(m_tl.x, m_tl.y, m_br.x, m_br.y, cur_x, cur_y);

	if (isSelected)
		Render::Box(m_tl, m_br, Color(0, 70, 200));
	else if (m_hover)
		Render::Box(m_tl, m_br, Color(45, 45, 45));
	else
		Render::Box(m_tl, m_br, Color(30, 30, 30));
	Render::CleanBox(m_tl.x + 2, m_tl.y + 2, m_br.x - 2, m_br.y - 2, true, Color(24, 24, 24));
	//Render::Box(m_tl.x + 1, m_tl.y + 1, m_br.x - 1 - m_tl.x, m_br.y - 1 - m_tl.y,true,  Color(248, 248, 248));

	Render::Text(m_tl.x + 4, m_tl.y + (m_height / 2) - 6, false, false, VGUIMenuFont, Color(100, 100, 100), keyNames[*m_val]);

	*controlheight += m_height + VGUIMenu_PADDING;
}
void CGetKey::Click()
{
	isSelected = m_hover;
}

void CGetKey::HandleInput(WPARAM vk)
{
	if (m_val != nullptr && isSelected)
	{
		for (int k = 1; k < 255; k++)
		{
			if (InputSys::Get().IsKeyDown(k))
			{
				if (k == VK_ESCAPE || k == VK_INSERT) {
					isSelected = false;
					return;
				}
				isSelected = false;
				*m_val = k;
			}
		}
	}
}
//--------------------------------------------------------------------------------

CCombo::CCombo(std::wstring dlabel, int* value, std::vector<std::wstring> labels)
	: m_val(value), m_labels(labels), selected(false), label(dlabel)
{}

void CCombo::Paint(int t_x, int t_y, float * controlheight)
{
	if (controlheight == nullptr) return;
	t_y += *controlheight;

	const auto width = 150;
	const auto height = 20;

	// root vectors
	
	if (selected) ban_input = true;
	int cur_x, cur_y;
	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);
	int ts = Render::GetTextSize(L"apocalypsecheats v1 beste", VGUIMenuFont).y;

	Render::Text(t_x + m_x, t_y + m_y, false, false, VGUIMenuFont, Color::White, label.data());
	t_y += ts;

	if (!selected) {
		Vector2D r_tl(t_x + m_x, t_y + m_y);
		Vector2D r_br(t_x + m_x + width, t_y + m_y + height);
		Render::Box(r_tl.x, r_tl.y, r_br.x, r_br.y, false, Color(30, 30, 30));
		Render::CleanBox(r_tl.x + 1, r_tl.y + 1, r_br.x - 1, r_br.y - 1, true, Color(24, 24, 24));
		Render::Text(r_tl.x + 5, r_tl.y + 3, false, false, VGUIMenuFont, Color::White, m_labels.at(*m_val).data());
		hover = Math::InBox(r_tl, r_br, cur_x, cur_y);
	}
	else {
		Vector2D r_tl(t_x + m_x, t_y + m_y);
		Vector2D r_br(t_x + m_x + width, t_y + m_y + height * m_labels.size());
		Render::Box(r_tl.x, r_tl.y, r_br.x, r_br.y, false, Color(30, 30, 30));
		Render::CleanBox(r_tl.x + 1, r_tl.y + 1, r_br.x - 1, r_br.y - 1, true, Color(24, 24, 24));
		for (int x = 0; x < m_labels.size(); x++)
		{
			if (Math::InBox(r_tl.x, r_tl.y + height * x, r_br.x, r_tl.y + height * (x + 1), cur_x, cur_y))
				index = x;
			Render::Text(r_tl.x + 5, r_tl.y + 3 + height * x, false, false, VGUIMenuFont, Color::White, m_labels.at(x).data());
			if(x != 0)
			Render::Line(r_tl.x, r_tl.y + height * x, r_br.x - 2, r_tl.y + height * x, Color(50, 50,50));
		}
		
	}
	
	

	*controlheight += height + VGUIMenu_PADDING + 3+ ts;
}

void CCombo::Click()
{
	if (selected)
		*m_val = index;
	if (hover)
		selected = !selected;
}

CListBox::CListBox(int melements, int* scr, int* value, std::vector<std::wstring> labels)
	:elements(melements), m_val(value), m_labels(labels), scrolloffset(scr) {}

void CListBox::Paint(int t_x, int t_y, float* controlheight)
{
	if (controlheight == nullptr) return;
	t_y += *controlheight;

	const auto width = 200;
	const auto height = 20;

	// root vectors
	int size;
	Vector2D r_tl(t_x + m_x, t_y + m_y);
	if ((unsigned int)elements < m_labels.size())
		size = elements;
	else
		size = m_labels.size();
	Vector2D r_br;
	bool need4scroll = false;
	if ((unsigned int)elements < m_labels.size())
	{// if we need a scroll bar
		r_br = Vector2D(t_x + m_x + width + 17, t_y + m_y + height * (size + 1) - 12);
		need4scroll = true;
	}
	else
		r_br = Vector2D(t_x + m_x + width, t_y + m_y + height * (size + 1) - 12);

	int cur_x, cur_y;

	g_VGuiSurface->SurfaceGetCursorPos(cur_x, cur_y);
	Render::Box(r_tl.x, r_tl.y, r_br.x, r_br.y, false, Color(30, 30, 30));
	Render::CleanBox(r_tl.x + 2, r_tl.y + 2, r_br.x - 2, r_br.y - 2, true, Color(24, 24, 24));
	if (need4scroll)
	{
		m_scrollhover = Math::InBox(r_br.x - 19, r_tl.y + 5, r_br.x - 4, r_br.y - 7,
			cur_x, cur_y);
		Render::CleanBox(r_br.x - 19, r_tl.y + 5, r_br.x - 4, r_br.y - 7, true, Color(30, 30, 30));
		m_lhover = Math::InBox(r_br.x - 19, r_tl.y + 5, r_br.x - 4, r_tl.y + 20,
			cur_x, cur_y);
		if (m_lhover)
			Render::Text(r_br.x - 10, r_tl.y + 17, true, false, VGUIMenuFont, Color(0, 70, 200),
				L"▲");
		else
			Render::Text(r_br.x - 10, r_tl.y + 17, true, false, VGUIMenuFont, Color(200, 200, 200),
				L"▲");
		m_rhover = Math::InBox(r_br.x - 19, r_br.y - 20, r_br.x - 4, r_br.y - 5,
			cur_x, cur_y);
		if (m_rhover)
			Render::Text(r_br.x - 10, r_br.y - 10, true, false, VGUIMenuFont, Color(0, 70, 200),
				L"▼");
		else
			Render::Text(r_br.x - 10, r_br.y - 10, true, false, VGUIMenuFont, Color(200, 200, 200),
				L"▼");

	}
	static std::vector<std::wstring> processed;

	//std::copy(m_labels[*scrolloffset], m_labels[*scrolloffset + size], processed.begin());
	for (int index = 0; index < size; index++)
	{
		try {
			processed.push_back(m_labels.at(index + *scrolloffset));
		}
		catch (std::exception ex) {
			NotificatonSys.output(Utils::to_wstring(ex.what()), Color(255, 0, 0));
		}
	}
	hoverindex = -1;
	for (int index = 0; index < size; index++)
	{
		Vector2D tl(t_x + m_x + 2, t_y + m_y + height * index + 3);
		Vector2D br(t_x + m_x + width - 2, t_y + m_y + height * (index + 1) + 3);
		bool hover = Math::InBox(tl.x + 2, tl.y + 2, br.x - 2, br.y - 2, cur_x, cur_y);
		if (index < 0)
			continue;
		if (hover) {
			if (*m_val == (index))
				Render::CleanBox(tl.x + 2, tl.y + 2, br.x - 2, br.y - 2, true, Color(0, 70, 255));
			else
				Render::CleanBox(tl.x + 2, tl.y + 2, br.x - 2, br.y - 2, true, Color(50, 50, 50));
			hoverindex = index + *scrolloffset;
		}
		else {
			if (*m_val == index + *scrolloffset)
				Render::CleanBox(tl.x + 2, tl.y + 2, br.x - 2, br.y - 2, true, Color(0, 70, 200));
			else
				Render::CleanBox(tl.x + 2, tl.y + 2, br.x - 2, br.y - 2, true, Color(30, 30, 30));
		}
		try
		{
			Render::Text(tl.x + 5, tl.y + 3, false, true, VGUIMenuFont, Color(200, 200, 200),
				processed.at(index).data());
		}
		catch (std::exception ex)
		{
			NotificatonSys.output(Utils::to_wstring(ex.what()), Color(255, 0, 0));
		}
	}
	processed.clear();
	*controlheight += size * height + 10;
}

void CListBox::HandleInput(WPARAM vk)
{
}

void CListBox::Click()
{
	if (hoverindex != -1)
		*m_val = hoverindex;
	if (*scrolloffset > 0)
		if (m_lhover) *scrolloffset -= 1;
	if (m_rhover) *scrolloffset += 1;
}

//--------------------------------------------------------------------------------
static bool test = false;

std::vector<CObject*> legitbot_update()
{
	std::vector<CObject*> obj;
	std::vector<CObject*> s;
	switch (vars.legit.mode)
	{
	case 0:
		obj.push_back(new CCheckBox(L"Active", &vars.legit.conf.active));
		obj.push_back(new CCheckBox(L"Silent", &vars.legit.conf.silent));
		obj.push_back(new CSlider<float>(L"Fields of View", 0, 40, &vars.legit.conf.fov));
		obj.push_back(new CSlider<float>(L"Smooth", 0, 100, &vars.legit.conf.smooth));
		obj.push_back(new CSlider<float>(L"RCS", 0, 100, &vars.legit.conf.rcs));
		obj.push_back(new CSlider<float>(L"Humanize", 0, 100, &vars.legit.conf.humanize));
		obj.push_back(new CSlider<int>(L"Backtrack", 0, 12, &vars.legit.conf.backtrack));
		obj.push_back(new CCheckBox(L"Hitscan", &vars.legit.conf.hitscan));
		if (!vars.legit.conf.hitscan)
			obj.push_back(new CSlider<int>(L"Hitbox", 0, 40, &vars.legit.conf.hitbox));
		obj.push_back(new CCheckBox(L"Legit Anti-aim", &vars.legit.conf.legitaa));
		obj.push_back(new CCheckBox(L"Legit Anti-aim Resolver", &vars.legit.conf.legitaaresolver));
		obj.push_back(new CCheckBox(L"One Tap Mode", &vars.legit.conf.tap));
		obj.push_back(new CSlider<float>(L"One Tap Mode Snap", 0, 2, &vars.legit.conf.tapsnap));
		obj.push_back(new CCheckBox(L"One Tap Mode Safe", &vars.legit.conf.tapsafe));
		obj.push_back(new CGetKey(L"Trigger Key", &vars.legit.conf.triggerkey));
		break;
	case 1:
		obj.push_back(new CLabel(L"mode 1", VGUIMenuFont));
		break;
	case 2:
		if (g_EngineClient->IsInGame() || local)
		{
			if (!local->alive())
				return s;
			auto w = local->m_hActiveWeapon();
			if (w)
			{
				int wid = w->m_Item().m_iItemDefinitionIndex();
				std::wstring weapn = Utils::to_wstring(w->GetClientClass()->m_pNetworkName);

				if (weapn == L"CBaseWeaponWorldModel")
					return s;
				obj.push_back(new CLabel(weapn, VGUIMenuFont, Color(200, 200, 200)));
				obj.push_back(new CCheckBox(L"Active", &vars.legit.weapons[wid].active));
				obj.push_back(new CCheckBox(L"Silent", &vars.legit.weapons[wid].silent));
				obj.push_back(new CSlider<float>(L"Fields of View", 0, 10, &vars.legit.weapons[wid].fov));
				obj.push_back(new CSlider<float>(L"Smooth", 0, 100, &vars.legit.weapons[wid].smooth));
				obj.push_back(new CSlider<float>(L"RCS", 0, 100, &vars.legit.weapons[wid].rcs));
				obj.push_back(new CSlider<float>(L"Humanize", 0, 100, &vars.legit.weapons[wid].humanize));
				obj.push_back(new CSlider<int>(L"Backtrack", 0, 12, &vars.legit.weapons[wid].backtrack));
				obj.push_back(new CCheckBox(L"Triggerbot", &vars.legit.weapons[wid].trigger));
				obj.push_back(new CSlider<int>(L"Triggerbot Hitchance", 0, 100, &vars.legit.weapons[wid].thitchance));
				obj.push_back(new CCheckBox(L"Hitscan", &vars.legit.weapons[wid].hitscan));
				if (!vars.legit.weapons[wid].hitscan)
					obj.push_back(new CSlider<int>(L"Hitbox", 0, 40, &vars.legit.weapons[wid].hitbox));
				obj.push_back(new CCheckBox(L"Legit Anti-aim", &vars.legit.weapons[wid].legitaa));
				obj.push_back(new CCheckBox(L"Legit Anti-aim Resolver", &vars.legit.weapons[wid].legitaaresolver));
				obj.push_back(new CCheckBox(L"One Tap Mode", &vars.legit.weapons[wid].tap));
				obj.push_back(new CSlider<float>(L"One Tap Mode Snap", 0, 2, &vars.legit.weapons[wid].tapsnap));
				obj.push_back(new CCheckBox(L"One Tap Mode Safe", &vars.legit.weapons[wid].tapsafe));
				obj.push_back(new CGetKey(L"Trigger Key", &vars.legit.weapons[wid].triggerkey));
			}
		}
		else
		{
			obj.push_back(new CLabel(L"Not in game", VGUIMenuFont));
		}
		break;
	}
	return obj;
}
static int scrl1 = 0;
static int scrl2 = 0;
static int scrl3 = 0;
static int selectedweapon = 0;
std::vector<CObject*> skinchanger_update()
{
	std::vector<CObject*> obj;
	std::vector<CObject*> s;
	obj.push_back(new CButton(60, 20, L"Apply", []() { ApplySkins(); }));
	obj.push_back(new CListBox(8, &scrl1, &selectedweapon, wnames));
	obj.push_back(new CListBox(8, &scrl2, &globs.skins[selectedweapon], wskins));
	//vars.settings.skins[weaponcomboname[selectedweapon].first].weapon_skin_id = k_skins[globs.skins[selectedweapon]].id;
	return obj;
}

std::vector<CObject*> colors_update()
{
	static int selected_color;
	std::vector<CObject*> obj;
	obj.push_back(new CListBox(8, &scrl1, &selected_color, colornames));
	obj.push_back(new CColorPicker(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(color_names[selected_color]), &vars.settings.colors[selected_color]));
	return obj;
}

void load() {
	char output[256];
	sprintf(output, "%ws", vars.settings.configname.data());
	configs::load(output);
	for (int x = 0; x < 34; x++)
		vars.settings.skins[weaponcomboname[x].first].weapon_skin_id = k_skins[globs.skins[x]].id;
	if (g_EngineClient->IsInGame())
		ApplySkins();
}
void save() {
	char output[256];
	sprintf(output, "%ws", vars.settings.configname.data());
	configs::save(output);
}

#include <string>
#include <codecvt>
#include <locale>


void VGUIMenu::Init()
{

	for (auto x : color_names)
		colornames.push_back(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(x));

	cl_mouseenable = g_CVar->FindVar("cl_mouseenable");

	VGUIMenuFont = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(VGUIMenuFont, "Arial", 13, 700, 0, 0, FONTFLAG_NONE);
	globs.ufont = VGUIMenuFont;
	TitleFont = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(TitleFont, "Arial", 17, 700, 0, 0, FONTFLAG_NONE);
	globs.lbyfont = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(globs.lbyfont, "Arial", 25, 700, 0, 0, FONTFLAG_NONE);
	m_isVisible = false;

	std::vector<std::wstring> infovec;
	infovec.push_back(L"Off");
	infovec.push_back(L"Normal");
	infovec.push_back(L"Normal + Resolver Mode");
	//creating the tabs and the tab controls
	CWindow LegitBot(100, 100, 780, 380);
	CWindow RageBot(100, 100, 780, 380);
	CWindow Visuals(100, 100, 780, 380);
	CWindow Misc(100, 100, 780, 380);
	CWindow Settings(100, 100, 780, 380);
	LegitBot.AddControl(new CButton(145, 40, L"Legitbot", []() { tab = 0; }, 10, 10));
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
	LegitBot.AddControl(new CCombo(L"LegitBot Modes", &vars.legit.mode, legitmodes));
	auto test = new CUpdateChild(10, 270, 600, L"", legitbot_update, 10);
	LegitBot.AddControl(test);
	//ragebot
	RageBot.AddControl(new CCheckBox(L"Active", &vars.rage.active));
	RageBot.AddControl(new CCheckBox(L"Autoshoot", &vars.rage.autoshoot));
	RageBot.AddControl(new CCheckBox(L"No recoil", &vars.rage.norecoil));
	RageBot.AddControl(new CCheckBox(L"Silent", &vars.rage.sillent));
	RageBot.AddControl(new CCheckBox(L"Resolver", &vars.rage.resolver));
	RageBot.AddControl(new CCheckBox(L"aa", &vars.rage.aa));
	RageBot.AddControl(new CSlider<int>(L"Fields Of View", 0, 180, &vars.rage.fov));
	RageBot.AddControl(new CCheckBox(L"Autowall", &vars.rage.awall));
	RageBot.AddControl(new CSlider<int>(L"Minimum Damage", 0, 100, &vars.rage.mindmg));
	RageBot.AddControl(new CSlider<int>(L"Hitchance", 0, 100, &vars.rage.hitchance));
	RageBot.AddControl(new CCheckBox(L"Hitscan", &vars.rage.hitscan));
	RageBot.AddControl(new CCheckBox(L"Auto config", &vars.rage.autoconf));

	//visuals
	auto Player = new CChild(10, 220, 300, L"Player", 10);
	Player->AddControl(new CCombo(L"Box", &vars.visuals.player.box, makevec(L"Off", L"Full", L"Full Clean", L"Corners")));
	Player->AddControl(new CCheckBox(L"Name", &vars.visuals.player.name));
	Player->AddControl(new CCombo(L"Health Bar", &vars.visuals.player.healthbar, makevec(L"Off", L"Side", L"Side + Text")));
	Player->AddControl(new CCombo(L"Armour Bar", &vars.visuals.player.armourbar, makevec(L"Off", L"Side", L"Side + Text")));
	Player->AddControl(new CCheckBox(L"Skeleton", &vars.visuals.player.skeleton));
	Player->AddControl(new CCombo(L"Weapon", &vars.visuals.player.weapon, makevec(L"Off", L"Normal", L"Without 'weapon_'")));
	Player->AddControl(new CCheckBox(L"Ammo", &vars.visuals.player.ammo));
	Player->AddControl(new CCheckBox(L"Snaplines", &vars.visuals.player.snaplines));
	Player->AddControl(new CCombo(L"Information", &vars.visuals.player.information, infovec));
	Player->AddControl(new CCheckBox(L"Dead Only", &vars.visuals.player.deadonly));
	Player->AddControl(new CCheckBox(L"ESP Enemies Only", &vars.visuals.player.espto));
	Player->AddControl(new CCheckBox(L"Chams", &vars.visuals.player.chams));
	Player->AddControl(new CCheckBox(L"Chams Flat", &vars.visuals.player.chamsflat));
	Player->AddControl(new CCheckBox(L"Chams XQZ", &vars.visuals.player.chamsxqz));
	Player->AddControl(new CCheckBox(L"Chams Enemies Only", &vars.visuals.player.chamsto));
	Player->AddControl(new CCheckBox(L"Glow", &vars.visuals.player.glow));
	Player->AddControl(new CCheckBox(L"Glow Enemies Only", &vars.visuals.player.glowto));
	Visuals.AddControl(Player);
	auto World = new CChild(320, 220, 300, L"World", 99);
	World->AddControl(new CCheckBox(L"Weapon ESP", &vars.visuals.world.weapons));
	World->AddControl(new CCheckBox(L"Weapon Glow", &vars.visuals.world.weaponsglow));
	World->AddControl(new CCheckBox(L"Bomb ESP", &vars.visuals.world.bomb));
	World->AddControl(new CCheckBox(L"Bomb Glow", &vars.visuals.world.bombglow));
	World->AddControl(new CCheckBox(L"Defuse ESP", &vars.visuals.world.defuse));
	World->AddControl(new CCheckBox(L"Defuse Glow", &vars.visuals.world.defuseglow));
	World->AddControl(new CCheckBox(L"Night Mode", &vars.visuals.world.nightmode));
	World->AddControl(new CCheckBox(L"No Smoke", &vars.visuals.world.nosmoke));
	World->AddControl(new CCheckBox(L"No Flash", &vars.visuals.world.noflash));
	World->AddControl(new CCheckBox(L"No Post Processing", &vars.visuals.world.nopostprocess));
	World->AddControl(new CSlider<int>(L"Fields Of View", 0, 180, &vars.visuals.world.fov));
	vars.visuals.world.vmodelfov = 68; // default
	World->AddControl(new CSlider<int>(L"View Model Fields Of View", 68, 180, &vars.visuals.world.vmodelfov));

	Visuals.AddControl(World);

	Misc.AddControl(new CCheckBox(L"Skinchanger", &vars.misc.skinchanger));
	Misc.AddControl(new CCheckBox(L"Thirdperson", &vars.misc.thirdperson));
	vars.misc.thirdpersonrange = 150; // default
	Misc.AddControl(new CSlider<int>(L"Thirdperson Range", 0, 300, &vars.misc.thirdpersonrange));
	Misc.AddControl(new CCheckBox(L"Anti Aim Lines", &vars.misc.aalines));
	Misc.AddControl(new CCheckBox(L"Bunnyhop", &vars.misc.bhop));
	Misc.AddControl(new CCheckBox(L"Auto Strafe", &vars.misc.autostrafe));
	Misc.AddControl(new CCheckBox(L"Engine Prediction", &vars.misc.engine_pred));

	auto skinch = new CUpdateChild(320, 220, 300, L"Skinchanger", skinchanger_update);
	Misc.AddControl(skinch);



	auto Config = new CChild(10, 220, 230, L"Configs", 10);
	Config->AddControl(new CTextBox(150, 20, L"Config Name", &vars.settings.configname));
	Config->AddControl(new CButton(60, 20, L"Save", []() { save(); }));
	Config->AddControl(new CButton(60, 20, L"Load", []() { load(); }));
	Settings.AddControl(Config);

	//auto espcolor = new CChild(10, 320, 230, L"ESP Colors", 10);
	//espcolor->AddControl(new CColorPicker(L"Enemy", &vars.settings.enemy));
	//vars.settings.enemy = Color(186, 60, 0);
	//espcolor->AddControl(new CColorPicker(L"Ally", &vars.settings.ally));
	//vars.settings.ally = Color(0, 190, 0);
	//Settings.AddControl(espcolor);

	auto other = new CUpdateChild(250, 220, 300, L"Colors", colors_update);
	Settings.AddControl(other);
	//Settings.
	// register the tabs to the main  rendering stuff
	VGUIMenuForm.AddWindow(LegitBot);
	VGUIMenuForm.AddWindow(RageBot);
	VGUIMenuForm.AddWindow(Visuals);
	VGUIMenuForm.AddWindow(Misc);
	VGUIMenuForm.AddWindow(Settings);

}

void VGUIMenu::Kill()
{}

void VGUIMenu::PaintTraverse()
{
	if (m_isVisible)
	{
		VGUIMenuForm.Paint();
	}
}

void VGUIMenu::Click()
{
	if (m_isVisible)
		VGUIMenuForm.Click();
}

void VGUIMenu::HandleInput(WPARAM vk)
{
	if (m_isVisible)
		VGUIMenuForm.HandleInput(vk);
}

void VGUIMenu::Toggle()
{
	m_isVisible = !m_isVisible;
	cl_mouseenable->SetValue(!m_isVisible);
}

