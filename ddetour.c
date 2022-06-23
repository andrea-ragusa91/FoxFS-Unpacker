#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "ddetour.h"
#include "ADE32.h"

#define DETOUR_MAX_SRCH_OPLEN 64

int GetDetourLenAuto(char *pbFuncOrig, int minDetLen)
{
	int len = 0;
	char *pbCurOp = pbFuncOrig;

	while(len < minDetLen)
	{
		int i = oplen((uint8_t*)pbCurOp);

		if(i == 0 || i == -1)
			return 0;

		if(len > DETOUR_MAX_SRCH_OPLEN)
			return 0;

		len += i;
		pbCurOp += i;
	}

	return len;
}

void* detour(void* org, void* pnew, int size)
{
	DWORD dwProt = 0;

	if (size == DETOUR_LEN_AUTO)
		size = GetDetourLenAuto(org, 5);

    VirtualProtect(org, 5, PAGE_EXECUTE_READWRITE, &dwProt);

	unsigned long jmprel = (unsigned long)pnew - (unsigned long)org - 5;
	unsigned char* gateway = (unsigned char *)malloc(size+5);

    VirtualProtect(gateway, size+5, PAGE_EXECUTE_READWRITE, &dwProt);

	memcpy(gateway, org, size);
	gateway[size] = 0xE9;
	*(unsigned long*)(gateway+size+1) = (unsigned long)(((unsigned char *)org)+size) - (unsigned long)(gateway + size) - 5;
	unsigned char* p = (unsigned char*)org;
	p[0] = 0xE9;
	*(unsigned long*)(p+1) = jmprel;
	return gateway;
}
