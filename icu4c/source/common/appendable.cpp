// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  appendable.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010dec07
*   created by: Markus W. Scherer
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/appendable.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"

U_NAMESPACE_BEGIN

Appendable::~Appendable() {}

UBool
Appendable::append_ಸಂಕೇತ_ಬಿಂದು(UChar32 c) {
    ಆದರೆ(c<=0xffff) {
        ಹಿಂತಿರುಗಿ append_ಸಂಕೇತ_Unit(static_cast<char16_t>(c));
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ append_ಸಂಕೇತ_Unit(U16_LEAD(c)) && append_ಸಂಕೇತ_Unit(U16_TRAIL(c));
    }
}

UBool
Appendable::appendString(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length) {
    ಆದರೆ(length<0) {
        char16_t c;
        ಆಗ((c=*s++)!=0) {
            ಆದರೆ(!append_ಸಂಕೇತ_Unit(c)) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length>0) {
        ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ=s+length;
        ಮಾಡು {
            ಆದರೆ(!append_ಸಂಕೇತ_Unit(*s++)) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        } ಆಗ(s<ಎಲ್ಲೆ);
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool
Appendable::reserve_Append_ಸಾಮರ್ಥ್ಯ(int32_t /*appendCapacity*/) {
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

char16_t*
Appendable::get_Append_ಬಫರ್(int32_t min_ಸಾಮರ್ಥ್ಯ,
                            int32_t /*desiredCapacityHint*/,
                            char16_t* scratch U_LIFETIME_BOUND,
                            int32_t scratch_ಸಾಮರ್ಥ್ಯ,
                            int32_t* ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ) U_LIFETIME_BOUND {
    ಆದರೆ(min_ಸಾಮರ್ಥ್ಯ<1 || scratch_ಸಾಮರ್ಥ್ಯ<min_ಸಾಮರ್ಥ್ಯ) {
        *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ=0;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ=scratch_ಸಾಮರ್ಥ್ಯ;
    ಹಿಂತಿರುಗಿ scratch;
}

// UnicodeStringAppendable is implemented in unistr.cpp.

U_NAMESPACE_ಕೊನೆ
