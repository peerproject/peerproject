//
// MatchListView.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#pragma once

class CMatchList;
class CMatchFile;
class CQueryHit;


class CMatchListView : public CComObject
{
	DECLARE_DYNAMIC(CMatchListView)

public:
	CMatchListView(LPCTSTR pszName, CMatchList* pList);
	virtual ~CMatchListView();

	static IGenericView* Attach(LPCTSTR pszName, CMatchList* pList);

protected:
	CString			m_sName;
	CMatchList*		m_pList;
	CList< void* >	m_pSelection;

	POSITION	GetIterator() const;
	INT_PTR		GetCount() const { return m_pSelection.GetCount(); }
	void		GetNext(POSITION& pos, CMatchFile** ppFile, CQueryHit** ppHit) const;

// Automation
protected:
	BEGIN_INTERFACE_PART(GenericView, IGenericView)
		DECLARE_DISPATCH()
		STDMETHOD(get_Name)(BSTR FAR* psName);
		STDMETHOD(get_Unknown)(IUnknown FAR* FAR* ppUnknown);
		STDMETHOD(get_Param)(LONG FAR* pnParam);
		STDMETHOD(get_Count)(LONG FAR* pnCount);
		STDMETHOD(get_Item)(VARIANT vIndex, VARIANT FAR* pvItem);
		STDMETHOD(get__NewEnum)(IUnknown FAR* FAR* ppEnum);
	END_INTERFACE_PART(GenericView)

	BEGIN_INTERFACE_PART(EnumVARIANT, IEnumVARIANT)
		STDMETHOD(Next)(THIS_ DWORD celt, VARIANT FAR* rgvar, DWORD FAR* pceltFetched);
		STDMETHOD(Skip)(THIS_ DWORD celt);
		STDMETHOD(Reset)(THIS);
		STDMETHOD(Clone)(THIS_ IEnumVARIANT FAR* FAR* ppenum);
		POSITION m_pos;
	END_INTERFACE_PART(EnumVARIANT)

	DECLARE_INTERFACE_MAP()
};
