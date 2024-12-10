#include "LeagueHelper.h"

#include <stdexcept>
#include <iostream>
#include <regex>

std::wstring GetCommandLineFromProcess(DWORD processID) {
    HMODULE hNtdll = LoadLibrary(L"ntdll.dll");
    if (!hNtdll) throw std::runtime_error("Filed to load ntdll.dll");

    auto NtQueryInformationProcess = (NtQueryInformationProcessPtr)GetProcAddress(hNtdll, "NtQueryInformationProcess");
    if (!NtQueryInformationProcess) {
        FreeLibrary(hNtdll);
        throw std::runtime_error("Failed to find NtQueryInformationProcess");
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (!hProcess) throw std::runtime_error("Failed to open process");

    PROCESS_BASIC_INFORMATION pbi;
    ULONG returnLength;
    NTSTATUS status = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &returnLength);

    if (FAILED(status))
    {
        CloseHandle(hProcess);
        FreeLibrary(hNtdll);
        throw std::runtime_error("NtQueryInformationProcess failed");
    }

    PEB peb;
    if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb), nullptr)) {
        CloseHandle(hProcess);
        FreeLibrary(hNtdll);
        throw std::runtime_error("Failed to read PEB");
    }

    RTL_USER_PROCESS_PARAMETERS processParameters;
    if (!ReadProcessMemory(hProcess, peb.ProcessParameters, &processParameters, sizeof(processParameters), nullptr)) {
        CloseHandle(hProcess);
        FreeLibrary(hNtdll);
        throw std::runtime_error("failed to read process parameters");
    }


    WCHAR* cmdLineBuffer = new WCHAR[processParameters.CommandLine.Length / sizeof(WCHAR) + 1];
    if (!ReadProcessMemory(hProcess, processParameters.CommandLine.Buffer, cmdLineBuffer, processParameters.CommandLine.Length, nullptr)) {
        delete[] cmdLineBuffer;
        CloseHandle(hProcess);
        FreeLibrary(hNtdll);
        throw std::runtime_error("Failed to read command line");
    }

    cmdLineBuffer[processParameters.CommandLine.Length / sizeof(WCHAR)] = L'\0';
    std::wstring commandLine(cmdLineBuffer);
    delete[] cmdLineBuffer;

    CloseHandle(hProcess);
    FreeLibrary(hNtdll);

    return commandLine;
}


std::pair<std::string, std::string> GetLeagueClientAPIInfo()
{
    std::wregex portRegex(L"--app-port=(\\d+)");
    std::wregex tokenRegex(L"--remoting-auth-token=([^\\s\"]+)");

    std::wsmatch portMatch, tokenMatch;
    try {
        std::wstring cmdLineStr = GetLeagueCommandLine();

        std::string port, token;
        if (std::regex_search(cmdLineStr, portMatch, portRegex))
        {
            port = std::string(portMatch[1].first, portMatch[1].second);
        }
        if (std::regex_search(cmdLineStr, tokenMatch, tokenRegex))
        {
            token = std::string(tokenMatch[1].first, tokenMatch[1].second);
        }
        return { port, token };
    }
    catch (const std::exception err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return {};
    }
}

std::wstring GetLeagueCommandLine()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return L"";
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    std::wstring leagueProcessName = L"LeagueClientUx.exe";

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (std::wstring(pe.szExeFile) == leagueProcessName) {
                CloseHandle(hSnapshot);
                return GetCommandLineFromProcess(pe.th32ProcessID);
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return L"";
}