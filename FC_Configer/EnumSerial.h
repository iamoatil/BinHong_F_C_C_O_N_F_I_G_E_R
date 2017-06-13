/************************************************************************* 
* Serial port enumeration routines
*
* The EnumSerialPort function will populate an array of SSerInfo structs,
* each of which contains information about one serial port present in
* the system. Note that this code must be linked with setupapi.lib,
* which is included with the Win32 SDK.
*
* by Zach Gorman <gormanjz@hotmail.com>
*
* Copyright (c) 2002 Archetype Auction Software, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following condition is
* met: Redistributions of source code must retain the above copyright
* notice, this condition and the following disclaimer.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL ARCHETYPE AUCTION SOFTWARE OR ITS
* AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************/
#pragma once
#include <afxtempl.h>
#include <assert.h>

BOOL UsingQueryDosDevice(CUIntArray& ports);
BOOL IsNumeric(LPCWSTR pszString, BOOL bIgnoreColon);
BOOL IsNumeric(LPCSTR pszString, BOOL bIgnoreColon);
///////////////////////// Classes /////////////////////////////////////////////

class CAutoHeapAlloc
{
public:
	//Constructors / Destructors
	CAutoHeapAlloc(HANDLE hHeap = GetProcessHeap(), DWORD dwHeapFreeFlags = 0) : m_pData(NULL),
		m_hHeap(hHeap),
		m_dwHeapFreeFlags(dwHeapFreeFlags)
	{
	}

	BOOL Allocate(SIZE_T dwBytes, DWORD dwFlags = 0)
	{
		//Validate our parameters
		assert(m_pData == NULL);

		m_pData = HeapAlloc(m_hHeap, dwFlags, dwBytes);
		return (m_pData != NULL);
	}

	~CAutoHeapAlloc()
	{
		if (m_pData != NULL)
		{
			HeapFree(m_hHeap, m_dwHeapFreeFlags, m_pData);
			m_pData = NULL;
		}
	}

	//Methods

	//Member variables
	LPVOID m_pData;
	HANDLE m_hHeap;
	DWORD  m_dwHeapFreeFlags;
};