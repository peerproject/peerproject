//
// PageProfileProfile.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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
#include "Settings.h"
#include "PeerProject.h"
#include "PageProfileProfile.h"
#include "Buffer.h"
#include "GProfile.h"
#include "WorldGPS.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CProfileProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CProfileProfilePage, CSettingsPage)
	ON_CBN_SELCHANGE(IDC_LOC_COUNTRY, OnSelChangeCountry)
	ON_CBN_SELCHANGE(IDC_LOC_CITY, OnSelChangeCity)
	ON_LBN_SELCHANGE(IDC_INTEREST_LIST, OnSelChangeInterestList)
	ON_CBN_SELCHANGE(IDC_INTEREST_ALL, OnSelChangeInterestAll)
	ON_CBN_EDITCHANGE(IDC_INTEREST_ALL, OnEditChangeInterestAll)
	ON_BN_CLICKED(IDC_INTEREST_ADD, OnInterestAdd)
	ON_BN_CLICKED(IDC_INTEREST_REMOVE, OnInterestRemove)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProfileProfilePage property page

CProfileProfilePage::CProfileProfilePage()
	: CSettingsPage( CProfileProfilePage::IDD )
	, m_pWorld			( NULL )
//	, m_sLocCity		( _T("") )
//	, m_sLocCountry 	( _T("") )
//	, m_sLocLatitude	( _T("") )
//	, m_sLocLongitude	( _T("") )
{
}

CProfileProfilePage::~CProfileProfilePage()
{
	if ( m_pWorld ) delete m_pWorld;
}

void CProfileProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INTEREST_LIST, m_wndInterestList);
	DDX_Control(pDX, IDC_INTEREST_ALL, m_wndInterestAll);
	DDX_Control(pDX, IDC_INTEREST_ADD, m_wndInterestAdd);
	DDX_Control(pDX, IDC_INTEREST_REMOVE, m_wndInterestRemove);
	DDX_Control(pDX, IDC_LOC_CITY, m_wndCity);
	DDX_Control(pDX, IDC_LOC_COUNTRY, m_wndCountry);
	DDX_CBString(pDX, IDC_LOC_CITY, m_sLocCity);
	DDX_CBString(pDX, IDC_LOC_COUNTRY, m_sLocCountry);
	DDX_Text(pDX, IDC_LOC_LAT, m_sLocLatitude);
	DDX_Text(pDX, IDC_LOC_LONG, m_sLocLongitude);
}

/////////////////////////////////////////////////////////////////////////////
// CProfileProfilePage message handlers

BOOL CProfileProfilePage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	m_pWorld = new CWorldGPS();
	m_pWorld->Load();

	CWorldCountry* pCountry = m_pWorld->m_pCountry;

	for ( int nCountry = m_pWorld->m_nCountry ; nCountry ; nCountry--, pCountry++ )
	{
		m_wndCountry.SetItemData( m_wndCountry.AddString( pCountry->m_sName ), (LPARAM)pCountry );
	}

	if ( CXMLElement* pLocation = MyProfile.GetXML( _T("location") ) )
	{
		if ( CXMLElement* pPolitical = pLocation->GetElementByName( _T("political") ) )
		{
			m_sLocCountry	= pPolitical->GetAttributeValue( _T("country") );
			m_sLocCity		= pPolitical->GetAttributeValue( _T("city") );
			CString str		= pPolitical->GetAttributeValue( _T("state") );
			if ( ! str.IsEmpty() && ! m_sLocCity.IsEmpty() )
				m_sLocCity += _T(", ");
			m_sLocCity += str;
		}

		if ( CXMLElement* pCoordinates = pLocation->GetElementByName( _T("coordinates") ) )
		{
			CString str = pCoordinates->GetAttributeValue( _T("latitude") );
			float nValue;

			if ( _stscanf( str, _T("%f"), &nValue ) == 1 )
			{
				m_sLocLatitude.Format( nValue >= 0 ?
					_T("%.2f\xb0 N") : _T("%.2f\xb0 S"), double( fabs( nValue ) ) );
			}

			str = pCoordinates->GetAttributeValue( _T("longitude") );

			if ( _stscanf( str, _T("%f"), &nValue ) == 1 )
			{
				m_sLocLongitude.Format( nValue >= 0 ? _T("%.1f\xb0 E") : _T("%.1f\xb0 W"),
					double( fabs( nValue ) ) );
			}
		}
	}

	if ( CXMLElement* pInterests = MyProfile.GetXML( _T("interests") ) )
	{
		for ( POSITION pos = pInterests->GetElementIterator() ; pos ; )
		{
			CXMLElement* pInterest = pInterests->GetNextElement( pos );

			if ( pInterest->IsNamed( _T("interest") ) )
				m_wndInterestList.AddString( pInterest->GetValue() );
		}
	}

	LoadDefaultInterests();

	UpdateData( FALSE );

	m_wndInterestAdd.EnableWindow( FALSE );
	m_wndInterestRemove.EnableWindow( FALSE );

	OnSelChangeCountry();
	RecalcDropWidth( &m_wndCountry );

	return TRUE;
}

