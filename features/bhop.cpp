#include "bhop.hpp"

#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "../helpers/input.hpp"
#include "../helpers/math.hpp"
void BunnyHop::OnCreateMove(CUserCmd* cmd)
{
  static bool jumped_last_tick = false;
  static bool should_fake_jump = false;

  if(!jumped_last_tick && should_fake_jump) {
    should_fake_jump = false;
    cmd->buttons |= IN_JUMP;
  } else if(cmd->buttons & IN_JUMP) {
    if(local->m_fFlags() & FL_ONGROUND) {
      jumped_last_tick = true;
      should_fake_jump = true;
    } else {
      cmd->buttons &= ~IN_JUMP;
      jumped_last_tick = false;
    }
  } else {
    jumped_last_tick = false;
    should_fake_jump = false;
  }
  if (vars.misc.autostrafe && InputSys::Get().IsKeyDown(VK_SPACE))
  {
	  static bool AutoStrafeFlip = false;

	  cmd->forwardmove = 0.0f;
	  cmd->sidemove = 0.0f;

	  if (cmd->mousedx < 0.f)
	  {
		  cmd->sidemove = -450.0f;
	  }
	  else if (cmd->mousedx > 0.f)
	  {
		  cmd->sidemove = 450.0f;
	  }
	  else
	  {
		  if (AutoStrafeFlip)
		  {
			  QAngle new_ang;
			  new_ang = QAngle(cmd->viewangles.pitch, cmd->viewangles.yaw - 1.0f, 0.0f);
			  Math::ClampAngles(new_ang);

			  cmd->viewangles = new_ang;
			  cmd->sidemove = -450.0f;
			  AutoStrafeFlip = false;
		  }
		  else
		  {
			  QAngle new_ang;
			  new_ang = QAngle(cmd->viewangles.pitch, cmd->viewangles.yaw + 1.0f, 0.0f);
			  Math::ClampAngles(new_ang);

			  cmd->viewangles = new_ang;
			  cmd->sidemove = 450.0f;
			  AutoStrafeFlip = true;
		  }
	  }
  }
}