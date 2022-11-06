// ExePrj.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Rawetrip.h"

int _tmain(int argc, _TCHAR* argv[])
{
	/* Invoke the generated code, just like invoke the DLL. */
	Rawetrip_Init();
	Rawetrip_LoadLibrary();

	/* 
		Here you can call Rawetrip_GetProcAddress to get the addresses of exported functions. 

		Example:
		void (*fTestFun)(LPCSTR);
		*(FARPROC*)&fTestFun = Win32Dll_GetProcAddress("TestFun");
		fTestFun("Hello World!");
	*/

	Rawetrip_FreeLibrary();

	return 0;
}