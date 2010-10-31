//
// EDPartImporter.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
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

#include "PeerProjectThread.h"

class CEDPartImporter : public CAppThread
{
public:
	CEDPartImporter();
	virtual ~CEDPartImporter();

	DECLARE_DYNAMIC(CEDPartImporter)

public:
	void	AddFolder(LPCTSTR pszFolder);
	void	Start(CEdit* pCtrl);
	void	Stop();
	BOOL	IsRunning();

protected:
	CList< CString >	m_pFolders;
	CEdit	*	m_pTextCtrl;
	int			m_nCount;

	void	ImportFolder(LPCTSTR pszPath);
	BOOL	ImportFile(LPCTSTR pszPath, LPCTSTR pszFile);
	BOOL	CopyFile(LPCTSTR pszSource, LPCTSTR pszTarget);
	void	Message(UINT nMessageID, ...);

	virtual int Run();

	DECLARE_MESSAGE_MAP()
};