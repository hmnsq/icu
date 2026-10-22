// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1997-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  resbund_cnv.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004aug25
*   created by: Markus W. Scherer
*
*   Character conversion functions moved here from resbund.cpp
*/

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/resbund.h"
#অন্তর্ভাব "uinvchar.h"

U_NAMESPACE_BEGIN

ResourceBundle::ResourceBundle( ধ্রুবক UnicodeString&    path,
                                ধ্রুবক Locale&           locale,
                                UErrorCode&              error)
                                :UObject(), fLocale(nullptr)
{
    constructForLocale(path, locale, error);
}

ResourceBundle::ResourceBundle( ধ্রুবক UnicodeString&    path,
                                UErrorCode&              error)
                                :UObject(), fLocale(nullptr)
{
    constructForLocale(path, Locale::getDefault(), error);
}

শূন্য 
ResourceBundle::constructForLocale(ধ্রুবক UnicodeString& path,
                                   ধ্রুবক Locale& locale,
                                   UErrorCode& error)
{
    যদি (path.isEmpty()) {
        fResource = ures_open(nullptr, locale.getName(), &error);
    }
    অন্যথা {
        UnicodeString nullTerminatedPath(path);
        nullTerminatedPath.append(static_cast<char16_t>(0));
        fResource = ures_openU(nullTerminatedPath.get_বাফার(), locale.getName(), &error);
    }
}

U_NAMESPACE_END
