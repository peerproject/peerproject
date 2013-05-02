//
// WizardProfilePage.cpp
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
#include "WizardProfilePage.h"
#include "GProfile.h"
#include "WorldGPS.h"
#include "Network.h"
#include "Skin.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CWizardProfilePage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardProfilePage, CWizardPage)
	ON_WM_XBUTTONDOWN()
	ON_CBN_SELCHANGE(IDC_LOC_COUNTRY, OnSelChangeCountry)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardProfilePage property page

CWizardProfilePage::CWizardProfilePage()
	: CWizardPage(CWizardProfilePage::IDD)
	, m_nAge	( 0 )
	, m_nGender	( 0 )
	, m_pWorld	( NULL )
{
}

CWizardProfilePage::~CWizardProfilePage()
{
	delete m_pWorld;
}

void CWizardProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROFILE_NICK, m_sNick);
	DDX_Control(pDX, IDC_LOC_CITY, m_wndCity);
	DDX_Control(pDX, IDC_LOC_COUNTRY, m_wndCountry);
	DDX_CBString(pDX, IDC_LOC_CITY, m_sLocCity);
	DDX_CBString(pDX, IDC_LOC_COUNTRY, m_sLocCountry);
	DDX_Control(pDX, IDC_PROFILE_AGE, m_wndAge);
	DDX_CBIndex(pDX, IDC_PROFILE_AGE, m_nAge);
	DDX_CBIndex(pDX, IDC_PROFILE_GENDER, m_nGender);
	DDX_Control(pDX, IDC_PROFILE_BIO, m_wndComments);
}

/////////////////////////////////////////////////////////////////////////////
// CWizardProfilePage message handlers

BOOL CWizardProfilePage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	Skin.Apply( _T("CWizardProfilePage"), this );

	for ( int nAge = 13 ; nAge < 91 ; nAge++ )
	{
		CString str, strYearsOld;
		LoadString( strYearsOld, IDS_WIZARD_YEARS_OLD );
		str.Format( _T(" %i ") + strYearsOld, nAge );
		m_wndAge.SetItemData( m_wndAge.AddString( str ), nAge );
	}

	if ( CXMLElement* pNotes = MyProfile.GetXML( _T("notes") ) )
		m_wndComments.SetWindowText( pNotes->GetValue() );

	m_pWorld = new CWorldGPS();
	m_pWorld->Load();

	CWorldCountry* pCountry = m_pWorld->m_pCountry;

	for ( int nCountry = m_pWorld->m_nCountry ; nCountry ; nCountry--, pCountry++ )
	{
		m_wndCountry.SetItemData( m_wndCountry.AddString( pCountry->m_sName ), (LPARAM)pCountry );
	}

	//UpdateData( FALSE );

	return TRUE;
}

BOOL CWizardProfilePage::OnSetActive()
{
	// Wizard Window Caption Workaround
	CString strCaption;
	GetWindowText( strCaption );
	GetParent()->SetWindowText( strCaption );

	SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );

	m_sNick = MyProfile.GetNick();

	if ( m_sNick.IsEmpty() )
	{
		TCHAR pBuffer[64];
		DWORD nSize = 64;
		if ( GetUserNameW( pBuffer, &nSize ) )
			m_sNick = pBuffer;
	}

	if ( CXMLElement* pVitals = MyProfile.GetXML( _T("vitals") ) )
	{
		CString strGender	= pVitals->GetAttributeValue( _T("gender") );
		CString strAge		= pVitals->GetAttributeValue( _T("age") );

		if ( strGender.CompareNoCase( _T("male") ) == 0 )
			m_nGender = 1;
		else if ( strGender.CompareNoCase( _T("female") ) == 0 )
			m_nGender = 2;

		int nAge = 0;
		_stscanf( strAge, _T("%i"), &nAge );

		for ( int nAgeItem = 0 ; nAgeItem < m_wndAge.GetCount() ; nAgeItem ++ )
		{
			if ( m_wndAge.GetItemData( nAgeItem ) == (DWORD)nAge )
			{
				m_nAge = nAgeItem;
				break;
			}
		}
	}

	if ( CXMLElement* pLocation = MyProfile.GetXML( _T("location") ) )
	{
		if ( CXMLElement* pPolitical = pLocation->GetElementByName( _T("political") ) )
		{
			m_sLocCountry	= pPolitical->GetAttributeValue( _T("country") );
			m_sLocCity		= pPolitical->GetAttributeValue( _T("city") ) + _T(", ")
							+ pPolitical->GetAttributeValue( _T("state") );
		}
	}

	if ( m_sLocCountry.IsEmpty() && Network.m_pHost.sin_addr.S_un.S_addr )
		m_sLocCountry = theApp.GetCountryName( Network.m_pHost.sin_addr );

	UpdateData( FALSE );

	OnSelChangeCountry();

	return CWizardPage::OnSetActive();
}

void CWizardProfilePage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		GetSheet()->PressButton( PSBTN_BACK );
	else if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}

