#ifndef UNICODE
#define UNICODE
#endif 

#include "LeagueWebSocket.h"

#include <iostream>

#include <windows.h>
#include <shobjidl.h>
#include <atlbase.h>

#include <string>

#include "MainWindow.h"
#include "LeagueHelper.h"

#define CHECK_HR(hr) if(FAILED(hr)) { return 1;}

void ToggleTaskbarVisibility();
void AttachToConsole();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    AttachToConsole();

    auto clientDetails = GetLeagueClientAPIInfo();

    LeagueWebSocket leagueClient(clientDetails);
    leagueClient.run();

    MainWindow win;
    if (!win.Create(L"Circle", WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }

    ShowWindow(win.Window(), nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void ToggleTaskbarVisibility() {
    static bool isHidden = true;
    isHidden = !isHidden;

    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);

    if (!hTaskbar) {
        MessageBox(NULL, L"Taskbar or Start Button not found!", L"Error", MB_ICONERROR);
        return;
    }

    if (isHidden)
        ShowWindow(hTaskbar, SW_HIDE);
    else
        ShowWindow(hTaskbar, SW_SHOW);
}
void AttachToConsole() {
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONIN$", "r", stdin);
        freopen_s(&fp, "CONOUT$", "w", stderr);

        std::ios::sync_with_stdio(true);

        std::wcout.clear();
        std::wcin.clear();
        std::wcerr.clear();
    }
    else {
        std::cerr << "Failed to attach to parent console." << std::endl;
    }
}