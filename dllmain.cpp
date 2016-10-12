// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <stdio.h>

FARPROC p[5] = { 0 };

void LoadFunctions() {

	char * sz_buffer = new char[255];
	char * sz_systemDirectory = new char[255];
	memset(sz_systemDirectory, 0, 255);

	GetSystemDirectory(sz_systemDirectory, 255);

	sprintf(sz_buffer, "%s\\dinput8.dll", sz_systemDirectory);

	HMODULE hL = LoadLibrary(sz_buffer);
	if (hL == 0)
	{
		return;
	}

	p[0] = GetProcAddress(hL, "DirectInput8Create");
	p[1] = GetProcAddress(hL, "DllCanUnloadNow");
	p[2] = GetProcAddress(hL, "DllGetClassObject");
	p[3] = GetProcAddress(hL, "DllRegisterServer");
	p[4] = GetProcAddress(hL, "DllUnregisterServer");

}

// DirectInput8Create
extern "C" __declspec(naked) void __stdcall __E__0__()
{
	__asm
	{
		jmp p[0];
	}
}
// DllCanUnloadNow
extern "C" __declspec(naked) void __stdcall __E__1__()
{
	__asm
	{
		jmp p[1];
	}
}
// DllGetClassObject
extern "C" __declspec(naked) void __stdcall __E__2__()
{
	__asm
	{
		jmp p[2];
	}
}
// DllRegisterServer
extern "C" __declspec(naked) void __stdcall __E__3__()
{
	__asm
	{
		jmp p[3];
	}
}
// DllUnregisterServer
extern "C" __declspec(naked) void __stdcall __E__4__()
{
	__asm
	{
		jmp p[4];
	}
}



typedef struct DIJOYSTATE {
	LONG    lX;                     /* x-axis position              */
	LONG    lY;                     /* y-axis position              */
	LONG    lZ;                     /* z-axis position              */
	LONG    lRx;                    /* x-axis rotation              */
	LONG    lRy;                    /* y-axis rotation              */
	LONG    lRz;                    /* z-axis rotation              */
	LONG    rglSlider[2];           /* extra axes positions         */
	DWORD   rgdwPOV[4];             /* POV directions               */
	BYTE    rgbButtons[32];         /* 32 buttons                   */
} DIJOYSTATE, *LPDIJOYSTATE;

void DevilMayCryAxisInvert()
{

	int result;

	DIJOYSTATE *dijoystate;

	__asm
	{
		push edx;
		push 0x50;
		mov dijoystate, EDX;
		MOV ECX, DWORD PTR DS : [EAX];
		PUSH EAX;
		CALL DWORD PTR DS : [ECX + 0x24];
		mov result, eax
	}

	LONG lX = dijoystate->lX;
	LONG lY = dijoystate->lY;

	dijoystate->lX = dijoystate->lRx;
	dijoystate->lY = dijoystate->lRy;
	dijoystate->lRx = lX;
	dijoystate->lRy = lY;

	__asm mov eax, result;
}

void DevilMayCryHook()
{

	DWORD dwOldProtect, dwNewProtect, dwOldCall, dwNewCall, dwNewAddress, dwAddress;

	dwAddress = 0x00404590;
	dwNewAddress = (DWORD)&DevilMayCryAxisInvert;

	BYTE opCALL[9] = { 0xE8, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90 };
	dwNewCall = dwNewAddress - dwAddress - 5;
	memcpy(&opCALL[1], &dwNewCall, 4);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)dwAddress, 9, PAGE_EXECUTE_WRITECOPY, &dwOldProtect);
	memcpy((LPVOID)dwAddress, &opCALL, 9);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)dwAddress, 9, dwOldProtect, &dwNewProtect);

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		LoadFunctions();
		DevilMayCryHook();
		break;
			
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

