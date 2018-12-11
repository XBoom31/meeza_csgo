#include "chrome.h"

chrome game;


void chrome::init()
{
	menux = 100;
	menuy = 100;

}

void chrome::paint()
{
	box(menux, menuy, menuwidth, menuheight, true, Color(200, 200, 200));
}

bool chrome::state()
{
	return toggled;
}

void chrome::toggle()
{
	toggled = !toggled;
}
