//
// AutocompleteEdit.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2008.
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

class CRegEnum : public CComObject
{
public:
	CRegEnum();
	virtual ~CRegEnum();

	// Create IAutoComplete object and attach HWND to it
	//	szSection	: HKCU\SOFTWARE\{CompanyKey}\{ApplicationKey}\{szSection}
	//	szRoot		: Must contain inside a "%i" format specifier (n = 1,2..200)
	BOOL AttachTo(HWND hWnd, LPCTSTR szSection, LPCTSTR szRoot);

	// Add new string to autocomplete list
	void AddString(const CString& rString) const;

protected:
	BEGIN_INTERFACE_PART(EnumString, IEnumString)
		STDMETHOD(Next)(
			/* [in] */ ULONG celt,
			/* [length_is][size_is][out] */ LPOLESTR* rgelt,
			/* [out] */ ULONG *pceltFetched);
		STDMETHOD(Skip)(
			/* [in] */ ULONG celt);
		STDMETHOD(Reset)(void);
		STDMETHOD(Clone)(
			/* [out] */ IEnumString** ppenum);
	END_INTERFACE_PART(EnumString)

	DECLARE_DYNCREATE(CRegEnum)
	DECLARE_OLECREATE(CRegEnum)
	DECLARE_INTERFACE_MAP()

protected:
	CString						m_sect;
	CString						m_root;
	int							m_iter;
	CComPtr< IAutoComplete >	m_pIAutoComplete;
};

class CAutocompleteEdit : public CEdit
{
	DECLARE_DYNCREATE(CAutocompleteEdit)

public:
	CAutocompleteEdit();
	virtual ~CAutocompleteEdit();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
		LPCTSTR szSection, LPCTSTR szRoot);

	virtual int GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const;
	virtual void GetWindowText(CString& rString) const;

protected:
	CRegEnum*	m_pData;

	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};
