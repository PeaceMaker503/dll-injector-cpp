// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

HMODULE hmod = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{

	switch (ul_reason_for_call)
	{

	case DLL_PROCESS_ATTACH:
		hmod = hModule;
		DisableThreadLibraryCalls(hmod);
		MessageBox(NULL, "Welcome", "Dll injected !", MB_OK);
		break;

	case DLL_PROCESS_DETACH: 	
		if (lpReserved) {
			FreeLibrary(hmod);
			MessageBox(NULL, "Goodbye", "Dll uninjected !", MB_OK);
		}
		break;

	case DLL_THREAD_ATTACH : 
	case DLL_THREAD_DETACH : break; //those two won't be used anyway thanks to DisableThreadLibraryCalls(hmod);

	}

	return TRUE;
}

