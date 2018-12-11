#pragma once
#include "Render.h"
#include "valve_sdk\csgostructs.hpp"

//replace this with your own functions
#define box(x,y,w,h,filled, color)   Render::Box(x,y,w,h,filled,color)
#define line(x,y, x1,y1, color)      Render::Line(x,y,x1,y2, color)

#define menuwidth 780
#define menuheight 380


class chrome {
public:
	void init();
	void paint();
	bool state();
	void toggle();
private:
	bool toggled;
	int menux;
	int menuy;
};

extern chrome game;