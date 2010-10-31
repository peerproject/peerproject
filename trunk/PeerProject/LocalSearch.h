//
// LocalSearch.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
	bool		Execute(INT_PTR nMaximum = -1, bool bPartial = true, bool bShared = true);
	void		WriteVirtualTree();
	const CQuerySearch* GetSearch() const{ return m_pSearch; }

protected:
	bool		ExecuteSharedFiles(INT_PTR nMaximum, INT_PTR& nHits);
	bool		ExecutePartialFiles(INT_PTR nMaximum, INT_PTR& nHits);
	template< typename T > void SendHits(const CList< const T * >& oFiles);
	template< typename T > void AddHit(CPacket* pPacket, CSchemaMap& pSchemas, const T * pHit, int nIndex);
	void		AddHitG1(CG1Packet* pPacket, CSchemaMap& pSchemas, CLibraryFile const * const pFile, int nIndex);
	void		AddHitG2(CG2Packet* pPacket, CSchemaMap& pSchemas, CLibraryFile const * const pFile, int nIndex);
	void		AddHitG1(CG1Packet* pPacket, CSchemaMap& pSchemas, CDownload const * const pDownload, int nIndex);
	void		AddHitG2(CG2Packet* pPacket, CSchemaMap& pSchemas, CDownload const * const pDownload, int nIndex);
	template< typename T > bool IsValidForHit(const T * pHit) const;
	bool		IsValidForHitG1(CLibraryFile const * const pFile) const;
	bool		IsValidForHitG2(CLibraryFile const * const pFile) const;

	CPacket*	CreatePacket();
	CG1Packet*	CreatePacketG1();
	CG2Packet*	CreatePacketG2();
	void		WriteTrailer(CPacket* pPacket, CSchemaMap& pSchemas, BYTE nHits);
	void		WriteTrailerG1(CG1Packet* pPacket, CSchemaMap& pSchemas, BYTE nHits);
	void		WriteTrailerG2(CG2Packet* pPacket, CSchemaMap& pSchemas, BYTE nHits);
	void		DispatchPacket(CPacket* pPacket);
	CG2Packet*	AlbumToPacket(CAlbumFolder* pFolder);
	CG2Packet*	FoldersToPacket();
	CG2Packet*	FolderToPacket(CLibraryFolder* pFolder);

private:
	// Limit query answer packet size since Gnutella 1/2
	// drops packets larger than Settings.Gnutella.MaximumPacket
	static const DWORD	MAX_QUERY_PACKET_SIZE		= 16384; // (bytes)
	static const BYTE	MAX_QUERY_PACKET_HITCOUNT	= (BYTE)~0;
};
