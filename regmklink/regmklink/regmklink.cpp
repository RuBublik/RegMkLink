// Based on Pavel  Yosifovitch's blog -
// https://scorpiosoftware.net/2020/07/17/creating-registry-links/

#include <windows.h>
#include <iostream>
#include <unordered_map>
#include "regmklink.h"

extern "C" int NTAPI NtDeleteKey(HKEY);

#pragma comment(lib, "ntdll")


static const std::unordered_map<std::wstring, HKEY> hiveMap = {
	{L"HKEY_CLASSES_ROOT", HKEY_CLASSES_ROOT},
	{L"HKEY_CURRENT_USER", HKEY_CURRENT_USER},
	{L"HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE},
	{L"HKEY_USERS", HKEY_USERS},
	{L"HKEY_CURRENT_CONFIG", HKEY_CURRENT_CONFIG},
	{L"HKEY_PERFORMANCE_DATA", HKEY_PERFORMANCE_DATA},
	{L"HKEY_DYN_DATA", HKEY_DYN_DATA} // Deprecated
};

HKEY GetRegistryHive(const std::wstring& wstrHive) {
	auto it = hiveMap.find(wstrHive);
	if (it != hiveMap.end()) {
		return it->second; // Return the stored HKEY directly
	}
	return nullptr; // Return nullptr for invalid input
}

int wmain(int argc, wchar_t* argv[])
{
	LONG status;
	HKEY hive, hKey = NULL;
	WCHAR* link;
	WCHAR* target;
	BOOL isDelete;

	if (argc != 4) {
		std::wcerr << L"Usage:	regmklink.exe <Hive> <Link> [<Target>|-d]" << std::endl;
		std::wcerr << L"NOTE :	<Link> requires native registry path. Use this tool: 'https://github.com/zodiacon/AllTools/blob/master/TotalReg.exe'" << std::endl;
		goto Leave;
	}

	hive = GetRegistryHive(argv[1]);
	if (NULL == hive)
	{
		std::wcerr << L"[-] Error: invalid hive." << std::endl;
		goto Leave;
	}
	link = argv[2];
	target = argv[3];
	isDelete = (wcscmp(target, L"-d") == 0);

	if (isDelete)
	{
		std::cout << "[*] Deleting key." << std::endl;
		status = RegOpenKeyEx(
			hive,
			link,
			REG_OPTION_OPEN_LINK,
			DELETE,
			&hKey
		);
		if (status != ERROR_SUCCESS)
		{
			if (status == ERROR_FILE_NOT_FOUND)
			{
				std::cout << "[-] Key not found." << std::endl;
			}
			else
			{
				std::cout << "[-] Error in RegOpenKeyEx() - err: " << status << std::endl;
			}
			goto Leave;
		}
		status = NtDeleteKey(hKey);
		if (status != ERROR_SUCCESS)
		{
			std::cout << "[-] Error in NtDeleteKey() - err: " << GetLastError() << std::endl;
			goto Leave;
		}
	}
	else
	{
		// MSDN: The key that the RegCreateKeyEx function creates has no values.
		std::cout << "[*] Creating key." << std::endl;
		status = RegCreateKeyEx(
			hive,
			link,
			0,
			nullptr,
			REG_OPTION_CREATE_LINK,
			KEY_WRITE,
			nullptr,
			&hKey,
			nullptr
		);
		if (status != ERROR_SUCCESS)
		{
			if (status == ERROR_ALREADY_EXISTS)
			{
				std::cout << "[-] Key already exists." << std::endl;
			}
			else
			{
				std::cout << "[-] Error in RegCreateKeyEx() - err: " << status << std::endl;
			}
			goto Leave;
		}

		// MSDN: "The target path must be an absolute registry path."
		// use this tool to get native registry path: "https://github.com/zodiacon/AllTools/blob/master/TotalReg.exe"
		std::cout << "[*] Setting SymbolicLinkValue." << std::endl;
		status = RegSetValueEx(
			hKey,
			L"SymbolicLinkValue",
			0,
			REG_LINK,
			(const BYTE*)target,
			wcslen(target) * sizeof(WCHAR)
		);
		std::cout << status << std::endl;
		if (status != ERROR_SUCCESS)
		{
			std::cout << "[-] Error in RegSetValueEx() - err: " << GetLastError() << std::endl;
			goto Leave;
		}
	}

	std::cout << "[*] Done." << std::endl;

Leave:
	if (NULL != hKey)
	{
		RegCloseKey(hKey);
	}
	return 0;
}