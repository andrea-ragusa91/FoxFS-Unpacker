/*******************************************************************
FoxFS file dumper
Copyright (C) 2018 Andrea Ragusa

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include <Windows.h>
#include <cstdio>
#include <string>
#include <vector>
#include "main.h"
#include "ddetour.h"

#define GETFROMPACK_ADDR 0x00591200 //change this with CEterPackManager::GetFromPack address

//had to use __fastcall because visual studio doesn't let me use __thiscall
typedef bool(__fastcall *tGetFromPack)(char *bp, void *ebx, void *rMappedFile, const char * c_szFileName, LPCVOID * pData);
//unsigned int(__fastcall *FoxFS_SizeA)(unsigned int, const char*) = (unsigned int(__fastcall *)(unsigned int, const char *))0x6326F0;

tGetFromPack oGetFromPack;

void createDirectoryRecursively(const std::string &directory)
{
	static const std::string separators("\\/");

	// If the specified directory name doesn't exist, do our thing
	DWORD fileAttributes = ::GetFileAttributes(directory.c_str());
	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		// Recursively do it all again for the parent directory, if any
		std::size_t slashIndex = directory.find_last_of(separators);
		if (slashIndex != std::string::npos)
		{
			createDirectoryRecursively(directory.substr(0, slashIndex));
		}

		// Create the last directory on the path (the recursive calls will have taken
		// care of the parent directories by now)
		//if (directory.find('.') == std::string::npos)
		{
			BOOL result = ::CreateDirectory(directory.c_str(), nullptr);
			if (result == FALSE)
			{
				//throw std::runtime_error("Could not create directory");
			}
		}
	}
	else     // Specified directory name already exists as a file or directory
	{
		bool isDirectoryOrJunction =
			((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
			((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);

		if (!isDirectoryOrJunction)
		{
			//throw std::runtime_error("Could not create directory because a file with the same name exists");
		}

	}
}

bool __fastcall hGetFromPack(char *ecx, void *ebx, void *rMappedFile, const char *c_szFileName, LPCVOID * pData)
{
	std::string str;

	bool ret = oGetFromPack(ecx, NULL, rMappedFile, c_szFileName, pData);

	//unsigned int m_pFoxFS = *(unsigned int *)(ecx + 5240);
	unsigned int fsize = *((unsigned int *)rMappedFile + 71); //FoxFS_SizeA(m_pFoxFS, c_szFileName);

	if (c_szFileName[1] == ':')
		str = std::string(c_szFileName + 3);
	else
		str = std::string(c_szFileName);

	std::size_t slashIndex = str.find_last_of("\\/");
	if (slashIndex != std::string::npos)
	{
		createDirectoryRecursively(str.substr(0, slashIndex));
	}
	
	FILE *fOut;
	errno_t fret = fopen_s(&fOut, str.c_str(), "wb");
	if (fret != 0)
	{
		MessageBoxA(0, str.c_str(), "Dump error", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		fwrite(*pData, 1, fsize, fOut);
		fclose(fOut);
	}
	
	return ret;
}

void DLL_EXPORT SomeFunction(const LPCSTR sometext)
{
	MessageBoxA(0, sometext, "DLL Message", MB_OK | MB_ICONINFORMATION);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		oGetFromPack = (tGetFromPack)detour((void *)GETFROMPACK_ADDR, (void *)hGetFromPack, DETOUR_LEN_AUTO);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

