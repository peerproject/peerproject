//
// MatchListView.cpp
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "MatchListView.h"
#include "MatchObjects.h"
#include "QueryHit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CMatchListView, CComObject)

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

void CMatchListView::GetNext(POSITION& pos, CMatchFile** ppFile, CQueryHit** ppHit) const
{
	LPVOID pItem = pos ? m_pSelection.GetNext( pos ) : NULL;

	if ( ppFile != NULL )
	{
		if ( pItem && m_pList->m_pSelectedFiles.Find( static_cast< CMatchFile* >( pItem ) ) )
			*ppFile = static_cast< CMatchFile* >( pItem );
		else
			*ppFile = NULL;
	}

	if ( ppHit != NULL )
	{
		if ( pItem && m_pList->m_pSelectedHits.Find( static_cast< CQueryHit* >( pItem ) ) )
			*ppHit = static_cast< CQueryHit* >( pItem );
		else
			*ppHit = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// CMatchListView IGenericView

IMPLEMENT_DISPATCH(CMatchListView, GenericView)

STDMETHODIMP CMatchListView::XGenericView::get_Name(BSTR FAR* psName)
{
	METHOD_PROLOGUE( CMatchListView, GenericView )
	*psName = CComBSTR( pThis->m_sName ).Detach();
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

		CMatchFile* pFile = NULL;
		CQueryHit* pHit = NULL;
		pThis->GetNext( pos, &pFile, &pHit );
		if ( bThis )
		{
			if ( pFile )
			{
				pvItem->vt = VT_DISPATCH;
				pvItem->punkVal = pFile->GetDispatch( TRUE );
			}
			else if ( pHit )
			{
				pvItem->vt = VT_DISPATCH;
				pvItem->punkVal = pHit->GetDispatch( TRUE );
			}
			break;
		}
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

	if ( pceltFetched )
		*pceltFetched = 0;
	else if ( celt > 1 )
		return E_INVALIDARG;

	if ( m_pos == NULL )
		return S_FALSE;

	VariantInit( &rgvar[0] );

	CMatchFile* pFile = NULL;
	CQueryHit* pHit = NULL;
	pThis->GetNext( m_pos, &pFile, &pHit );
	if ( pFile )
	{
		rgvar[0].vt = VT_DISPATCH;
		rgvar[0].punkVal = pFile->GetDispatch( TRUE );
	}
	else if ( pHit )
	{
		rgvar[0].vt = VT_DISPATCH;
		rgvar[0].punkVal = pHit->GetDispatch( TRUE );
	}

	if ( pceltFetched ) (*pceltFetched)++;

	return S_OK;
}

STDMETHODIMP CMatchListView::XEnumVARIANT::Skip(ULONG celt)
{
	METHOD_PROLOGUE( CMatchListView, EnumVARIANT )

	while ( celt-- && m_pos )
		pThis->GetNext( m_pos, NULL, NULL );

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
