// WinWallpaper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <comdef.h>
#include "combaseapi.h"
#include "shobjidl_core.h"
#include <string>
#include <vector>

struct Monitor {
    LPWSTR monitorIndex;
    LPWSTR wallpaper;

    Monitor(LPWSTR monitorIndex , LPWSTR wallpaper) {
        this->monitorIndex = monitorIndex;
        this->wallpaper = wallpaper;
    }
};

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

    std::vector<Monitor> getMonitors() {
        UINT monitorCount = 0;
        pDesk->GetMonitorDevicePathCount(&monitorCount);

        std::vector<Monitor> monitors;

        for(UINT index = 0; index < monitorCount; index++) {
            LPWSTR monitorId = NULL;
            pDesk->GetMonitorDevicePathAt(index, &monitorId);

            LPWSTR wallpaper = NULL;
            pDesk->GetWallpaper(monitorId, &wallpaper);

            monitors.push_back(Monitor(monitorId, wallpaper));
        }

        return monitors;
    }

    void SetWallpaper(LPCWSTR fullPath, Monitor monitor) {

        pDesk->SetWallpaper(LPCWSTR(monitor.monitorIndex), fullPath);
        pDesk->SetPosition(DWPOS_SPAN);
    }

    LPWSTR GetWallpaper(Monitor monitor) {
        LPWSTR wallpaper = nullptr;

        pDesk->GetWallpaper(monitor.monitorIndex, &wallpaper);

        return wallpaper;
    }
};

int main(int argc, char* argv[])
{
    LPCWSTR path = NULL;
    auto wallpaper = MyDesktopWallpaper();
    auto monitors = wallpaper.getMonitors();

    if (argc > 1) {
        if (std::string(argv[1]) == "get") {
            auto monitor = monitors.at(0);
            if (argv[2] != nullptr) {
                int index;
                sscanf_s((argv[2]), "%d", &index);
                monitor = monitors.at(index);
            }

            std::wcout << wallpaper.GetWallpaper(monitor) << std::endl;;
        } else if (std::string(argv[1]) == "set") {
            auto monitor = monitors.at(0);
            if (argv[2] != nullptr) {
                int size_needed = MultiByteToWideChar(CP_UTF8, 0, argv[2], -1, NULL, 0);
                path = new WCHAR[size_needed];
                MultiByteToWideChar(CP_UTF8, 0, argv[2], -1, (LPWSTR)path, size_needed);

                wallpaper.SetWallpaper(path, monitor);
            } else {
                std::cout << "Please provide a path to an image" << std::endl;
            }
        }
        
    }

    delete[] path;
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
