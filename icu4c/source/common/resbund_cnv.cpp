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

ResourceBundle::ResourceBundle( ثابت UnicodeString&    رستو,
                                ثابت Locale&           locale,
                                UError_ڪوڊ&              error)
                                :UObject(), fLocale(خالي_اشارو)
{
    constructForLocale(رستو, locale, error);
}

ResourceBundle::ResourceBundle( ثابت UnicodeString&    رستو,
                                UError_ڪوڊ&              error)
                                :UObject(), fLocale(خالي_اشارو)
{
    constructForLocale(رستو, Locale::getDefault(), error);
}

فراغ 
ResourceBundle::constructForLocale(ثابت UnicodeString& رستو,
                                   ثابت Locale& locale,
                                   UError_ڪوڊ& error)
{
    إذا (رستو.is_خالي_جايون()) {
        fResource = ures_open(خالي_اشارو, locale.getName(), &error);
    }
    وإلا {
        UnicodeString null_Terminated_رستو(رستو);
        null_Terminated_رستو.append(static_cast<char16_t>(0));
        fResource = ures_openU(null_Terminated_رستو.get_بفر(), locale.getName(), &error);
    }
}

U_NAMESPACE_پڇاڙي