int CProfileProfilePage::LoadDefaultInterests()
{
	int nCount = 0;
	const CString strFile = Settings.General.Path + _T("\\Data\\Interests.dat");	// Settings.General.DataPath ?

	CFile pFile;
	if ( ! pFile.Open( strFile, CFile::modeRead ) )
		return nCount;

	try
	{
		CString strLine;
		CString strLang = _T(" ") + Settings.General.Language;
		CBuffer pBuffer;

		pBuffer.EnsureBuffer( (DWORD)pFile.GetLength() );
		pBuffer.m_nLength = (DWORD)pFile.GetLength();
		pFile.Read( pBuffer.m_pBuffer, pBuffer.m_nLength );
		pFile.Close();

		// Format: Delineated List, enabled by prespecified #languages:	#start en ... #end en
		// (Allows multiple/nested/overlapped languages, all applicable results displayed alphabetically)

		BOOL bActive = FALSE;

		while ( pBuffer.ReadLine( strLine ) )
		{
			if ( strLine.GetLength() < 2 ) continue;		// Blank line

			if ( strLine.GetAt( 0 ) == '#' )				// Language start/end line
			{
				if ( strLine.Find( strLang, 4 ) < 1 && strLine.Find( _T(" all"), 4 ) < 1 )
				{
					if ( strLine.Left( 10 ) == _T("#languages") )
						strLang = _T(" en");
				}
				else if ( strLine.Left( 6 ) == _T("#start") || strLine.Left( 6 ) == _T("#begin") )
					bActive = TRUE;
				else if ( strLine.Left( 4 ) == _T("#end") )
					bActive = FALSE;	//break;

				continue;
			}

			if ( ! bActive ) continue;						// Disinterested language

			if ( strLine.Find( _T("\t") ) > 0 ) 			// Trim at whitespace (remove any comments)
				strLine.Left( strLine.Find( _T("\t") ) );

			nCount++;
			m_wndInterestAll.AddString( strLine );
		}
	}
	catch ( CException* pException )
	{
		if ( pFile.m_hFile != CFile::hFileNull )
			pFile.Close();	// File is still open so close it
		pException->Delete();
	}

	return nCount;
}

void CProfileProfilePage::OnSelChangeCountry()
{
	CWaitCursor pCursor;

	int nSel = m_wndCountry.GetCurSel();
	if ( nSel < 0 ) return;
	CWorldCountry* pCountry = (CWorldCountry*)m_wndCountry.GetItemData( nSel );
	if ( ! pCountry ) return;

	if ( m_wndCity.GetCount() ) m_wndCity.ResetContent();

	CWorldCity* pCity = pCountry->m_pCity;
	CString strCity;

	for ( int nCity = pCountry->m_nCity ; nCity ; nCity--, pCity++ )
	{
		if ( ! pCity->m_sName.IsEmpty() )
		{
			if ( ! pCity->m_sState.IsEmpty() )
				strCity = pCity->m_sName + _T(", ") + pCity->m_sState;
			else
				strCity = pCity->m_sName;
		}
		else if ( ! pCity->m_sState.IsEmpty() )
		{
			strCity = pCity->m_sState;
		}
		else
		{
			continue;
		}
		m_wndCity.SetItemData( m_wndCity.AddString( strCity ), (LPARAM)pCity );
	}
	RecalcDropWidth( &m_wndCity );
}

void CProfileProfilePage::OnSelChangeCity()
{
	int nSel = m_wndCity.GetCurSel();
	if ( nSel < 0 ) return;
	CWorldCity* pCity = (CWorldCity*)m_wndCity.GetItemData( nSel );
	if ( ! pCity ) return;

	m_sLocLatitude.Format( pCity->m_nLatitude >= 0 ? _T("%.2f� N") : _T("%.2f� S"),
		double( fabs( pCity->m_nLatitude ) ) );

	m_sLocLongitude.Format( pCity->m_nLongitude >= 0 ? _T("%.1f� E") : _T("%.1f� W"),
		double( fabs( pCity->m_nLongitude ) ) );

	GetDlgItem( IDC_LOC_LAT )->SetWindowText( m_sLocLatitude );
	GetDlgItem( IDC_LOC_LONG )->SetWindowText( m_sLocLongitude );
}

