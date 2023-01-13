// InternetX.cpp

#include "stdafx.h"
#include "shlwapi.h"
#include "InternetX.h"

#pragma comment(lib, "wininet.lib")

#if FALSE
#define URLUNESCAPE_ENABLED
#endif

static BOOL _ParseURLWorkerX(LPCTSTR lpszURL, LPURL_COMPONENTS lpComponents, DWORD& dwServiceType, INTERNET_PORT& nPort, DWORD dwFlags)
{
	// This function will return bogus stuff if lpComponents isn't set up to copy the components
	ASSERT((lpComponents != NULL) && (lpszURL != NULL));

	if ((lpComponents == NULL) || (lpszURL == NULL))
	{
		return FALSE;
	}

	ASSERT((lpComponents->dwHostNameLength == 0) || (lpComponents->lpszHostName != NULL));
	ASSERT((lpComponents->dwUrlPathLength  == 0) || (lpComponents->lpszUrlPath  != NULL));
	ASSERT((lpComponents->dwUserNameLength == 0) || (lpComponents->lpszUserName != NULL));
	ASSERT((lpComponents->dwPasswordLength == 0) || (lpComponents->lpszPassword != NULL));

	LPTSTR	lpszCanonicalizedURL;
	TCHAR	szCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
	DWORD	dwNeededLength	= INTERNET_MAX_URL_LENGTH;
	BOOL	bRetVal;
	BOOL	bMustFree		= FALSE;

	// Decoding is done in InternetCrackUrl/UrlUnescape so we don't need the ICU_DECODE flag here.
	DWORD	dwCanonicalizeFlags	= (dwFlags & (ICU_NO_ENCODE | ICU_NO_META | ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE));
	DWORD	dwCrackFlags		= 0;
	BOOL	bUnescape			= FALSE;

	if ((dwFlags & (ICU_ESCAPE | ICU_DECODE)) && (lpComponents->dwUrlPathLength != 0))
	{
		// We use only the ICU_ESCAPE flag for decoding even if ICU_DECODE is passed.
		// Also, if ICU_BROWSER_MODE is passed we do the unescaping manually because InternetCrackUrl doesn't do Browser mode unescaping
		if (dwFlags & ICU_BROWSER_MODE)
		{
			bUnescape	= TRUE;
		}
		else
		{
			dwCrackFlags	|= ICU_ESCAPE;
		}
	}

	bRetVal	= InternetCanonicalizeUrl(lpszURL, szCanonicalizedURL, &dwNeededLength, dwCanonicalizeFlags);

	if (!bRetVal)
	{
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			return FALSE;
		}

		lpszCanonicalizedURL	= new TCHAR[dwNeededLength];

		if (lpszCanonicalizedURL == NULL)
		{
			return FALSE;
		}

		bMustFree	= TRUE;
		bRetVal		= InternetCanonicalizeUrl(lpszURL, lpszCanonicalizedURL, &dwNeededLength, dwCanonicalizeFlags);

		if (!bRetVal)
		{
			delete [] lpszCanonicalizedURL;
			return FALSE;
		}
	}
	else
	{
		lpszCanonicalizedURL	= szCanonicalizedURL;
	}

	// now that it's safely canonicalized, crack it
	bRetVal	= InternetCrackUrl(lpszCanonicalizedURL, 0, dwCrackFlags, lpComponents);

#ifdef URLUNESCAPE_ENABLED
	if (bUnescape)
	{
		if (FAILED(UrlUnescape(lpComponents->lpszUrlPath, NULL, NULL, URL_UNESCAPE_INPLACE | URL_DONT_UNESCAPE_EXTRA_INFO)))
		{
			if (bMustFree)
			{
				delete [] lpszCanonicalizedURL;
			}

			return FALSE;
		}

		lpComponents->dwUrlPathLength	= lstrlen(lpComponents->lpszUrlPath);
	}
#endif

	if (bMustFree)
	{
		delete [] lpszCanonicalizedURL;
	}

	// convert to MFC-style service ID
	if (!bRetVal)
	{
		dwServiceType	= INLIB_INET_SERVICE_UNK;
	}
	else
	{
		nPort	= lpComponents->nPort;

		switch (lpComponents->nScheme)
		{
		case INTERNET_SCHEME_FTP:
			{
				dwServiceType	= INLIB_INET_SERVICE_FTP;
			}
			break;
		case INTERNET_SCHEME_GOPHER:
			{
				dwServiceType	= INLIB_INET_SERVICE_GOPHER;
			}
			break;
		case INTERNET_SCHEME_HTTP:
			{
				dwServiceType	= INLIB_INET_SERVICE_HTTP;
			}
			break;
		case INTERNET_SCHEME_HTTPS:
			{
				dwServiceType	= INLIB_INET_SERVICE_HTTPS;
			}
			break;
		case INTERNET_SCHEME_FILE:
			{
				dwServiceType	= INLIB_INET_SERVICE_FILE;
			}
			break;
		case INTERNET_SCHEME_NEWS:
			{
				dwServiceType	= INLIB_INET_SERVICE_NNTP;
			}
			break;
		case INTERNET_SCHEME_MAILTO:
			{
				dwServiceType	= INLIB_INET_SERVICE_MAILTO;
			}
			break;
		default:
			{
				dwServiceType	= INLIB_INET_SERVICE_UNK;
			}
			break;
		}
	}

	return bRetVal;
}

