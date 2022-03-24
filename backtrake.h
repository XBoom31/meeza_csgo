#pragma once
#include "valve_sdk/csgostructs.hpp"

#define NUM_OF_TICKS 12

struct StoredData
{
	float simtime;
	Vector hitboxPos;
};

class TimeWarp : public Singleton<TimeWarp>
{
	int nLatestTick;
	
public:
	StoredData TimeWarpData[64][NUM_OF_TICKS];
	void CreateMove(CUserCmd* cmd);
};