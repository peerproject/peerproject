//
// PageProfileIdentity.cpp
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
#include "PageProfileIdentity.h"
#include "GProfile.h"
#include "Skin.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CIdentityProfilePage, CSettingsPage)

//BEGIN_MESSAGE_MAP(CIdentityProfilePage, CSettingsPage)
//END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CIdentityProfilePage property page

CIdentityProfilePage::CIdentityProfilePage()
	: CSettingsPage( CIdentityProfilePage::IDD )
//	, m_sNick	( _T("") )
//	, m_sFirst	( _T("") )
//	, m_sLast	( _T("") )
//	, m_sAge	( _T("") )
//	, m_sGender	( _T("") )
	, m_bBrowseUser	( FALSE )
{
}

CIdentityProfilePage::~CIdentityProfilePage()
{
}

void CIdentityProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROFILE_AGE, m_wndAge);
	DDX_CBString(pDX, IDC_PROFILE_AGE, m_sAge);
	DDX_CBString(pDX, IDC_PROFILE_GENDER, m_sGender);
	DDX_Text(pDX, IDC_PROFILE_NICK, m_sNick);
	DDX_Text(pDX, IDC_PROFILE_FIRST, m_sFirst);
	DDX_Text(pDX, IDC_PROFILE_LAST, m_sLast);
	DDX_Check(pDX, IDC_BROWSE_USER, m_bBrowseUser);
}

/////////////////////////////////////////////////////////////////////////////
// CIdentityProfilePage message handlers

BOOL CIdentityProfilePage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	if ( CXMLElement* pIdentity = MyProfile.GetXML( _T("identity") ) )
	{
		if ( CXMLElement* pHandle = pIdentity->GetElementByName( _T("handle") ) )
			m_sNick  = pHandle->GetAttributeValue( _T("primary") );

		if ( CXMLElement* pName = pIdentity->GetElementByName( _T("name") ) )
		{
			m_sFirst = pName->GetAttributeValue( _T("first") );
			m_sLast  = pName->GetAttributeValue( _T("last") );
		}
	}

	if ( CXMLElement* pVitals = MyProfile.GetXML( _T("vitals") ) )
	{
		m_sAge = pVitals->GetAttributeValue( _T("age") );

		CString strGenderMale, strGenderFemale;
		GetGenderTranslations( strGenderMale, strGenderFemale );

		m_sGender = pVitals->GetAttributeValue( _T("gender") );

		if ( ! m_sGender.IsEmpty() )
		{
			CComboBox* pGender = (CComboBox*) GetDlgItem( IDC_PROFILE_GENDER );
			if ( m_sGender.CompareNoCase( _T("male") ) == 0 )
				pGender->SelectString( -1, (LPCTSTR)strGenderMale );
			else if ( m_sGender.CompareNoCase( _T("female") ) == 0 )
				pGender->SelectString( -1, (LPCTSTR)strGenderFemale );
			else
				m_sGender.Empty();
		}

		int nAge = 0;

		if ( _stscanf( m_sAge, _T("%i"), &nAge ) == 1 && nAge > 0 )
			m_sAge.Format( _T("%i"), nAge );
		else
			m_sAge.Empty();
	}

	CString str;
	for ( int nAge = 10 ; nAge < 91 ; nAge++ )
	{
		str.Format( _T("%i"), nAge );
		m_wndAge.AddString( str );
	}

	m_bBrowseUser = Settings.Community.ServeProfile;

	UpdateData( FALSE );

	return TRUE;
}

void CIdentityProfilePage::OnOK()
{
	UpdateData();

	Settings.Community.ServeProfile = m_bBrowseUser != FALSE;

	if ( CXMLElement* pIdentity = MyProfile.GetXML( _T("identity"), TRUE ) )
	{
		if ( CXMLElement* pHandle = pIdentity->GetElementByName( _T("handle"), TRUE ) )
		{
			pHandle->AddAttribute( _T("primary"), m_sNick );
			if ( m_sNick.IsEmpty() )
				pHandle->Delete();
		}

		if ( CXMLElement* pName = pIdentity->GetElementByName( _T("name"), TRUE ) )
		{
			pName->AddAttribute( _T("first"), m_sFirst );
			pName->AddAttribute( _T("last"), m_sLast );
			if ( m_sFirst.IsEmpty() && m_sLast.IsEmpty() )
				pName->Delete();
		}

		if ( pIdentity->GetElementCount() == 0 )
			pIdentity->Delete();
	}

	if ( CXMLElement* pVitals = MyProfile.GetXML( _T("vitals"), TRUE ) )
	{
		int nAge = 0;

		if ( _stscanf( m_sAge, _T("%i"), &nAge ) == 1 && nAge > 0 )
			m_sAge.Format( _T("%i"), nAge );
		else
			m_sAge.Empty();

		CString strGenderMale, strGenderFemale;
		GetGenderTranslations( strGenderMale, strGenderFemale );

		if ( m_sGender.CompareNoCase( strGenderMale ) == 0 || m_sGender.CompareNoCase( _T("male") ) == 0 )
			pVitals->AddAttribute( _T("gender"), _T("Male") );
		else if ( m_sGender.CompareNoCase( strGenderFemale ) == 0 || m_sGender.CompareNoCase( _T("female") ) == 0 )
			pVitals->AddAttribute( _T("gender"), _T("Female") );
		else
			pVitals->DeleteAttribute( _T("gender") );

		if ( ! m_sAge.IsEmpty() )
			pVitals->AddAttribute( _T("age"), m_sAge );
		else
			pVitals->DeleteAttribute( _T("age") );

		if ( pVitals->GetElementCount() == 0 &&
			 pVitals->GetAttributeCount() == 0 )
			pVitals->Delete();
	}
}

void CIdentityProfilePage::GetGenderTranslations(CString& pMale, CString& pFemale)
{
	// Using data from CBrowseHostProfile.1 translation since
	// the control in the dialog may change its order and it does not have its identifier.

	BOOL bCollected = FALSE;

	CXMLElement* pXML = Skin.GetDocument( _T("CBrowseHostProfile.1") );

	for ( POSITION posGroup = pXML->GetElementIterator() ; posGroup && ! bCollected ; )
	{
		CXMLElement* pGroups = pXML->GetNextElement( posGroup );

		if ( pGroups->IsNamed( _T("group") ) && pGroups->GetAttributeValue( _T("id") ) == "3" )
		{
			for ( POSITION posText = pGroups->GetElementIterator() ; posText && ! bCollected ; )
			{
				CXMLElement* pText = pGroups->GetNextElement( posText );

				if ( pText->IsNamed( _T("text") ) )
				{
					CString strTemp;
					strTemp = pText->GetAttributeValue( _T("id") );
					if ( strTemp.CompareNoCase( _T("gendermale") ) == 0 )
					{
						pMale = pText->GetValue();
						bCollected = pFemale.IsEmpty() ? FALSE : TRUE;
					}
					else if ( strTemp.CompareNoCase( _T("genderfemale") ) == 0 )
					{
						pFemale = pText->GetValue();
						bCollected = pMale.IsEmpty() ? FALSE : TRUE;
					}
				}
			}
		}
	}
}