BOOL ParseURLX(LPCTSTR lpszURL, DWORD& dwServiceType, LPTSTR szServer, LPTSTR szObject, INTERNET_PORT& nPort)
{
	dwServiceType	= INLIB_INET_SERVICE_UNK;

	ASSERT(lpszURL != NULL);

	if (lpszURL == NULL)
	{
		return FALSE;
	}

	URL_COMPONENTS	urlComponents;

	memset(&urlComponents, 0, sizeof(URL_COMPONENTS));

	urlComponents.dwStructSize		= sizeof(URL_COMPONENTS);
	urlComponents.dwHostNameLength	= INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszHostName		= szServer;
	urlComponents.dwUrlPathLength	= INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszUrlPath		= szObject;

	return _ParseURLWorkerX(lpszURL, &urlComponents, dwServiceType, nPort, ICU_BROWSER_MODE);
}

#define BUFFER_SIZE	16384

int HTTPQueryX(LPTSTR szAddress, LPTSTR szResponse, LPDWORD lpdwResponseLength)
{
	int				nRetCode			= 0;
	DWORD			dwAccessType		= INTERNET_OPEN_TYPE_PRECONFIG;
	const TCHAR		szAgent[]			= _T("Anise/5.0");
	const TCHAR		szHeaders[]			= _T("Accept: text/*\r\nUser-Agent: Anise/5.0\r\n");
	DWORD			dwHttpRequestFlags	= INTERNET_FLAG_NO_AUTO_REDIRECT;
	BOOL			bSuccess			= FALSE;
	TCHAR			szServerName[INTERNET_MAX_URL_LENGTH + 1];
	TCHAR			szObject[INTERNET_MAX_URL_LENGTH + 1];
	INTERNET_PORT	nPort;
	DWORD			dwServiceType;

	HINTERNET	hInternet	= InternetOpen(szAgent, dwAccessType, NULL, NULL, 0);

	if (hInternet == NULL)
	{
		return (nRetCode = (-1));
	}

	if (!ParseURLX(szAddress, dwServiceType, szServerName, szObject, nPort) || dwServiceType != INTERNET_SERVICE_HTTP)
	{
		InternetCloseHandle(hInternet);
		return (nRetCode = (-1));
	}

	HINTERNET	hSession	= InternetConnect(hInternet, szServerName, nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

	if (hSession == NULL)
	{
		InternetCloseHandle(hInternet);
		return (nRetCode = (-1));
	}

	HINTERNET	hRequest	= HttpOpenRequest(hSession, _T("GET"), szObject, _T("HTTP/1.1"), NULL, NULL, dwHttpRequestFlags, 0);

	if (hRequest == NULL)
	{
		InternetCloseHandle(hSession);
		InternetCloseHandle(hInternet);
		return (nRetCode = (-1));
	}

	bSuccess	= HttpAddRequestHeaders(hRequest, szHeaders, (-1L), HTTP_ADDREQ_FLAG_ADD_IF_NEW);

	if (!bSuccess)
	{
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hSession);
		InternetCloseHandle(hInternet);
		return (nRetCode = (-1));
	}

	bSuccess	= HttpSendRequest(hRequest, NULL, 0, NULL, 0);

	if (!bSuccess)
	{
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hSession);
		InternetCloseHandle(hInternet);
		return (nRetCode = (-1));
	}

	DWORD	dwStatusCode	= 0;
	DWORD	dwStatusLength	= sizeof(DWORD);
	DWORD	dwIndex			= 0;

	bSuccess	= HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, reinterpret_cast<LPVOID>(&dwStatusCode), &dwStatusLength, &dwIndex);

	if (!bSuccess)
	{
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hSession);
		InternetCloseHandle(hInternet);
		return (nRetCode = (-1));
	}

	if (dwStatusCode == HTTP_STATUS_DENIED)
	{
		nRetCode	= 401;
	}

	BYTE*	szABuffer	= new BYTE[BUFFER_SIZE + 1];
	TCHAR*	szWBuffer	= new TCHAR[BUFFER_SIZE + 1];
	DWORD	dwReadSize	= 0;

	memset(szABuffer, 0, sizeof(BYTE) * (BUFFER_SIZE + 1));
	memset(szWBuffer, 0, sizeof(TCHAR) * (BUFFER_SIZE + 1));

	bSuccess	= InternetReadFile(hRequest, reinterpret_cast<LPVOID>(szABuffer), BUFFER_SIZE, &dwReadSize);

	if (!bSuccess)
	{
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hSession);
		InternetCloseHandle(hInternet);
		return (nRetCode = (-1));
	}

	if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, reinterpret_cast<LPCSTR>(szABuffer), (-1), szWBuffer, BUFFER_SIZE) == 0)
	{
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hSession);
		InternetCloseHandle(hInternet);
		return (nRetCode = (-1));
	}

	if (_tcslen(szWBuffer) > (*lpdwResponseLength - 1))
	{
		*lpdwResponseLength	= _tcslen(szWBuffer) + 1;

		return (nRetCode = (-2));
	}

	_tcscpy(szResponse, szWBuffer);

	delete [] szWBuffer;
	delete [] szABuffer;

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInternet);
	return nRetCode;
}
