/*
 * MkShared, (c) 2006 Christoph Giesselink (c.giesselink@gmx.de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <crtdbg.h>
#include "resource.h"

#define _KB(n)	(2*(n)*1024)

#define ARRAYSIZEOF(a) (sizeof(a) / sizeof(a[0]))

#define DEFAULTFILE "SHARED.BIN"

typedef enum
{
	STATE_UNKOWN,
	STATE_GOOD,
	STATE_FAIL
} CheckState;

static HBRUSH hBrushGreen;
static HBRUSH hBrushRed;

static CheckState eState = STATE_UNKOWN;

static VOID SetInformation(HWND hWnd,LPCTSTR strSize,LPCTSTR strNoOfPorts,LPCTSTR strPorts)
{
	SetDlgItemText(hWnd,IDC_FILE_SIZE,strSize);
	SetDlgItemText(hWnd,IDC_NO_OF_PORTS,strNoOfPorts);
	SetDlgItemText(hWnd,IDC_PORT_NO,strPorts);
	eState = STATE_UNKOWN;
	SetDlgItemText(hWnd,IDC_RESULT,_T(""));
	InvalidateRect(GetDlgItem(hWnd,IDC_RESULT),NULL,TRUE);
	return;
}

static BOOL WriteCardFile(LPCTSTR strFilename,INT nBlocks)
{
	HANDLE hFile = CreateFile(strFilename,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD  dwWritten;

		LPBYTE pbyBuffer = LocalAlloc(LPTR,_KB(1));

		while (nBlocks--) WriteFile(hFile, pbyBuffer, _KB(1), &dwWritten, NULL);

		LocalFree(pbyBuffer);

		CloseHandle(hFile);
		return FALSE;
	}
	return TRUE;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static WORD wSize;

	TCHAR   szFilename[MAX_PATH];
	HCURSOR hCursor;

	switch (iMsg)
	{
	case WM_INITDIALOG:
		// filename
		SetDlgItemText(hWnd,IDC_FILENAME,_T(DEFAULTFILE));

		// set to 32kb
		SendDlgItemMessage(hWnd,IDC_CARD32,BM_SETCHECK,1,0);
		PostMessage(hWnd,WM_COMMAND,IDC_CARD32,0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CARD32:
			wSize = 32;
			SetInformation(hWnd,_T("64kb"),_T("1"),_T("2"));
			return 0;
		case IDC_CARD128:
			wSize = 128;
			SetInformation(hWnd,_T("256kb"),_T("1"),_T("2"));
			return 0;
		case IDC_CARD256:
			wSize = 256;
			SetInformation(hWnd,_T("512kb"),_T("2"),_T("2,3"));
			return 0;
		case IDC_CARD512:
			wSize = 512;
			SetInformation(hWnd,_T("1mb"),_T("4"),_T("2 through 5"));
			return 0;
		case IDC_CARD1024:
			wSize = 1024;
			SetInformation(hWnd,_T("2mb"),_T("8"),_T("2 through 9"));
			return 0;
		case IDC_CARD2048:
			wSize = 2048;
			SetInformation(hWnd,_T("4mb"),_T("16"),_T("2 through 17"));
			return 0;
		case IDC_CARD4096:
			wSize = 4096;
			SetInformation(hWnd,_T("8mb"),_T("32"),_T("2 through 33"));
			return 0;
		case IDOK:
			GetDlgItemText(hWnd,IDC_FILENAME,szFilename,ARRAYSIZEOF(szFilename));
			hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));

			// create file
			if (WriteCardFile(szFilename,wSize))
			{
				eState = STATE_FAIL;
				SetDlgItemText(hWnd,IDC_RESULT,_T("Fail!"));
			}
			else
			{
				eState = STATE_GOOD;
				SetDlgItemText(hWnd,IDC_RESULT,_T("Done!"));
			}
			InvalidateRect(GetDlgItem(hWnd,IDC_RESULT),NULL,TRUE);
			SetCursor(hCursor);				// restore cursor
			return 0;
		}
		return 0;
	case WM_CTLCOLORSTATIC:
		if (GetDlgCtrlID((HWND) lParam) == IDC_RESULT)
		{
			switch (eState)
			{
			case STATE_GOOD:
				SetTextColor((HDC) wParam,(COLORREF) 0xFFFFFF);	// white
				SetBkMode((HDC) wParam,TRANSPARENT);
				return (LRESULT) hBrushGreen;
			case STATE_FAIL:
				SetTextColor((HDC) wParam,(COLORREF) 0xFFFFFF);	// white
				SetBkMode((HDC) wParam,TRANSPARENT);
				return (LRESULT) hBrushRed;
			}
		}
		break;								// default handler for all other windows
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd,iMsg,wParam,lParam);
}

INT WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR lpszCmdLine,INT nCmdShow)
{
	HWND     hWnd;
	MSG	     msg;
	WNDCLASS wc;
//	RECT     rc;
	HFONT    hFont;

	InitCommonControls();

	// create background brushes
	hBrushGreen = CreateSolidBrush(0x008000);
	hBrushRed   = CreateSolidBrush(0x0000FF);

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = DLGWINDOWEXTRA;
	wc.hInstance     = hInst;
	wc.hIcon         = LoadIcon(hInst,MAKEINTRESOURCE(IDI_MKSHARED));
	wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = _T("CMkShared");
	RegisterClass(&wc);

	hWnd = CreateDialog(hInst,MAKEINTRESOURCE(IDD_MAIN),0,WndProc);
	_ASSERT(hWnd);

#if 0
	// center window
	GetWindowRect(hWnd, &rc);
	SetWindowPos(hWnd, HWND_TOP,
	             ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
	             ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
	             0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
#endif

	// initialization
	hFont = CreateFont(20,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,
		               OUT_DEVICE_PRECIS,CLIP_DEFAULT_PRECIS,
					   PROOF_QUALITY,DEFAULT_PITCH|TMPF_TRUETYPE|FF_ROMAN,
					   _T("Times New Roman"));
	_ASSERT(hFont);
	SendDlgItemMessage(hWnd,IDC_STATIC_TITLE,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(TRUE,0));
	SendDlgItemMessage(hWnd,IDC_RESULT,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(TRUE,0));

	while(GetMessage(&msg,NULL,0,0))
	{
		if(!IsDialogMessage(hWnd,&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	DeleteObject(hFont);
	DeleteObject(hBrushGreen);
	DeleteObject(hBrushRed);

	return msg.wParam;
	UNREFERENCED_PARAMETER(hPrev);
	UNREFERENCED_PARAMETER(lpszCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
}
