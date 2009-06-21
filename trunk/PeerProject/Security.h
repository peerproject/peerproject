//
// Security.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#pragma once

#include "QuerySearch.h"

class CSecureRule;
class CXMLElement;


#define SECURITY_SER_VERSION	5
// History:
// 5 - extended security rule type (ryo-oh-ki)

enum
{
	banSession, ban5Mins, ban30Mins, ban2Hours, banWeek, banForever
};

class CSecurity
{
// Construction
public:
	CSecurity();
	virtual ~CSecurity();

// Attributes
public:
	mutable CCriticalSection	m_pSection;
	BOOL						m_bDenyPolicy;
	static LPCTSTR				xmlns;

protected:
	typedef struct
	{
		DWORD	m_nExpire;
		BYTE	m_nScore;
	} CComplain;
	typedef CMap< DWORD, DWORD, CComplain*, CComplain* > CComplainMap;

	CList< CSecureRule* >		m_pRules;
	CComplainMap				m_Complains;

// Operations
public:
	POSITION		GetIterator() const;
	CSecureRule*	GetNext(POSITION& pos) const;
	INT_PTR			GetCount() const;
	BOOL			Check(CSecureRule* pRule) const;
	void			Add(CSecureRule* pRule);
	void			Remove(CSecureRule* pRule);
	void			MoveUp(CSecureRule* pRule);
	void			MoveDown(CSecureRule* pRule);

	inline void		Ban(const IN_ADDR* pAddress, int nBanLength, BOOL bMessage = TRUE)
	{
		BanHelper( pAddress, NULL, nBanLength, bMessage );
	}

	inline void		Ban(const CPeerProjectFile* pFile, int nBanLength, BOOL bMessage = TRUE)
	{
		BanHelper( NULL, pFile, nBanLength, bMessage );
	}

	bool			Complain(const IN_ADDR* pAddress, int nBanLength = ban5Mins, int nExpire = 30, int nCount = 3);
	void			Clear();
	BOOL			IsDenied(const IN_ADDR* pAddress);
	BOOL			IsDenied(LPCTSTR pszContent);
	BOOL			IsDenied(const CPeerProjectFile* pFile);
	BOOL			IsDenied(CQuerySearch::const_iterator itStart,
							 CQuerySearch::const_iterator itEnd, LPCTSTR pszContent);
	void			Expire();
	BOOL			Load();
	BOOL			Save();
	BOOL			Import(LPCTSTR pszFile);

	// Don't ban GPL breakers, but don't offer leaf slots to them. Ban others.
	BOOL			IsClientBad(const CString& sUserAgent);
	BOOL			IsClientBanned(const CString& sUserAgent);

protected:
	void			BanHelper(const IN_ADDR* pAddress, const CPeerProjectFile* pFile, int nBanLength, BOOL bMessage);
	CSecureRule*	GetGUID(const GUID& oGUID) const;
	CXMLElement*	ToXML(BOOL bRules = TRUE);
	BOOL			FromXML(CXMLElement* pXML);
	void			Serialize(CArchive& ar);
};

class CSecureRule
{
public:
	CSecureRule(BOOL bCreate = TRUE);
	CSecureRule(const CSecureRule& pRule);
	CSecureRule& operator=(const CSecureRule& pRule);
	virtual ~CSecureRule();

	typedef enum { srAddress, srContentAny, srContentAll, srContentRegExp } RuleType;
	enum { srNull, srAccept, srDeny };
	enum { srIndefinite = 0, srSession = 1 };

	RuleType	m_nType;
	BYTE		m_nAction;
	CString		m_sComment;
	GUID		m_pGUID;
	DWORD		m_nExpire;
	DWORD		m_nToday;
	DWORD		m_nEver;
	BYTE		m_nIP[4];
	BYTE		m_nMask[4];
	TCHAR*		m_pContent;
	DWORD		m_nContentLength;

	void	Remove();
	void	Reset();
	void	MaskFix();
	BOOL	IsExpired(DWORD nNow, BOOL bSession = FALSE) const;
	BOOL	Match(const IN_ADDR* pAddress) const;
	BOOL	Match(LPCTSTR pszContent) const;
	BOOL	Match(const CPeerProjectFile* pFile) const;
	BOOL	Match(CQuerySearch::const_iterator itStart,
				  CQuerySearch::const_iterator itEnd, LPCTSTR pszContent) const;
	void	SetContentWords(const CString& strContent);
	CString	GetContentWords();
	void			Serialize(CArchive& ar, int nVersion);
	CXMLElement*	ToXML();
	BOOL			FromXML(CXMLElement* pXML);
	CString			ToGnucleusString();
	BOOL			FromGnucleusString(CString& str);
};

// An adult filter class, used in searches, chat, etc
class CAdultFilter
{
// Construction
public:
	CAdultFilter();
	virtual ~CAdultFilter();

// Attributes
private:
	LPTSTR		m_pszBlockedWords;			// Definitely adult content
	LPTSTR		m_pszDubiousWords;			// Possibly adult content
	LPTSTR		m_pszChildWords;			// Words related to child ponography

// Operations
public:
	void		Load();
	BOOL		IsHitAdult(LPCTSTR);		// Does this search result have adult content?
	BOOL		IsSearchFiltered(LPCTSTR);	// Check if search is filtered
	BOOL		IsChatFiltered(LPCTSTR);	// Check filter for chat
	BOOL		Censor(TCHAR*);				// Censor (remove) bad words from a string
	BOOL		IsChildPornography(LPCTSTR);
private:
	BOOL		IsFiltered(LPCTSTR);
};

// A message filter class for chat messages. (Spam protection)
class CMessageFilter
{
// Construction
public:
	CMessageFilter();
	virtual ~CMessageFilter();

// Attributes
private:
	LPTSTR		m_pszED2KSpam;				// Known ED2K spam phrases
	LPTSTR		m_pszFilteredPhrases;		// Known spam phrases

// Operations
public:
	void		Load();
	BOOL		IsED2KSpam( LPCTSTR );		// ED2K message spam filter (ED2K only, always on)
	BOOL		IsFiltered( LPCTSTR );		// Chat message spam filter
};

extern CMessageFilter MessageFilter;
extern CAdultFilter AdultFilter;
extern CSecurity Security;