//
// HGlobal.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

template < typename T > class CHGlobal
{
public:
	CHGlobal( SIZE_T dwBytes  = sizeof( T ) ) throw() :
		m_phGlobal( GlobalAlloc( GHND, dwBytes ) ),
		m_pCached( NULL )
	{
	}

	CHGlobal( HGLOBAL hglobIn ) throw() :
		m_phGlobal( NULL ),
		m_pCached( NULL )
	{
		if( hglobIn )
		{
			SIZE_T cb = GlobalSize( hglobIn );
			LPVOID pvIn = GlobalLock( hglobIn );
			if ( pvIn )
			{
				m_phGlobal = GlobalAlloc( GHND, cb );
				if ( m_phGlobal )
				{
					LPVOID pvOut = GlobalLock( m_phGlobal );
					if ( pvOut )
					{
						CopyMemory( pvOut, pvIn, cb );
					}
					GlobalUnlock( m_phGlobal );
				}
				GlobalUnlock( hglobIn );
			}
		}
	}

	virtual ~CHGlobal() throw()
	{
		Clean();
	}

	inline void Clean() throw()
	{
		if ( m_phGlobal )
		{
			if ( m_pCached )
			{
				GlobalUnlock( m_phGlobal );
				m_pCached = NULL;
			}
			GlobalFree( m_phGlobal );
			m_phGlobal = NULL;
		}
	}

	inline operator T*() throw()
	{
		if ( m_phGlobal )
		{
			if ( ! m_pCached )
			{
				m_pCached = reinterpret_cast < T* > ( GlobalLock( m_phGlobal ) );
			}
			return m_pCached;
		}
		return NULL;
	}

	inline T* operator ->() throw()
	{
		return operator T*();
	}

	inline bool IsValid() const throw()
	{
		return ( m_phGlobal != NULL );
	}

	inline operator HGLOBAL() throw()
	{
		return m_phGlobal;
	}

	inline HGLOBAL Detach() throw()
	{
		if ( m_phGlobal )
		{
			if ( m_pCached )
			{
				GlobalUnlock( m_phGlobal );
				m_pCached = NULL;
			}
		}
		HGLOBAL hGlobal = m_phGlobal;
		m_phGlobal = NULL;
		return hGlobal;
	}

	inline SIZE_T Size() const throw()
	{
		if ( m_phGlobal )
		{
			return GlobalSize( m_phGlobal );
		}
		return 0;
	}

protected:
	HGLOBAL	m_phGlobal;
	T*		m_pCached;
};
