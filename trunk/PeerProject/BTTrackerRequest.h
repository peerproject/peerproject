//
// BTTrackerRequest.h
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

#include "HttpRequest.h"

class CDownloadWithTorrent;
class CBENode;


class CBTTrackerRequest
{
public:
	CBTTrackerRequest(CDownloadWithTorrent* pDownload, LPCTSTR pszVerb, DWORD nNumWant, bool bProcess);
	virtual ~CBTTrackerRequest();

	static CString	Escape(const Hashes::BtHash& oBTH);
	static CString	Escape(const Hashes::BtGuid& oGUID);

	void Cancel()
	{
		m_pRequest.Cancel();
	}

protected:
	CDownloadWithTorrent*	m_pDownload;
	CHttpRequest			m_pRequest;
	bool					m_bProcess;

	void		Process(bool bRequest);
	void		Process(CBENode* pRoot);
	static UINT	ThreadStart(LPVOID pParam);
	void		OnRun();
};
