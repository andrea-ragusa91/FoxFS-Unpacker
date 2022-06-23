#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */


#define DLL_EXPORT __declspec(dllexport)


#ifdef __cplusplus
extern "C"
{
#endif

	void DLL_EXPORT SomeFunction(const LPCSTR sometext);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__