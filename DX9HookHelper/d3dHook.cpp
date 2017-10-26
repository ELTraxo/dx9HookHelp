#include "Interface.h"
#define ESCALLRVA 0x7F694B

typedef HRESULT(__stdcall * _EndScene)(IDirect3DDevice9* pDevice);
_EndScene oEndScene;


Interface * g_pFace = nullptr;

HRESULT __stdcall hEndScene(IDirect3DDevice9* pDevice)
{
	if (g_pFace)
	{
		if(g_pFace->bRunning)
			g_pFace->Update(pDevice);
	}

	Draw::RectUP(pDevice, 5, 5, 30, 30, DXColor::Red);
	return oEndScene(pDevice);
}


d3dHook::d3dHook(Console & con, Memory & mem)
	:
	pCon(con), pMem(mem)
{
}

d3dHook::~d3dHook()
{
}


void * d3dHook::GetDxDeviceFuncByIndex(int iIndex)
{
	if (pDevice == nullptr)
	{
		IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
		if (pD3D)
		{
			D3DPRESENT_PARAMETERS d3dPP = { 0 };
			d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
			d3dPP.hDeviceWindow = GetForegroundWindow();
			d3dPP.Windowed = TRUE;

			IDirect3DDevice9* pDummy = nullptr;

			HRESULT hr = pD3D->CreateDevice(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				d3dPP.hDeviceWindow,
				D3DCREATE_HARDWARE_VERTEXPROCESSING,
				&d3dPP, &pDummy);

			void** vTable = *(void***)(pDummy);

			return vTable[iIndex];
		}
	}
	else
	{
		void** vTable = *(void***)(pDevice);
		return vTable[iIndex];
	}
	return nullptr;
}

void d3dHook::PrintDeviceAddresses()
{
	for (int x = 0x69; x < 0x80; x++)
		pCon.WriteF("[%d]-> %p\n", x, GetDxDeviceFuncByIndex(x));
}

