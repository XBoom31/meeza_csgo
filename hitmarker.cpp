#include "hitmarker.h"
#include "data.h"
#include <Windows.h>
#pragma comment(lib, "Winmm.lib") 
phitmarker hitmarker;

void phitmarker::paint()
{
	if (time != 0) {
		int w, h;
		g_EngineClient->GetScreenSize(w, h);
		Render::Line(w / 2 - 20, h / 2 - 20, w / 2 - 10, h / 2 - 10, Color(200, 200, 200, time * 8.5)); //tpp left
		Render::Line(w / 2 + 20, h / 2 - 20, w / 2 + 10, h / 2 - 10, Color(200, 200, 200, time * 8.5)); //top right
		Render::Line(w / 2 - 20, h / 2 + 20, w / 2 - 10, h / 2 + 10, Color(200, 200, 200, time * 8.5)); //bottom left
		Render::Line(w / 2 + 20, h / 2 + 20, w / 2 + 10, h / 2 + 10, Color(200, 200, 200, time * 8.5)); //bottom right
		time--;
	}
}

void phitmarker::hit()
{
	time = 60;
	PlaySoundA(bubble, NULL, SND_MEMORY | SND_ASYNC);
}
