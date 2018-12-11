#pragma once

#include <cstdint>

// Created with ReClass.NET by KN4CK3R
#pragma pack(push, 1)
class INetChannel
{
public:
	char pad_0000[20]; //0x0000
	bool m_bProcessingMessages; //0x0014
	bool m_bShouldDelete; //0x0015
	char pad_0016[2]; //0x0016
	int m_nOutSequenceNr; //0x0018
	int m_nInSequenceNr; //0x001C
	int m_nOutSequenceNrAck; //0x0020
	int m_nOutReliableState; //0x0024
	int m_nInReliableState; //0x0028
	int m_nChokedPackets; //0x002C
	char pad_0030[108]; //0x0030
	int m_Socket; //0x009C
	int m_StreamSocket; //0x00A0
	int m_MaxReliablePayloadSize; //0x00A4
	char pad_00A8[100]; //0x00A8
	float last_received; //0x010C
	float connect_time; //0x0110
	char pad_0114[4]; //0x0114
	int m_Rate; //0x0118
	char pad_011C[4]; //0x011C
	float m_fClearTime; //0x0120
	char pad_0124[16688]; //0x0124
	char m_Name[32]; //0x4254
	unsigned int m_ChallengeNr; //0x4274
	float m_flTimeout; //0x4278
	char pad_427C[32]; //0x427C
	float m_flInterpolationAmount; //0x429C
	float m_flRemoteFrameTime; //0x42A0
	float m_flRemoteFrameTimeStdDeviation; //0x42A4
	int m_nMaxRoutablePayloadSize; //0x42A8
	int m_nSplitPacketSequence; //0x42AC
	char pad_42B0[40]; //0x42B0
	bool m_bIsValveDS; //0x42D8
	char pad_42D9[65]; //0x42D9
}; //Size: 0x0444

class CClockDriftMgr
{
public:
    float m_ClockOffsets[16];   //0x0000
    uint32_t m_iCurClockOffset; //0x0044
    uint32_t m_nServerTick;     //0x0048
    uint32_t m_nClientTick;     //0x004C
}; //Size: 0x0050

// This struct is most likely wrong
// Only fields that I know to be valid are:
// - m_NetChannel
// - m_nCurrentSequence
// - m_nDeltaTick
// - m_nMaxClients
// - viewangles
class CClientState
{
public:
    void ForceFullUpdate()
    {
        m_nDeltaTick = -1;
    }

    char pad_0000[156];             //0x0000
    uint32_t m_NetChannel;          //0x009C
    uint32_t m_nChallengeNr;        //0x00A0
    char pad_00A4[100];             //0x00A4
    uint32_t m_nSignonState;        //0x0108
    char pad_010C[8];               //0x010C
    float m_flNextCmdTime;          //0x0114
    uint32_t m_nServerCount;        //0x0118
    uint32_t m_nCurrentSequence;    //0x011C
    char pad_0120[8];               //0x0120
    CClockDriftMgr m_ClockDriftMgr; //0x0128
    uint32_t m_nDeltaTick;          //0x0178
    bool m_bPaused;                 //0x017C
    char pad_017D[3];               //0x017D
    uint32_t m_nViewEntity;         //0x0180
    uint32_t m_nPlayerSlot;         //0x0184
    char m_szLevelName[260];        //0x0188
    char m_szLevelNameShort[40];    //0x028C
    char m_szGroupName[40];         //0x02B4
    char pad_02DC[56];              //0x02DC
    uint32_t m_nMaxClients;         //0x0310
    char pad_0314[18940];           //0x0314
    Vector viewangles;              //0x4D10
}; //Size: 0x4D1C

#pragma pack(pop)

/*static_assert(FIELD_OFFSET(CClientState, m_NetChannel)       == 0x009C, "Wrong struct offset");
static_assert(FIELD_OFFSET(CClientState, m_nCurrentSequence) == 0x011C, "Wrong struct offset");
static_assert(FIELD_OFFSET(CClientState, m_nDeltaTick)       == 0x0174, "Wrong struct offset");
static_assert(FIELD_OFFSET(CClientState, m_nMaxClients)      == 0x0310, "Wrong struct offset");
static_assert(FIELD_OFFSET(CClientState, viewangles)         == 0x4D10, "Wrong struct offset");*/