// TestProgram.cpp : Demonstrate that LoadImage() on windows 10
//                   fails with top-down bitmaps but Windows 7 succeeds.

#include <windows.h>
#include <tchar.h>

int main()
{
    for (int i = 0; i < 2; ++i)
    {
        const LPCWSTR psz_filename = (i == 0)
            ? L"bitmap-bottom-up.bmp"
            : L"bitmap-top-down.bmp";
        const HANDLE h_bit_map
            = LoadImage(nullptr, psz_filename, IMAGE_BITMAP, 0, 0,
                LR_LOADFROMFILE | LR_CREATEDIBSECTION| LR_DEFAULTSIZE);
        if (h_bit_map == nullptr)
        {
            // An "invalid" bitmap does not call SetLastError()
            // if (GetLastError() == NO_ERROR)
            //     SetLastError(ERROR_INVALID_DATA);

            const DWORD dw_last_error = GetLastError();

            LPWSTR sz_msg_buf{};
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                nullptr, dw_last_error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                // LPWSTR cast required (see docs)
                reinterpret_cast<LPWSTR>(&sz_msg_buf), 0, nullptr);

            fwprintf_s(stdout, L"%s: LoadImage API call failed to return a handle;\n"
                "\tGetLastError = %d %s\n",
                psz_filename, dw_last_error, sz_msg_buf);
            LocalFree(sz_msg_buf);

            DeleteObject(h_bit_map);
        }
        else
            (void)fwprintf_s(stdout, L"%s: LoadImage API call succeeded!\n", psz_filename);
    }
    return (0);
}