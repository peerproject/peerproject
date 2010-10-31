//
// FragmentBar.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

class CDownload;
class CDownloadSource;
class CUpload;
class CUploadFile;


class CFragmentBar
{
public:
	static void DrawFragment(CDC* pDC, CRect* prcCell, QWORD nTotal, QWORD nOffset, QWORD nLength, COLORREF crFill, BOOL b3D);
	static void DrawStateBar(CDC* pDC, CRect* prcBar, QWORD nTotal, QWORD nOffset, QWORD nLength, COLORREF crFill, BOOL bTop = FALSE);
	static void DrawDownload(CDC* pDC, CRect* prcBar, CDownload* pDownload, COLORREF crNatural);
	static void DrawDownloadSimple(CDC* pDC, CRect* prcBar, CDownload* pDownload, COLORREF crNatural);
	static void DrawUpload(CDC* pDC, CRect* prcBar, CUploadFile* pFile, COLORREF crNatural);
// DrawSource moved to DownloadSource:
//	static void DrawSource(CDC* pDC, CRect* prcBar, CDownloadSource* pSource, COLORREF crNatural);
//protected:
//	static void DrawSourceImpl(CDC* pDC, CRect* prcBar, CDownloadSource* pSource);
};
