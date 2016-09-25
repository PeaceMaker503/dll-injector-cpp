#include "Injector.h"
#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <conio.h>
#include <stdio.h> 

Injector::Injector(void)
{
}


Injector::~Injector(void)
{
} 

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ) 

bool Injector::Inject(char* procName,char* dllName)
{
	DWORD pID = GetTargetThreadIDFromProcName(procName); 
	return this->Inject(pID, dllName);
}

bool Injector::Inject(DWORD pID,char* dllName)
{
   HANDLE Proc = 0; 
   HMODULE hLib = 0; 
   char buf[50] = {0}; 
   LPVOID RemoteString, LoadLibAddy; 

   if(!pID) 
      return false; 

   Proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID); 
   if(!Proc) 
   { 
      snprintf(buf, sizeof(buf), "OpenProcess() failed: %d", GetLastError()); 
      MessageBox(NULL, buf, "Loader", MB_OK); 
      printf(buf); 
      return false; 
   } 
    
   LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA"); 
   RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, strlen(dllName), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
   WriteProcessMemory(Proc, (LPVOID)RemoteString, dllName, strlen(dllName), NULL);
   CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL); 
   CloseHandle(Proc); 
   return true; 
}

DWORD Injector::GetTargetThreadIDFromProcName(const char * ProcName)
{
	PROCESSENTRY32 pe;
	HANDLE thSnapShot;
	BOOL retval, ProcFound = false;
 
	thSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(thSnapShot == INVALID_HANDLE_VALUE)
	{
		printf("Error: Unable to create toolhelp snapshot!");
		return false;
	}
 
	pe.dwSize = sizeof(PROCESSENTRY32);
 
	retval = Process32First(thSnapShot, &pe);
	while(retval)
	{
		if(!strcmp(pe.szExeFile, ProcName))
		{
			return pe.th32ProcessID;
		}
		retval = Process32Next(thSnapShot, &pe);
	}
	return 0;
}

