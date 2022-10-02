/////////////////////////////////////////////////////////////////////////////
// Name:        my_dll.h
// Purpose:     Sample showing how to use wx from a DLL
// Author:      Vaclav Slavik
// Created:     2009-12-03
// Copyright:   (c) 2009 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _MY_DLL_H_
#define _MY_DLL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MY_DLL_BUILDING
    #define MY_DLL_DECL __declspec(dllexport)
#else
    #define MY_DLL_DECL __declspec(dllimport)
#endif

// launch wx UI from some application that may or may not be written in wx
MY_DLL_DECL void __stdcall api_object_init(const void *title,unsigned long);

// run this to shutdown running threads etc.
MY_DLL_DECL void __stdcall api_object_uninit(void);


#ifdef __cplusplus
}
#endif

#endif // _MY_DLL_H_
