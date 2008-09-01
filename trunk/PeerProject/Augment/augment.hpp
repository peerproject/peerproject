////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// augment/augment.hpp                                                        //
//                                                                            //
// This file is part of PeerProject (peerproject.org) © 2008                  //
// Portions Copyright Shareaza Development Team, 2002-2007.                   //
//                                                                            //
// PeerProject is free software; you can redistribute it and/or               //
// modify it under the terms of the GNU General Public License                //
// as published by the Free Software Foundation; either version 3             //
// of the License, or later version (at your option).                         //
//                                                                            //
// PeerProject is distributed in the hope that it will be useful,             //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       //
// See the See the GNU General Public License for more details.               //
//                                                                            //
// You should have received a copy of the GNU General Public License 3.0          //
// along with PeerProject; if not, write to Free Software Foundation, Inc.    //
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef AUGMENT_AUGMENT_HPP_INCLUDED
#define AUGMENT_AUGMENT_HPP_INCLUDED

namespace augment
{

	template<typename T, typename U>
	T implicit_cast(U u)
	{
		return u;
	}

} // namespace augment

#include "auto_ptr.hpp"
#include "auto_array.hpp"
#include "com_ptr.hpp"
#include "IUnknownImplementation.hpp"

#endif // #ifndef AUGMENT_AUGMENT_HPP_INCLUDED
