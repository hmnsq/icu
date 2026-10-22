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

#నిర్వచితం_కాకపోతే __UTYPEINFO_H__
#నిర్వచించు __UTYPEINFO_H__

// Windows header <typeinfo> does not define 'exception' in 'std' namespace.
// Therefore, a project using ICU cannot be compiled with _HAS_EXCEPTIONS
// set to 0 on Windows with Visual Studio. To work around that, we have to
// include <exception> explicitly and add using statement below.
// Whenever 'typeid' is used, this header has to be included
// instead of <typeinfo>.
// Visual Studio 10 emits warning 4275 with this change. If you compile
// with exception disabled, you have to suppress warning 4275.
#ఒకవేళ defined(_MSC_VER) && _HAS_EXCEPTIONS == 0
#అన్తర్భావ <exception>
using std::exception;
#అయితే_ముగింపు
#ఒకవేళ defined(__GLIBCXX__)
namespace std { class type_info; } // WORKAROUND: http://llvm.org/bugs/show_bug.cgi?id=13364
#అయితే_ముగింపు
#అన్తర్భావ <typeinfo>  // for 'typeid' to work

#అయితే_ముగింపు
