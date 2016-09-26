// DLLInjector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Injector.h"
#include <iostream>
#include <string>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{

	TCHAR processName[MAX_PATH];
	TCHAR dllDir[MAX_PATH];

	cout << "DLL absolute path: ";
	cin >> dllDir;
	cout << "Process (<name>.exe): ";
	cin >> processName;
	
	Injector* injector = new Injector();

	if(injector->Inject(processName, dllDir)){
		printf("DLL injected!\n");
	} else {
		printf("Failed to inject the dll...\n");
	}
	
	system("PAUSE");
	free(injector);

	return 0;
}