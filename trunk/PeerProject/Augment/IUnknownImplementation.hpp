////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// augment/IUnknownImplementation.hpp                                         //
//                                                                            //
// This file is part of PeerProject (peerproject.org) � 2008                  //
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

#ifndef AUGMENT_IUNKNOWNIMPLEMENTATION_HPP_INCLUDED
#define AUGMENT_IUNKNOWNIMPLEMENTATION_HPP_INCLUDED

namespace augment
{

	struct NoInterface : IUnknown {};

	template<typename I0, typename I1, typename I2, typename I3, typename I4,
			typename I5, typename I6, typename I7, typename I8, typename I9>
	struct InheritAll
		: I0, I1, I2, I3, I4, I5, I6, I7, I8, I9
	{};
	template<typename I0, typename I1, typename I2, typename I3, typename I4,
			typename I5, typename I6, typename I7, typename I8>
	struct InheritAll< I0, I1, I2, I3, I4, I5, I6, I7, I8, NoInterface >
		: I0, I1, I2, I3, I4, I5, I6, I7, I8
	{};
	template<typename I0, typename I1, typename I2, typename I3, typename I4,
			typename I5, typename I6, typename I7>
	struct InheritAll< I0, I1, I2, I3, I4, I5, I6, I7, NoInterface, NoInterface >
		: I0, I1, I2, I3, I4, I5, I6, I7
	{};
	template<typename I0, typename I1, typename I2, typename I3, typename I4,
			typename I5, typename I6>
	struct InheritAll< I0, I1, I2, I3, I4, I5, I6, NoInterface, NoInterface, NoInterface >
		: I0, I1, I2, I3, I4, I5, I6
	{};
	template<typename I0, typename I1, typename I2, typename I3, typename I4,
			typename I5>
	struct InheritAll< I0, I1, I2, I3, I4, I5, NoInterface, NoInterface, NoInterface, NoInterface >
		: I0, I1, I2, I3, I4, I5
	{};
	template<typename I0, typename I1, typename I2, typename I3, typename I4>
	struct InheritAll< I0, I1, I2, I3, I4,
			NoInterface, NoInterface, NoInterface, NoInterface, NoInterface >
		: I0, I1, I2, I3, I4
	{};
	template<typename I0, typename I1, typename I2, typename I3>
	struct InheritAll< I0, I1, I2, I3, NoInterface,
			NoInterface, NoInterface, NoInterface, NoInterface, NoInterface >
		: I0, I1, I2, I3
	{};
	template<typename I0, typename I1, typename I2>
	struct InheritAll< I0, I1, I2, NoInterface, NoInterface,
			NoInterface, NoInterface, NoInterface, NoInterface, NoInterface >
		: I0, I1, I2
	{};
	template<typename I0, typename I1>
	struct InheritAll< I0, I1, NoInterface, NoInterface, NoInterface,
			NoInterface, NoInterface, NoInterface, NoInterface, NoInterface >
		: I0, I1
	{};
	template<typename I0>
	struct InheritAll< I0, NoInterface, NoInterface, NoInterface, NoInterface,
			NoInterface, NoInterface, NoInterface, NoInterface, NoInterface >
		: I0
	{};

	template<typename I0 = NoInterface,
			typename I1 = NoInterface, typename I2 = NoInterface, typename I3 = NoInterface,
			typename I4 = NoInterface, typename I5 = NoInterface, typename I6 = NoInterface,
			typename I7 = NoInterface, typename I8 = NoInterface, typename I9 = NoInterface>
	class IUnknownImplementation : public InheritAll< I0, I1, I2, I3, I4, I5, I6, I7, I8, I9 >
	{
		BOOST_STATIC_ASSERT(( boost::is_convertible< I0*, IUnknown* >::value ));
		BOOST_STATIC_ASSERT(( boost::is_convertible< I1*, IUnknown* >::value ));
		BOOST_STATIC_ASSERT(( boost::is_convertible< I2*, IUnknown* >::value ));
		BOOST_STATIC_ASSERT(( boost::is_convertible< I3*, IUnknown* >::value ));
		BOOST_STATIC_ASSERT(( boost::is_convertible< I4*, IUnknown* >::value ));
		BOOST_STATIC_ASSERT(( boost::is_convertible< I5*, IUnknown* >::value ));
		BOOST_STATIC_ASSERT(( boost::is_convertible< I6*, IUnknown* >::value ));
		BOOST_STATIC_ASSERT(( boost::is_convertible< I7*, IUnknown* >::value ));
		BOOST_STATIC_ASSERT(( boost::is_convertible< I8*, IUnknown* >::value ));
		BOOST_STATIC_ASSERT(( boost::is_convertible< I9*, IUnknown* >::value ));
	protected:
		IUnknownImplementation()
			: ref_count_( 0 )
		{
#ifndef NDEBUG
			ATLTRACE2( atlTraceCOM, 1, L"%s(%p)::ctor()\n", \
				AfxGetIIDString( __uuidof( I0 ) ), this );
#endif
		}
		virtual ~IUnknownImplementation() {}
	private:
		template<typename Interface>
		bool query(const IID& iid, void** ppvObject)
		{
			if ( iid == __uuidof( Interface ) )
			{
				*ppvObject = static_cast< Interface* >( this );
				return true;
			}
			return false;
		}
		template<>
		bool query< NoInterface >(const IID& /*iid*/, void** /*ppvObject*/)
		{
			return false;
		}

		HRESULT __stdcall QueryInterface(const IID& iid, void** ppvObject)
		{
#ifndef NDEBUG
			ATLTRACE2( atlTraceQI, 1, L"%s(%p)->QueryInterface(%s)\n", \
				AfxGetIIDString( __uuidof( I0 ) ), this, AfxGetIIDString( iid ) );
#endif
			if ( !ppvObject )
				return E_POINTER;

			if ( query< IUnknown >( iid, ppvObject )
				|| query< I0 >( iid, ppvObject ) || query< I1 >( iid, ppvObject )
				|| query< I2 >( iid, ppvObject ) || query< I3 >( iid, ppvObject )
				|| query< I4 >( iid, ppvObject ) || query< I5 >( iid, ppvObject )
				|| query< I6 >( iid, ppvObject ) || query< I7 >( iid, ppvObject )
				|| query< I8 >( iid, ppvObject ) || query< I9 >( iid, ppvObject ) )
			{
				AddRef();
				return S_OK;
			}

			ATLTRACE2( atlTraceQI, 1, "--> failed\n" );
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		ULONG __stdcall AddRef()
		{
			ULONG ref_count = ULONG( InterlockedIncrement( &ref_count_ ) );
#ifndef NDEBUG
			ATLTRACE2( atlTraceRefcount, 1, L"%s(%p)->AddRef - ref_count = %lu\n", \
				AfxGetIIDString( __uuidof( I0 ) ), this, ref_count );
#endif
			return ref_count;
		}
		ULONG __stdcall Release()
		{
			ULONG ref_count = ULONG( InterlockedDecrement( &ref_count_ ) );
#ifndef NDEBUG
			ATLTRACE2( atlTraceRefcount, 1, L"%s(%p)->Release - ref_count = %lu\n", \
				AfxGetIIDString( __uuidof( I0 ) ), this, ref_count );
#endif
			if ( ref_count != 0 )
				return ref_count;
			delete this;
			return 0;
		};
	private:
		volatile LONG ref_count_;
	};

} // namespace augment

#endif // #ifndef AUGMENT_IUNKNOWNIMPLEMENTATION_HPP_INCLUDED