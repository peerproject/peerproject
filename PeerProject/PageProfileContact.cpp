//
// PageProfileContact.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
#include "PageProfileContact.h"
#include "GProfile.h"
#include "CoolInterface.h"
#include "XML.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CContactProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CContactProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CContactProfilePage)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CContactProfilePage property page

CContactProfilePage::CContactProfilePage() : CSettingsPage( CContactProfilePage::IDD )
{
	//{{AFX_DATA_INIT(CContactProfilePage)
	//}}AFX_DATA_INIT
}

CContactProfilePage::~CContactProfilePage()
{
}

void CContactProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CContactProfilePage)
	DDX_Text(pDX, IDC_PROFILE_EMAIL, m_sEmail);
	DDX_Text(pDX, IDC_PROFILE_MSN, m_sMSN);
	DDX_Text(pDX, IDC_PROFILE_YAHOO, m_sYahoo);
	DDX_Text(pDX, IDC_PROFILE_AOL, m_sAOL);
	DDX_Text(pDX, IDC_PROFILE_ICQ, m_sICQ);
	DDX_Text(pDX, IDC_PROFILE_JABBER, m_sJabber);
	DDX_Text(pDX, IDC_PROFILE_TWITTER, m_sTwitter);
	DDX_Text(pDX, IDC_PROFILE_FACEBOOK, m_sFacebook);
	DDX_Text(pDX, IDC_PROFILE_PEERPROJECTORG, m_sPeerProjectOrg);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CContactProfilePage message handlers

BOOL CContactProfilePage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	if ( CXMLElement* pContacts = MyProfile.GetXML( _T("contacts") ) )
	{
		for ( POSITION pos1 = pContacts->GetElementIterator() ; pos1 ; )
		{
			CXMLElement* pGroup = pContacts->GetNextElement( pos1 );

			if ( pGroup->IsNamed( _T("group") ) )
			{
				CString strGroup = pGroup->GetAttributeValue( _T("class") );

				if ( CXMLElement* pAddress = pGroup->GetElementByName( _T("address") ) )
				{
					CString strAddress = pAddress->GetAttributeValue( _T("content") );

					if ( strGroup.CompareNoCase( _T("email") ) == 0 )
						m_sEmail = strAddress;
					else if ( strGroup.CompareNoCase( _T("msn") ) == 0 )
						m_sMSN = strAddress;
					else if ( strGroup.CompareNoCase( _T("yahoo") ) == 0 )
						m_sYahoo = strAddress;
					else if ( strGroup.CompareNoCase( _T("icq") ) == 0 )
						m_sICQ = strAddress;
					else if ( strGroup.CompareNoCase( _T("aol") ) == 0 )
						m_sAOL = strAddress;
					else if ( strGroup.CompareNoCase( _T("google") ) == 0 )
						m_sJabber = strAddress;
					else if ( strGroup.CompareNoCase( _T("jabber") ) == 0 )
						m_sJabber = strAddress;
					else if ( strGroup.CompareNoCase( _T("twitter") ) == 0 )
						m_sTwitter = strAddress;
					else if ( strGroup.CompareNoCase( _T("facebook") ) == 0 )
						m_sFacebook = strAddress;
					else if ( strGroup.CompareNoCase( _T("peerproject.org") ) == 0 )
						m_sPeerProjectOrg = strAddress;
					else if ( strGroup.CompareNoCase( _T("peerproject") ) == 0 )
						m_sPeerProjectOrg = strAddress;
					else
						MsgBox( _T("\nUnrecognized contact:  ") + strGroup );	// Should never happen
				}
			}
		}
	}

	if ( m_sEmail.Find( '@' ) < 0 || m_sEmail.Find( '.' ) < 0 )
		m_sEmail.Empty();

	UpdateData( FALSE );

	return TRUE;
}

