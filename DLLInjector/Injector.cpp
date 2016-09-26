#include "Injector.h"
#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <conio.h>
#include <stdio.h> 
#include <iostream>
#include <fstream>

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ) 

bool Injector::Inject(char* procName,char* dllName)
{
	DWORD pID = GetPIDFromProcName(procName);
	return this->Inject(pID, dllName);
}

bool Injector::Inject(DWORD pID,char* dllName)
{
	if (!pID) {
		std::cout << "Invalid process id." << std::endl;
		return false;
   }

	if (!std::ifstream(dllName).good()) {
		std::cout << "Dll does not exist." << std::endl;
		return false;
   }

   HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);

   if(!proc)
   { 
	  std::cout << "OpenProcess() failed: " << GetLastError() << std::endl;
      return false; 
   } 

   LPVOID LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA"); 

   if (!LoadLibAddy) {
	   std::cout << "GetProcAddress() failed: " << GetLastError() << std::endl;
	   return false;
   }

	LPVOID RemoteString = (LPVOID)VirtualAllocEx(proc, NULL, strlen(dllName), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (!RemoteString) {
		std::cout << "VirtualAllocEx() failed: " << GetLastError() << std::endl;
		return false;
	}

	BOOL writeStatus = WriteProcessMemory(proc, (LPVOID)RemoteString, dllName, strlen(dllName), NULL);

	if (!writeStatus) {
		std::cout << "WriteProcessMemory() failed: " << GetLastError() << std::endl;
		return false;
	}

	HANDLE thread = CreateRemoteThread(proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL);

	if (!thread) {
		std::cout << "CreateRemoteThread() failed: " << GetLastError() << std::endl;
		return false;
	}

	BOOL closeStatus = CloseHandle(proc);

	if (!closeStatus) {
		std::cout << "CloseHandle() failed: " << GetLastError() << std::endl;
		return false;
	}

	return true;
}

DWORD Injector::GetPIDFromProcName(const char * ProcName)
{
	PROCESSENTRY32 pe;
	HANDLE thSnapShot;
	BOOL retval, ProcFound = false;
 
	thSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(thSnapShot == INVALID_HANDLE_VALUE)
	{
		std::cout << "Error: Unable to create toolhelp snapshot!" << GetLastError() << std::endl;
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

	return 0; //no process found
}

