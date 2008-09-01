//
// BTTrackerRequest.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#include "HttpRequest.h"

class CDownloadWithTorrent;
class CBENode;


class CBTTrackerRequest : public boost::noncopyable
{
public:
	CBTTrackerRequest(CDownloadWithTorrent* pDownload, LPCTSTR pszVerb, DWORD nNumWant, bool bProcess);
	virtual ~CBTTrackerRequest();

    static CString	Escape(const Hashes::BtHash& oBTH);
    static CString	Escape(const Hashes::BtGuid& oGUID);

protected:
	CDownloadWithTorrent*	m_pDownload;
	bool					m_bProcess;
	CHttpRequest			m_pRequest;

	void		Process(bool bRequest);
	void		Process(CBENode* pRoot);
	static UINT	ThreadStart(LPVOID pParam);
	void		OnRun();
};
