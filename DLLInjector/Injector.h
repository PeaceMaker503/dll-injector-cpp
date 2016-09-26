#pragma once
#include <Windows.h>

class Injector
{
public:
	bool Inject(char* procName,char* dllName);
	bool Inject(DWORD pID,char* dllName);

private:
	DWORD GetPIDFromProcName(const char * ProcName);
};