void * d3dHook::GetEndSceneCaller(void * pFuncLoc) // this only works for directx EndScene... lol!
{
	void * pLoc = VirtualAlloc(nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	uintptr_t pFunc = (uintptr_t)pLoc + sizeof(uintptr_t);

	BYTE buff[] = {
		0x50,										// push eax
		0x8B, 0x44, 0x24, 0x04						// mov eax, [esp+04]
	};
	pMem.WriteBytes(pFunc, buff, 5);

	BYTE buff2[] = { 0x89, 0x05 };					//
	pMem.WriteBytes(pFunc + 5, buff2, 2);				//
	pMem.WriteValue<void*>(pFunc + 7, pLoc);				//mov [retnAddy], eax

	BYTE buff3[] = {
		0x58,										// pop eax
		0x6A, 0x18, 								// push 18
		0xB8, 0x0A, 0x61, 0xF7, 0x62, 				// mov eax, d3d8d.ll+7610A
		0xE9 										// jmp
	};
	pMem.WriteBytes(pFunc + 7 + sizeof(uintptr_t), buff3, 9);

	uintptr_t jmpBack = (uintptr_t)pFuncLoc - (pFunc + 7 + sizeof(uintptr_t) + 9) + 2;
	pMem.WriteValue<uintptr_t>((pFunc + 7 + sizeof(uintptr_t) + 9), jmpBack);

	//hook EndScene
	pMem.Hook(pFuncLoc, (void*)pFunc, JMP, 7);

	//while it's doing it's thang
	while (*(uintptr_t*)pLoc == 0);

	BYTE ogBytes[] = { 0x6A, 0x18, 					// push 18
		0xB8, 0x0A, 0x61, 0xF7, 0x62 };				// mov eax, d3d8.dll+7610A

	pMem.WriteBytes((uintptr_t)pFuncLoc, ogBytes, 7);

	//now lets read the thang?
	uintptr_t retAddy = (uintptr_t)*(void**)pLoc;

	VirtualFree(pLoc, 0x100, MEM_RELEASE);

	return (void*)(retAddy - 6);
}

void * d3dHook::GetFunctionThisPtr(void * pFunc)
{
	//this is where we'll save the pointer, create code cave
	void * pLoc = VirtualAlloc(nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	ZeroMemory(pLoc, 0x100);

	//this is where code will go
	uintptr_t pFuncLoc = (uintptr_t)pLoc + sizeof(uintptr_t);

	//build cave
	BYTE buff[] = { 0x89, 0x0D };
	pMem.WriteBytes(pFuncLoc, buff, 2);
	pMem.WriteValue<uintptr_t>(pFuncLoc + 2, RC<uintptr_t>(pLoc));  // mov [thisPtr], ecx

	BYTE buff1[] = { 0x6A, 0x18, 0xB8, 0x0A, 0x61, 0xF7, 0x62, 0xE9 };
	pMem.WriteBytes(pFuncLoc + 2 + sizeof(uintptr_t), buff1, 8);

	//calculate jmp back
	uintptr_t jmpBack = (uintptr_t)pFunc - (pFuncLoc + 2 + sizeof(uintptr_t) + 8) + 3;
	pMem.WriteValue<uintptr_t>(pFuncLoc + 2 + sizeof(uintptr_t) + 8, jmpBack);

	//hook
	pMem.Hook(pFunc, (void*)pFuncLoc, JMP, 7);

	//wait til space is filled
	while (*(uintptr_t*)pLoc == 0);

	//read it
	uintptr_t thisPtr = *(uintptr_t*)pLoc;
	
	//clean up
	BYTE ogBytes[] = { 0x6A, 0x18, 					// push 18
		0xB8, 0x0A, 0x61, 0xF7, 0x62 };				// mov eax, d3d8.dll+7610A

	pMem.WriteBytes((uintptr_t)pFunc, ogBytes, 7); //unhook

	VirtualFree(pLoc, 0x100, MEM_RELEASE);

	return RC<void*>(thisPtr);
}


void * d3dHook::GetHookLocation()
{
	//Get the address of the function
	void * pEndScene = GetDxDeviceFuncByIndex(42);

	return GetEndSceneCaller(pEndScene);
}

void d3dHook::HookEndScene()
{
	if (!bHooked)
	{
		pCon.Write("Getting EndScene Address...\n");
		oEndScene = (_EndScene)GetDxDeviceFuncByIndex(42);
		pCon.WriteF("Found EndScene at: 0x%p\n", (uintptr_t)oEndScene);

		pCon.Write("Figuring out where the game calls EndScene...\n");
		void * pAddress = GetHookLocation();
		pCon.WriteF("Found the call at: 0x%p\n", pAddress);
		//pCon.WritePointer(RC<int>(pAddress));

		pCon.Write("Hooking from there...\n");
		pCallES = (uintptr_t)pAddress;
		memcpy(pOgBytes, pAddress, 6);
		if (pMem.Hook(pAddress, hEndScene, CALL, 6))
		{
			Address2Clipboard(pCallES);
			pCon.Write("We're hooked!\n");
		}
		else
			pCon.Write("Something went wrong... :/");

		bHooked = true;
	}
	else
	{
		pCon.Write("Unhooking...\n");
		Unhook();
		pCon.Write("Done...\n");
	}
}

void d3dHook::Unhook()
{
	BYTE ogBytes[] = { 0xFF, 0x91, 0xA8, 0x00, 0x00, 0x00 };
	pMem.WriteBytes(pCallES, pOgBytes, 6);
	bHooked = false;
}

void d3dHook::Address2Clipboard(uintptr_t addy)
{
	char buffer[20]{ 0 };
	sprintf(buffer, "%p", addy);
	std::string sBuff = "0x";
	sBuff += buffer;
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, sBuff.size());
	if (!hg)
	{
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), sBuff.c_str(), sBuff.size());
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}

bool d3dHook::isHooked()
{
	return bHooked;
}

