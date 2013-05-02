//
// PeerProjectDataSource.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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

#pragma once

class CLibraryList;
class CLibraryTreeItem;
class CAlbumFolder;

#define CF_PEERPROJECT			_T("InPeerProjectDragLoop")
#define CF_PEERPROJECT_ALBUMS	_T("PeerProjectAlbums")
#define CF_PEERPROJECT_FILES	_T("PeerProjectFiles")

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectDataSource

class CPeerProjectDataSource : public CComObject
{
	DECLARE_DYNAMIC(CPeerProjectDataSource)

public:
	CPeerProjectDataSource();
	virtual ~CPeerProjectDataSource();

public:
	// Perform CLibraryList drag operation
	// pList - list item being dragged
	// pImage - bitmap of item
	// oGUID - GUID of parent folder ( only for albums )
	static HRESULT DoDragDrop(const CLibraryList* pList, HBITMAP pImage, const Hashes::Guid& oGUID, const CPoint& ptOffset);

	// Perform CLibraryTreeItem drag operation
	// pList - tree item being dragged
	// pImage - bitmap of item
	// oGUID - GUID of parent folder ( only for albums )
	static HRESULT DoDragDrop(const CLibraryTreeItem* pList, HBITMAP pImage, const Hashes::Guid& oGUID, const CPoint& ptOffset);

	// Get CFSTR_SHELLURL as string from data object
	static HRESULT ObjectToURL(IDataObject* pObject, CString& str);

	// Get CF_HDROP as string list from data object
	static HRESULT ObjectToFiles(IDataObject* pObject, CList < CString >& oFiles);

	// Put CFSTR_PERFORMEDDROPEFFECT value to data object
	static HRESULT SetDropEffect(IDataObject* pObject, DWORD dwEffect);

	// Perform basic file operations (copy or move)
	static BOOL DropToFolder(IDataObject* pDataObj, DWORD grfKeyState, DWORD* pdwEffect, BOOL bDrop, LPCTSTR pszDest);

	// Perform basic album operations (copy or move)
	static BOOL DropToAlbum(IDataObject* pDataObj, DWORD grfKeyState, DWORD* pdwEffect, BOOL bDrop, CAlbumFolder* pAlbumFolder);

	// Test if object is a PeerProjects drag-n-drop object
	static BOOL IsPeerProjectObject(IDataObject* pDataObj);

protected:
	typedef struct
	{
		FORMATETC					fe;
		STGMEDIUM					stgm;
	} DATAENTRY, *LPDATAENTRY;

// Obsolete:
//	template < typename T > class THREADDATA
//	{
//	public:
//		THREADDATA(const T* pList, HBITMAP pImage, const Hashes::Guid& oGUID) :
//			m_pList (pList),
//			m_pImage (pImage),
//			m_oGUID (oGUID),
//			m_hReady (CreateEvent( NULL, TRUE, FALSE, NULL ))
//		{
//		}
//		const T*					m_pList;
//		HBITMAP						m_pImage;
//		Hashes::Guid				m_oGUID;
//		HANDLE						m_hReady;
//	};

	LPDATAENTRY						m_rgde;		// Array of active DATAENTRY entries
	int								m_cde;		// Size of m_rgde
	CMutex							m_pSection;	// Guard
	CComPtr < IDragSourceHelper >	m_pdsh;

	BEGIN_INTERFACE_PART(DropSource, IDropSource)
		virtual ~XDropSource() {}
		STDMETHOD(QueryContinueDrag)(BOOL fEscapePressed, DWORD grfKeyState);
		STDMETHOD(GiveFeedback)(DWORD dwEffect);
	END_INTERFACE_PART(DropSource)

