#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#define HP38G	0
#define HP48S	1
#define HP48G	2

LPBYTE pRom;
WORD   wCRC;
WORD   wType;

static WORD crc_table[16] =
{
	0x0000, 0x1081, 0x2102, 0x3183, 0x4204, 0x5285, 0x6306, 0x7387,
	0x8408, 0x9489, 0xA50A, 0xB58B, 0xC60C, 0xD68D, 0xE70E, 0xF78F
};
static __inline VOID CRC(BYTE nib)
{
	wCRC = (WORD)((wCRC>>4)^crc_table[(wCRC^nib)&0xf]);
}

BOOL CheckCRC()
{
	DWORD dwD0, dwD1;
	WORD  wRomCRC;
	UINT  i;
	DWORD dwBase = 0x00000;
	UINT  nPass = 0;
	UINT  nPasses = (wType != HP48S)?2:1;

again:

	wRomCRC = pRom[dwBase+0x7FFFC]
			|(pRom[dwBase+0x7FFFD]<<4)
			|(pRom[dwBase+0x7FFFE]<<8)
			|(pRom[dwBase+0x7FFFF]<<12);

	wCRC = 0x0000;
	dwD0 = dwBase + 0x00000;
	dwD1 = dwBase + 0x40000;
	do
	{
		for (i=0; i<16; i++) CRC(pRom[dwD0+i]);
		for (i=0; i<16; i++) CRC(pRom[dwD1+i]);
		dwD0 += 16;
		dwD1 += 16;
	} while (dwD0&0x3FFFF);

	if (wCRC==0xFFFF)
	{
		printf("CRC%i: %04X Ok\n", nPass, wRomCRC);
	}
	else
	{
		printf("CRC%i: %04X Failed (%04X)\n", nPass, wRomCRC, wCRC);
		return FALSE;
	}

	if (++nPass == nPasses) return TRUE;

	dwBase += 0x80000;
	goto again;

}

static BYTE Asc2Nib(char c)
{
	if (c<'0') return 0;
	if (c<='9') return c-'0';
	if (c<'A') return 0;
	if (c<='F') return c-'A'+10;
	if (c<'a') return 0;
	if (c<='f') return c-'a'+10;
	return 0;
}

static DWORD Asc2Nib5(LPSTR lpBuf)
{
	return (
		 ((DWORD)Asc2Nib(lpBuf[0])<<16)
		|((DWORD)Asc2Nib(lpBuf[1])<<12)
		|((DWORD)Asc2Nib(lpBuf[2])<<8)
		|((DWORD)Asc2Nib(lpBuf[3])<<4)
		|((DWORD)Asc2Nib(lpBuf[4])));
}

static BOOL IsHP(DWORD dwAddress)
{
	char cH = (pRom[dwAddress + 1] << 4) | pRom[dwAddress];
	char cP = (pRom[dwAddress + 3] << 4) | pRom[dwAddress + 2];
	return cH == 'H' && cP == 'P';
}

