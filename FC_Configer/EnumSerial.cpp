#include <stdafx.h>
#include "EnumSerial.h"



BOOL IsNumeric(LPCWSTR pszString, BOOL bIgnoreColon)
{
	size_t nLen = wcslen(pszString);
	if (nLen == 0)
		return FALSE;

	//What will be the return value from this function (assume the best)
	BOOL bNumeric = TRUE;

	for (size_t i=0; i<nLen && bNumeric; i++)
	{
		bNumeric = (iswdigit(pszString[i]) != 0);
		if (bIgnoreColon && (pszString[i] == L':'))
			bNumeric = TRUE;
	}

	return bNumeric;
}

BOOL IsNumeric(LPCSTR pszString, BOOL bIgnoreColon)
{
	size_t nLen = strlen(pszString);
	if (nLen == 0)
		return FALSE;

	//What will be the return value from this function (assume the best)
	BOOL bNumeric = TRUE;

	for (size_t i=0; i<nLen && bNumeric; i++)
	{
		bNumeric = (isdigit(static_cast<int>(pszString[i])) != 0);
		if (bIgnoreColon && (pszString[i] == ':'))
			bNumeric = TRUE;
	}

	return bNumeric;
}

BOOL UsingQueryDosDevice(CUIntArray& ports)
{
	//What will be the return value from this function (assume the worst)
	BOOL bSuccess = FALSE;

	//Make sure we clear out any elements which may already be in the array
	ports.RemoveAll();


	//Determine what OS we are running on
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bGetVer = GetVersionEx(&osvi);

	//On NT use the QueryDosDevice API
	if (bGetVer && (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT))
	{
		//Use QueryDosDevice to look for all devices of the form COMx. Since QueryDosDevice does
		//not consitently report the required size of buffer, lets start with a reasonable buffer size
		//of 4096 characters and go from there
		int nChars = 4096;
		BOOL bWantStop = FALSE;
		while (nChars && !bWantStop)
		{
			CAutoHeapAlloc devices;
			if (devices.Allocate(nChars * sizeof(TCHAR)))
			{
				LPTSTR pszDevices = static_cast<LPTSTR>(devices.m_pData);
				DWORD dwChars = QueryDosDevice(NULL, pszDevices, nChars);
				if (dwChars == 0)
				{
					DWORD dwError = GetLastError();
					if (dwError == ERROR_INSUFFICIENT_BUFFER)
					{
						//Expand the buffer and  loop around again
						nChars *= 2;
					}
					else
						bWantStop = TRUE;
				}
				else
				{
					bSuccess = TRUE;
					bWantStop = TRUE;
					size_t i=0;

					while (pszDevices[i] != _T('\0'))
					{
						//Get the current device name
						TCHAR* pszCurrentDevice = &(pszDevices[i]);

						//If it looks like "COMX" then
						//add it to the array which will be returned
						size_t nLen = _tcslen(pszCurrentDevice);
						if (nLen > 3)
						{
							if ((_tcsnicmp(pszCurrentDevice, _T("COM"), 3) == 0) && IsNumeric(&(pszCurrentDevice[3]), FALSE))
							{
								//Work out the port number
								int nPort = _ttoi(&pszCurrentDevice[3]);
								ports.Add(nPort);
							}
						}

						//Go to next device name
						i += (nLen + 1);
					}
				}
			}
			else
			{
				bWantStop = TRUE;
				SetLastError(ERROR_OUTOFMEMORY);        
			}
		}
	}
	else
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

	return bSuccess;
}



