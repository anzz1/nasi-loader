// global.h

#ifndef __GLOBAL_H
#define __GLOBAL_H

// Windows XP+
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501

#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if defined(_MSC_VER) && _MSC_VER < 1800
#pragma comment(linker, "/IGNORE:4104")
#endif

#define __noinline __declspec(noinline)

// whether proxyed functions should be loaded as they're called
#define DLL_PROXY_DELAY_LOAD

static char gSysDir[MAX_PATH+2];
static unsigned int gSysLen = 0;

#pragma function(memset)
void* __cdecl memset(void* dst, int val, size_t count) {
  void* start = dst;
  while (count--) {
    *(char*)dst = (char)val;
    dst = (char*)dst + 1;
  }
  return start;
}

__forceinline static void __strcpy(char* dst, const char* src) {
  while (*src != 0) *dst++ = *src++;
  *dst = 0;
}
__forceinline static char* __strrchr(const char* s, char c) {
  char *p = 0;
  while (*s != 0) {
    if (*s == c)
      p = (char*)s;
    s++;
  }
  return p;
}
__forceinline static char* __strncpy(char* dst, const char* src, unsigned int len) {
  unsigned int i;
  for (i = 0; i < len; i++) {
    if (src[i] == 0) break;
    dst[i] = src[i];
  }
  dst[i] = 0;
  return dst;
}
__forceinline static int __stricmp(const char* s1, const char* s2) {
  char c1, c2;
  do {
    if (*s1 == 0 && *s2 == 0) return 0;
    c1 = (*s1>64 && *s1<91) ? (*s1+32):*s1; // A-Z -> a-z
    c2 = (*s2>64 && *s2<91) ? (*s2+32):*s2; // A-Z -> a-z
    s1++; s2++;
  } while (c1 == c2);
  return (*s1 > *s2) ? 1 : -1;
}

#ifndef _WIN64
__forceinline static BOOL IsWow64(void) {
  void* fnIsWow64Process;
  BOOL bIsWow64 = FALSE;
  HMODULE k32 = GetModuleHandleA("kernel32.dll");
  if (!k32) return FALSE;
  fnIsWow64Process = GetProcAddress(k32, "IsWow64Process");
  return (fnIsWow64Process && ((BOOL (__stdcall *)(HANDLE,PBOOL)) (void*)(fnIsWow64Process))((HANDLE)-1, &bIsWow64) && bIsWow64);
}
__forceinline static UINT GetSysWow64Dir(LPSTR lpBuffer, UINT uSize) {
  void* fnGetSystemWow64DirectoryA;
  HMODULE k32 = GetModuleHandleA("kernel32.dll");
  if (!k32) return 0;
  fnGetSystemWow64DirectoryA = GetProcAddress(k32, "GetSystemWow64DirectoryA");
  if (!fnGetSystemWow64DirectoryA) return 0;
  return ((UINT (__stdcall *)(LPSTR,UINT)) (void*)(fnGetSystemWow64DirectoryA))(lpBuffer, uSize);
}
#endif // !_WIN64

__forceinline static void InitSysDir() {
#ifndef _WIN64
  if (IsWow64()) gSysLen = GetSysWow64Dir(gSysDir, MAX_PATH+1);
  else
#endif
  gSysLen = GetSystemDirectoryA(gSysDir, MAX_PATH+1);
  if (gSysLen && gSysLen < MAX_PATH) {
    while (gSysDir[gSysLen] == '\0')
      gSysLen--;
    if (gSysDir[gSysLen++] != '\\')
      gSysDir[gSysLen++] = '\\';

    gSysDir[gSysLen] = 0;
  } else {
    gSysLen = 0;
    gSysDir[0] = 0;
  }
}

__forceinline static HMODULE LoadSysMod(const char* modname) {
  char modpath[MAX_PATH+20];
  if (gSysLen) {
    __strcpy(modpath, gSysDir);
    __strcpy(modpath+gSysLen, modname);
    return LoadLibraryA(modpath);
  }

  return 0;
}

static void* GetSysProc(const char* modname, const char* funcname) {
  HMODULE hm;
  hm = LoadSysMod(modname);
  return (hm ? GetProcAddress(hm, funcname) : 0);
}

#endif // __GLOBAL_H
