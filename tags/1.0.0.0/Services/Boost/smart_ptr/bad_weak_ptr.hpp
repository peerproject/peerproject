#ifndef BOOST_SMART_PTR_BAD_WEAK_PTR_HPP_INCLUDED
#define BOOST_SMART_PTR_BAD_WEAK_PTR_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER)
# pragma once
#endif

//
//  boost/smart_ptr/bad_weak_ptr.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <exception>

// Removed #ifdef __BORLANDC__ pragmas

namespace boost
{

// The standard library that comes with Borland C++ 5.5.1, 5.6.4
// defines std::exception and its members as having C calling
// convention (-pc). When the definition of bad_weak_ptr
// is compiled with -ps, the compiler issues an error.
// Hence, the temporary #pragma option -pc below. (Removed)

class bad_weak_ptr: public std::exception
{
public:

    virtual char const * what() const throw()
    {
        return "tr1::bad_weak_ptr";
    }
};

} // namespace boost

#endif  // #ifndef BOOST_SMART_PTR_BAD_WEAK_PTR_HPP_INCLUDED