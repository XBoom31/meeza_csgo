#pragma once

#include "../misc/CUserCmd.hpp"

#define MULTIPLAYER_BACKUP 150

class bf_write;
class bf_read;

class CInput
{
public:
    virtual void  Init_All(void);
    virtual void  Shutdown_All(void);
    virtual int   GetButtonBits(int);
    virtual void  CreateMove(int sequence_number, float input_sample_frametime, bool active);
    virtual void  ExtraMouseSample(float frametime, bool active);
    virtual bool  WriteUsercmdDeltaToBuffer(bf_write* buf, int from, int to, bool isnewcommand);
    virtual void  EncodeUserCmdToBuffer(bf_write& buf, int slot);
    virtual void  DecodeUserCmdFromBuffer(bf_read& buf, int slot);


    inline CUserCmd* GetUserCmd(int sequence_number);
    inline CVerifiedUserCmd* GetVerifiedCmd(int sequence_number);

    char pad_0000[8]; //0x0000
    bool m_fTrackIRAvailable; //0x000C
    bool m_fMouseInitialized; //0x000D
    bool m_fMouseActive; //0x000E
    bool m_fJoystickAdvancedInit; //0x000F
    char pad_0010[156]; //0x0010
    bool m_fCameraInterceptingMouse; //0x00C0
    bool m_fCameraInThirdPerson; //0x00C1
    bool m_fCameraMovingWithMouse; //0x00C2
    char pad_00C3[1]; //0x00C3
    Vector m_vecCameraOffset; //0x00C4
    bool m_fCameraDistanceMove; //0x00D0
    char pad_00D1[19]; //0x00D1
    bool m_CameraIsOrthographic; //0x00E4
    bool m_CameraIsThirdPersonOverview; //0x00E5
    char pad_00E6[2]; //0x00E6
    QAngle m_angPreviousViewAngles;
    QAngle m_angPreviousViewAnglesTilt;
    float m_flLastForwardMove; //0x0100
    int32_t m_nClearInputState; //0x0104
    CUserCmd* m_pCommands; //0x0108
    CVerifiedUserCmd* m_pVerifiedCommands; //0x010C
};

CUserCmd* CInput::GetUserCmd(int sequence_number)
{
    return &m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
}

CVerifiedUserCmd* CInput::GetVerifiedCmd(int sequence_number)
{
    return &m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
}
