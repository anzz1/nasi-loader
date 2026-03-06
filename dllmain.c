// dllmain.c

/*
 * Nano ASI Loader
 * https://github.com/anzz1/nasi-loader
 *
 */

#include "include/global.h"
#include "include/shared.h"
#include "include/version_dll.h"
#include "include/dinput_dll.h"
#include "include/dinput8_dll.h"
#include "include/dsound_dll.h"
#include "include/winmm_dll.h"

static volatile int initialized = 0;

static void load_plugins(void) {
  WIN32_FIND_DATAA fd;
  HANDLE hFind = FindFirstFileA(".\\*.asi", &fd);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        LoadLibraryA(fd.cFileName);
      }
    } while (FindNextFileA(hFind, &fd));
    FindClose(hFind);
  }
}

int __stdcall DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpReserved) {
  if (dwReason == DLL_PROCESS_ATTACH && !initialized) {
    HMODULE hm = 0;
    char* p = 0;
    char s[512];

    initialized = 1;
    DisableThreadLibraryCalls(hInstDLL);

    // Pin this module to memory
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCSTR)&p_DirectInput8Create, &hm);

    // Load system directory to memory
    InitSysDir();

    // Proxy DLL feature
    if (GetModuleFileNameA(hm, s, 511)) {
      s[511] = 0;
      p = __strrchr(s, '\\');
      if (p) {
        p++;
        if (!__stricmp(p, sDInput)) // dinput.dll
          dinput_hook();
        else if (!__stricmp(p, sDInput8)) // dinput8.dll
          dinput8_hook();
        else if (!__stricmp(p, sDSound)) // dsound.dll
          dsound_hook();
        else if (!__stricmp(p, sVersion)) // version.dll
          version_hook();
        else if (!__stricmp(p, sWinmm)) // winmm.dll
          winmm_hook();
      }
    }

    // Load *.asi
    load_plugins();
  }

  return TRUE;
}
