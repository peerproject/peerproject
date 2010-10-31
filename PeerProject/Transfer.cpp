//
// Transfer.cpp
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "Transfers.h"
#include "Transfer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename


//////////////////////////////////////////////////////////////////////
// CTransfer construction

CTransfer::CTransfer(PROTOCOLID nProtocol)
	: CConnection		( nProtocol )
	, m_nRunCookie		( 0 )
{
}

CTransfer::~CTransfer()
{
	ASSERT( ! IsValid() );
	if ( IsValid() ) Close();
}

//////////////////////////////////////////////////////////////////////
// CTransfer operations

BOOL CTransfer::ConnectTo(const IN_ADDR* pAddress, WORD nPort)
{
	if ( CConnection::ConnectTo( pAddress, nPort ) )
	{
		Transfers.Add( this );
		return TRUE;
	}

	return FALSE;
}

void CTransfer::AttachTo(CConnection* pConnection)
{
	CConnection::AttachTo( pConnection );
	Transfers.Add( this );
}

void CTransfer::Close()
{
	Transfers.Remove( this );
	CConnection::Close();
}

//////////////////////////////////////////////////////////////////////
// CTransfer HTTP headers

void CTransfer::ClearHeaders()
{
	m_pHeaderName.RemoveAll();
	m_pHeaderValue.RemoveAll();
}

BOOL CTransfer::OnHeaderLine(CString& strHeader, CString& strValue)
{
	m_pHeaderName.Add( strHeader );
	m_pHeaderValue.Add( strValue );

	return CConnection::OnHeaderLine( strHeader, strValue );
}