	BEGIN_INTERFACE_PART(DataObject, IDataObject)
		virtual ~XDataObject() {}
		STDMETHOD(GetData)(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
		STDMETHOD(GetDataHere)(FORMATETC* pformatetc, STGMEDIUM* pmedium);
		STDMETHOD(QueryGetData)(FORMATETC* pformatetc);
		STDMETHOD(GetCanonicalFormatEtc)(FORMATETC* pformatectIn, FORMATETC* pformatetcOut);
		STDMETHOD(SetData)(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease);
		STDMETHOD(EnumFormatEtc)(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc);
		STDMETHOD(DAdvise)(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);
		STDMETHOD(DUnadvise)(DWORD dwConnection);
		STDMETHOD(EnumDAdvise)(IEnumSTATDATA **ppenumAdvise);
		CComPtr < IDataAdviseHolder > m_spDataAdviseHolder;
	END_INTERFACE_PART(DataObject)

	BEGIN_INTERFACE_PART(DragSourceHelper, IDragSourceHelper)
		virtual ~XDragSourceHelper() {}
		STDMETHOD (InitializeFromBitmap)(LPSHDRAGIMAGE pshdi, IDataObject* pDataObject);
		STDMETHOD (InitializeFromWindow)(HWND hwnd, POINT* ppt, IDataObject* pDataObject);
	END_INTERFACE_PART(DragSourceHelper)

	// Clear all data
	void Clean();

	// Perform universal drag operation
	template < typename T > static UINT DragDropThread(LPVOID param);
	template < typename T > static HRESULT DoDragDropHelper(const T* pList, HBITMAP pImage, const Hashes::Guid& oGUID, const CPoint& ptOffset);

	// Add CF_PEERPROJECT
	static HRESULT	Add(IDataObject* pIDataObject);

	// Add medias by IDragSourceHelper
	static HRESULT	Add(IDataObject* pIDataObject, HBITMAP pImage, const CPoint& ptOffset);

	// Add CF_HDROP/CF_PEERPROJECT_ALBUMS/CF_PEERPROJECT_FILES
	template < typename T > static HRESULT AddFiles(IDataObject* pIDataObject, const T* pSelFirst, const Hashes::Guid& oGUID);

	// Find data in data cahe (and allocate new if not found but need)
	//	Returns:
	//	S_OK - added new entry			DV_E_DVTARGETDEVICE - not supported format
	//	S_FALSE - found old entry		DV_E_FORMATETC - not found
	//	E_OUTOFMEMORY - out of memory	DV_E_TYMED - found but different storage type
	HRESULT	FindFORMATETC(FORMATETC *pfe, LPDATAENTRY *ppde, BOOL fAdd);

	// Add data to data cache
	HRESULT	AddRefStgMedium(STGMEDIUM *pstgmIn, STGMEDIUM *pstgmOut, BOOL fCopyIn);

	// Service methods
	static IUnknown* GetCanonicalIUnknown(IUnknown *punk);
	static void GetTotalLength(const CLibraryList* pList, size_t& size_HDROP, size_t& size_Archive, size_t& size_Files, BOOL bRoot);
	static void GetTotalLength(const CLibraryTreeItem* pSelFirst, size_t& size_HDROP, size_t& size_Archive, size_t& size_Files, BOOL bRoot);
	static void FillBuffer(const CLibraryList* pList, LPTSTR& buf_HDROP, CArchive& buf_Archive, LPBYTE& buf_Files, CString& buf_Text, BOOL bRoot, const Hashes::Guid& oGUID);
	static void FillBuffer(const CLibraryTreeItem* pSelFirst, LPTSTR& buf_HDROP, CArchive& buf_Archive, LPBYTE& buf_Files, CString& buf_Text, BOOL bRoot, const Hashes::Guid& oGUID);

	DECLARE_OLECREATE(CPeerProjectDataSource)

	DECLARE_INTERFACE_MAP()
};

#ifdef _DEBUG
	void DumpIDataObject(IDataObject* pIDataObject);
#else	// No Debug
	#define DumpIDataObject __noop
#endif	// Debug

/////////////////////////////////////////////////////////////////////////////
// Drop target helpers

// Place in class declaration
#define DECLARE_DROP() \
public:\
	virtual BOOL OnDrop(IDataObject* pDataObj, DWORD grfKeyState, POINT ptScreen, DWORD* pdwEffect, BOOL bDrop);\
	BEGIN_INTERFACE_PART(DropTarget, IDropTarget) \
		virtual ~XDropTarget(){} \
		STDMETHOD(DragEnter)(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect); \
		STDMETHOD(DragOver)(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect); \
		STDMETHOD(DragLeave)(); \
		STDMETHOD(Drop)(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect); \
		CComPtr< IDropTargetHelper >	m_spdth; \
		CComPtr< IDataObject >			m_pDataObj; \
	END_INTERFACE_PART(DropTarget)\
	DECLARE_INTERFACE_MAP()

// Place in class definition
#define IMPLEMENT_DROP(class_name, base_class_name) \
	BEGIN_INTERFACE_MAP(class_name, base_class_name) \
		INTERFACE_PART(class_name, IID_IDropTarget, DropTarget) \
	END_INTERFACE_MAP() \
	IMPLEMENT_UNKNOWN(class_name, DropTarget) \
	STDMETHODIMP class_name::XDropTarget::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect) \
	{ \
		METHOD_PROLOGUE( class_name, DropTarget ) \
		DumpIDataObject( pDataObj ); \
		m_pDataObj = pDataObj; \
		HRESULT hr = m_spdth.CoCreateInstance( CLSID_DragDropHelper ); \
		if ( FAILED( hr ) ) return hr; \
		POINT point = { ptl.x, ptl.y }; \
		if ( ! pThis->OnDrop( m_pDataObj, grfKeyState, point, pdwEffect, FALSE ) ) \
			*pdwEffect = DROPEFFECT_NONE; \
		if ( m_spdth ) m_spdth->DragEnter( pThis->GetSafeHwnd(), pDataObj, &point, *pdwEffect ); \
		return S_OK; \
	} \
	STDMETHODIMP class_name::XDropTarget::DragOver(DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect) \
	{ \
		METHOD_PROLOGUE( class_name, DropTarget ) \
		POINT point = { ptl.x, ptl.y }; \
		if ( ! pThis->OnDrop( m_pDataObj, grfKeyState, point, pdwEffect, FALSE ) ) \
			*pdwEffect = DROPEFFECT_NONE; \
		if ( m_spdth ) m_spdth->DragOver( &point, *pdwEffect ); \
		return S_OK; \
	} \
	STDMETHODIMP class_name::XDropTarget::DragLeave() \
	{ \
		METHOD_PROLOGUE( class_name, DropTarget ) \
		POINT point = { 0, 0 }; \
		pThis->OnDrop( NULL, 0, point, 0, FALSE ); \
		if ( m_spdth ) m_spdth->DragLeave(); \
		m_pDataObj.Release(); \
		m_spdth.Release(); \
		return S_OK; \
	} \
	STDMETHODIMP class_name::XDropTarget::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect) \
	{ \
		METHOD_PROLOGUE( class_name, DropTarget ) \
		POINT point = { ptl.x, ptl.y }; \
		if ( ! pThis->OnDrop( pDataObj, grfKeyState, point, pdwEffect, TRUE ) ) \
			*pdwEffect = DROPEFFECT_NONE; \
		if ( m_spdth ) m_spdth->Drop( pDataObj, &point, *pdwEffect ); \
		m_pDataObj.Release(); \
		m_spdth.Release(); \
		return S_OK; \
	}

// Place in OnCreate method of class
#define ENABLE_DROP() \
	RegisterDragDrop( GetSafeHwnd(), &m_xDropTarget );

// Place in OnDestroy method of class
#define DISABLE_DROP() \
	RevokeDragDrop( GetSafeHwnd() );
