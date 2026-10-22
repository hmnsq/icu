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

#شامل "unicode/utypes.h"
#شامل "unicode/resbund.h"
#شامل "uinvchar.h"

U_NAMESPACE_BEGIN

ResourceBundle::ResourceBundle( ثابت UnicodeString&    راستہ,
                                ثابت Locale&           locale,
                                UError_رمز&              error)
                                :UObject(), fLocale(خالی_اشاریہ)
{
    constructForLocale(راستہ, locale, error);
}

ResourceBundle::ResourceBundle( ثابت UnicodeString&    راستہ,
                                UError_رمز&              error)
                                :UObject(), fLocale(خالی_اشاریہ)
{
    constructForLocale(راستہ, Locale::getDefault(), error);
}

خالی 
ResourceBundle::constructForLocale(ثابت UnicodeString& راستہ,
                                   ثابت Locale& locale,
                                   UError_رمز& error)
{
    اگر (راستہ.is_خالی_خانے()) {
        fResource = ures_open(خالی_اشاریہ, locale.getName(), &error);
    }
    ورنہ {
        UnicodeString null_Terminated_راستہ(راستہ);
        null_Terminated_راستہ.append(static_cast<char16_t>(0));
        fResource = ures_openU(null_Terminated_راستہ.get_بفر(), locale.getName(), &error);
    }
}

U_NAMESPACE_انتہا
