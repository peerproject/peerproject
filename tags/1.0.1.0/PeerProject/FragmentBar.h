//
// FragmentBar.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
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

#pragma once

class CDownload;
class CDownloadSource;
class CDownloadDisplayData;
class CSourceDisplayData;
class CUpload;
class CUploadFile;
class CUploadDisplayData;


class CFragmentBar
{
public:
	static void DrawFragment(CDC* pDC, CRect* prcCell, QWORD nTotal, QWORD nOffset, QWORD nLength, COLORREF crFill, BOOL b3D);
	static void DrawStateBar(CDC* pDC, CRect* prcBar, QWORD nTotal, QWORD nOffset, QWORD nLength, COLORREF crFill, BOOL bTop = FALSE);
//	static void DrawDownloadSimple(CDC* pDC, CRect* prcBar, QWORD nSize, QWORD nProgress, COLORREF crNatural);
	static void DrawSource(CDC* pDC, CRect* prcBar, const CSourceDisplayData* pSourceData, COLORREF crNatural, BOOL bDrawEmpty = TRUE);
	static void DrawDownload(CDC* pDC, CRect* prcBar, const CDownloadDisplayData* pDownloadData, COLORREF crNatural);
	static void DrawDownload(CDC* pDC, CRect* prcBar, const CDownload* pDownload, COLORREF crNatural);	// Legacy locking method
	static void DrawUpload(CDC* pDC, CRect* prcBar, const CUploadDisplayData* pUploadData, COLORREF crNatural);
	static void DrawUpload(CDC* pDC, CRect* prcBar, CUploadFile* pFile, COLORREF crNatural);			// Legacy locking method
// Legacy DrawSource moved to DownloadSource:
//	static void DrawSource(CDC* pDC, CRect* prcBar, CDownloadSource* pSource, COLORREF crNatural);
//	static void DrawSourceImpl(CDC* pDC, CRect* prcBar, CDownloadSource* pSource);
};
