//
// CtrlLibraryCollectionView.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "CtrlLibraryFileView.h"

class CCollectionFile;
class CLibraryFile;
class CWebCtrl;


class CLibraryCollectionView : public CLibraryFileView
{
// Construction
public:
	CLibraryCollectionView();
	virtual ~CLibraryCollectionView();

	DECLARE_DYNCREATE(CLibraryCollectionView)

// Attributes
protected:
	CWebCtrl*			m_pWebCtrl;
	DWORD				m_nWebIndex;
	CCollectionFile*	m_pCollection;
	Hashes::Sha1Hash	m_oSHA1;

	virtual BOOL Create(CWnd* pParentWnd);

// Operations
public:
	virtual BOOL	CheckAvailable(CLibraryTreeItem* pSel);
	virtual void	SelectAll() {}
	virtual void	Update();
protected:
	virtual DWORD_PTR HitTestIndex(const CPoint& /*point*/) const { return 0; };
	BOOL			ShowCollection(CLibraryFile* pFile);

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWebContextMenu(NMHDR* pNMHDR, LPARAM* pResult);
	afx_msg void OnUpdateLibraryFolderDownload(CCmdUI *pCmdUI);
	afx_msg void OnLibraryFolderDownload();
	afx_msg UINT OnGetDlgCode();

// Object Model
protected:
	class External : public CComObject
	{
	// Construction
	public:
		External();

	// Attributes
	public:
		CLibraryCollectionView* m_pView;
		BOOL m_bLockdown;
		void CheckLockdown();

	// Implementation
	protected:
		BEGIN_INTERFACE_PART(View, ICollectionHtmlView)
			DECLARE_DISPATCH()
			STDMETHOD(get_Application)(IApplication **ppApplication);
			STDMETHOD(Detect)(BSTR sURN, BSTR *psState);
			STDMETHOD(Hover)(BSTR sURN);
			STDMETHOD(Open)(BSTR sURN, VARIANT_BOOL *pbResult);
			STDMETHOD(Enqueue)(BSTR sURN, VARIANT_BOOL *pbResult);
			STDMETHOD(Download)(BSTR sURN, VARIANT_BOOL *pbResult);
			STDMETHOD(DownloadAll)();
			STDMETHOD(get_MissingCount)(LONG *pnCount);
		END_INTERFACE_PART(View)
		DECLARE_INTERFACE_MAP()
	} m_xExternal;
};
