#pragma once


#include "valve_sdk\csgostructs.hpp"


class CNotificatonSys
{
public:
	void painttransverse();
	void output(std::wstring text, Color col = Color(255, 255, 255), bool fixed2 = false);
	void remove();
private:
	float last_update;
	std::vector<Color>        colors; //std::pair is hard
	std::vector<std::wstring> strings;
	std::vector<std::wstring> fixed;
}; extern CNotificatonSys NotificatonSys;