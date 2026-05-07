#pragma comment(lib, "Wtsapi32.lib")
#include<iostream>
#include<windows.h>
#include<string>
#include<vector>
#include <tlhelp32.h>
#include<fstream>
#include <cwchar>
#include <WtsApi32.h>
#include<algorithm>
#include<cctype>
void SetProcessState(DWORD targetPID, bool freeze);
DWORD GetPIDByName(std::wstring processName);
DWORD GetPIDByHWND(HWND win);
bool IsProtectedProcess(DWORD PID);
void GetFileIn(std::vector<std::wstring>& GreenList, std::vector<std::wstring>& WhiteList);
int main() {
	std::vector<std::wstring> WhiteList;
	std::vector<std::wstring> GreenList;
	GetFileIn(GreenList, WhiteList);
	HANDLE server = 0;
	PWTS_PROCESS_INFO Info = nullptr;
	DWORD count = 0;
	HWND activewin = nullptr;
	DWORD activeWinPID = 0;
	bool ispress = false;
	bool ispressf = false;
	bool countofwork = false;
	bool isGrListWork = false;
	bool isWL = false;
	bool countofGLwork = false;
	std::wcout << L"Hold F8 to start...\n";
	while (true) {
		if (GetAsyncKeyState(VK_F8) & 0x8000) {
			ispress = true;
			ispressf = false;
			isGrListWork = false;
			countofGLwork = false;
			std::wcout << L"Program had started\n";
			std::wcout << L"Hold F12 to stop...\n"<<L"Hold F9 to unfreeze GreenList\n";
			Sleep(1000);
		}
		if (GetAsyncKeyState(VK_F9) & 0x8000 && isGrListWork == false && ispress == true) {
		
			std::wcout << L"GreenList had freezed\n";
			std::wcout << L"Hold F9 to freeze it...\n";
			isGrListWork = true;
			Sleep(1000);
		}
		else if (GetAsyncKeyState(VK_F9) & 0x8000 && isGrListWork == true && ispress == true) {

			std::wcout << L"GreenList had unfreezed\n";
			std::wcout << L"Hold F9 to unfreeze it...\n";
			isGrListWork = false;
			Sleep(1000);
		}
		if (GetAsyncKeyState(VK_F12) & 0x8000) {
			ispressf = true;
			ispress = false;
			isGrListWork = true;
			std::wcout << L"program had stoped...\nHold F8 to start...\n";
			Sleep(1000);
		}
		if (ispress == true && ispressf == false && isGrListWork == false && countofGLwork == false) {
			if (WTSEnumerateProcesses(server,
				0,
				1,
				&Info,
				&count)
				) {
				for (unsigned int i = 0; unsigned int(count) > i; i++) {
					for (unsigned short b = 0; GreenList.size() > b; b++) {
						if (GreenList[b] == Info[i].pProcessName && Info[i].ProcessId != GetCurrentProcessId() && IsProtectedProcess(Info[i].ProcessId) == false) {
							SetProcessState(Info[i].ProcessId, isGrListWork);
						}
					}
				}
				countofGLwork = true;
				WTSFreeMemory(Info);
				Sleep(100);
			}
		}
		else if (ispress == true && ispressf == false && isGrListWork == true && countofGLwork == true) {
			if (WTSEnumerateProcesses(server,
				0,
				1,
				&Info,
				&count)
				) {
				for (unsigned int i = 0; unsigned int(count) > i; i++) {
					for (unsigned short b = 0; GreenList.size() > b; b++) {
						if (GreenList[b] == Info[i].pProcessName ) {
							SetProcessState(Info[i].ProcessId, isGrListWork);
						}
					}
				}
				countofGLwork = false;
				WTSFreeMemory(Info);
				Sleep(100);
			}
		}
		if (ispress == true && ispressf == false && countofwork == false) {
			countofwork = true;
			if (WTSEnumerateProcesses(
				server,
				0,
				1,
				&Info,
				&count)) {
				activewin = GetForegroundWindow();
				activeWinPID = GetPIDByHWND(activewin);
				for (unsigned int i = 0;unsigned int(count) > i; i++) {
					if (Info[i].SessionId == 1 && activeWinPID != Info[i].ProcessId && Info[i].ProcessId != GetCurrentProcessId()) {
						for (unsigned short b = 0; WhiteList.size() > b; b++) {
							if (WhiteList[b] == Info[i].pProcessName) {
								isWL = true;
							}
						} 
						if (isWL == false && IsProtectedProcess(Info[i].ProcessId) == false) { 
							SetProcessState(Info[i].ProcessId, true); }
					}
					isWL = false;
				}
				WTSFreeMemory(Info);
				Sleep(100);
			}
		}
		else if (ispress == false && ispressf == true && countofwork == true) {
			countofwork = false;
			if (WTSEnumerateProcesses(
				server,
				0,
				1,
				&Info,
				&count)) {
				activewin = GetForegroundWindow();
				activeWinPID = GetPIDByHWND(activewin);
				
				for (unsigned int i = 0; unsigned int(count) > i; i++) {
					if (IsProtectedProcess(Info[i].ProcessId) == false) { SetProcessState(Info[i].ProcessId, false); }
				}
				WTSFreeMemory(Info);
				}
			Sleep(100);
			}
				Sleep(50);
		}
	CloseHandle(server);
}
void SetProcessState(DWORD targetPID, bool freeze) {
	HANDLE OM;
	if (freeze == true) {
		OM = OpenProcess(PROCESS_SET_QUOTA | PROCESS_SET_INFORMATION, false, targetPID);
		if (OM != 0) {
			SetProcessWorkingSetSize(OM, (SIZE_T)-1, (SIZE_T)-1);
			CloseHandle(OM);
		}
	}
	HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	THREADENTRY32 te32;
	te32.dwSize = sizeof(THREADENTRY32);
	if (Thread32First(Snap, &te32)) {
		do {
			if (te32.th32OwnerProcessID == targetPID) {
				HANDLE hThreat = OpenThread(THREAD_SUSPEND_RESUME, false, te32.th32ThreadID);
				if (hThreat != NULL) {
					if (freeze == true) {
						SuspendThread(hThreat);
					}
					else {
						while ((int)ResumeThread(hThreat) > 1) {}

					}CloseHandle(hThreat);
				}
			}
		} while (Thread32Next(Snap, &te32));
		{ CloseHandle(Snap); }
	}
}
DWORD GetPIDByHWND(HWND win) {
	DWORD winPID = 0;
	GetWindowThreadProcessId(win, &winPID);
	return winPID;

}
void GetFileIn(std::vector<std::wstring>& GreenList, std::vector<std::wstring>& WhiteList) {
	std::vector<std::wstring> Files;
	std::wstring line;
	std::wifstream file3(L"FileLazUNV.txt");
	if (file3.is_open()) {
		while (std::getline(file3, line)) {
			while (!line.empty() && (line.back() == L'\r' || line.back() == L' ')) {
				line.pop_back();
			}
			if (!line.empty() && line[0] == 0xFEFF) {
				line.erase(0, 1);
			}

			if (line.length() > 0) {
				Files.push_back(line);
			}
		}
	}
	if (Files.size() < 2) {
		std::wcout << L"Unknown first file name(put txt file to folder of Lazarus)\n";
		return;
	}
	file3.close();
	std::wifstream fileGL(Files[0]);
	if (fileGL.is_open()) {
		while (std::getline(fileGL, line)) {
			while (!line.empty() && (line.back() == L'\r' || line.back() == L' ')) {
				line.pop_back();
			}
			if (!line.empty() && line[0] == 0xFEFF) {
				line.erase(0, 1);
			}

			if (line.length() > 0) {
				GreenList.push_back(line);
			}
		}
	}
	else { std::wcout << L"Unknown BL file name(put txt file to folder of Lazarus)\n"; return; }
	fileGL.close();
	std::wifstream fileWL(Files[1]);
	if (fileWL.is_open()) {
		while (std::getline(fileWL, line)) {
			while (!line.empty() && (line.back() == L'\r' || line.back() == L' ')) {
				line.pop_back();
			}
			if (!line.empty() && line[0] == 0xFEFF) {
				line.erase(0, 1);
			}

			if (line.length() > 0) {
				WhiteList.push_back(line);
			}
		}
	}
	else {
		std::wcout << L"Unknown GL file name (put txt file to folder of Lazarus)\n";
		return;
	}
	fileWL.close();
}
DWORD GetPIDByName(std::wstring processName) {
	HANDLE process1 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pr32;
	pr32.dwSize = sizeof(PROCESSENTRY32);
	DWORD nPID = 0;
	if (Process32First(process1, &pr32)) {
		do {
			if (pr32.szExeFile == processName) {
				nPID = pr32.th32ProcessID;
				break;
			}
		} while (Process32Next(process1, &pr32));
	}
	CloseHandle(process1);
	return nPID;
}
bool IsProtectedProcess(DWORD PID) {
	HANDLE Process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, PID);
	if (Process == 0) {  return true; }
	wchar_t path[MAX_PATH];
	DWORD size = MAX_PATH;
	if(QueryFullProcessImageNameW(Process, 0, path, &size)){
		std::wstring fullPath(path);
		std::transform(fullPath.begin(), fullPath.end(), fullPath.begin(), ::towlower);
		if (fullPath.find(L"\\windows\\") != std::wstring::npos 
			|| fullPath.find(L"\\recovery\\") != std::wstring::npos 
			|| fullPath.find(L"\\sys") != std::wstring::npos 
			|| fullPath.find(L"logitech") != std::wstring::npos 
			|| fullPath.find(L"razer") != std::wstring::npos 
			|| fullPath.find(L"corsair") != std::wstring::npos 
			|| fullPath.find(L"steelseries") != std::wstring::npos 
			|| fullPath.find(L"amd") != std::wstring::npos 
			|| fullPath.find(L"nvidia") != std::wstring::npos 
			|| fullPath.find(L"intel") != std::wstring::npos
			|| fullPath.find(L"lghub") != std::wstring::npos
			) {
			CloseHandle(Process); return true;
		}
		else { CloseHandle(Process); return false; }
	}
	CloseHandle(Process);
	return true;
}