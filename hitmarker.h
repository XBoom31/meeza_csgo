#pragma once
#include "render.h"

class phitmarker {
public:
	void paint();
	void hit();
private:
	int time;
}; extern phitmarker hitmarker;