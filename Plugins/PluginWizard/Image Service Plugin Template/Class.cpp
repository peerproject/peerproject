//
// Class.cpp : Implementation of CClass
//
// #COPYRIGHT#
// This file is part of PeerProject (peerproject.org) © 2008
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

#include "StdAfx.h"
#include "Class.h"

STDMETHODIMP C#PROJECT#::LoadFromFile (
	/* [in] */ BSTR sFile,
	/* [in,out] */ IMAGESERVICEDATA* pParams,
	/* [out] */ SAFEARRAY** ppImage )
{
	ATLTRACENOTIMPL ("C#PROJECT#::LoadFromFile");
}

STDMETHODIMP C#PROJECT#::LoadFromMemory (
	/* [in] */ BSTR sType,
	/* [in] */ SAFEARRAY* pMemory,
	/* [in,out] */ IMAGESERVICEDATA* pParams,
	/* [out] */ SAFEARRAY** ppImage )
{
	ATLTRACENOTIMPL ("C#PROJECT#::LoadFromMemory");
}

STDMETHODIMP C#PROJECT#::SaveToFile (
	/* [in] */ BSTR sFile,
	/* [in,out] */ IMAGESERVICEDATA* pParams,
	/* [in] */ SAFEARRAY* pImage)
{
	ATLTRACENOTIMPL ("C#PROJECT#::SaveToFile");
}

STDMETHODIMP C#PROJECT#::SaveToMemory (
	/* [in] */ BSTR sType,
	/* [out] */ SAFEARRAY** ppMemory,
	/* [in,out] */ IMAGESERVICEDATA* pParams,
	/* [in] */ SAFEARRAY* pImage)
{
	ATLTRACENOTIMPL ("C#PROJECT#::SaveToMemory");
}
