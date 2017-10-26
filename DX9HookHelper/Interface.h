#pragma once
#include <stdio.h>
#include <string>
#include "Memory.h"
#include "keyDefines.h"
#include "Console.h"
#include "d3dHook.h"
#include "draw.h"

class Interface
{
public:
	Interface();
	~Interface();
	void Init();
	void Unload();
	bool HandleInput();
	void Update(IDirect3DDevice9 * pDevice);
	IDirect3DDevice9 * GetDevicePtr();

private:
	void PrintIntro();
	bool gaks(DWORD vKey);
private:
	Memory mem;
	Console con;
	d3dHook dxHook;
	Draw draw;
	IDirect3DDevice9 * pDevice;

public:
	bool bRunning = true;
};


