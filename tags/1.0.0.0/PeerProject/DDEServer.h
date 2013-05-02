//
// DDEServer.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once


class CDDEServer
{
public:
	CDDEServer(LPCTSTR pszService);
	virtual ~CDDEServer();

public:
	BOOL	Create();
	void	Close();

protected:
	static CDDEServer*	m_pServer;
	DWORD				m_hInstance;
	HSZ					m_hszService;
	CString				m_sService;

	CString				StringFromHsz(HSZ hsz);
	static CString		ReadArgument(LPCTSTR& pszMessage);
	static FNCALLBACK	DDECallback;
	virtual BOOL		CheckAccept(LPCTSTR pszTopic);
	virtual BOOL		Execute(LPCTSTR pszTopic, HDDEDATA hData);
	virtual BOOL		Execute(LPCTSTR pszTopic, LPCVOID pData, DWORD nLength);
	virtual BOOL		Execute(LPCTSTR pszTopic, LPCTSTR pszMessage);
};

extern CDDEServer DDEServer;
