//
// Hresults.cs
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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

using System;

namespace PeerProject
{
	internal sealed class Hresults
	{
		public const int E_FAIL = unchecked((int)0x80004005);
        public const int E_INVALIDARG = unchecked((int)0x80070057);
		public const int E_UNEXPECTED = unchecked((int)0x8000FFFF);
		public const int E_NOINTERFACE = unchecked((int)0x80004002);
		public const int E_OUTOFMEMORY = unchecked((int)0x8007000E);
		public const int E_NOTIMPL = unchecked((int)0x80004001);
		public const int S_OK = 0;
		public const int S_FALSE = 1;
	}
}
