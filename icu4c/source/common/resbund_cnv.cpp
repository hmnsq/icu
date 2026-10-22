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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"

U_NAMESPACE_BEGIN

ResourceBundle::ResourceBundle( ಸ್ಥಿರಾಂಕ UnicodeString&    ಮಾರ್ಗ,
                                ಸ್ಥಿರಾಂಕ Locale&           locale,
                                UError_ಸಂಕೇತ&              error)
                                :UObject(), fLocale(ಶೂನ್ಯಸೂಚಕ)
{
    constructForLocale(ಮಾರ್ಗ, locale, error);
}

ResourceBundle::ResourceBundle( ಸ್ಥಿರಾಂಕ UnicodeString&    ಮಾರ್ಗ,
                                UError_ಸಂಕೇತ&              error)
                                :UObject(), fLocale(ಶೂನ್ಯಸೂಚಕ)
{
    constructForLocale(ಮಾರ್ಗ, Locale::getDefault(), error);
}

ಶೂನ್ಯ 
ResourceBundle::constructForLocale(ಸ್ಥಿರಾಂಕ UnicodeString& ಮಾರ್ಗ,
                                   ಸ್ಥಿರಾಂಕ Locale& locale,
                                   UError_ಸಂಕೇತ& error)
{
    ಆದರೆ (ಮಾರ್ಗ.is_ಖಾಲಿಗಳು()) {
        fResource = ures_open(ಶೂನ್ಯಸೂಚಕ, locale.getName(), &error);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        UnicodeString null_Terminated_ಮಾರ್ಗ(ಮಾರ್ಗ);
        null_Terminated_ಮಾರ್ಗ.append(static_cast<char16_t>(0));
        fResource = ures_openU(null_Terminated_ಮಾರ್ಗ.get_ಬಫರ್(), locale.getName(), &error);
    }
}

U_NAMESPACE_ಕೊನೆ
