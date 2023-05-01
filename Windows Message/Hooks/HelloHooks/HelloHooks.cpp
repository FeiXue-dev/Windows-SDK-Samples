// HelloHooks.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>

int i = 0;
LRESULT CALLBACK myMouse(int nCode, WPARAM wParm, LPARAM lParm)
{
	std::cout << "yes " << i++ << std::endl;
	return CallNextHookEx(NULL, nCode, wParm, lParm);
}

void PrintLastErrorAsString(DWORD errorMessageID)
{
	LPSTR messageBuffer = nullptr;

	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);
	std::cout << "Error id is " << errorMessageID << std::endl;
	std::cout << "Error msg is " << message << std::endl;
}

int main()
{
	DWORD err = 0;
	HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, myMouse, 0, 0);
	if (mouseHook == NULL)
	{
		err = GetLastError();
		PrintLastErrorAsString(err);
	}


	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
	}
	UnhookWindowsHookEx(mouseHook);
	return 0;
}

int FindWindTitle()
{
	LPWSTR pszMem;
	int windowTitleLen = 0;
	windowTitleLen = GetWindowTextLength((HWND)0x00060CB0);
	pszMem = (LPWSTR)VirtualAlloc((LPVOID)NULL,
		(DWORD)(windowTitleLen + 1), MEM_COMMIT,
		PAGE_READWRITE);
	GetWindowText((HWND)0x00060CB0, pszMem, windowTitleLen + 1);
	return 0;
}

