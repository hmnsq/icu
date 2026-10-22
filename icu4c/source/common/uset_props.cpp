// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uset_props.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004aug30
*   created by: Markus W. Scherer
*
*   C wrappers around UnicodeSet functions that are implemented in
*   uniset_props.cpp, split off for modularization.
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_open_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ char16_t* ನಮೂನೆ, int32_t ನಮೂನೆ_ಉದ್ದಳತೆ,
                 UError_ಸಂಕೇತ* ec)
{
    UnicodeString pat(ನಮೂನೆ_ಉದ್ದಳತೆ==-1, ನಮೂನೆ, ನಮೂನೆ_ಉದ್ದಳತೆ);
    Unicode_ಗಣ* ಗಣ = ಹೊಸ Unicode_ಗಣ(pat, *ec);
    /* test for nullptr */
    ಆದರೆ (ಗಣ == ಶೂನ್ಯಸೂಚಕ) {
        *ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (U_FAILURE(*ec)) {
        ಅಳಿಸು ಗಣ;
        ಗಣ = ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ (USet*) ಗಣ;
}

U_CAPI USet* U_EXPORT2
uset_open_ನಮೂನೆ_Options(ಸ್ಥಿರಾಂಕ char16_t* ನಮೂನೆ, int32_t ನಮೂನೆ_ಉದ್ದಳತೆ,
                 uint32_t options,
                 UError_ಸಂಕೇತ* ec)
{
    UnicodeString pat(ನಮೂನೆ_ಉದ್ದಳತೆ==-1, ನಮೂನೆ, ನಮೂನೆ_ಉದ್ದಳತೆ);
    Unicode_ಗಣ* ಗಣ = ಹೊಸ Unicode_ಗಣ(pat, options, ಶೂನ್ಯಸೂಚಕ, *ec);
    /* test for nullptr */
    ಆದರೆ (ಗಣ == ಶೂನ್ಯಸೂಚಕ) {
        *ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (U_FAILURE(*ec)) {
        ಅಳಿಸು ಗಣ;
        ಗಣ = ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ (USet*) ಗಣ;
}


U_CAPI int32_t U_EXPORT2 
uset_apply_ನಮೂನೆ(USet *ಗಣ,
                  ಸ್ಥಿರಾಂಕ char16_t *ನಮೂನೆ, int32_t ನಮೂನೆ_ಉದ್ದಳತೆ,
                  uint32_t options,
                  UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2){

    // status code needs to be checked since we 
    // dereference it
    ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    ಆದರೆ(ಗಣ == ಶೂನ್ಯಸೂಚಕ){
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    UnicodeString pat(ನಮೂನೆ, ನಮೂನೆ_ಉದ್ದಳತೆ);

    ParsePosition pos;
   
    ((Unicode_ಗಣ*) ಗಣ)->apply_ನಮೂನೆ(pat, pos, options, ಶೂನ್ಯಸೂಚಕ, *ಸ್ಥಿತಿ_2);
    
    ಹಿಂತಿರುಗಿ pos.get_ಸೂಚ್ಯಂಕ();
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_apply_Int_Property_ಮೌಲ್ಯ(USet* ಗಣ,
               UProperty prop, int32_t value, UError_ಸಂಕೇತ* ec) {
    ((Unicode_ಗಣ*) ಗಣ)->apply_Int_Property_ಮೌಲ್ಯ(prop, value, *ec);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_applyPropertyAlias(USet* ಗಣ,
                        ಸ್ಥಿರಾಂಕ char16_t *prop, int32_t prop_ಉದ್ದಳತೆ,
                        ಸ್ಥಿರಾಂಕ char16_t *value, int32_t ಮೌಲ್ಯ_ಉದ್ದಳತೆ,
            UError_ಸಂಕೇತ* ec) {

    UnicodeString p(prop, prop_ಉದ್ದಳತೆ);
    UnicodeString v(value, ಮೌಲ್ಯ_ಉದ್ದಳತೆ);

    ((Unicode_ಗಣ*) ಗಣ)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resembles_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ char16_t *ನಮೂನೆ, int32_t ನಮೂನೆ_ಉದ್ದಳತೆ,
                      int32_t pos) {

    UnicodeString pat(ನಮೂನೆ, ನಮೂನೆ_ಉದ್ದಳತೆ);

    ಹಿಂತಿರುಗಿ ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            Unicode_ಗಣ::resembles_ನಮೂನೆ(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_ಗಮ್ಯ_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ USet* ಗಣ,
               char16_t* ಫಲಿತಾಂಶ, int32_t ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ,
               UBool escapeUnprintable,
               UError_ಸಂಕೇತ* ec) {
    UnicodeString pat;
    ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) ಗಣ)->ಗಮ್ಯ_ನಮೂನೆ(pat, escapeUnprintable);
    ಹಿಂತಿರುಗಿ pat.extract(ಫಲಿತಾಂಶ, ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ, *ec);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_closeOver(USet* ಗಣ, int32_t ಗುಣಲಕ್ಷಣಗಳು) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::closeOver(ಗುಣಲಕ್ಷಣಗಳು);
}
