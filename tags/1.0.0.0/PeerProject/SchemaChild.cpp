//
// SchemaChild.cpp
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
#include "Settings.h"
#include "PeerProject.h"
#include "Schema.h"
#include "SchemaChild.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug


//////////////////////////////////////////////////////////////////////
// CSchemaChild construction

CSchemaChild::CSchemaChild(CSchemaPtr pSchema)
{
	m_pSchema	= pSchema;
	m_nType		= CSchema::stFile;
}

CSchemaChild::~CSchemaChild()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CSchemaChild load

BOOL CSchemaChild::Load(CXMLElement* pXML)
{
	m_sURI = pXML->GetAttributeValue( _T("location") );
	if ( m_sURI.IsEmpty() ) return FALSE;

	CString strType = pXML->GetAttributeValue( _T("type") );

	if ( strType == _T("folder") )
		m_nType = CSchema::stFolder;
	else if ( strType == _T("file") )
		m_nType = CSchema::stFile;
	else
		return FALSE;

	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pElement = pXML->GetNextElement( pos );

		if ( pElement->IsNamed( _T("identity") ) ||
			 pElement->IsNamed( _T("shared") ) )
		{
			CSchemaChildMap* pMap = new CSchemaChildMap();

			if ( pMap->Load( pElement ) )
			{
				m_pMap.AddTail( pMap );
			}
			else
			{
				delete pMap;
				return FALSE;
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSchemaChild clear

void CSchemaChild::Clear()
{
	for ( POSITION pos = m_pMap.GetHeadPosition() ; pos ; )
	{
		delete m_pMap.GetNext( pos );
	}
	m_pMap.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CSchemaChild member copy

BOOL CSchemaChild::MemberCopy(CXMLElement* pLocal, CXMLElement* pRemote, BOOL bToRemote, BOOL bAggressive)
{
	if ( ! pLocal || ! pRemote ) return FALSE;

	BOOL bChanged = FALSE;

	for ( POSITION pos = m_pMap.GetHeadPosition() ; pos ; )
	{
		CSchemaChildMap* pMap		= m_pMap.GetNext( pos );
		CXMLAttribute* pAttribute1	= NULL;
		CXMLAttribute* pAttribute2	= NULL;

		if ( bToRemote )
		{
			pAttribute1 = pLocal->GetAttribute( pMap->m_sLocal );
			pAttribute2 = pRemote->GetAttribute( pMap->m_sRemote );
		}
		else
		{
			pAttribute1 = pRemote->GetAttribute( pMap->m_sRemote );
			pAttribute2 = pLocal->GetAttribute( pMap->m_sLocal );
		}

		if ( pAttribute1 && ( ! pAttribute2 || bAggressive ) )
		{
			CString strValue( pAttribute1->GetValue() );

			if ( pMap->m_bIdentity )
				CXMLNode::UniformString( strValue );

			if ( bToRemote )
				pRemote->AddAttribute( pMap->m_sRemote, strValue );
			else
				pLocal->AddAttribute( pMap->m_sLocal, strValue );

			bChanged = TRUE;
		}
	}

	return bChanged;
}

//////////////////////////////////////////////////////////////////////
// CSchemaChildMap construction

CSchemaChildMap::CSchemaChildMap()
{
}

CSchemaChildMap::~CSchemaChildMap()
{
}

//////////////////////////////////////////////////////////////////////
// CSchemaChildMap operation

BOOL CSchemaChildMap::Load(CXMLElement* pXML)
{
	if ( pXML->IsNamed( _T("identity") ) )
		m_bIdentity = TRUE;
	else if ( pXML->IsNamed( _T("shared") ) )
		m_bIdentity = FALSE;
	else
		return FALSE;

	m_sLocal	= pXML->GetAttributeValue( _T("local") );
	m_sRemote	= pXML->GetAttributeValue( _T("remote") );

	if ( m_sLocal.IsEmpty() || m_sRemote.IsEmpty() ) return FALSE;

	return TRUE;
}
