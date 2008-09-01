//
// WorldGPS.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#if !defined(AFX_WORLDGPS_H__763AAF9D_7289_45A1_9026_4E2BF703C0E8__INCLUDED_)
#define AFX_WORLDGPS_H__763AAF9D_7289_45A1_9026_4E2BF703C0E8__INCLUDED_

#pragma once

class CWorldCountry;
class CWorldCity;
class CXMLElement;


class CWorldGPS
{
// Construction
public:
	CWorldGPS();
	~CWorldGPS();

// Attributes
public:
	CWorldCountry*	m_pCountry;
	DWORD			m_nCountry;

// Operations
public:
	BOOL		Load();
	void		Clear();
protected:
	void		Serialize(CArchive& ar);
	BOOL		LoadFrom(CXMLElement* pRoot);


};

class CWorldCountry
{
// Construction
public:
	CWorldCountry();
	~CWorldCountry();

// Attributes
public:
	CHAR		m_szID[2];
	CString		m_sName;
public:
	CWorldCity*	m_pCity;
	DWORD		m_nCity;

// Operations
public:
	void		Serialize(CArchive& ar);
	BOOL		LoadFrom(CXMLElement* pRoot);
	void		Clear();

};

class CWorldCity
{
// Attributes
public:
	CString		m_sName;
	CString		m_sState;
	float		m_nLatitude;
	float		m_nLongitude;

// Operations
public:
	void		Serialize(CArchive& ar);
	BOOL		LoadFrom(CXMLElement* pRoot);

};

#endif // !defined(AFX_WORLDGPS_H__763AAF9D_7289_45A1_9026_4E2BF703C0E8__INCLUDED_)
