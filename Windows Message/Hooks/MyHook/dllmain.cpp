// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <strsafe.h>
#include <stdio.h>
#include <tchar.h>

#define BUFSIZE 512
HHOOK g_hHook;

int SendMsg(TCHAR* pchReply, HANDLE hPipe, LPCTSTR lpszPipename, LPCWSTR msg, HWND hWnd);

extern "C" LIBRARY_API LRESULT CALLBACK HookProc(
	int nCode,
	WPARAM wParam,
	LPARAM lParam
);

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//g_hHook = SetWindowsHookEx(WH_GETMESSAGE, HookProc, hModule, 0);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		//UnhookWindowsHookEx(g_hHook);
		break;
	}
	return TRUE;
}

extern "C" LIBRARY_API LRESULT CALLBACK HookProc(
	int nCode,
	WPARAM wParam,
	LPARAM lParam
)
{
	BOOL   fConnected = FALSE;
	HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
	LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	HANDLE hHeap = GetProcessHeap();
	TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));


	switch (nCode)
	{
	case HCBT_ACTIVATE:
	{
		LPCWSTR msg = TEXT("Active the window: ");
		SendMsg(pchReply, hPipe, lpszPipename, msg,(HWND)wParam);
		break;
	}
	case HCBT_CREATEWND:
	{
		LPCWSTR msg = TEXT("Create the window: ");
		StringCchCopy(pchReply, BUFSIZE, msg);
		SendMsg(pchReply, hPipe, lpszPipename, msg,(HWND)wParam);
		break;
	}
	case HCBT_DESTROYWND:
	{
		LPCWSTR msg = TEXT("Destory the window: ");
		StringCchCopy(pchReply, BUFSIZE, msg);
		SendMsg(pchReply, hPipe, lpszPipename, msg,(HWND)wParam);
		break;
	}
	case HCBT_MINMAX:
	{
		LPCWSTR msg = TEXT("Minimized or maximized the window: ");
		StringCchCopy(pchReply, BUFSIZE, msg);
		SendMsg(pchReply, hPipe, lpszPipename, msg,(HWND)wParam);
		break;
	}
	case HCBT_MOVESIZE:
	{
		LPCWSTR msg = TEXT("Move or size the window: ");
		StringCchCopy(pchReply, BUFSIZE, msg);
		SendMsg(pchReply, hPipe, lpszPipename, msg,(HWND)wParam);
		break;
	}
	case HCBT_SETFOCUS:
	{
		LPCWSTR msg = TEXT("Set focuse the window: ");
		StringCchCopy(pchReply, BUFSIZE, msg);
		SendMsg(pchReply, hPipe, lpszPipename, msg,(HWND)wParam);
		break;
	}
	default:
		break;
	}

	int i = 0;
	i++;
	std::cout << "yes " << i++ << std::endl;
	HeapFree(hHeap, 0, pchReply);
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int SendMsg(TCHAR* pchReply, HANDLE hPipe, LPCTSTR lpszPipename, LPCWSTR msg, HWND hWnd)
{
	LPWSTR pszMem;
	int windowTitleLen = 0;
	windowTitleLen = GetWindowTextLength((HWND)hWnd);
	pszMem = (LPWSTR)VirtualAlloc((LPVOID)NULL,
		(DWORD)(windowTitleLen + 1), MEM_COMMIT,
		PAGE_READWRITE);
	GetWindowText((HWND)hWnd, pszMem, windowTitleLen + 1);
	std::wstring windowText(pszMem);
	std::wstring mywstring = msg + windowText;
	LPCWSTR sendMsg = mywstring.c_str();
	StringCchCopy(pchReply, BUFSIZE, sendMsg);

	BOOL fSuccess;
	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0, dwMode;

	while (1)
	{
		hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

		// Break if the pipe handle is valid. 
		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return -1;
		}
		// All pipe instances are busy, so wait for 20 seconds. 
		if (!WaitNamedPipe(lpszPipename, 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return -1;
		}
	}

	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 

	cbReplyBytes = (lstrlen(pchReply) + 1) * sizeof(TCHAR);

	fSuccess = WriteFile(
		hPipe,        // handle to pipe 
		pchReply,     // buffer to write from 
		cbReplyBytes, // number of bytes to write 
		&cbWritten,   // number of bytes written 
		NULL);        // not overlapped I/O 
	OutputDebugString(msg);

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	return fSuccess;
}
