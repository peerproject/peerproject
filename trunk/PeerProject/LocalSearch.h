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
	CLocalSearch(CQuerySearch* pSearch, const CNeighbour* pNeighbour);
	CLocalSearch(CQuerySearch* pSearch);
	CLocalSearch(CQuerySearch* pSearch, CBuffer* pBuffer, PROTOCOLID nProtocol);

// Attributes
protected:
	CQuerySearchPtr	m_pSearch;
	SOCKADDR_IN		m_pEndpoint;	// Endpoint or neighbour address
	CBuffer*		m_pBuffer;		// Save packets to this buffer or...
	BOOL			m_bUDP;			// ...send them via UDP or TCP
	Hashes::Guid	m_oGUID;
	PROTOCOLID		m_nProtocol;

	typedef CMap< CSchemaPtr, CSchemaPtr, CXMLElement*, CXMLElement* > CSchemaMap;

// Operations
public:
	// Search library files and active downloads ( -1 = use default limit, 0 = no limit)
	bool		Execute(int nMaximum = -1, bool bPartial = true, bool bShared = true);
	void		WriteVirtualTree();
	const CQuerySearch* GetSearch() const{ return m_pSearch; }

protected:
	bool		ExecuteSharedFiles(int nMaximum, int& nHits);
	bool		ExecutePartialFiles(int nMaximum, int& nHits);
	template< typename T > void SendHits(const CList< const T * >& oFiles);
	template< typename T > void AddHit(CPacket* pPacket, CSchemaMap& pSchemas, const T * pHit, int nIndex);
	void		AddHitG1(CG1Packet* pPacket, CSchemaMap& pSchemas, CLibraryFile const * const pFile, int nIndex);
	void		AddHitG2(CG2Packet* pPacket, CSchemaMap& pSchemas, CLibraryFile const * const pFile, int nIndex);
	void		AddHitG1(CG1Packet* pPacket, CSchemaMap& pSchemas, CDownload const * const pDownload, int nIndex);
	void		AddHitG2(CG2Packet* pPacket, CSchemaMap& pSchemas, CDownload const * const pDownload, int nIndex);
	template< typename T > bool IsValidForHit(const T * pHit) const;
	inline bool	IsValidForHitG1(CLibraryFile const * const pFile) const;
	inline bool	IsValidForHitG2(CLibraryFile const * const pFile) const;
protected:
	CPacket*	CreatePacket();
	CG1Packet*	CreatePacketG1();
	CG2Packet*	CreatePacketG2();
	void		WriteTrailer(CPacket* pPacket, CSchemaMap& pSchemas, int nHits);
	void		WriteTrailerG1(CG1Packet* pPacket, CSchemaMap& pSchemas, int nHits);
	void		WriteTrailerG2(CG2Packet* pPacket, CSchemaMap& pSchemas, int nHits);
	void		DispatchPacket(CPacket* pPacket);
	CG2Packet*	AlbumToPacket(CAlbumFolder* pFolder);
	CG2Packet*	FoldersToPacket();
	CG2Packet*	FolderToPacket(CLibraryFolder* pFolder);
};
