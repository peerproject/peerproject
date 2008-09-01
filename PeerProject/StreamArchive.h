//
// StreamArchive.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2006.
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

class CStreamArchive : public CArchive
{
public:
	CStreamArchive(UINT nMode, int nBufSize = 4096, void* lpBuf = NULL) throw() :
		CArchive( ( ( m_pStreamFile = new COleStreamFile ),
			( m_pStreamFile ? m_pStreamFile->CreateMemoryStream() : NULL ), m_pStreamFile ),
			nMode, nBufSize, lpBuf )
	{
	}

	CStreamArchive(IStream* pIStream, UINT nMode, int nBufSize = 4096, void* lpBuf = NULL) throw() :
		CArchive( ( ( m_pStreamFile = new COleStreamFile ),
			( m_pStreamFile ? m_pStreamFile->Attach( pIStream ) : NULL ), m_pStreamFile ),
			nMode, nBufSize, lpBuf )
	{
	}

	virtual ~CStreamArchive() throw()
	{
		if ( m_pFile )
		{
			Close();
		}

		delete m_pStreamFile;
	}

	inline operator LPSTREAM() throw()
	{
		return m_pStreamFile ? m_pStreamFile->m_lpStream : NULL;
	}

	inline LPSTREAM Detach() throw()
	{
		if ( m_pFile )
		{
			Close();
		}
		
		LPSTREAM pStream = m_pStreamFile ? m_pStreamFile->Detach() : NULL;
		
		delete m_pStreamFile;
		m_pStreamFile = NULL;

		return pStream;
	}

	inline bool IsValid() const throw()
	{
		return ( m_pStreamFile && ( m_pStreamFile->m_lpStream != NULL ) );
	}

protected:
	COleStreamFile*	m_pStreamFile;
};
