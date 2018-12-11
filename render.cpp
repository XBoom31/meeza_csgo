#include "Render.h"

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
		//g_VGuiSurface->DrawOutlinedRect(x-1, y-1, w+1, h+1);
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

