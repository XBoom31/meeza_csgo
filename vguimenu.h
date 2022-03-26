#pragma once
#include "valve_sdk\sdk.hpp"
#include "helpers\input.hpp"
#include "helpers\utils.hpp"
#include "Render.h"
#include "singleton.hpp"
#include "helpers\math.hpp"
#include "options.hpp"
#include <array>

// VGUIVGUIMenu styling
// Layout
#define VGUIMenu_PADDING 2
// Colors
#define VGUIMenu_COLOR_CONTROL Color(80, 80, 80)
#define VGUIMenu_COLOR_CONTROL_BORDER Color(0, 0, 0)
#define VGUIMenu_COLOR_CONTROL_HOVER Color(144, 144, 144)
#define VGUIMenu_COLOR_CONTROL_ACTIVE Color(195, 195, 195)
#define VGUIMenu_COLOR_CONTROL_ACTIVE_HOVER Color(230, 230, 230)

void InitializePaintKits();
void initialize();

struct paint_kit {
	int id;
	std::string name;

	auto operator < (const paint_kit& other) const -> bool {
		return name < other.name;
	}
};

extern std::vector<paint_kit> k_skins;

enum TABS {
	TAB_LEGITBOT,
	TAB_RAGEBOT,
	TAB_VISUALS,
	TAB_MISC,
	TAB_SETTINGS
};
class CObject
{
protected:
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_layer;
	bool m_hover;
public:
	CObject(int x = 0, int y = 0, int width = 0, int height = 0, int m_layer = 0);
	virtual void Paint(int = 0, int = 0, float* = nullptr) = 0;
	virtual void Click() {};
	virtual void HandleInput(WPARAM vk) {};

	bool operator < (const CObject& obj) const
	{
		return (m_layer < obj.m_layer);
	}
};

class CObjectContainer
{
protected:
	std::vector<CObject*> Objects;
public:
	void AddControl(CObject* obj);
};

class CWindow : public CObject, public CObjectContainer
{
	bool* m_open;
	float m_controlheight;
	bool m_dragging;
	int m_dragoffsetx, m_dragoffsety;
public:
	CWindow(int x = 0, int y = 0, int width = 0, int height = 0, bool* open = nullptr);
	void Paint(int t_x = 0, int t_y = 0, float* controlheight = nullptr);
	void Click();
	void HandleInput(WPARAM vk);
};

class CForm
{
	std::vector<CWindow> Windows;
public:
	void Paint();
	void Click();
	void Clear();
	void AddWindow(CWindow window) { Windows.push_back(window); };
	void HandleInput(WPARAM vk);
};

class CChild : public CObject, public CObjectContainer
{
	float m_controlheight;
	std::wstring m_title;
	int me;
	bool mem;
public:
	CChild(int x, int y, int width, std::wstring title, int mes, bool meme = false);
	void Paint(int t_x = 0, int t_y = 0, float* controlheight = nullptr);
	void Click();
	void HandleInput(WPARAM vk);
};

class CUpdateChild : public CObject, public CObjectContainer
{
	float m_controlheight;
	std::wstring m_title;
	std::function<std::vector<CObject*>()> updatefunc;
	int me;
public:
	CUpdateChild(int x, int y, int width, std::wstring title, std::function<std::vector<CObject*>()> update = {}, int mes = 999);
	void Paint(int t_x = 0, int t_y = 0, float* controlheight = nullptr);
	void Click();
	void HandleInput(WPARAM vk);
};

class CSpacing : public CObject
{
	int m_height;
public:
	CSpacing(int height);
	void Paint(int t_x = 0, int t_y = 0, float* controlheight = nullptr);
};

class CLabel : public CObject
{
	std::wstring m_text;
	vgui::HFont m_font;
	Color m_color;
	bool m_centered;
public:
	CLabel(std::wstring text = L"", vgui::HFont font = 0, Color color = Color::Error, bool centered = false);
	void Paint(int t_x = 0, int t_y = 0, float* controlheight = nullptr);
};

class CCheckBox : public CObject
{
	std::wstring m_label;
	bool* m_val;
public:
	CCheckBox(std::wstring label = L"", bool* value = nullptr);
	void Paint(int t_x = 0, int t_y = 0, float* controlheight = nullptr);
	void Click();
};

template<typename T>
class CSlider : public CObject
{
	std::wstring m_label;
	T m_min, m_max;
	T* m_val;
public:
	CSlider(std::wstring label, T min, T max, T* value);
	void Paint(int t_x, int t_y, float* controlheight);
};