void CProfileProfilePage::OnSelChangeInterestList()
{
	m_wndInterestRemove.EnableWindow( m_wndInterestList.GetCurSel() >= 0 );
}

void CProfileProfilePage::OnSelChangeInterestAll()
{
	m_wndInterestAdd.EnableWindow( m_wndInterestAll.GetCurSel() >= 0 || m_wndInterestAll.GetWindowTextLength() > 0 );
}

void CProfileProfilePage::OnEditChangeInterestAll()
{
	m_wndInterestAdd.EnableWindow( m_wndInterestAll.GetCurSel() >= 0 || m_wndInterestAll.GetWindowTextLength() > 0 );
}

void CProfileProfilePage::OnInterestAdd()
{
	CString str;
	m_wndInterestAll.GetWindowText( str );
	m_wndInterestList.AddString( str );
}

void CProfileProfilePage::OnInterestRemove()
{
	int nItem = m_wndInterestList.GetCurSel();
	if ( nItem >= 0 ) m_wndInterestList.DeleteString( nItem );
	m_wndInterestRemove.EnableWindow( FALSE );
}

void CProfileProfilePage::OnOK()
{
	UpdateData();

	if ( CXMLElement* pLocation = MyProfile.GetXML( _T("location"), TRUE ) )
	{
		if ( CXMLElement* pPolitical = pLocation->GetElementByName( _T("political"), TRUE ) )
		{
			if ( ! m_sLocCountry.IsEmpty() )
			{
				pPolitical->AddAttribute( _T("country"), m_sLocCountry );
			}
			else if ( CXMLAttribute* pAttr = pPolitical->GetAttribute( _T("country") ) )
			{
				pAttr->Delete();
			}

			int nPos = m_sLocCity.Find( _T(", ") );

			if ( nPos >= 0 )
			{
				pPolitical->AddAttribute( _T("city"), m_sLocCity.Left( nPos ) );
				pPolitical->AddAttribute( _T("state"), m_sLocCity.Mid( nPos + 2 ) );
			}
			else if ( ! m_sLocCity.IsEmpty() )
			{
				pPolitical->AddAttribute( _T("city"), m_sLocCity );
				if ( CXMLAttribute* pAttr = pPolitical->GetAttribute( _T("state") ) )
					pAttr->Delete();
			}
			else
			{
				if ( CXMLAttribute* pAttr = pPolitical->GetAttribute( _T("city") ) )
					pAttr->Delete();
				if ( CXMLAttribute* pAttr = pPolitical->GetAttribute( _T("state") ) )
					pAttr->Delete();
			}

			if ( pPolitical->GetElementCount() == 0 && pPolitical->GetAttributeCount() == 0 )
				pPolitical->Delete();
		}

		if ( CXMLElement* pCoordinates = pLocation->GetElementByName( _T("coordinates"), TRUE ) )
		{
			CString strValue;
			float nValue = 0;

			if ( _stscanf( m_sLocLatitude, _T("%f"), &nValue ) == 1 )
			{
				if ( m_sLocLatitude.Find( 'S' ) >= 0 ) nValue *= -1;
				strValue.Format( _T("%f"), double( nValue ) );
				pCoordinates->AddAttribute( _T("latitude"), strValue );
			}

			if ( _stscanf( m_sLocLongitude, _T("%f"), &nValue ) == 1 )
			{
				if ( m_sLocLongitude.Find( 'W' ) >= 0 ) nValue *= -1;
				strValue.Format( _T("%f"), double( nValue ) );
				pCoordinates->AddAttribute( _T("longitude"), strValue );
			}

			if ( nValue == 0 ) pCoordinates->Delete();
		}

		if ( pLocation->GetElementCount() == 0 ) pLocation->Delete();
	}

	if ( CXMLElement* pInterests = MyProfile.GetXML( _T("interests"), TRUE ) )
	{
		pInterests->DeleteAllElements();

		for ( int nItem = 0 ; nItem < m_wndInterestList.GetCount() ; nItem++ )
		{
			CString str;
			m_wndInterestList.GetText( nItem, str );
			CXMLElement* pInterest = pInterests->AddElement( _T("interest") );
			pInterest->SetValue( str );
		}

		if ( pInterests->GetElementCount() == 0 )
			pInterests->Delete();
	}

	CSettingsPage::OnOK();
}
