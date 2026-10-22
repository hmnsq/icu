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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/resbund.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uinvchar.h"

U_NAMESPACE_BEGIN

ResourceBundle::ResourceBundle( ਅਚਲ UnicodeString&    ਰਸਤਾ,
                                ਅਚਲ Locale&           locale,
                                UError_ਕੋਡ&              error)
                                :UObject(), fLocale(ਖਾਲੀ_ਸੂਚਕ)
{
    constructForLocale(ਰਸਤਾ, locale, error);
}

ResourceBundle::ResourceBundle( ਅਚਲ UnicodeString&    ਰਸਤਾ,
                                UError_ਕੋਡ&              error)
                                :UObject(), fLocale(ਖਾਲੀ_ਸੂਚਕ)
{
    constructForLocale(ਰਸਤਾ, Locale::getDefault(), error);
}

ਖਾਲੀ 
ResourceBundle::constructForLocale(ਅਚਲ UnicodeString& ਰਸਤਾ,
                                   ਅਚਲ Locale& locale,
                                   UError_ਕੋਡ& error)
{
    ਜੇ (ਰਸਤਾ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        fResource = ures_open(ਖਾਲੀ_ਸੂਚਕ, locale.getName(), &error);
    }
    ਨਹੀਂਤਾਂ {
        UnicodeString null_Terminated_ਰਸਤਾ(ਰਸਤਾ);
        null_Terminated_ਰਸਤਾ.append(static_cast<char16_t>(0));
        fResource = ures_openU(null_Terminated_ਰਸਤਾ.get_ਬਫਰ(), locale.getName(), &error);
    }
}

U_NAMESPACE_ਸਿਰਾ