void CContactProfilePage::OnPaint()
{
	CPaintDC dc( this );
	CRect rc( 0,0,0,0 );

	GetDlgItem( IDC_PROFILE_EMAIL )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	CoolInterface.Draw( &dc, IDI_MAIL, 16, rc.left - 20, rc.top + 2 );

	GetDlgItem( IDC_PROFILE_PEERPROJECTORG )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	CoolInterface.Draw( &dc, IDR_MAINFRAME, 16, rc.left - 20, rc.top + 2 );

	GetDlgItem( IDC_PROFILE_FACEBOOK )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	CoolInterface.Draw( &dc, IDI_CONTACT_FACEBOOK, 16, rc.left - 20, rc.top + 2 );

	GetDlgItem( IDC_PROFILE_TWITTER )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	CoolInterface.Draw( &dc, IDI_CONTACT_TWITTER, 16, rc.left - 20, rc.top + 2 );

	GetDlgItem( IDC_PROFILE_JABBER )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	CoolInterface.Draw( &dc, IDI_CONTACT_GOOGLE, 16, rc.left - 20, rc.top + 2 );

	GetDlgItem( IDC_PROFILE_MSN )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	CoolInterface.Draw( &dc, IDI_CONTACT_MSN, 16, rc.left - 20, rc.top + 2 );

	GetDlgItem( IDC_PROFILE_YAHOO )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	CoolInterface.Draw( &dc, IDI_CONTACT_YAHOO, 16, rc.left - 20, rc.top + 2 );

	GetDlgItem( IDC_PROFILE_AOL )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	CoolInterface.Draw( &dc, IDI_CONTACT_AOL, 16, rc.left - 20, rc.top + 2 );

	GetDlgItem( IDC_PROFILE_ICQ )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	CoolInterface.Draw( &dc, IDI_CONTACT_ICQ, 16, rc.left - 20, rc.top + 2 );
}

void CContactProfilePage::OnOK()
{
	UpdateData();

	if ( m_sEmail.Find( '@' ) < 0 || m_sEmail.Find( '.' ) < 0 )
		m_sEmail.Empty();

	AddAddress( _T("Email"), _T("Primary"), m_sEmail );
	AddAddress( _T("MSN"), _T("Primary"), m_sMSN );
	AddAddress( _T("Yahoo"), _T("Primary"), m_sYahoo );
	AddAddress( _T("ICQ"), _T("Primary"), m_sICQ );
	AddAddress( _T("AOL"), _T("Primary"), m_sAOL );
	AddAddress( _T("Jabber"), _T("Primary"), m_sJabber );
	AddAddress( _T("Twitter"), _T("Primary"), m_sTwitter );
	AddAddress( _T("Facebook"), _T("Primary"), m_sFacebook );
	AddAddress( _T("PeerProject.org"), _T("Primary"), m_sPeerProjectOrg );
}

void CContactProfilePage::AddAddress(LPCTSTR pszClass, LPCTSTR pszName, LPCTSTR pszAddress)
{
	if ( CXMLElement* pContacts = MyProfile.GetXML( _T("contacts"), TRUE ) )
	{
		for ( POSITION pos1 = pContacts->GetElementIterator() ; pos1 ; )
		{
			CXMLElement* pGroup = pContacts->GetNextElement( pos1 );

			if ( pGroup->IsNamed( _T("group") ) &&
				 pGroup->GetAttributeValue( _T("class") ).CompareNoCase( pszClass ) == 0 )
			{
				for ( POSITION pos2 = pGroup->GetElementIterator() ; pos2 ; )
				{
					CXMLElement* pAddress = pGroup->GetNextElement( pos2 );

					if ( pAddress->IsNamed( _T("address") ) &&
						 pAddress->GetAttributeValue( _T("name") ).CompareNoCase( pszName ) == 0 )
					{
						if ( pszAddress && *pszAddress )
						{
							pAddress->AddAttribute( _T("content"), pszAddress );
							return;
						}
						else
						{
							pAddress->Delete();
							break;
						}
					}
				}

				if ( pszAddress && *pszAddress )
				{
					CXMLElement* pAddress = pGroup->AddElement( _T("address") );
					pAddress->AddAttribute( _T("name"), pszName );
					pAddress->AddAttribute( _T("content"), pszAddress );
				}
				else if ( pGroup->GetElementCount() == 0 )
				{
					pGroup->Delete();
				}
				break;
			}
		}

		if ( pszAddress && *pszAddress )
		{
			CXMLElement* pGroup = pContacts->AddElement( _T("group") );
			pGroup->AddAttribute( _T("class"), pszClass );

			CXMLElement* pAddress = pGroup->AddElement( _T("address") );
			pAddress->AddAttribute( _T("name"), pszName );
			pAddress->AddAttribute( _T("content"), pszAddress );
		}
		else if ( pContacts->GetElementCount() == 0 )
		{
			pContacts->Delete();
		}
	}
}
