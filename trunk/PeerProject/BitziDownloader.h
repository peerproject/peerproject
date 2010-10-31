//
// BitziDownloader.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
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

#include "ThreadImpl.h"
#include "Schema.h"

class CXMLElement;
class CBitziDownloadDlg;


class CBitziDownloader :
	public CThreadImpl
{
// Construction
public:
	CBitziDownloader();
	virtual ~CBitziDownloader();

// Attributes
protected:
	CList< DWORD, DWORD > m_pFiles;
	CCriticalSection	m_pSection;
	CBitziDownloadDlg*	m_pDlg;

	HINTERNET		m_hInternet;
	HINTERNET		m_hSession;
	HINTERNET		m_hRequest;
	BOOL			m_bFinished;
	DWORD			m_nDelay;
	DWORD			m_nFailures;
	DWORD			m_nFileIndex;
	CString			m_sFileName;
	CString			m_sFileSHA1;
	CString			m_sFileTiger;
	CString			m_sURL;
	CString			m_sResponse;
	CXMLElement*	m_pXML;

// Operations
public:
	void			AddFile(DWORD nIndex);
	INT_PTR			GetFileCount();
	BOOL			IsWorking();
	BOOL			Start(CBitziDownloadDlg* pDlg = NULL);
	void			Stop();
protected:
	void			OnRun();
	BOOL			BuildRequest();
	BOOL			ExecuteRequest();
	BOOL			DecodeResponse();
	CString			LookupValue(LPCTSTR pszPath);
	CXMLElement*	ImportData(CSchemaPtr pSchema);
	BOOL			SubmitMetaData(CXMLElement* pXML);
	BOOL			MergeMetaData(CXMLElement* pOutput, CXMLElement* pInput);
};
