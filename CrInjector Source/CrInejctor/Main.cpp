#include "global.h"

DWORD GetGamePID() {
	HWND hwGame = FindWindowA(0, "Craftrise"); //Pencere Ad� Exe Ad� De�il!
	if (!hwGame) return 0;
	DWORD ret = 0;
	GetWindowThreadProcessId(hwGame, &ret);
	return ret;
}

std::string Lpcwstr2String(LPCWSTR lps) {
	int len = WideCharToMultiByte(CP_ACP, 0, lps, -1, NULL, 0, NULL, NULL);
	if (len <= 0) {
		return "";
	}
	else {
		char* dest = new char[len];
		WideCharToMultiByte(CP_ACP, 0, lps, -1, dest, len, NULL, NULL);
		dest[len - 1] = 0;
		std::string str(dest);
		delete[] dest;
		return str;
	}
}
std::string SelectDll() {
	OPENFILENAME ofn;
	char szFile[300];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	LPTSTR        lpstrCustomFilter;
	DWORD         nMaxCustFilter;
	ofn.nFilterIndex = 1;
	LPTSTR        lpstrFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"DLL File\0*.dll";
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	std::string path_image = "";
	if (GetOpenFileName(&ofn)) {
		path_image = Lpcwstr2String(ofn.lpstrFile);
		return path_image;
	}
	else {
		return "";
	}
}
void InjectDll(const char* path, DWORD pid) {
	HANDLE hGame = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);
	char dllpath[MAX_PATH];
	ZeroMemory(dllpath, sizeof(dllpath));
	strcpy_s(dllpath, path);
	LPVOID allocatedMem = VirtualAllocEx(hGame, NULL, sizeof(dllpath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hGame, allocatedMem, dllpath, sizeof(dllpath), NULL);
	HANDLE hThread = CreateRemoteThread(hGame, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocatedMem, 0, 0);
	WaitForSingleObject(hThread, INFINITE);
	VirtualFreeEx(hGame, allocatedMem, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}
int main()
{
	DWORD gamePID = GetGamePID();
	if (!gamePID) {
		MessageBoxA(0, "Oyun Bulunamad�", "L�tfen Oyunu Ba�lat�n!", 0);
		return 0;
	}
	HookBypass::SetGamepid(gamePID);
	std::string dllpath = SelectDll();
	if (dllpath == "") {
		MessageBoxA(0, "Dll Dosyas� Se�ilmedi", "L�tfen Bir Dll Dosyas� Se�in!", 0);
		return 0;
	}
	if (!HookBypass::Bypassrac_hook()) {
		MessageBoxA(0, "Oyuna Enjekte Edilemedi!", "Cant Bypass R.A.C Hook", 0);
		return 0;
	}
	InjectDll(dllpath.c_str(), gamePID);
	HookBypass::Restorerac_hook();
	MessageBoxA(0, "Enjekte Ba�ar�l�!", "CrInjectorc++", 0);
	return 0;
}