void CWizardProfilePage::OnSelChangeCountry()
{
	CWaitCursor pCursor;

	int nSel = m_wndCountry.GetCurSel();
	if ( nSel < 0 ) return;
	CWorldCountry* pCountry = (CWorldCountry*)m_wndCountry.GetItemData( nSel );
	if ( ! pCountry ) return;

	if ( m_wndCity.GetCount() )
		m_wndCity.ResetContent();

	CWorldCity* pCity = pCountry->m_pCity;
	CString strCity;

	for ( int nCity = pCountry->m_nCity ; nCity ; nCity--, pCity++ )
	{
		if ( ! pCity->m_sName.IsEmpty() && ! pCity->m_sState.IsEmpty() )
			strCity = pCity->m_sName + _T(", ") + pCity->m_sState;
		else if ( ! pCity->m_sName.IsEmpty() )
			strCity = pCity->m_sName;
		else if ( ! pCity->m_sState.IsEmpty() )
			strCity = pCity->m_sState;
		else
			continue;

		m_wndCity.SetItemData( m_wndCity.AddString( strCity ), (LPARAM)pCity );
	}
}

LRESULT CWizardProfilePage::OnWizardBack()
{
	return CWizardPage::OnWizardBack();
}

LRESULT CWizardProfilePage::OnWizardNext()
{
	UpdateData( TRUE );

	if ( CXMLElement* pIdentity = MyProfile.GetXML( _T("identity"), TRUE ) )
	{
		if ( CXMLElement* pHandle = pIdentity->GetElementByName( _T("handle"), TRUE ) )
		{
			pHandle->AddAttribute( _T("primary"), m_sNick );
			if ( m_sNick.IsEmpty() )
				pHandle->Delete();
		}
	}

	if ( CXMLElement* pNotes = MyProfile.GetXML( _T("notes"), TRUE ) )
	{
		CString str;
		m_wndComments.GetWindowText( str );

		if ( ! str.IsEmpty() )
			pNotes->SetValue( str );
		//else
		//	pNotes->Delete();
	}

	if ( CXMLElement* pVitals = MyProfile.GetXML( _T("vitals"), TRUE ) )
	{
		if ( m_nAge < 10 )
		{
			pVitals->DeleteAttribute( _T("age") );
		}
		else
		{
			CString strAge;
			strAge.Format( _T("%i"), m_wndAge.GetItemData( m_nAge ) );
			pVitals->AddAttribute( _T("age"), strAge );
		}

		if ( m_nGender == 1 || m_nGender == 3 )
			pVitals->AddAttribute( _T("gender"), _T("male") );
		else if ( m_nGender == 2 || m_nGender == 4 )
			pVitals->AddAttribute( _T("gender"), _T("female") );
		else
			pVitals->DeleteAttribute( _T("gender") );

		if ( pVitals->GetElementCount() == 0 &&
			 pVitals->GetAttributeCount() == 0 ) pVitals->Delete();
	}

	if ( CXMLElement* pLocation = MyProfile.GetXML( _T("location"), TRUE ) )
	{
		if ( CXMLElement* pPolitical = pLocation->GetElementByName( _T("political"), TRUE ) )
		{
			if ( ! m_sLocCountry.IsEmpty() )
				pPolitical->AddAttribute( _T("country"), m_sLocCountry );
			else
				pPolitical->DeleteAttribute( _T("country") );

			int nPos = m_sLocCity.Find( _T(", ") );

			if ( nPos >= 0 )
			{
				pPolitical->AddAttribute( _T("city"), m_sLocCity.Left( nPos ) );
				pPolitical->AddAttribute( _T("state"), m_sLocCity.Mid( nPos + 2 ) );
			}
			else if ( ! m_sLocCity.IsEmpty() )
			{
				pPolitical->AddAttribute( _T("city"), m_sLocCity );
				pPolitical->DeleteAttribute( _T("state") );
			}
			else
			{
				pPolitical->DeleteAttribute( _T("city") );
				pPolitical->DeleteAttribute( _T("state") );
			}

			if ( pPolitical->GetElementCount() == 0 && pPolitical->GetAttributeCount() == 0 )
				pPolitical->Delete();
		}

		if ( CXMLElement* pCoordinates = pLocation->GetElementByName( _T("coordinates"), TRUE ) )
		{
			int nSel = m_wndCity.GetCurSel();
			CWorldCity* pCity = (CWorldCity*)m_wndCity.GetItemData( m_wndCity.GetCurSel() );

			if ( nSel >= 0 && pCity != NULL )
			{
				CString strValue;

				strValue.Format( _T("%f"), pCity->m_nLatitude );
				pCoordinates->AddAttribute( _T("latitude"), strValue );

				strValue.Format( _T("%f"), pCity->m_nLongitude );
				pCoordinates->AddAttribute( _T("longitude"), strValue );
			}
			else
			{
				pCoordinates->Delete();
			}
		}

		if ( pLocation->GetElementCount() == 0 )
			pLocation->Delete();
	}

	// Annoying popups.  This information is underutilized, do not put so much emphasis.

	//if ( MyProfile.GetNick().IsEmpty() )
	//{
	//	MsgBox( IDS_PROFILE_NO_NICK, MB_ICONEXCLAMATION );
	//	return -1;
	//}
	//if ( MyProfile.GetXML( _T("vitals") ) == NULL )
	//{
	//	if ( MsgBox( IDS_PROFILE_NO_VITALS, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 ) != IDYES ) return -1;
	//}
	//if ( MyProfile.GetLocation().IsEmpty() )
	//{
	//	if ( MsgBox( IDS_PROFILE_NO_LOCATION, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 ) != IDYES ) return -1;
	//}

	MyProfile.Save();

	return 0;
}
