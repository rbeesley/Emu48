//
//	PCH.H (MinGW version)
//

#define _WIN32_IE 0x0200

#include <windows.h>
#include <tchar.h>
#include <shellapi.h>
#include <commctrl.h>
#include <shlobj.h>
#include <stdlib.h>
#include <malloc.h>
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <direct.h>
#include <conio.h>
// #include <crtdbg.h>                      // missing file

#if !defined IDC_HAND                       // Win2k specific definition
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

// normally defined in STDLIB.H
#if !defined __max
  #define __max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined __min
  #define __min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

// normally defined in missing CRTDBG.H
#if !defined _ASSERT
  #define _ASSERT(a)
#endif
#define _CrtSetDbgFlag(f)

#if !defined VERIFY
#if defined _DEBUG
#define VERIFY(f)			_ASSERT(f)
#else // _DEBUG
#define VERIFY(f)			((VOID)(f))
#endif // _DEBUG
#endif // VERIFY
