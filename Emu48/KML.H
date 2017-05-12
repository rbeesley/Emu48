/*
 *   kml.h
 *
 *   This file is part of Emu48
 *
 *   Copyright (C) 1995 Sebastien Carlier
 *
 */

#define LEX_BLOCK   0
#define LEX_COMMAND 1
#define LEX_PARAM   2

typedef enum eTokenId
{
	TOK_NONE, //0
	TOK_ANNUNCIATOR, //1
	TOK_BACKGROUND, //2
	TOK_IFPRESSED, //3
	TOK_RESETFLAG, //4
	TOK_SCANCODE, //5
	TOK_HARDWARE, //6
	TOK_MENUITEM, //7
	TOK_SYSITEM, //8
	TOK_INTEGER, //9
	TOK_SETFLAG, //10
	TOK_RELEASE, //11
	TOK_VIRTUAL, //12
	TOK_INCLUDE, //13
	TOK_NOTFLAG, //14
	TOK_STRING, //15
	TOK_GLOBAL, //16
	TOK_AUTHOR, //17
	TOK_BITMAP, //18
	TOK_OFFSET, //19
	TOK_BUTTON, //20
	TOK_IFFLAG, //21
	TOK_ONDOWN, //22
	TOK_NOHOLD, //23
	TOK_LOCALE, //24
	TOK_TOPBAR, //25
	TOK_MENUBAR, //26
	TOK_TITLE, //27
	TOK_OUTIN, //28
	TOK_PATCH, //29
	TOK_PRINT, //30
	TOK_DEBUG, //31
	TOK_COLOR, //32
	TOK_MODEL, //33
	TOK_CLASS, //34
	TOK_PRESS, //35
	TOK_IFMEM, //36
	TOK_TYPE, //37
	TOK_SIZE, //38
	TOK_DOWN, //39
	TOK_ZOOM, //40
	TOK_ELSE, //41
	TOK_ONUP, //42
	TOK_EOL, //43
	TOK_MAP, //44
	TOK_ROM, //45
	TOK_VGA, //46
	TOK_LCD, //47
	TOK_END //48
} TokenId;

#define TYPE_NONE    00
#define TYPE_INTEGER 01
#define TYPE_STRING  02

typedef struct KmlToken
{
	TokenId eId;
	DWORD   nParams;
	DWORD   nLen;
	LPCTSTR szName;
} KmlToken;

typedef struct KmlLine
{
	struct KmlLine* pNext;
	TokenId eCommand;
	DWORD_PTR nParam[6];
} KmlLine;

typedef struct KmlBlock
{
	TokenId eType;
	DWORD nId;
	struct KmlLine*  pFirstLine;
	struct KmlBlock* pNext;
} KmlBlock;

#define BUTTON_NOHOLD  0x0001
#define BUTTON_VIRTUAL 0x0002
typedef struct KmlButton
{
	UINT nId;
	BOOL bDown;
	UINT nType;
	DWORD dwFlags;
	UINT nOx, nOy;
	UINT nDx, nDy;
	UINT nCx, nCy;
	UINT nOut, nIn;
	KmlLine* pOnDown;
	KmlLine* pOnUp;
} KmlButton;

typedef struct KmlAnnunciator
{
	UINT nOx, nOy;
	UINT nDx, nDy;
	UINT nCx, nCy;
} KmlAnnunciator;

extern KmlBlock* pKml;
extern BOOL DisplayChooseKml(CHAR cType);
extern VOID FreeBlocks(KmlBlock* pBlock);
extern VOID DrawAnnunciator(UINT nId, BOOL bOn);
extern VOID ReloadButtons(BYTE *Keyboard_Row, UINT nSize);
extern VOID RefreshButtons(RECT *rc);
extern BOOL MouseIsButton(DWORD x, DWORD y);
extern VOID MouseButtonDownAt(UINT nFlags, DWORD x, DWORD y);
extern VOID MouseButtonUpAt(UINT nFlags, DWORD x, DWORD y);
extern VOID MouseMovesTo(UINT nFlags, DWORD x, DWORD y);
extern VOID RunKey(BYTE nId, BOOL bPressed);
extern VOID PlayKey(UINT nOut, UINT nIn, BOOL bPressed);
extern BOOL InitKML(LPCTSTR szFilename, BOOL bNoLog);
extern VOID KillKML(VOID);
