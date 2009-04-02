//
// MatchListView.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "MatchObjects.h"
#include "QueryHit.h"
#include "MatchListView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_INTERFACE_MAP(CMatchListView, CComObject)
	INTERFACE_PART(CMatchListView, IID_IGenericView, GenericView)
	INTERFACE_PART(CMatchListView, IID_IEnumVARIANT, EnumVARIANT)
END_INTERFACE_MAP()


//////////////////////////////////////////////////////////////////////
// CMatchListView construction

CMatchListView::CMatchListView(LPCTSTR pszName, CMatchList* pList)
{
	EnableDispatch( IID_IGenericView );

	m_sName = pszName;
	m_pList = pList;
	m_pList->m_pSection.Lock();

	for ( POSITION pos = m_pList->m_pSelectedFiles.GetHeadPosition() ; pos ; )
	{
		m_pSelection.AddTail( m_pList->m_pSelectedFiles.GetNext( pos ) );
	}

	for ( POSITION pos = m_pList->m_pSelectedHits.GetHeadPosition() ; pos ; )
	{
		m_pSelection.AddTail( m_pList->m_pSelectedHits.GetNext( pos ) );
	}
}

CMatchListView::~CMatchListView()
{
	m_pList->m_pSection.Unlock();
}

//////////////////////////////////////////////////////////////////////
// CMatchListView operations

IGenericView* CMatchListView::Attach(LPCTSTR pszName, CMatchList* pList)
{
	CMatchListView* pView = new CMatchListView( pszName, pList );
	return (IGenericView*)pView->GetInterface( IID_IGenericView, FALSE );
}

POSITION CMatchListView::GetIterator() const
{
	return m_pSelection.GetHeadPosition();
}

// TODO
void CMatchListView::GetNext(POSITION& pos, CMatchFile** ppFile, CQueryHit** ppHit) const
{
	LPVOID pItem = (LPVOID)( pos != NULL ? m_pSelection.GetNext( pos ) : NULL );

	if ( ppFile != NULL )
	{
		if ( m_pList->m_pSelectedFiles.Find( static_cast< CMatchFile* >( pItem ) ) != NULL )
		{
			*ppFile = static_cast< CMatchFile* >( pItem );
		}
	}
	else
	{
		*ppFile = NULL;
	}

	if ( ppHit != NULL )
	{
		if ( m_pList->m_pSelectedHits.Find( static_cast< CQueryHit* >( pItem ) ) != NULL )
		{
			*ppHit = static_cast< CQueryHit* >( pItem );
		}
	}
	else
	{
		*ppHit = NULL;
	}
}

void CMatchListView::GetNext(POSITION& pos, VARIANT* pVar) const
{
	CMatchFile* pFile;
	CQueryHit* pHit;

	GetNext( pos, &pFile, &pHit );
	if ( pVar == NULL ) return;
	
    const Hashes::TigerHash& rTiger = pFile ? pFile->m_oTiger : pHit->m_oTiger;
    const Hashes::Sha1Hash& rSHA1 = pFile ? pFile->m_oSHA1 : pHit->m_oSHA1;
    const Hashes::Ed2kHash& rED2K = pFile ? pFile->m_oED2K : pHit->m_oED2K;
	const Hashes::BtHash& rBTH = pFile ? pFile->m_oBTH : pHit->m_oBTH;
	const Hashes::Md5Hash& rMD5 = pFile ? pFile->m_oMD5 : pHit->m_oMD5;
	
	CString strURN;
	VariantClear( pVar );
	
	if ( rSHA1 && rTiger )
	{
		strURN	= _T("urn:bitprint:")
                + rSHA1.toString() + '.'
				+ rTiger.toString();
	}
	else if ( rSHA1 )
	{
		strURN = rSHA1.toUrn();
	}
	else if ( rTiger )
	{
		strURN = rTiger.toUrn();
	}
	else if ( rED2K )
	{
		strURN = rED2K.toUrn();
	}
	else if ( rBTH )
	{
		strURN = rBTH.toUrn();
	}
	else if ( rMD5 )
	{
		strURN = rMD5.toUrn();
	}
	else
	{
		return;
	}

	pVar->vt		= VT_BSTR;
	pVar->bstrVal	= strURN.AllocSysString();
}

