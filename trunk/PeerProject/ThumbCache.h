//
// ThumbCache.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2008.
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

class CImageFile;

class CThumbCache
{
public:
	static void InitDatabase();
	static BOOL	Load(LPCTSTR pszPath, CImageFile* pImage);
	static void Delete(LPCTSTR pszPath);
	static BOOL	Store(LPCTSTR pszPath, CImageFile* pImage);
	static BOOL Cache(LPCTSTR pszPath, CImageFile* pImage = NULL, BOOL bLoadFromFile = TRUE);
};
