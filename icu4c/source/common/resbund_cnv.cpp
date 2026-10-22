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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "uinvchar.h"

U_NAMESPACE_BEGIN

ResourceBundle::ResourceBundle( ध्रुव UnicodeString&    मार्गः,
                                ध्रुव Locale&           locale,
                                UError_संहिता&              error)
                                :UObject(), fLocale(शून्यसूचकः)
{
    constructForLocale(मार्गः, locale, error);
}

ResourceBundle::ResourceBundle( ध्रुव UnicodeString&    मार्गः,
                                UError_संहिता&              error)
                                :UObject(), fLocale(शून्यसूचकः)
{
    constructForLocale(मार्गः, Locale::getDefault(), error);
}

शून्य 
ResourceBundle::constructForLocale(ध्रुव UnicodeString& मार्गः,
                                   ध्रुव Locale& locale,
                                   UError_संहिता& error)
{
    यदि (मार्गः.is_रिक्तानि()) {
        fResource = ures_open(शून्यसूचकः, locale.getName(), &error);
    }
    अन्यथा {
        UnicodeString null_Terminated_मार्गः(मार्गः);
        null_Terminated_मार्गः.append(static_cast<char16_t>(0));
        fResource = ures_openU(null_Terminated_मार्गः.get_बफर(), locale.getName(), &error);
    }
}

U_NAMESPACE_अन्तः
