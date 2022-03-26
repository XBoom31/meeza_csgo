#define NOMINMAX
#include <Windows.h>

#include "valve_sdk/sdk.hpp"
#include "helpers/utils.hpp"
#include "helpers/input.hpp"
#include "notificationsystem.h"
#include "hooks.hpp"
#include "menu.h"
#include "options.hpp"
#include "chrome.h"
#include "config.h"
#include "vguimenu.h"


RecvVarProxyFn fnNoSmoke;
void NoSmoke(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	if (vars.visuals.world.nosmoke) *(bool*)((DWORD)pOut + 0x1) = true;
	fnNoSmoke(pData, pStruct, pOut);
}

void NetvarHook()
{
	ClientClass *pClass = g_CHLClient->GetAllClasses();
	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
		if (!strcmp(pszName, "DT_SmokeGrenadeProjectile"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;
				if (!strcmp(name, "m_bDidSmokeEffect"))
				{
					fnNoSmoke = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = NoSmoke;
				}

			}
		}
		pClass = pClass->m_pNext;
	}
}


DWORD WINAPI OnDllAttach(LPVOID base)
{
    // 
    // Wait at most 10s for the main game modules to be loaded.
    // 
    if(Utils::WaitForModules(10000, { L"client.dll", L"engine.dll", L"shaderapidx9.dll" }) == WAIT_TIMEOUT) {
        // One or more modules were not loaded in time
        return FALSE;
    }

#ifdef _DEBUG
    Utils::AttachConsole();
#endif

    try {
		Utils::ConsolePrint("Welcome to meeza.cc built on %s %s!\n", __DATE__, __TIME__);
        Utils::ConsolePrint("Starting...\n");

        Interfaces::Initialize();
		Utils::ConsolePrint("Dumping...\n");
        Interfaces::Dump();
		//p internet u know
        NetvarSys::Get().Initialize();
        NetvarSys::Get().Dump();
        InputSys::Get().Initialize();
		game.init();
        Hooks::Initialize();
		//NetvarHook();

        // Register some hotkeys.
        // - Note:  The function that is called when the hotkey is pressed
        //          is called from the WndProc thread, not this thread.
        // 

		
        InputSys::Get().RegisterHotkey(VK_LBUTTON, []() {
            oldmenu.Click();
            });

		InputSys::Get().RegisterHotkey(VK_INSERT, []() {
			menu.Toggle();
		});
        InputSys::Get().RegisterHotkey(VK_HOME, []() {
            oldmenu.Toggle();
        });
        // Panic button
        InputSys::Get().RegisterHotkey(VK_DELETE, []() {
			if (menu.IsVisible())
				menu.Toggle();
            g_Unload = true;
        });
        // Menu Toggle

		InputSys::Get().RegisterHotkey(VK_F3, []() {
			//resolver.reset = true;
		});
		InputSys::Get().RegisterHotkey(VK_F1, []() {
			configs::save("smt.meeza");
		});
		InputSys::Get().RegisterHotkey(VK_F2, []() {
			NotificatonSys.output(L"Hey");
			NotificatonSys.output(L"Hey", Color(255,0,0));
		});

        Utils::ConsolePrint("Finished.\n");

		globs.gsa = (DWORD)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 83 7D 08 FF 56 8B F1 74"));
		globs.aim = true;
        vars.visuals.world.vmodelfov = 70;

        while(!g_Unload)
            Sleep(1000);

        g_CVar->FindVar("crosshair")->SetValue(true);

        FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);

    } catch(const std::exception& ex) {
        Utils::ConsolePrint("An error occured during initialization:\n");
        Utils::ConsolePrint("%s\n", ex.what());
        Utils::ConsolePrint("Press any key to exit.\n");
        Utils::ConsoleReadKey();
        Utils::DetachConsole();

        FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
    }

    // unreachable
    //return TRUE;
}

BOOL WINAPI OnDllDetach()
{
#ifdef _DEBUG
    Utils::DetachConsole();
#endif

    Hooks::Shutdown();
    return TRUE;
}

BOOL WINAPI DllMain(
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved
)
{
    switch(fdwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDll);
            CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
            return TRUE;
        case DLL_PROCESS_DETACH:
            if(lpvReserved == nullptr)
                return OnDllDetach();
            return TRUE;
        default:
            return TRUE;
    }
}
