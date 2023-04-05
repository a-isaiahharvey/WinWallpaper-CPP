// WinWallpaper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <comdef.h>
#include "combaseapi.h"
#include "shobjidl_core.h"

class MyDesktopWallpaper {
    IDesktopWallpaper* pDesk;

public:
    MyDesktopWallpaper() {
        CoInitialize(NULL);
        CoCreateInstance(__uuidof(DesktopWallpaper), NULL, CLSCTX_LOCAL_SERVER, IID_IDesktopWallpaper, (void**)&pDesk);
    }

    ~MyDesktopWallpaper() {
        pDesk->Release();
        CoFreeUnusedLibraries();
        CoUninitialize();
    }

    void SetWallpaper(LPCWSTR fullPath) {
        LPWSTR monitorId = nullptr;
        pDesk->GetMonitorDevicePathAt(0, &monitorId);

        pDesk->SetWallpaper(LPCWSTR(monitorId), fullPath);
        pDesk->SetPosition(DWPOS_SPAN);
    }

    LPWSTR GetWallpaper() {
        LPWSTR monitorId = nullptr;
        LPWSTR wallpaper = nullptr;
        pDesk->GetMonitorDevicePathAt(0, &monitorId);

        pDesk->GetWallpaper(monitorId, &wallpaper);
    }
};

int main(int argc, char* argv[])
{
    LPCWSTR arg = NULL;

    if (argc > 1) {
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, NULL, 0);
        arg = new WCHAR[size_needed];
        MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, (LPWSTR)arg, size_needed);
    }

    MyDesktopWallpaper wallpaper = MyDesktopWallpaper();

    wallpaper.SetWallpaper(arg);

    
    delete[] arg;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
