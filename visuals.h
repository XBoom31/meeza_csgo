#pragma once
#include "helpers\math.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "hooks.hpp"
#include "helpers/utils.hpp"
#include "Render.h"

typedef struct ESPBOX
{
	ESPBOX(int x1, int x2, int x3, int x4)
	{
		x = x1;
		y = x2;
		w = x3;
		h = x4;
	}
    ESPBOX()
	{
	}
	int x;
	int y;
	int w;
	int h;
}ESPBOX;
typedef struct ESPDATA
{
	ESPDATA() {};
	player_t* pl;
	Vector feetpos;
	Vector headpos;
	ESPBOX box = ESPBOX();
	int team;
	bool vis;
	Color col;
	float curtime;

}ESPDATA;
class CVisuals
{
public:
	CVisuals() {}
	void Run();
	void CreateFonts();
	void DestroyFonts();
private:
	void Box();
	ESPDATA data;
	void Clear();
	void Main(player_t* entity);
	ESPBOX GetBox(entity_t* entity);
	void Skeleton();
	void Name();
	void HealthBar();
	void ArmourBar();
	void Weapon();
	void Snaplines();
	void Info();
	void GetColor();
}; extern CVisuals Visuals;