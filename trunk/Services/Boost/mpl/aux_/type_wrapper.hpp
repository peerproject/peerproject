
#ifndef BOOST_MPL_AUX_TYPE_WRAPPER_HPP_INCLUDED
#define BOOST_MPL_AUX_TYPE_WRAPPER_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
// Copyright Peter Dimov 2000-2003
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: type_wrapper.hpp 49239 2008-10-10 09:10:26Z agurtovoy $
// $Date: 2008-10-10 05:10:26 -0400 (Fri, 10 Oct 2008) $
// $Revision: 49239 $

#include <boost/mpl/aux_/config/ctps.hpp>

namespace boost { namespace mpl { namespace aux {

template< typename T > struct type_wrapper
{
    typedef T type;
};

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
// agurt 08/may/03: a complicated way to extract the wrapped type; need it
// mostly for the sake of GCC (3.2.x), which ICEs if you try to extract the
// nested 'type' from 'type_wrapper<T>' when the latter was the result of a
// 'typeof' expression
template< typename T > struct wrapped_type;

template< typename T > struct wrapped_type< type_wrapper<T> >
{
    typedef T type;
};
#else
template< typename W > struct wrapped_type
{
    typedef typename W::type type;
};
#endif

}}}

#endif // BOOST_MPL_AUX_TYPE_WRAPPER_HPP_INCLUDED
