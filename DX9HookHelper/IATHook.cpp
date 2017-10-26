#include "Memory.h"

IATHook::IATHook(Console * con)
{
	SetConsole(con);
}

IATHook::IATHook(char* sModule, void* pHook, char* sSymbol)
{
	Init(sModule, pHook, sSymbol);
}

IATHook::IATHook(HMODULE hModule, void * pHook, char * sSymbol)
{
	Init(hModule, pHook, sSymbol);
}

IATHook::~IATHook()
{
	Unload();
}

void IATHook::SetConsole(Console * con)
{
	this->con = con;
}

void IATHook::Init(char * sModule, void * pHook, char * sSymbol)
{
	this->sModule = sModule;
	this->hModule = GetModuleHandleA(sModule);
	this->pHook = pHook;
	this->sSymbol = sSymbol;
	pFunctionPtr = (uintptr_t)FindInIAT(sSymbol, this->hModule);
	pOldFunc = *(uintptr_t*)pFunctionPtr;
}

void IATHook::Init(HMODULE hModule, void * pHook, char * sSymbol)
{
	this->hModule = (hModule ? hModule : GetModuleHandle(0));
	this->pHook = pHook;
	this->sSymbol = sSymbol;
	pFunctionPtr = (uintptr_t)FindInIAT(sSymbol, this->hModule);
	pOldFunc = *(uintptr_t*)pFunctionPtr;
}


//stole this off the interwebz :D
void** IATHook::FindInIAT(const char* sFunction, HMODULE hModule)
{
	if (!hModule)
		hModule = GetModuleHandle(0);

	PIMAGE_DOS_HEADER img_dos_headers = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS img_nt_headers = (PIMAGE_NT_HEADERS)((byte*)img_dos_headers + img_dos_headers->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR img_import_desc = 
		(PIMAGE_IMPORT_DESCRIPTOR)((byte*)img_dos_headers + 
			img_nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	if (img_dos_headers->e_magic != IMAGE_DOS_SIGNATURE)
	{
		if(con)
			con->Write("ERROR: e_magic is not a valid DOS signature\n");
	}
		

	for (IMAGE_IMPORT_DESCRIPTOR *iid = img_import_desc; iid->Name != 0; iid++)
	{
		for (int func_idx = 0; *(func_idx + (void**)(iid->FirstThunk + (size_t)hModule)) != nullptr; func_idx++)
		{
			char* mod_func_name = (char*)(*(func_idx + (size_t*)(iid->OriginalFirstThunk + (size_t)hModule)) + (size_t)hModule + 2);
			const intptr_t nmod_func_name = (intptr_t)mod_func_name;
			if (nmod_func_name >= 0)
			{
				if (!::strcmp(sFunction, mod_func_name))
					return func_idx + (void**)(iid->FirstThunk + (size_t)hModule);
			}
		}
	}

	return 0;
}

bool IATHook::Hook()
{
	if (!VirtualProtect((void*)this->pFunctionPtr, sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &this->dwOldPrt))
		return false;
	*(uintptr_t*)pFunctionPtr = (uintptr_t)pHook;
	VirtualProtect((void*)this->pFunctionPtr, sizeof(uintptr_t), dwOldPrt, nullptr);
	return true;
}

bool IATHook::Unload()
{
	if (!VirtualProtect((void*)this->pFunctionPtr, sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &this->dwOldPrt))
		return false;
	*(uintptr_t*)pFunctionPtr = pOldFunc;
	VirtualProtect((void*)this->pFunctionPtr, sizeof(uintptr_t), dwOldPrt, nullptr);
	return true;
}
