#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "DXColor.h"
#include "dxFuncIdxx.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
class d3dHook
{
public:
	d3dHook(Console & con, Memory & mem);
	~d3dHook();
	
	void * GetDxDeviceFuncByIndex(int iIndex);
	void PrintDeviceAddresses();
	void * GetEndSceneCaller(void* pFuncLoc);
	void * GetFunctionThisPtr(void * pFunc);

	void * GetHookLocation();
	void HookEndScene();
	void Unhook();
	void Address2Clipboard(uintptr_t addy);
	bool isHooked();
private:
	Console & pCon;
	Memory & pMem;

private:
	IDirect3DDevice9 * pDevice = nullptr;
	BYTE pOgBytes[6]{ 0 };
	uintptr_t pCallES;

private:
	bool bHooked = false;
};

