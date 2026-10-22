// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2012-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*/

#यदि_अपरिभाषितम् __UTYPEINFO_H__
#परिभाषय __UTYPEINFO_H__

// Windows header <typeinfo> does not define 'exception' in 'std' namespace.
// Therefore, a project using ICU cannot be compiled with _HAS_EXCEPTIONS
// set to 0 on Windows with Visual Studio. To work around that, we have to
// include <exception> explicitly and add using statement below.
// Whenever 'typeid' is used, this header has to be included
// instead of <typeinfo>.
// Visual Studio 10 emits warning 4275 with this change. If you compile
// with exception disabled, you have to suppress warning 4275.
#यदि defined(_MSC_VER) && _HAS_EXCEPTIONS == 0
#अन्तर्भाव <exception>
using std::exception;
#यदि_अन्तः
#यदि defined(__GLIBCXX__)
namespace std { class type_info; } // WORKAROUND: http://llvm.org/bugs/show_bug.cgi?id=13364
#यदि_अन्तः
#अन्तर्भाव <typeinfo>  // for 'typeid' to work

#यदि_अन्तः
