//
// FictionBookImage.cs
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Shareaza is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)

using System.Xml.Serialization;

namespace Schemas
{
	[XmlType(Namespace = "http://www.gribuser.ru/xml/fictionbook/2.0")]
	[XmlRoot(ElementName = "FictionBook", Namespace = "http://www.gribuser.ru/xml/fictionbook/2.0", IsNullable = false)]
	public class FictionBookImage
	{
		/// <remarks>
		/// Any binary data that is required for the presentation of this book in base64 format.
		/// Currently only images are used.
		///</remarks>
		[XmlElement("binary")]
		public FictionBookBinary[] binary;	
	}
}
