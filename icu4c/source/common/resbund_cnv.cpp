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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/resbund.h"
#అన్తర్భావ "uinvchar.h"

U_NAMESPACE_BEGIN

ResourceBundle::ResourceBundle( స్థిరరాశి UnicodeString&    మార్గం,
                                స్థిరరాశి Locale&           locale,
                                UError_సంకేతం&              error)
                                :UObject(), fLocale(శూన్యసూచిక)
{
    constructForLocale(మార్గం, locale, error);
}

ResourceBundle::ResourceBundle( స్థిరరాశి UnicodeString&    మార్గం,
                                UError_సంకేతం&              error)
                                :UObject(), fLocale(శూన్యసూచిక)
{
    constructForLocale(మార్గం, Locale::getDefault(), error);
}

శూన్య 
ResourceBundle::constructForLocale(స్థిరరాశి UnicodeString& మార్గం,
                                   స్థిరరాశి Locale& locale,
                                   UError_సంకేతం& error)
{
    ఒకవేళ (మార్గం.is_ఖాళీలు()) {
        fResource = ures_open(శూన్యసూచిక, locale.getName(), &error);
    }
    లేకపోతే {
        UnicodeString null_Terminated_మార్గం(మార్గం);
        null_Terminated_మార్గం.append(static_cast<char16_t>(0));
        fResource = ures_openU(null_Terminated_మార్గం.get_బఫరు(), locale.getName(), &error);
    }
}

U_NAMESPACE_చివర
