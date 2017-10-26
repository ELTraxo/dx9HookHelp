#include "Interface.h"


extern Interface * g_pFace;

Interface::Interface() : dxHook(con, mem)
{
	Init();
}


Interface::~Interface()
{
}

void Interface::Init()
{
	//uintptr_t hLoc = (uintptr_t)dxHook.GetHookLocation();
	//con.WritePointer(hLoc);
	//dxHook.HookEndScene();
	PrintIntro();
	dxHook.PrintDeviceAddresses();
	g_pFace = this;
}

void Interface::Unload()
{
	if(dxHook.isHooked())
		dxHook.Unhook();
	con.~Console();
	bRunning = false;
}

bool Interface::HandleInput()
{
	if (GetAsyncKeyState(ENDKEY))
		Unload();
	else if (GetAsyncKeyState(VK_NUMPAD1) & 1)
	{
		con.Clear();
		con.SetCursorPos(COORD{ 0,0 });
		PrintIntro();
		/*				get device pointer
		so... how the fuck are we going to do this...
		We can hook EndScene and get ecx. basically the same thing 
		as GetCaller except we'll move the device pointer instead of the ret address.*/
		void * pEndScene = dxHook.GetDxDeviceFuncByIndex(IdxEndScene);
		void * pThis = dxHook.GetFunctionThisPtr(pEndScene);

		con.WriteF("\npEndScene: %p\npThis: %p\n", pEndScene, pThis);

		void * pDIP = dxHook.GetDxDeviceFuncByIndex(d3dDeviceFuncs::DrawIndexedPrimitive);
		con.WriteF("pDIP: %p\n", pDIP);
	}
	else if (GetAsyncKeyState(VK_NUMPAD2) & 1)
	{
		// print a few useful functions and their addresses
	}
	else if (GetAsyncKeyState(VK_NUMPAD3) & 1)
	{
		con.Clear();
		con.SetCursorPos(COORD{ 0,0 });
		PrintIntro();
		dxHook.HookEndScene();
	}
	else
		return true;
}

void Interface::Update(IDirect3DDevice9 * pDevice)
{
	this->pDevice = pDevice;
	//draw.FilledRect(pDevice, 5, 5, 150, 20, DXColor::Red);
	LPD3DXFONT f = nullptr;
	draw.CreateFontW(pDevice, 16, f);
	if (f)
	{
		draw.DrawTextW(pDevice, f, 16, 6, 6, DXColor::Black, L"Woop woop bitches, auto-hookin!!");
		//draw.DrawTextW(pDevice, f, 16, 8, 7, DXColor::Black, L"Woop woop bitches, auto-hookin!!");
		draw.DrawTextW(pDevice, f, 16, 7, 7, DXColor::Cyan, L"Woop woop bitches, auto-hookin!!");
	}
}

IDirect3DDevice9 * Interface::GetDevicePtr()
{
	if (pDevice)
		return pDevice;
	else
		return nullptr;
}

void Interface::PrintIntro()
{
	con.Write("Welcome to Traxin's DX9 Helper!\n");
	con.Write("Press [Num1] to get the device ptr.\n");
	con.Write("Press [Num2] to print a few different useful functions.\n");
	con.Write("Press [Num3] to hook and test drawing.\n");
	con.Write("\n[===================================================================]\n\n");
}

bool Interface::gaks(DWORD vKey)
{
	if (GetAsyncKeyState(vKey))
		return true;
	else
		return false;
}
