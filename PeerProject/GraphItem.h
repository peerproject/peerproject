//
// GraphItem.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2007.
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

typedef struct
{
	DWORD	m_nCode;
	UINT	m_nStringID;
	UINT	m_nUnits;
	float	m_nMultiplier;
} GRAPHITEM;


class CGraphItem
{
// Construction
public:
	CGraphItem(DWORD nCode = 0, float nMultiplier = 1.0f, COLORREF nColor = RGB(255,255,255));
	~CGraphItem();

// Attributes
public:
	COLORREF	m_nColor;
	DWORD		m_nCode;
	float		m_nMultiplier;

	COLORREF	m_cPen[4];
	CPen		m_pPen[4];
	CString		m_sName;

	DWORD*		m_pData;
	DWORD		m_nData;
	DWORD		m_nLength;
	DWORD		m_nPosition;

	static const GRAPHITEM	m_pItemDesc[];

// Operations
public:
	void		Clear();
	void		SetCode(DWORD nCode);
	DWORD		Add(DWORD nValue);
	DWORD		GetValueAt(DWORD nPosition) const;
	DWORD		GetMaximum() const;
	void		SetHistory(DWORD nSize, BOOL bMax = FALSE);
	void		Serialize(CArchive& ar);
	void		MakeGradient(COLORREF crBack);
	DWORD		Update();

	static QWORD GetValue(const DWORD nCode, const float nMultiplier = 1.0f);
	static const GRAPHITEM* GetItemDesc(const DWORD nCode);
};

#define GRC_TOTAL_BANDWIDTH_IN			1
#define GRC_TOTAL_BANDWIDTH_OUT			2
#define GRC_TOTAL_BANDWIDTH_TCP_IN		3
#define GRC_TOTAL_BANDWIDTH_TCP_OUT		4
#define GRC_TOTAL_BANDWIDTH_UDP_IN		5
#define GRC_TOTAL_BANDWIDTH_UDP_OUT		6

#define GRC_GNUTELLA_CONNECTIONS		11
#define GRC_GNUTELLA_CONNECTIONS_ALL	12
#define GRC_GNUTELLA_BANDWIDTH_IN		13
#define GRC_GNUTELLA_BANDWIDTH_OUT		14
#define GRC_GNUTELLA_PACKETS_IN			15
#define GRC_GNUTELLA_PACKETS_OUT		16

#define GRC_DOWNLOADS_FILES				21
#define GRC_DOWNLOADS_TRANSFERS			22
#define GRC_DOWNLOADS_BANDWIDTH			23

#define GRC_UPLOADS_TRANSFERS			31
#define GRC_UPLOADS_BANDWIDTH			32

#define GRC_GNUTELLA_ROUTED				41
#define GRC_GNUTELLA_DROPPED			42
#define GRC_GNUTELLA_LOST				43
#define GRC_GNUTELLA_QUERIES			44

#define GRC_GNUTELLA_PINGS				51
#define GRC_GNUTELLA_PONGS				52

#define GRC_CONNECTION_ERRORS			61

#define GRC_RANDOM						100
