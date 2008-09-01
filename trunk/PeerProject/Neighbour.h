//
// Neighbour.h
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

// CNeighbour is in the middle of the CConnection inheritance tree, adding compression and a bunch of member variables
// http://shareazasecurity.be/wiki/index.php?title=Developers.Code.CNeighbour

#pragma once

#include "Connection.h"
#include "zlib.h"

// Tell the compiler these classes exist
class CBuffer;
class CPacket;
class CVendor;
class CGProfile;
class CQuerySearch;
class CQueryHashTable;

// Keep track of what stage of communications we are in with the remote computer
typedef enum NeighbourStateEnum
{
	// One of these states describes what's happening with our connection to the remote computer right now
	nrsNull,       // No state recorded yet, the CNeighbour constructor sets m_nState to nrsNull
	nrsConnecting, // We called CConnection::ConnectTo, and are now waiting for the remote computer to do something
	nrsHandshake1, // We've finished sending a group of headers, and await the response
	nrsHandshake2, // We're reading the initial header group the remote computer has sent
	nrsHandshake3, // We're reading the final header group from the remote computer
	nrsRejected,   // The remote computer started with "GNUTELLA/0.6", but did not say "200 OK"
	nrsClosing,    // We called DelayClose to send buffered data and then close the socket connection
	nrsConnected   // The handshake is over, the CNeighbour copy constructor sets m_nState to nrsConnected

} NrsState;

// Record if the remote computer is in the same network role as us, or in a higher or lower one
typedef enum NeighbourNodeEnum
{
	// The remote computer can be a leaf, or an ultrapeer or hub, and so can we
	ntNode, // We are both Gnutella ultrapeers or Gnutella2 hubs
	ntHub,  // We are a leaf, and this connection is to a Gnutella ultrapeer or Gnutella2 hub above us
	ntLeaf  // We are a Gnutella ultrapeer or Gnutella2 hub, and this connection is to a leaf below us

} NrsNode;

// Make the m_nPongNeeded buffer an array of 32 bytes
const uchar PONG_NEEDED_BUFFER = 32;

// Define the CNeighbour class to inherit from CConnection, picking up a socket and methods to connect it and read data through it
class CNeighbour : public CConnection
{

// Construction
public:
	CNeighbour(PROTOCOLID nProtocol);
	CNeighbour(PROTOCOLID nProtocol, CNeighbour* pBase);
	virtual ~CNeighbour();

// Attributes: State
public:
	// Used by the list of neighbour objects in CNeighbours
	DWORD      m_nRunCookie; // The number of times this neighbour has been run, CNeighboursBase::OnRun uses this to run each neighbour in the list once
	DWORD      m_nUnique;    // A number, like 2, 3, 4 and so on, which is the unique key for this CNeighbour object in CNeighbour's m_pUniques map
	NrsState      m_nState;           // Neighbour state, like connecting, handshake 1, 2, or 3, or rejected
	CVendor*      m_pVendor;
	Hashes::Guid  m_oGUID;
	CGProfile*    m_pProfile;
	Hashes::Guid  m_oMoreResultsGUID; // GUID of the last search, used to get more results (do)

// Attributes: Capabilities
public:
	BOOL    m_bAutomatic;
	bool    m_bPeerProject;       // True if the remote computer is running PeerProject also
	NrsNode m_nNodeType;       // This connection is to a hub above us, ntHub, a leaf below us, ntLeaf, or a hub just like us, ntNode
	BOOL    m_bQueryRouting;
	BOOL    m_bPongCaching;
	BOOL    m_bVendorMsg;      // True if the remote computer told us it supports vendor-specific messages
	BOOL    m_bGGEP;
	DWORD   m_tLastQuery;      // The time we last got a query packet, recorded as the number of seconds since 1970
	BOOL    m_bBadClient;		// Is the remote client running a 'bad' client- GPL rip, buggy, etc. (not banned, though)

	DWORD	m_nDegree;					// "X-Degree: n" (-1 if not set)
	DWORD	m_nMaxTTL;					// "X-Max-TTL: n" (-1 if not set)
	BOOL	m_bDynamicQuerying;			// "X-Dynamic-Querying: 0.1" (default: false)
	BOOL	m_bUltrapeerQueryRouting;	// "X-Ultrapeer-Query-Routing: 0.1" (default: false)
	CString	m_sLocalePref;				// "X-Locale-Pref: en" ("" if not set)
	BOOL	m_bRequeries;				// "X-Requeries: false" (default: true)
	BOOL	m_bExtProbes;				// "X-Ext-Probes: 0.1" (default: false)

// Attributes: Statistics
public:
	DWORD m_nInputCount;
	DWORD m_nOutputCount;
	DWORD m_nDropCount;
	DWORD m_nLostCount;
	DWORD m_nOutbound;

	// If the remote computer sends us a pong packet it made, copy the sharing statistics here
	DWORD m_nFileCount;  // The number of files the remote computer is sharing, according to the pong packet it sent us
	DWORD m_nFileVolume; // The total size of all of those files, according to the same pong packet

// Attributes: Query Hash Tables
public:
	CQueryHashTable* m_pQueryTableRemote;
	CQueryHashTable* m_pQueryTableLocal;

// Attributes: Internals
protected:
	DWORD		m_tLastPacket;	// The time that we received the last packet
	CBuffer*	m_pZInput;		// The remote computer is sending compressed data, we'll save it in m_pInput, and then decompress it to here
	CBuffer*	m_pZOutput;		// We are sending the remote computer compressed data, we're writing it here, and then compressing it to m_pOutput
	DWORD		m_nZInput;		// The number of decompressed bytes of data the remote computer sent us
	DWORD		m_nZOutput;		// The number of not yet compressed bytes of data we've sent the remote computer
	z_streamp	m_pZSInput;		// Pointer to the zlib z_stream structure for decompression
	z_streamp	m_pZSOutput;	// Pointer to the zlib z_stream structure for compression
	BOOL		m_bZFlush;		// True to flush the compressed output buffer to the remote computer
	DWORD		m_tZOutput;		// The time that Zlib last compressed something

// Operations
public:
	virtual BOOL Send(CPacket* pPacket, BOOL bRelease = TRUE, BOOL bBuffered = FALSE);
	virtual void Close(UINT nError = IDS_CONNECTION_CLOSED);
	void         DelayClose(UINT nError = 0); // Send the buffer then close the socket, record the error given
	virtual BOOL SendQuery(CQuerySearch* pSearch, CPacket* pPacket, BOOL bLocal); // Validate query

protected:
	virtual BOOL OnRun();
	virtual void OnDropped();
	virtual BOOL OnRead();
	virtual BOOL OnWrite();
	virtual BOOL OnCommonHit(CPacket* pPacket);
	virtual BOOL OnCommonQueryHash(CPacket* pPacket);

public:
	DWORD GetMaxTTL() const;	// Get maximum TTL which is safe for both sides
	void GetCompression(float* pnInRate, float* pnOutRate);
};