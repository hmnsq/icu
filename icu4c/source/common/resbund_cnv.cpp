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

#समावेश "unicode/utypes.h"
#समावेश "unicode/resbund.h"
#समावेश "uinvchar.h"

U_NAMESPACE_BEGIN

ResourceBundle::ResourceBundle( स्थिर UnicodeString&    path,
                                स्थिर Locale&           locale,
                                UErrorCode&              error)
                                :UObject(), fLocale(nullptr)
{
    constructForLocale(path, locale, error);
}

ResourceBundle::ResourceBundle( स्थिर UnicodeString&    path,
                                UErrorCode&              error)
                                :UObject(), fLocale(nullptr)
{
    constructForLocale(path, Locale::getDefault(), error);
}

व्योम 
ResourceBundle::constructForLocale(स्थिर UnicodeString& path,
                                   स्थिर Locale& locale,
                                   UErrorCode& error)
{
    अगर (path.isEmpty()) {
        fResource = ures_open(nullptr, locale.getName(), &error);
    }
    अन्यथा {
        UnicodeString nullTerminatedPath(path);
        nullTerminatedPath.append(static_cast<char16_t>(0));
        fResource = ures_openU(nullTerminatedPath.get_बफर(), locale.getName(), &error);
    }
}

U_NAMESPACE_END
