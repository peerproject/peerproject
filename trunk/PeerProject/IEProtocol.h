//
// IEProtocol.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
// Portions copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

class CBuffer;
class CIEProtocolRequest;

class CZIPFile;

// {18D11ED9-1264-48A1-9E14-20F2C633242B}
class CIEProtocol : public CComObject
{
// Construction
public:
	CIEProtocol();
	virtual ~CIEProtocol();

	DECLARE_DYNCREATE(CIEProtocol)

// Operations
public:
	BOOL		Create();
	void		Close();
	HRESULT		OnRequest(LPCTSTR pszURL, CBuffer& oBuffer, CString& sMimeType, BOOL bParseOnly);

// Attributes
protected:
	static LPCWSTR					pszProtocols[];
	CCriticalSection				m_pSection;
	CComQIPtr< IInternetSession >	m_pSession;

// Implementation
protected:
	HRESULT		OnRequestP2PCOL(LPCTSTR pszURL, CBuffer& oBuffer, CString& sMimeType, BOOL bParseOnly);
	HRESULT		OnRequestP2PFILE(LPCTSTR pszURL, CBuffer& oBuffer, CString& sMimeType, BOOL bParseOnly);

// COM
protected:
	BEGIN_INTERFACE_PART(ClassFactory, IClassFactory)
		STDMETHOD(CreateInstance)(IUnknown* pUnkOuter, REFIID riid, void** ppvObject);
		STDMETHOD(LockServer)(BOOL fLock);
	END_INTERFACE_PART(ClassFactory)

	DECLARE_OLECREATE(CIEProtocol)
	DECLARE_INTERFACE_MAP()
};


// {E1A67AE5-7041-4AE1-94F7-DE03EF759E27}
class CIEProtocolRequest : public CComObject
{
// Construction
public:
	CIEProtocolRequest();
	virtual ~CIEProtocolRequest();

	DECLARE_DYNCREATE(CIEProtocolRequest)

// Attributes
protected:
	CComPtr<IInternetProtocolSink>	m_pSink;
	CString							m_strMimeType;	// Data MIME type
	CBuffer							m_oBuffer;		// Requested data

// Implementation
protected:
	HRESULT		OnStart(LPCTSTR pszURL, IInternetProtocolSink* pSink, IInternetBindInfo* pBindInfo, DWORD dwFlags);
	HRESULT		OnRead(void* pv, ULONG cb, ULONG* pcbRead);

// COM
protected:
	BEGIN_INTERFACE_PART(InternetProtocol, IInternetProtocol)
		STDMETHOD(Abort)(HRESULT hrReason, DWORD dwOptions);
		STDMETHOD(Continue)(PROTOCOLDATA *pProtocolData);
		STDMETHOD(Resume)();
		STDMETHOD(Start)(LPCWSTR szUrl, IInternetProtocolSink *pOIProtSink, IInternetBindInfo *pOIBindInfo, DWORD grfPI, HANDLE_PTR dwReserved);
		STDMETHOD(Suspend)();
		STDMETHOD(Terminate)(DWORD dwOptions);
		STDMETHOD(LockRequest)(DWORD dwOptions);
		STDMETHOD(Read)(void *pv, ULONG cb, ULONG *pcbRead);
		STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
		STDMETHOD(UnlockRequest)();
	END_INTERFACE_PART(InternetProtocol)

	BEGIN_INTERFACE_PART(InternetProtocolInfo , IInternetProtocolInfo )
		STDMETHOD(CombineUrl)(LPCWSTR pwzBaseUrl, LPCWSTR pwzRelativeUrl, DWORD dwCombineFlags, LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved);
		STDMETHOD(CompareUrl)(LPCWSTR pwzUrl1, LPCWSTR pwzUrl2, DWORD dwCompareFlags);
		STDMETHOD(ParseUrl)(LPCWSTR pwzUrl, PARSEACTION ParseAction, DWORD dwParseFlags, LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved);
		STDMETHOD(QueryInfo)(LPCWSTR pwzUrl, QUERYOPTION OueryOption, DWORD dwQueryFlags, LPVOID pBuffer, DWORD cbBuffer, DWORD *pcbBuf, DWORD dwReserved);
	END_INTERFACE_PART(InternetProtocolInfo )

	DECLARE_OLECREATE(CIEProtocolRequest)
	DECLARE_INTERFACE_MAP()

	friend class CIEProtocol;
};

extern CIEProtocol IEProtocol;