//////////////////////////////////////////////////////////////////////
// CMatchListView IGenericView

IMPLEMENT_DISPATCH(CMatchListView, GenericView)

STDMETHODIMP CMatchListView::XGenericView::get_Name(BSTR FAR* psName)
{
	METHOD_PROLOGUE( CMatchListView, GenericView )
	pThis->m_sName.SetSysString( psName );
	return S_OK;
}

STDMETHODIMP CMatchListView::XGenericView::get_Unknown(IUnknown FAR* FAR* /*ppUnknown*/)
{
	METHOD_PROLOGUE( CMatchListView, GenericView )
	return E_NOTIMPL;
}

STDMETHODIMP CMatchListView::XGenericView::get_Param(LONG FAR* /*pnParam*/)
{
	METHOD_PROLOGUE( CMatchListView, GenericView )
	return E_NOTIMPL;
}

STDMETHODIMP CMatchListView::XGenericView::get__NewEnum(IUnknown FAR* FAR* ppEnum)
{
	METHOD_PROLOGUE( CMatchListView, GenericView )
	*ppEnum = &pThis->m_xEnumVARIANT;
	pThis->m_xEnumVARIANT.m_pos = pThis->GetIterator();
	AddRef();
	return S_OK;
}

STDMETHODIMP CMatchListView::XGenericView::get_Item(VARIANT vIndex, VARIANT FAR* pvItem)
{
	METHOD_PROLOGUE( CMatchListView, GenericView )

	VARIANT va;
	VariantInit( &va );
	VariantClear( pvItem );

	if ( FAILED( VariantChangeType( &va, (VARIANT FAR*)&vIndex, 0, VT_I4 ) ) ) return E_INVALIDARG;

	if ( va.lVal < 0 || va.lVal >= pThis->GetCount() ) return S_OK;

	for ( POSITION pos = pThis->GetIterator() ; pos ; )
	{
		BOOL bThis = ( va.lVal-- == 0 );

		pThis->GetNext( pos, bThis ? pvItem : NULL );
		if ( bThis ) break;
	}

	return S_OK;
}

STDMETHODIMP CMatchListView::XGenericView::get_Count(LONG FAR* pnCount)
{
	METHOD_PROLOGUE( CMatchListView, GenericView )
	*pnCount = static_cast< LONG >( pThis->GetCount() );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMatchListView IEnumVARIANT enumerator

IMPLEMENT_UNKNOWN(CMatchListView, EnumVARIANT)

STDMETHODIMP CMatchListView::XEnumVARIANT::Next(ULONG celt, VARIANT FAR* rgvar, ULONG FAR* pceltFetched)
{
	METHOD_PROLOGUE( CMatchListView, EnumVARIANT )

	if ( pceltFetched ) *pceltFetched = 0;
	else if ( celt > 1 ) return E_INVALIDARG;

	if ( m_pos == NULL ) return S_FALSE;

	VariantInit( &rgvar[0] );
	pThis->GetNext( m_pos, rgvar );

	if ( pceltFetched ) (*pceltFetched)++;

	return S_OK;
}

STDMETHODIMP CMatchListView::XEnumVARIANT::Skip(ULONG celt)
{
    METHOD_PROLOGUE( CMatchListView, EnumVARIANT )

	while ( celt-- && m_pos ) pThis->GetNext( m_pos, NULL, NULL );

    return ( celt == 0 ? S_OK : S_FALSE );
}

STDMETHODIMP CMatchListView::XEnumVARIANT::Reset()
{
    METHOD_PROLOGUE( CMatchListView, EnumVARIANT )
	m_pos = pThis->GetIterator();
    return S_OK;
}

STDMETHODIMP CMatchListView::XEnumVARIANT::Clone(IEnumVARIANT FAR* FAR* /*ppenum*/)
{
    return E_NOTIMPL;
}
