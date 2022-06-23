//Detour function for unix, compatible with ADE32 disasm library for automatic length

#define DETOUR_LEN_AUTO 0 // Finds the detour length automatically

#ifndef DDETOUR_H
#define DDETOUR_H

#ifdef __cplusplus
extern "C"
{
#endif
void* detour(void* org, void* pnew, int size);
#ifdef __cplusplus
}
#endif

#endif
