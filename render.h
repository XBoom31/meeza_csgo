#pragma once
#include "../CSGOSimple/valve_sdk/csgostructs.hpp"

namespace Render
{
	void Box(int x, int y, int w, int h, bool filled, Color color);
	void Text(int x, int y, bool centered, bool outlined, vgui::HFont font, Color color, const wchar_t* text);
	void Line(int x, int y, int x2, int y2, Color color);
	void CleanBox(int x, int y, int w, int h, bool filled, Color color);
	inline void Box(Vector2D start_pos, Vector2D end_pos, Color col)
	{
		Box(start_pos.x, start_pos.y, end_pos.x, end_pos.y,false,  col);
	}
	inline void Box(Vector2D start_pos, Vector2D end_pos,bool filled, Color col)
	{
		Box(start_pos.x, start_pos.y, end_pos.x, end_pos.y, filled, col);
	}
	inline void FilledGradientRectangle(int x0, int y0, int x1, int y1, Color top, Color bottom)
	{
		g_VGuiSurface->DrawSetColor(top);
		g_VGuiSurface->DrawFilledRectFade(x0, y0, x1, y1, 255, 255, false);

		g_VGuiSurface->DrawSetColor(bottom);
		g_VGuiSurface->DrawFilledRectFade(x0, y0, x1, y1, 0, 255, false);
	}
	inline Vector2D GetTextSize(const wchar_t* text, vgui::HFont font)
	{
		int x_res, y_res;
		g_VGuiSurface->GetTextSize(font, text, x_res, y_res);
		return Vector2D(x_res, y_res);
	}
}
