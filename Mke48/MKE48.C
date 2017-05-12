/*
 * T48G, (c) 2000 Christoph Giesselink (cgiess@swol.de)
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
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "types.h"

#define VERSION		"1.0"

#define _KB(n)		(n*1024*2)				// KB emulator block

#define HP48SIG		"Emu48 Document\xFE"	// HP49 state file signature

VOID MakeTemplate(FILE *hFile,BYTE type,DWORD Port0Size,DWORD Port1Size)
{
	CHIPSET Chipset;
	DWORD   dwBytesWritten;
	UINT    nVar;
	BYTE	byZ;

	// file signature
	WriteFile(hFile,HP48SIG,sizeof(HP48SIG),&dwBytesWritten,NULL);
	assert(dwBytesWritten == sizeof(HP48SIG));

	// KML filename length
	nVar = 0;								// no name
	WriteFile(hFile,&nVar,sizeof(nVar),&dwBytesWritten,NULL);
	assert(dwBytesWritten == sizeof(nVar));

	// KML filename

	// Chipset Size
	nVar = sizeof(Chipset);					// length, no name
	WriteFile(hFile,&nVar,sizeof(nVar),&dwBytesWritten,NULL);
	assert(dwBytesWritten == sizeof(nVar));

	// Chipset
	ZeroMemory(&Chipset,sizeof(Chipset));
	Chipset.type = type;
	Chipset.Port0Size = Port0Size;
	Chipset.Port1Size = Port1Size;
	Chipset.Port2Size = 0;
	Chipset.cards_status = 0x0;
	
	WriteFile(hFile,&Chipset,sizeof(Chipset),&dwBytesWritten,NULL);
	assert(dwBytesWritten == sizeof(Chipset));

	byZ = 0;								// fill with zero nibble

	// write port0 memory content
	for (nVar = 0; nVar < _KB(Chipset.Port0Size); ++nVar)
	{
		WriteFile(hFile,&byZ,1,&dwBytesWritten,NULL);
		assert(dwBytesWritten == 1);
	}

	// write port1 memory content
	for (nVar = 0; nVar < _KB(Chipset.Port1Size); ++nVar)
	{
		WriteFile(hFile,&byZ,1,&dwBytesWritten,NULL);
		assert(dwBytesWritten == 1);
	}
	return;
}

UINT main(int argc, char *argv[])
{
	HANDLE hFile;

	BYTE   type;
	DWORD  Port0Size;
	DWORD  Port1Size;

	printf("HP48 State File Template for Emu48 V" VERSION "\n");
	if (argc != 5 || (*argv[2] != 'S' && *argv[2] != 'G'))
	{
		printf("\nUsage:\n\t%s <E48-File> <Model[S|G]> <Port0-Size> <Port1-Size>\n\n", argv[0]);
		return 1;
	}

	type = *argv[2];
	Port0Size = atoi(argv[3]);
	Port1Size = atoi(argv[4]);

	hFile = CreateFile(argv[1],GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		// write template
		MakeTemplate(hFile,type,Port0Size,Port1Size);
		puts("Generation successful.");
		CloseHandle(hFile);
	}
	else
	{
		printf("Cannot open file %s.\n", argv[1]);
		return TRUE;
	}
	return FALSE;
}
