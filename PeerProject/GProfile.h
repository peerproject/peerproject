//
// GProfile.h
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

#if !defined(AFX_GPROFILE_H__16D49240_3116_441C_9C15_2D604E02B73E__INCLUDED_)
#define AFX_GPROFILE_H__16D49240_3116_441C_9C15_2D604E02B73E__INCLUDED_

#pragma once

class CXMLElement;
class CG2Packet;


class CGProfile : public CComObject
{
// Construction
public:
	CGProfile();
	virtual ~CGProfile();

// Attributes
public:
	CGuarded< Hashes::Guid > oGUID;
	CGuarded< Hashes::BtGuid > oGUIDBT;
protected:
	CXMLElement*	m_pXML;
protected:
	static LPCTSTR	xmlns;

// Operations
public:
	void			Create();
	void			Clear();
	BOOL			IsValid() const;
	CXMLElement*	GetXML(LPCTSTR pszElement = NULL, BOOL bCreate = FALSE);
public:
	void			Serialize(CArchive& ar);
	BOOL			Load(LPCTSTR pszFile = NULL);
	BOOL			Save(LPCTSTR pszFile = NULL);
	BOOL			FromXML(CXMLElement* pXML);
public:
	CString			GetNick() const;
	CString			GetLocation() const;
	CString			GetContact(LPCTSTR pszType) const;
	DWORD			GetPackedGPS() const;
	CG2Packet*		CreateAvatar();

// Interfaces
protected:
	DECLARE_INTERFACE_MAP()

};

extern CGProfile MyProfile;

#endif // !defined(AFX_GPROFILE_H__16D49240_3116_441C_9C15_2D604E02B73E__INCLUDED_)
