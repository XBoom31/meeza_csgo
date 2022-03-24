#include "engine_prediction.h"
#include "hooks.hpp"
#include "options.hpp"
float flOldCurtime;
float flOldFrametime;

//antario paste ^^
//don't tell anybody, okay? and let's never talk about this.
void engine_prediction::run(CUserCmd* cmd)
{
	if (!vars.misc.engine_pred)
		return;
	static int flTickBase;
	static CUserCmd* pLastCmd;
	// fix tickbase if game didnt render previous tick
	if (pLastCmd)
	{
		if (pLastCmd->hasbeenpredicted)
			flTickBase = local->m_nTickBase();
		else
			++flTickBase;
	}
	int cm = cmd->command_number;
	// get random_seed as its 0 in clientmode->createmove
	const auto getRandomSeed = [cm]()
	{
		using MD5_PseudoRandomFn = unsigned long(__cdecl*)(std::uintptr_t);
		auto client = GetModuleHandleW(L"client.dll");
		static auto offset = Utils::PatternScan(client , "55 8B EC 83 E4 F8 83 EC 70 6A 58");
		static auto MD5_PseudoRandom = reinterpret_cast<MD5_PseudoRandomFn>(offset);
		
		return MD5_PseudoRandom(cm) & 0x7FFFFFFF;
	};


	pLastCmd = cmd;
	flOldCurtime = g_GlobalVars->curtime;
	flOldFrametime = g_GlobalVars->frametime;

	//int uRandomSeed = getRandomSeed();
	g_GlobalVars->curtime = flTickBase * g_GlobalVars->interval_per_tick;
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	g_GameMovement->StartTrackPredictionErrors(local);
	
	CMoveData data;
	memset(&data, 0, sizeof(CMoveData));

	g_MoveHelper->SetHost(local);
	//https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/gamemovement.cpp#L1135
	g_Prediction->SetupMove(local, cmd, g_MoveHelper, &data);
	g_GameMovement->ProcessMovement(local, &data);
	g_Prediction->FinishMove(local, cmd, &data);
}

void engine_prediction::end()
{
	if (!vars.misc.engine_pred)
		return;
	g_GameMovement->FinishTrackPredictionErrors(local);
	g_MoveHelper->SetHost(nullptr);

	g_GlobalVars->curtime = flOldCurtime;
	g_GlobalVars->frametime = flOldFrametime;
}
