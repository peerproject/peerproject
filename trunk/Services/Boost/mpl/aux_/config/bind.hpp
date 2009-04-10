
#ifndef BOOST_MPL_AUX_CONFIG_BIND_HPP_INCLUDED
#define BOOST_MPL_AUX_CONFIG_BIND_HPP_INCLUDED

// Copyright David Abrahams 2002
// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: bind.hpp 49239 2008-10-10 09:10:26Z agurtovoy $
// $Date: 2008-10-10 05:10:26 -0400 (Fri, 10 Oct 2008) $
// $Revision: 49239 $

#include <boost/mpl/aux_/config/msvc.hpp>
#include <boost/mpl/aux_/config/workaround.hpp>

#if    !defined(BOOST_MPL_CFG_NO_BIND_TEMPLATE) \
    && !defined(BOOST_MPL_PREPROCESSING_MODE) \
    && (   BOOST_WORKAROUND(BOOST_MSVC, <= 1300) \
        || BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x610)) \
        )

#   define BOOST_MPL_CFG_NO_BIND_TEMPLATE

#endif

//#define BOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT

#endif // BOOST_MPL_AUX_CONFIG_BIND_HPP_INCLUDED
