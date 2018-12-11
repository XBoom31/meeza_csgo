#include "notificationsystem.h"
#include "Render.h"
#include "options.hpp"
CNotificatonSys NotificatonSys;
template<typename T>
void pop_front(std::vector<T>& vec)
{
	if(vec.size() != 0)
	vec.erase(vec.begin());
}

static bool memed = false;
void CNotificatonSys::painttransverse()
{
	//gotta update this shit first, don't we, gentlemen?
	if ((int)(g_GlobalVars->realtime * 2) % 7 == 0) { //make it so it dissapears after some time
		if (!memed)
		{
			pop_front(strings);
			pop_front(colors);
			memed = true;
		}
	}
	else
		memed = false;

	fixed.clear(); //clear the fixed shit, it should get updated every run of this anyway
	
	//now we got to paint this.

	//painting dynamic shit
	int textsize = Render::GetTextSize(L"meme", globs.ufont).y+1;
	for (int x = 0; x != strings.size(); x++)
	{
		std::wstring str = strings[x];
		Color col        = colors[x];
		Render::Text(10, 30 + (textsize * x), false, true, globs.ufont, col, str.data());
	}

	//painting static shit
	int w, h;
	g_VGuiSurface->GetScreenSize(w, h);
	int size = textsize * fixed.size() / 2;
	static int xc = 0;
	for (std::wstring str : fixed)
	{
		int x = w - Render::GetTextSize(str.data(), globs.ufont).x- 2;
		int y = xc * textsize;
		Render::Text(x, size + y, false, true, globs.ufont, Color(255, 255, 255), str.data());
		xc++;
	}
}

void CNotificatonSys::output(std::wstring text, Color col, bool fixed2)
{
	if (!fixed2)
	{
		if (strings.size() > 5)
			pop_front(strings);
		strings.push_back(text);
		if (colors.size() > 5)
			pop_front(colors);
		colors.push_back(col);
	}
	else
	{
		fixed.push_back(text);
	}
}

void CNotificatonSys::remove()
{
	if(strings.size() != 0)
	strings.pop_back();
	if (colors.size() != 0)
	colors.pop_back();
}