UINT main(int argc, char *argv[])
{
	HANDLE hFile;
	HANDLE hMap;
	HANDLE hOut;
	LPBYTE pIn;
	DWORD  dwSizeLo;
	BYTE   szVersion[16];
	DWORD  dwWritten;
	UINT  i,uLen;
	DWORD dwAddress;

	DWORD dwAddrOffset = 0x00000;

	BOOL   bFormatDetected = FALSE;

	BOOL   bUnpack = FALSE;
	BOOL   bSwap = FALSE;
	BOOL   bText = FALSE;
	BOOL   bDA19 = FALSE;

	if ((argc!=2)&&(argc!=3))
	{
		printf("Usage:\n\t%s <old-rom-dump> [<new-rom-dump>]\n", argv[0]);
		return 1;
	}
	pRom = LocalAlloc(LMEM_FIXED,512*1024*2);
	if (pRom == NULL)
	{
		printf("Memory Allocation Failed !");
		return 1;
	}

	hFile = CreateFile(argv[1],GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		LocalFree(pRom);
		printf("Cannot open file %s.\n", argv[1]);
		return 1;
	}
	dwSizeLo = GetFileSize(hFile, NULL);
	hMap  = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMap == NULL)
	{
		LocalFree(pRom);
		CloseHandle(hFile);
		puts("CreateFileMapping failed.");
		return 1;
	}
	pIn   = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (pIn == NULL)
	{
		LocalFree(pRom);
		CloseHandle(hMap);
		CloseHandle(hFile);
		puts("MapViewOfFile failed.\n");
		return 1;
	}

	for (i = 0; i < 2 && !bFormatDetected; ++i)
	{
		switch (pIn[0+dwAddrOffset])
		{
		case '0':
			if (pIn[1+dwAddrOffset]!='0') break;
			if (pIn[2+dwAddrOffset]!='0') break;
			if (pIn[3+dwAddrOffset]!='0') break;
			if (pIn[4+dwAddrOffset]!='0') break;
			if (pIn[5+dwAddrOffset]!=':') break;
			bText = TRUE;
			bFormatDetected = TRUE;
			break;
		case 0x23:
			bUnpack = TRUE;
			bSwap   = TRUE;
			bFormatDetected = TRUE;
			break;
		case 0x32:
			bUnpack = TRUE;
			bFormatDetected = TRUE;
			break;
		case 0x03:
			bSwap = TRUE;
		case 0x02:
			if (pIn[1+dwAddrOffset] == (bSwap ? 0x02 : 0x03))
			{
				bFormatDetected = TRUE;
				break;
			}
			bSwap = FALSE;
		default:
			dwAddrOffset = dwSizeLo / 2;
			bDA19 = TRUE;
			break;
		}
	}

	if (!bFormatDetected)
	{
		LocalFree(pRom);
		UnmapViewOfFile(pIn);
		CloseHandle(hMap);
		CloseHandle(hFile);
		printf("Stopped, unknown format.\n");
		return 1;
	}

	if (bUnpack) printf("Unpacking nibbles.\n");
	if (bSwap)   printf("Swapping nibbles.\n");
	if (bText)   printf("Reading text file.\n");
	if (bDA19)   printf("Swapping banks.\n");

	if (bText)
	{
		DWORD i = 0;
		while (i<dwSizeLo)
		{
			DWORD d = Asc2Nib5(pIn+i);
			i+=6;
			pRom[d+0x0] = Asc2Nib(pIn[i+0x0]);
			pRom[d+0x1] = Asc2Nib(pIn[i+0x1]);
			pRom[d+0x2] = Asc2Nib(pIn[i+0x2]);
			pRom[d+0x3] = Asc2Nib(pIn[i+0x3]);
			pRom[d+0x4] = Asc2Nib(pIn[i+0x4]);
			pRom[d+0x5] = Asc2Nib(pIn[i+0x5]);
			pRom[d+0x6] = Asc2Nib(pIn[i+0x6]);
			pRom[d+0x7] = Asc2Nib(pIn[i+0x7]);
			pRom[d+0x8] = Asc2Nib(pIn[i+0x8]);
			pRom[d+0x9] = Asc2Nib(pIn[i+0x9]);
			pRom[d+0xA] = Asc2Nib(pIn[i+0xA]);
			pRom[d+0xB] = Asc2Nib(pIn[i+0xB]);
			pRom[d+0xC] = Asc2Nib(pIn[i+0xC]);
			pRom[d+0xD] = Asc2Nib(pIn[i+0xD]);
			pRom[d+0xE] = Asc2Nib(pIn[i+0xE]);
			pRom[d+0xF] = Asc2Nib(pIn[i+0xF]);
			i+=16;
			while ((i<dwSizeLo)&&((pIn[i]==0x0D)||(pIn[i]==0x0A))) i++;
		}
	}
	else
	{
		if (bUnpack)
		{
			if (bSwap)
			{
				DWORD i;
				for (i=0; i<dwSizeLo; i++)
				{
					BYTE byC = pIn[(i+dwAddrOffset)&(dwSizeLo-1)];
					pRom[(i<<1)  ] = byC>>4;
					pRom[(i<<1)+1] = byC&0xF;
				}
			}
			else
			{
				DWORD i;
				for (i=0; i<dwSizeLo; i++)
				{
					BYTE byC = pIn[(i+dwAddrOffset)&(dwSizeLo-1)];
					pRom[(i<<1)  ] = byC&0xF;
					pRom[(i<<1)+1] = byC>>4;
				}
			}
		}
		else
		{
			if (bSwap)
			{
				DWORD i;
				for (i=0; i<dwSizeLo; i+=2)
				{
					BYTE a, b;
					a = pIn[(i+dwAddrOffset)&(dwSizeLo-1)];
					b = pIn[(i+1+dwAddrOffset)&(dwSizeLo-1)];
					pRom[i] = b;
					pRom[i+1] = a;
				}
			}
			else
			{
				if(bDA19)
				{
					assert(dwAddrOffset == dwSizeLo/2);
					CopyMemory(&pRom[0]           , &pIn[dwAddrOffset], dwAddrOffset);
					CopyMemory(&pRom[dwAddrOffset], &pIn[0]           , dwAddrOffset);
				}
				else
				{
					CopyMemory(pRom, pIn, dwSizeLo);
				}
			}
		}
	}

	UnmapViewOfFile(pIn);
	CloseHandle(hMap);
	CloseHandle(hFile);

	if (bText||bUnpack||bSwap||bDA19)
	{
		printf("File converted.\n\n");
	}

	do
	{
		// HP38G
		wType = HP38G;
		dwAddress = 0x7FFAF;
		uLen = 10;
		if (IsHP(dwAddress)) break;

		// HP48SX
		wType = HP48S;
		dwAddress = 0x7FFF0;
		uLen = 6;
		if (IsHP(dwAddress)) break;

		// HP48GX
		wType = HP48G;
		dwAddress = 0x7FFBF;
		uLen = 6;
		if (IsHP(dwAddress)) break;

		// unknown
		uLen = 0;
	}
	while (0);

	printf("ROM Model Detected : HP%c8%c\n",(wType == HP38G)?'3':'4',(wType != HP48S)?'G':'S');

	for (i=0; i<uLen; ++i)
	{
		szVersion[i] = pRom[dwAddress + (i<<1) + 1] << 4;
		szVersion[i] |= pRom[dwAddress + (i<<1)];
	}
	szVersion[i] = 0;

	printf("ROM Version : %s\n", szVersion);

	ZeroMemory(pRom+0x100, 0x40);			// clear IO register area

	if (CheckCRC())
	{
		printf("ROM CRC Test Passed.\n");
	}
	else
	{
		printf("ROM CRC Test FAILED !\n");
	}

	if (argc == 3)
	{
		hOut = CreateFile(argv[2],GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			LocalFree(pRom);
			printf("Cannot open file %s.\n", argv[2]);
			return 1;
		}
		WriteFile(hOut,pRom,(wType != HP48S)?(512*1024*2):(256*1024*2),&dwWritten,NULL);
		CloseHandle(hOut);
	}

	LocalFree(pRom);

	return 0;
}
