// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2015-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*/
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"

#ಅನ್ತರ್ಭಾವ "cstr.h"

#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"

U_NAMESPACE_BEGIN

CStr::CStr(ಸ್ಥಿರಾಂಕ UnicodeString &in) {
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
#ಆದರೆ !UCONFIG_NO_CONVERSION || U_CHARSET_IS_UTF8
    int32_t length = in.extract(0, in.length(), static_cast<ಅಕ್ಷರ *>(ಶೂನ್ಯಸೂಚಕ), static_cast<uint32_t>(0));
    int32_t ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ = 0;
    ಅಕ್ಷರ *buf = s.get_Append_ಬಫರ್(length, length, ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        in.extract(0, in.length(), buf, ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ);
        s.append(buf, length, ಸ್ಥಿತಿ_2);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    // No conversion available. Convert any invariant characters; substitute '?' for the rest.
    // Note: can't just call u_UCharsToChars() or CharString.appendInvariantChars() on the 
    //       whole string because they require that the entire input be invariant.
    ಅಕ್ಷರ buf[2];
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i=0; i<in.length(); i = in.moveIndex32(i, 1)) {
        ಆದರೆ (uprv_isInvariantUString(in.get_ಬಫರ್()+i, 1)) {
            u_UChars_ಗಮ್ಯ_Chars(in.get_ಬಫರ್()+i, buf, 1);
        } ಇಲ್ಲದಿದ್ದರೆ {
            buf[0] = '?';
        }
        s.append(buf, 1, ಸ್ಥಿತಿ_2);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

CStr::~CStr() {
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * CStr::ಕ್ರಿಯಾಕಾರಕ ()() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ s.ದತ್ತಾಂಶ();
}

U_NAMESPACE_ಕೊನೆ