class CButton : public CObject
{
	std::wstring m_label;
	std::function<void()> m_cb;
	int posx;
	int posy;
public:
	CButton(int width = 0, int height = 0, std::wstring label = L"", std::function<void()> cb = {}, int posxz = 0, int posyz = 0);
	void Paint(int t_x = 0, int t_y = 0, float* controlheight = nullptr);
	void Click();
};

class CTextBox : public CObject
{
	std::wstring m_placeholder;
	std::wstring* m_val;
	bool m_selected;
	Vector2D m_tl, m_br;
public:
	CTextBox(int width = 0, int height = 0, std::wstring placeholder = L"", std::wstring* value = nullptr);
	void Paint(int t_x = 0, int t_y = 0, float* controlheight = nullptr);
	void Click();
	void HandleInput(WPARAM vk);
};

class CGetKey : public CObject
{
	std::wstring m_placeholder;
	int* m_val;
	bool isSelected;
	Vector2D m_tl, m_br;
public:
	CGetKey(std::wstring placeholder, int* val);
	void Paint(int t_x = 0, int t_y = 0, float* controlheight = nullptr);
	void Click();
	void HandleInput(WPARAM vk);
};

class CColorPicker : public CObject
{
	std::wstring m_label;
	Color* m_col;
public:
	CColorPicker(std::wstring label, Color* col);
	void Paint(int t_x = 0, int xt_y = 0, float* controlheight = nullptr);
};

class CCombo : public CObject
{
	int* m_val;
	std::vector<std::wstring> m_labels;
	bool hover;
	int index;
	bool selected;
	std::wstring label;
public:
	CCombo(std::wstring dlabel, int* value = nullptr, std::vector<std::wstring> labels = std::vector<std::wstring>());
	void Paint(int t_x = 0, int t_y = 0, float* controlheight = nullptr);
	void Click();
};
class CListBox : public CObject
{
	int* m_val;
	std::vector<std::wstring> m_labels;
	std::vector<std::wstring> processed;
	int size;
	bool m_lhover;
	int elements;
	bool m_rhover;
	bool m_scrollhover;
	int hoverindex;
	int* scrolloffset;
public:
	CListBox(int melements, int* scr = nullptr, int* value = nullptr, std::vector<std::wstring> labels = std::vector<std::wstring>());
	void Paint(int t_x = 0, int t_y = 0, float* controlheight = nullptr);
	void HandleInput(WPARAM vk);
	void Click();
};

class VGUIMenu
{
	bool m_isVisible;
	CForm VGUIMenuForm;
public:
	void Init();
	void Kill();

	void PaintTraverse();
	void Click();
	void HandleInput(WPARAM vk);

	void Toggle();

	bool IsVisible() const { return m_isVisible; }
};  
extern VGUIMenu oldmenu;


























//ugly tgf pasta
static/*pfix*/ const wchar_t* keyNames[] =
{
	L"",
	L"Mouse 1",
	L"Mouse 2",
	L"Cancel",
	L"Middle Mouse",
	L"Mouse 4",
	L"Mouse 5",
	L"",
	L"Backspace",
	L"Tab",
	L"",
	L"",
	L"Clear",
	L"Enter",
	L"",
	L"",
	L"Shift",
	L"Control",
	L"Alt",
	L"Pause",
	L"Caps",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"Escape",
	L"",
	L"",
	L"",
	L"",
	L"Space",
	L"Page Up",
	L"Page Down",
	L"End",
	L"Home",
	L"Left",
	L"Up",
	L"Right",
	L"Down",
	L"",
	L"",
	L"",
	L"Print",
	L"Insert",
	L"Delete",
	L"",
	L"0",
	L"1",
	L"2",
	L"3",
	L"4",
	L"5",
	L"6",
	L"7",
	L"8",
	L"9",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"A",
	L"B",
	L"C",
	L"D",
	L"E",
	L"F",
	L"G",
	L"H",
	L"I",
	L"J",
	L"K",
	L"L",
	L"M",
	L"N",
	L"O",
	L"P",
	L"Q",
	L"R",
	L"S",
	L"T",
	L"U",
	L"V",
	L"W",
	L"X",
	L"Y",
	L"Z",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"Numpad 0",
	L"Numpad 1",
	L"Numpad 2",
	L"Numpad 3",
	L"Numpad 4",
	L"Numpad 5",
	L"Numpad 6",
	L"Numpad 7",
	L"Numpad 8",
	L"Numpad 9",
	L"Multiply",
	L"Add",
	L"",
	L"Subtract",
	L"Decimal",
	L"Divide",
	L"F1",
	L"F2",
	L"F3",
	L"F4",
	L"F5",
	L"F6",
	L"F7",
	L"F8",
	L"F9",
	L"F10",
	L"F11",
	L"F12",

};