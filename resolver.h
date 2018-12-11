#pragma once
#include "valve_sdk\csgostructs.hpp"


struct resolver_data
{
	player_t* player;
	float fake_angle;
	float lowerbody_angle;

};

class cresolver
{
public:
	void run();
	bool used_fake[65];
	bool reset;
	bool breaking_lby[65];
private:
	resolver_data data[64][12]; //for every nigga
};
extern cresolver resolver;