#pragma once
#include "Autowall.h"
enum AA_Y
{
	AA_Y_NONE,
	AA_Y_DOWN,
	AA_Y_UP,
	AA_Y_RANDOM
};

enum AA_X
{
	AA_X_NONE,
	AA_X_BACKWARDS,
	AA_X_SIDE,
	AA_X_SLOWSPIN,
	AA_X_FASTSPIN,
	AA_X_LBY,
	AA_X_LBYBREAKER,
	AA_X_JITTLERSPIN,
	AA_X_RANDOM
};
class CCSGOPlayerAnimState
{
public:
	char	__pad[0xEC];
	float	m_flSpeed;
};

class Animations
{
public:
	void UpdateServerAnimations(CUserCmd * cmd, bool * bsendpacket);
	float m_flFirstBodyUpdate;
	CCSGOPlayerAnimState*	m_serverAnimState;
private:
	void CreateAnimationState(CCSGOPlayerAnimState* state, player_t* player);
	void UpdateAnimationState(CCSGOPlayerAnimState* state, QAngle ang);
	void ResetAnimationState(CCSGOPlayerAnimState* state);


	CBaseHandle*				m_ulEntHandle;
	float					m_flSpawnTime;
	QAngle					m_angRealAngle = QAngle(0, 0, 0);
	player_t*			m_pLocalPlayer;
	float					m_flNextBodyUpdate;
};
extern Animations anims;
class CRagebot
{
public:
	void Run(CUserCmd* cmd, bool& bSendPacket);
private:
	int HitScan(player_t* player);
	float FovToPlayer(Vector ViewOffset, player_t* player, int hitbox, CUserCmd* cmd);
	void SetAngles(CUserCmd* cmd, int x, int y);
	void SetAngles(CUserCmd* cmd, QAngle x);
	bool Hitchance();
	bool CanHit(Vector point, int mindmg);
	float getbestyaw(CUserCmd* cmd);
	void AntiAim(CUserCmd* cmd, bool& bSendPacket);
	bool locked;
    bool newtarg;
}; extern CRagebot rage;