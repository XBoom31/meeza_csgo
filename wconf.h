#pragma once


#define targetbydistance 0
#define targetbyfov 1
#define targetbyhealth 2

struct wconf
{
	bool active;
	float smooth;
	float fov;
	float rcs;
	float humanize;
	int backtrack;
	bool hitscan;
	bool trigger;
	int thitchance;
	int hitbox;
	bool silent;
	bool legitaa;
	bool legitaaresolver;
	int key;
	float tapsnap;
	bool tap;
	bool tapsafe;
	int triggerkey;
};

/*
typedef struct Weapon
{
Weapon()
{
active = false;
fov = 0;
smooth = 0;
sillent = false;
randomness = 0;
hitscan = false;
hitbox = 0; //HITBOX_HEAD
rcs = 0;
rcsaimbotonly = true;
rcsx = false;
rcsy = false;
targetby = targetbyfov;
int key = 1;
bool smokecheck = false;
}
bool active;
float fov;
int smooth;
bool sillent;
int randomness;
bool hitscan;
int hitbox;
int rcs;
bool rcsaimbotonly;
bool rcsx;
bool rcsy;
int targetby;
int key;
bool smokecheck;

}Weapon;*/
