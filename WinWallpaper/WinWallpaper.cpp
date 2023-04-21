// WinWallpaper.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include "combaseapi.h"
#include "shobjidl_core.h"
#include <comdef.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

struct Monitor {
	LPWSTR monitorIndex;
	LPWSTR wallpaper;

	Monitor(LPWSTR monitorIndex, LPWSTR wallpaper) {
		this->monitorIndex = monitorIndex;
		this->wallpaper = wallpaper;
	}

	~Monitor() {

	}
};

class MyDesktopWallpaper {
	IDesktopWallpaper* pDesk = nullptr;

public:
	MyDesktopWallpaper() {
		HRESULT hr = CoInitialize(NULL);
		if (FAILED(hr)) {
			std::cout << "Failed to initialize COM" << std::endl;
			return;
		}

		hr = CoCreateInstance(__uuidof(DesktopWallpaper), NULL, CLSCTX_LOCAL_SERVER,
			IID_IDesktopWallpaper, (void**)&pDesk);

		if (FAILED(hr)) {
			std::cout << "Failed to create COM instance" << std::endl;
			return;
		}
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

		for (UINT index = 0; index < monitorCount; index++) {
			LPWSTR monitorId = NULL;
			pDesk->GetMonitorDevicePathAt(index, &monitorId);

			LPWSTR wallpaper = NULL;
			pDesk->GetWallpaper(monitorId, &wallpaper);

			monitors.push_back(Monitor(monitorId, wallpaper));
		}

		return monitors;
	}

	void SetWallpaper(LPCWSTR fullPath, Monitor monitor,
		DESKTOP_WALLPAPER_POSITION position) {

		HRESULT hr = pDesk->SetWallpaper(LPCWSTR(monitor.monitorIndex), fullPath);
		if (FAILED(hr)) {
			std::cout << "Failed to set the desktop wallpaper" << std::endl;
			return;
		}
		hr = pDesk->SetPosition(position);
		if (FAILED(hr)) {
			std::cout << "Failed to set the desktop wallpaper position" << std::endl;
			return;
		}
	}

	LPWSTR GetWallpaper(Monitor monitor) {
		LPWSTR wallpaper = nullptr;

		HRESULT hr = pDesk->GetWallpaper(monitor.monitorIndex, &wallpaper);
		
		if (FAILED(hr)) {
			std::cout << "Failed to get the desktop wallpaper path" << std::endl;
		}

		return wallpaper;
	}
};

int main(int argc, char* argv[]) {
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

			std::wcout << wallpaper.GetWallpaper(monitor) << std::endl;

		}
		else if (std::string(argv[1]) == "set") {
			auto monitor = monitors.at(0);
			auto position = DESKTOP_WALLPAPER_POSITION::DWPOS_SPAN;

			if (argv[2] != nullptr) {
				auto imagePath = std::filesystem::path(argv[2]);
				if (!std::filesystem::exists(imagePath)) {
					std::cout << "The specified path does not exist" << std::endl;
					return 1;
				}

				wchar_t fullPath[MAX_PATH];
				if (GetFullPathName(imagePath.c_str(), MAX_PATH, fullPath, nullptr) == 0)
				{
					std::cout << "Could not get the full path to file" << std::endl;
					return 1;
				}

				if (argv[3] != nullptr) {
					if (std::string(argv[3]).find("--scale=") != std::string::npos) {
						auto scaleAsString = std::string(argv[3]).replace(0, 8, "");
						if (scaleAsString == "center") {
							position = DWPOS_CENTER;
						}
						else if (scaleAsString == "stretch") {
							position = DWPOS_STRETCH;
						}
						else if (scaleAsString == "tile") {
							position = DWPOS_TILE;
						}
						else if (scaleAsString == "span") {
							position = DWPOS_SPAN;
						}
						else if (scaleAsString == "fit") {
							position = DWPOS_FIT;
						}
						else if (scaleAsString == "fill") {
							position = DWPOS_FILL;
						}
					}
				}

				wallpaper.SetWallpaper(fullPath, monitor, position);
			}
			else {
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
//   5. Go to Project > Add New Item to create new code files, or Project > Add
//   Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project
//   and select the .sln file
