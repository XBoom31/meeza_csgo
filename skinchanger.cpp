#include "ItemDefinitions.h"
#include "valve_sdk\csgostructs.hpp"
#include "skinchanger.h"
#include "options.hpp"
cskinchanger skinchanger;
void cskinchanger::frame_stage_notify()
{
	if (!vars.misc.skinchanger)
		return;

	player_info_t localPlayerInfo;
	if (!g_EngineClient->player_info(g_EngineClient->GetLocalPlayer(), &localPlayerInfo))
		return;

	auto local_id = g_EngineClient->GetLocalPlayer();
	if (!local_id) return;
	player_t* localplayer = (player_t*)g_EntityList->GetClientEntity(local_id);
	if (!localplayer) return;

	int* hweapons = localplayer->get_weapons();
	if (!hweapons)
		return;

	for (int i = 0; hweapons[i] != INVALID_EHANDLE_INDEX; i++)
	{
		entity_t* Weapon = (entity_t*)g_EntityList->GetClientEntity(hweapons[i] & 0xFFF);
		weapon_t* pWeapon = (weapon_t*)Weapon;

		if (!Weapon)
			continue;

		if (localPlayerInfo.xuid_low != pWeapon->m_OriginalOwnerXuidLow())
			continue;

		if (localPlayerInfo.xuid_high != pWeapon->m_OriginalOwnerXuidHigh())
			continue;

		int definition_index = pWeapon->m_Item().m_iItemDefinitionIndex();
		if (definition_index == 0)
			continue;

		if (definition_index == 42 || definition_index == 508)
		{

			int mdl = g_MdlInfo->GetModelIndex(ItemDefinitions::WeaponInfo.at(WEAPON_KNIFE_M9_BAYONET).model);
			pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET;
			pWeapon->m_nModelIndex() = mdl;
			pWeapon->m_iViewModelIndex() = mdl;
			pWeapon->m_nFallbackPaintKit() = vars.settings.skins[42].weapon_skin_id;
			pWeapon->m_flFallbackWear() = 0.001f;
			pWeapon->m_Item().m_iItemIDHigh() = -1;
			pWeapon->m_Item().m_iEntityQuality() = 3;
		}
		else
		{
			//if (g_Options.W[i].ChangerSkin)
			//{
				pWeapon->m_nFallbackPaintKit() = vars.settings.skins[definition_index].weapon_skin_id;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
			//}

		}

	}

} 