/*
 * cardcopy, (c) 2000 Christoph Giesselink (cgiess@swol.de)
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
#include <stdio.h>
#include <assert.h>
#include "types.h"

#define VERSION		"2.1"

#define FT_ERR		0						// illegal format
#define FT_NEW		1						// empty file
#define FT_SXGX		2						// Emu48 HP48SX/GX state file

#define _KB(n)		((n)*1024*2)			// KB in state file
#define HP48SIG		"Emu48 Document\xFE"	// HP48 state file signature


UINT CheckType(char *lpszFileName)
{
	BYTE   pbyFileSignature[16];
	HANDLE hFile;
	DWORD  FileSizeHigh,FileSizeLow;

	UINT   nType = FT_ERR;

	hFile = CreateFile(lpszFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FT_NEW;

	// check filesize
	FileSizeLow = GetFileSize(hFile,&FileSizeHigh);
	if (FileSizeHigh == 0 && (FileSizeLow == _KB(32) || FileSizeLow == _KB(128)))
		nType = FileSizeLow;				// return card size

	// Read and Compare signature
	ReadFile(hFile,pbyFileSignature,sizeof(pbyFileSignature),&FileSizeLow,NULL);
	if (FileSizeLow == sizeof(pbyFileSignature) && strcmp(pbyFileSignature,HP48SIG) == 0)
		nType = FT_SXGX;

	CloseHandle(hFile);
	return nType;
}

BOOL SeekData(HANDLE hFile,UINT *nPortSize)
{
	BYTE    byBuffer[16];
	CHIPSET	*pChipset;
	UINT    i;
	DWORD   lBytes;

	SetFilePointer(hFile,0,NULL,FILE_BEGIN);

	// read and check signature
	ReadFile(hFile,byBuffer,sizeof(byBuffer),&lBytes,NULL);
	if (lBytes != sizeof(HP48SIG) || strcmp(byBuffer,HP48SIG) != 0) return TRUE;

	// read KML file length
	ReadFile(hFile,&i,sizeof(i),&lBytes,NULL);
	if (lBytes != sizeof(i)) return TRUE;

	// skip KML file name
	SetFilePointer(hFile,i,NULL,FILE_CURRENT);

	// read CHIPSET structure length
	ReadFile(hFile,&i,sizeof(i),&lBytes,NULL);
	if (lBytes != sizeof(i)) return TRUE;

	// read CHIPSET structure
	if ((pChipset = LocalAlloc(LMEM_FIXED,i)) == NULL)
		return TRUE;

	ReadFile(hFile,pChipset,i,&lBytes,NULL);
	if (lBytes != i) { LocalFree(pChipset); return TRUE; }

	// skip port0
	SetFilePointer(hFile,_KB(pChipset->Port0Size),NULL,FILE_CURRENT);

	*nPortSize = _KB(pChipset->Port1Size);	// expected filesize

	LocalFree(pChipset);
	return FALSE;
}

BOOL CopyData(HANDLE hFileSource,HANDLE hFileDest,UINT nSize)
{
	BYTE  byBuffer[16];
	INT   i;
	DWORD lBytes;

	assert(nSize % sizeof(byBuffer) == 0);
	for (i = nSize / sizeof(byBuffer); i > 0; --i)
	{
		ReadFile(hFileSource,byBuffer,sizeof(byBuffer),&lBytes,NULL);
		if (lBytes != sizeof(byBuffer)) return TRUE;

		WriteFile(hFileDest,byBuffer,sizeof(byBuffer),&lBytes,NULL);
		if (lBytes != sizeof(byBuffer)) return TRUE;
	}
	return FALSE;
}

UINT main(int argc, char *argv[])
{
	HANDLE hFileSource,hFileDest;
	UINT   nSourceType,nDestType;
	UINT   nError = 0;

	printf("HP48 Port1 Import/Export Tool for Emu48 V" VERSION "\n");
	if (argc != 3)
	{
		printf("\nUsage:\n\t%s <SourceFile> <DestinationFile>\n\n", argv[0]);
		return 1;
	}

	// check source file type
	nSourceType = CheckType(argv[1]);
	if (nSourceType == FT_ERR || nSourceType == FT_NEW)
	{
		printf("Error: Illegal source file type\n");
		return 2;
	}

	// check destination file type
	nDestType = CheckType(argv[2]);
	if (nDestType == FT_ERR)
	{
		printf("Error: Illegal destination file type\n");
		return 3;
	}

	// open source file
	hFileSource = CreateFile(argv[1],GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
	if (hFileSource != INVALID_HANDLE_VALUE)
	{
		hFileDest = CreateFile(argv[2],GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_ALWAYS,0,NULL);
		if (hFileDest != INVALID_HANDLE_VALUE)
		{
			BOOL bFormatErr = FALSE;

			if (nSourceType == FT_SXGX) bFormatErr |= SeekData(hFileSource,&nSourceType);
			if (nDestType   == FT_SXGX) bFormatErr |= SeekData(hFileDest,&nDestType);

			if (!bFormatErr && (nSourceType == nDestType || nDestType == FT_NEW))
			{
				assert(nSourceType > FT_SXGX);
				CopyData(hFileSource,hFileDest,nSourceType);
				puts("Copy successful.");
			}
			else
			{
				printf("Error: Non matching file size or format\n");
				nError = 4;
			}

			CloseHandle(hFileDest);
		}
		else
		{
			printf("Error: Can't open destination file %s\n",argv[2]);
			nError = 5;
		}

		CloseHandle(hFileSource);
	}
	else
	{
		printf("Error: Can't open source file %s\n",argv[1]);
		nError = 6;
	}

	return nError;
}
