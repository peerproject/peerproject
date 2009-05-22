//
// QuerySearch.h
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

#include "PeerProjectFile.h"

class CPacket;
class CSchema;
class CXMLElement;
class CSearchWnd;
class CG1Packet;
class CG2Packet;
class CEDPacket;


class CQuerySearch : public CPeerProjectFile
{
// Construction
public:
	CQuerySearch(BOOL bGUID = TRUE);
	virtual ~CQuerySearch();
private:
	CQuerySearch(const CQuerySearch* pOrigin);
public:
	auto_ptr< CQuerySearch > clone() const;
	typedef std::vector<DWORD>					Hash32List;
// Attributes
public:
	bool				m_bAutostart;	// Autostart search (default - true)
	Hashes::Guid		m_oGUID;		// G1,G2: Search ID
	CString				m_sSearch;		// search string, transformed by lowercase table
	CString				m_sKeywords;	// search keywords (stems, minus words, split asian phrase etc.)
	CString				m_sPosKeywords;	// Positive keywords ( no minus, no quotes basically for Gnutella1 Query)
	CString				m_sG2Keywords;	// Query string for G2, containing Positive keywords and Minus Prefixed negative keywords.
	CSchema*			m_pSchema;		// G1,G2,ED2K: Metadata schema
	CXMLElement*		m_pXML;			// G1,G2,ED2K: Metadata
	QWORD				m_nMinSize;		// G2,ED2K: Minimal file size
	QWORD				m_nMaxSize;		// G2,ED2K: Maximal file size
	Hashes::Ed2kHash	m_oSimilarED2K;	// ED2K: Search for Similar Files
	BOOL				m_bWantURL;		// G2: Sources, preview URL request
	BOOL				m_bWantDN;		// G2: Descriptive name request
	BOOL				m_bWantXML;		// G1,G2: Metadata request
	BOOL				m_bWantCOM;		// G2: Comments request
	BOOL				m_bWantPFS;		// G2: Partial Files Search request
	BOOL				m_bAndG1;
	BYTE				m_nTTL;			// G1: Suggested TTL for answer
	BOOL				m_bUDP;			// G2: Packet received over UDP
	SOCKADDR_IN			m_pEndpoint;	// G2: Packet received from this host
	DWORD				m_nKey;			// G2: Hub query key
	bool				m_bFirewall;	// G1: Firewalled host
	bool				m_bDynamic;		// G1: Leaf Guided Dynamic Query
	bool				m_bBinHash;		// G1: GGEP "H" allowed
	bool				m_bOOB;			// G1: Out of Band Query
	bool				m_bOOBv3;		// G1: OOB v3 Security Token support
	BYTE				m_nMeta;		// G1: MetaType query mask
	bool				m_bPartial;		// G1: Partial results support
	bool				m_bNoProxy;		// G1: Disable OOB proxying
	bool				m_bExtQuery;	// G1: Extended query (long query)
	bool				m_bWarning;		// Got parser warnings

	Hash32List			m_oURNs;			// Hashed URNs
	Hash32List			m_oKeywordHashList;	// list of hashed keywords to BOOST QUery Routing.

private:
	typedef std::pair< LPCTSTR, size_t > WordEntry;
	struct CompareWordEntries
	{
		bool operator()(const WordEntry& lhs, const WordEntry& rhs) const
		{
			int cmp = _tcsnicmp( lhs.first, rhs.first, min( lhs.second, rhs.second ) );
			return cmp < 0 || cmp == 0 && lhs.second < rhs.second;
		}
	};
	struct FindStr
	{
		const WordEntry& m_entry;
		FindStr(const WordEntry& entry) : m_entry( entry ) {}
		bool operator()(const LPCTSTR& arg) const
		{
			//! \todo verify this - this will succeed for every arg that starts withthe search string
			//!                     it doesn't have to be an exact match
			return _tcsnicmp( arg, m_entry.first, m_entry.second ) == 0;
		}
	};
public:
	typedef std::set< WordEntry, CompareWordEntries > WordTable;
	typedef WordTable::iterator iterator;
	typedef WordTable::const_iterator const_iterator;
	typedef Hash32List::iterator hash_iterator;
	typedef Hash32List::const_iterator const_hash_iterator;
	//positive keywords
	const_iterator			begin() const { return m_oWords.begin(); }
	const_iterator			end()   const { return m_oWords.end(); }
	size_t					tableSize() const { return m_oWords.size(); }
	//Negative keywords
	const_iterator			beginNeg() const { return m_oNegWords.begin(); }
	const_iterator			endNeg()   const { return m_oNegWords.end(); }
	size_t					tableSizeNeg() const { return m_oNegWords.size(); }
	// hashed URNs
	const_hash_iterator		urnBegin() const { return m_oURNs.begin(); }
	const_hash_iterator		urnEnd()   const { return m_oURNs.end(); }
	// hashed keywords (Positive words only)
	const_hash_iterator		keywordBegin() const { return m_oKeywordHashList.begin(); }
	const_hash_iterator		keywordEnd()   const { return m_oKeywordHashList.end(); }
private:
	WordTable m_oWords;
	WordTable m_oNegWords;

// Packet Operations
public:
	CG1Packet*				ToG1Packet(DWORD nTTL = 0);
	CG2Packet*				ToG2Packet(SOCKADDR_IN* pUDP, DWORD nKey);
	CEDPacket*				ToEDPacket(BOOL bUDP, DWORD nServerFlags = 0);
private:
	BOOL					ReadG1Packet(CG1Packet* pPacket);
	void					ReadGGEP(CG1Packet* pPacket);
	void					ReadExtension(CG1Packet* pPacket);
	BOOL					ReadG2Packet(CG2Packet* pPacket, SOCKADDR_IN* pEndpoint = NULL);

// Operations
public:
	BOOL					Match(LPCTSTR pszFilename, QWORD nSize, LPCTSTR pszSchemaURI, CXMLElement* pXML, const Hashes::Sha1Hash& oSHA1,
								const Hashes::TigerHash& oTiger, const Hashes::Ed2kHash& oED2K, const Hashes::BtHash& oBTH, const Hashes::Md5Hash& oMD5) const;
	TRISTATE				MatchMetadata(LPCTSTR pszSchemaURI, CXMLElement* pXML) const;
	BOOL					MatchMetadataShallow(LPCTSTR pszSchemaURI, CXMLElement* pXML, bool* bReject=NULL) const;
	void					BuildWordList(bool bExpression=true, bool bLocal=false);
	void					Serialize(CArchive& ar);
	BOOL					CheckValid(bool bExpression=true);
	void					PrepareCheck();
private:
	void					BuildWordTable();
	void					BuildG2PosKeywords();
	void					SlideKeywords(CString& strPhrase);
	BOOL					WriteHashesToEDPacket(CEDPacket* pPacket, BOOL bUDP);

// Utilities
public:
	static CQuerySearch*	FromPacket(CPacket* pPacket, SOCKADDR_IN* pEndpoint = NULL);
	static	CSearchWnd*		OpenWindow(auto_ptr< CQuerySearch > pSearch);
	static	BOOL			WordMatch(LPCTSTR pszString, LPCTSTR pszFind, bool* bReject=NULL);
	static	BOOL			NumberMatch(const CString& strValue, const CString& strRange);
	static	void			MakeKeywords(CString& strPhrase, bool bExpression=true);
	static	void			SearchHelp();	// Shows some search help dialogs
};
