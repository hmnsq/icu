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

#ضمن "unicode/utypes.h"
#ضمن "unicode/resbund.h"
#ضمن "uinvchar.h"

U_NAMESPACE_BEGIN

ResourceBundle::ResourceBundle( ثابت UnicodeString&    مسیر,
                                ثابت Locale&           locale,
                                UError_کد&              error)
                                :UObject(), fLocale(اشاره_تهی)
{
    constructForLocale(مسیر, locale, error);
}

ResourceBundle::ResourceBundle( ثابت UnicodeString&    مسیر,
                                UError_کد&              error)
                                :UObject(), fLocale(اشاره_تهی)
{
    constructForLocale(مسیر, Locale::getDefault(), error);
}

فراغ 
ResourceBundle::constructForLocale(ثابت UnicodeString& مسیر,
                                   ثابت Locale& locale,
                                   UError_کد& error)
{
    إذا (مسیر.is_خالی()) {
        fResource = ures_open(اشاره_تهی, locale.getName(), &error);
    }
    وإلا {
        UnicodeString null_Terminated_مسیر(مسیر);
        null_Terminated_مسیر.append(static_cast<char16_t>(0));
        fResource = ures_openU(null_Terminated_مسیر.get_میانگیر(), locale.getName(), &error);
    }
}

U_NAMESPACE_انتها
