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

#pragma once

class CXMLElement;
class CG2Packet;


class CGProfile : public CComObject
{
// Construction
public:
	CGProfile();
	virtual ~CGProfile();

	// Gnutella GUID (128 bit)
	CGuarded< Hashes::Guid >	oGUID;
	// BitTorrent GUID (160 bit), first 128 bits same as oGUID
	CGuarded< Hashes::BtGuid >	oGUIDBT;

	void			Create();							// Create default local profile
	BOOL			Load();								// Get local profile from file at PeerProject start up
	BOOL			Save();								// Save local profile to file
	BOOL			FromXML(const CXMLElement* pXML);	// Create remote profile from XML
	void			Serialize(CArchive& ar);			// Load/Save browsed host profile
	BOOL			IsValid() const;

	CXMLElement*	GetXML(LPCTSTR pszElement = NULL, BOOL bCreate = FALSE);
	CString			GetNick() const;
	CString			GetContact(LPCTSTR pszType) const;
	CString			GetLocation() const;
	DWORD			GetPackedGPS() const;

	CG2Packet*		CreateAvatar() const;

protected:
	CXMLElement*	m_pXML;
	static LPCTSTR	xmlns;

	void			CreateBT();		// Create BitTorrent GUID from Gnutella GUID

	DECLARE_INTERFACE_MAP()
};

extern CGProfile MyProfile;
