#pragma once
#include <windows.h>
#include <winternl.h>
#include <TlHelp32.h>

#include <string>

typedef NTSTATUS(WINAPI* NtQueryInformationProcessPtr)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

std::wstring GetCommandLineFromProcess(DWORD processID);
std::pair<std::string, std::string> GetLeagueClientAPIInfo();
std::wstring GetLeagueCommandLine();