//
// LocalSearch.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

class CNeighbour;
class CLibraryFile;
class CAlbumFolder;
class CLibraryFolder;
class CXMLElement;
class CBuffer;
class CDownload;
class CPacket;
class CG2Packet;


class CLocalSearch
{
// Construction
public:
	CLocalSearch(CQuerySearch* pSearch, CNeighbour* pNeighbour, BOOL bWrapped = FALSE);
	CLocalSearch(CQuerySearch* pSearch, SOCKADDR_IN* pEndpoint);
	CLocalSearch(CQuerySearch* pSearch, CBuffer* pBuffer, PROTOCOLID nProtocol = PROTOCOL_G1);
	virtual ~CLocalSearch();

// Attributes
protected:
	CQuerySearchPtr	m_pSearch;
	CNeighbour*		m_pNeighbour;
	SOCKADDR_IN*	m_pEndpoint;
	CBuffer*		m_pBuffer;
	Hashes::Guid	m_oGUID;
	PROTOCOLID		m_nProtocol;
	BOOL			m_bWrapped;
protected:
	CPacket*		m_pPacket;
	CMap< CSchemaPtr, CSchemaPtr, CXMLElement*, CXMLElement* > m_pSchemas;

// Operations
public:
	// Search library and downloads (-1 - use default limit, 0 - no limit)
	INT_PTR		Execute(INT_PTR nMaximum = -1);
	void		WriteVirtualTree();
protected:
	INT_PTR		ExecuteSharedFiles(INT_PTR nMaximum);
	INT_PTR		ExecutePartialFiles(INT_PTR nMaximum);
	template< typename T > INT_PTR SendHits(const CList< const T * >& oFiles);
	template< typename T > void AddHit(const T * pHit, int nIndex);
	template< typename T > bool IsValidForHit(const T * pHit) const;
	void		AddHitG1(CLibraryFile const * const pFile, int nIndex);
	void		AddHitG2(CLibraryFile const * const pFile, int nIndex);
	inline bool	IsValidForHitG1(CLibraryFile const * const pFile) const;
	inline bool	IsValidForHitG2(CLibraryFile const * const pFile) const;
protected:
	void		CreatePacket(int nCount);
	void		CreatePacketG1(int nCount);
	void		CreatePacketG2();
	void		AddMetadata(CSchemaPtr pSchema, CXMLElement* pXML, int nIndex);
	CString		GetXMLString(BOOL bNewlines = TRUE);
	void		WriteTrailer();
	void		WriteTrailerG1();
	void		WriteTrailerG2();
	void		DispatchPacket();
	void		DestroyPacket();
	CG2Packet*	AlbumToPacket(CAlbumFolder* pFolder);
	CG2Packet*	FoldersToPacket();
	CG2Packet*	FolderToPacket(CLibraryFolder* pFolder);
};
