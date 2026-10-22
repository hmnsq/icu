// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uniset_closure.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011may30
*   created by: Markus W. Scherer
*
*   UnicodeSet::closeOver() and related methods moved here from uniset_props.cpp
*   to simplify dependencies.
*   In particular, this depends on the BreakIterator, but the BreakIterator
*   code also builds UnicodeSets from patterns and needs uniset_props.
*/

#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ruleiter.h"
#ಅನ್ತರ್ಭಾವ "ucase.h"
#ಅನ್ತರ್ಭಾವ "uprops.h"
#ಅನ್ತರ್ಭಾವ "util.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

U_NAMESPACE_BEGIN

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#ವ್ಯಾಖ್ಯಾನಿಸು _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

Unicode_ಗಣ::Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ,
                       uint32_t options,
                       ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ* symbols,
                       UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    apply_ನಮೂನೆ(ನಮೂನೆ, options, symbols, ಸ್ಥಿತಿ_2);
    _dbgct(ಇದು);
}

Unicode_ಗಣ::Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ, ParsePosition& pos,
                       uint32_t options,
                       ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ* symbols,
                       UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    apply_ನಮೂನೆ(ನಮೂನೆ, pos, options, symbols, ಸ್ಥಿತಿ_2);
    _dbgct(ಇದು);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_ಗಣ& Unicode_ಗಣ::apply_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ,
                                     uint32_t options,
                                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ* symbols,
                                     UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    apply_ನಮೂನೆ(ನಮೂನೆ, pos, options, symbols, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ *ಇದು;

    int32_t i = pos.get_ಸೂಚ್ಯಂಕ();

    ಆದರೆ (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(ನಮೂನೆ, i, ನಿಜ);
    }

    ಆದರೆ (i != ನಮೂನೆ.length()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Unicode_ಗಣ& Unicode_ಗಣ::apply_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ* symbols,
                                     UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (isFrozen()) {
        ಸ್ಥಿತಿ_2 = U_NO_WRITE_PERMISSION;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    ನಿಯಮ_Character_Iterator chars(ನಮೂನೆ, symbols, pos);
    apply_ನಮೂನೆ(ನಮೂನೆ, pos, chars, symbols, rebuiltPat, options, &Unicode_ಗಣ::closeOver, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ *ಇದು;
    ಆದರೆ (chars.ಒಳ_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        ಸ್ಥಿತಿ_2 = U_MALFORMED_ಗಣ;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಗಣ_ನಮೂನೆ(rebuiltPat);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ಗಣ_ಕೂಡಿಸು(USet *ಗಣ, UChar32 c) {
    reinterpret_cast<Unicode_ಗಣ*>(ಗಣ)->ಕೂಡಿಸು(c);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ಗಣ_ಕೂಡಿಸು_Range(USet *ಗಣ, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) {
    reinterpret_cast<Unicode_ಗಣ*>(ಗಣ)->ಕೂಡಿಸು(ಆರಂಭ, ಕೊನೆ);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ಗಣ_ಕೂಡಿಸು_String(USet *ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_ಗಣ*>(ಗಣ)->ಕೂಡಿಸು(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
ಕೂಡಿಸು_Case_Mapping(Unicode_ಗಣ &ಗಣ, int32_t ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ char16_t *ತುಂಬಿದ, UnicodeString &str) {
    ಆದರೆ(ಫಲಿತಾಂಶ >= 0) {
        ಆದರೆ(ಫಲಿತಾಂಶ > UCASE_MAX_STRING_ಉದ್ದಳತೆ) {
            // add a single-code point case mapping
            ಗಣ.ಕೂಡಿಸು(ಫಲಿತಾಂಶ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            // add a string case mapping from full with length result
            str.ಗಣ_ಗಮ್ಯ(static_cast<UBool>(ಸುಳ್ಳು), ತುಂಬಿದ, ಫಲಿತಾಂಶ);
            ಗಣ.ಕೂಡಿಸು(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

ನಾಮಾಕಾಶ {

/** For case closure on a large set, look only at code points with relevant properties. */
ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &maybeOnlyCaseSensitive(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &src U_LIFETIME_BOUND,
                                         Unicode_ಗಣ &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.ಒಳಗೊಂಡಿದೆ(0, 0x10ffff));
    ಆದರೆ (src.size() < 30) {
        ಹಿಂತಿರುಗಿ src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *sensitive =
        CharacterProperties::get_ದ್ವಿಮಾನ_Property_ಗಣ(UCHAR_CASE_SENSITIVE, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    ಆದರೆ (src.get_Range_ಎಣಿಕೆ() > sensitive->get_Range_ಎಣಿಕೆ()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } ಇಲ್ಲದಿದ್ದರೆ {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    ಹಿಂತಿರುಗಿ subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
ತಾರ್ಕಿಕ scfString(ಸ್ಥಿರಾಂಕ UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    ಸ್ಥಿರಾಂಕ char16_t *p = s.get_ಬಫರ್();
    int32_t length = s.length();
    // Loop while not needing modification.
    ಗಾಗಿ (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_ಮುಂದಿನ(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        ಆದರೆ (scfChar != c) {
            // Copy the characters before c.
            scf.ಗಣ_ಗಮ್ಯ(p, i - U16_ಉದ್ದಳತೆ(c));
            // Loop over the rest of the string and keep case-folding.
            ಗಾಗಿ (;;) {
                scf.append(scfChar);
                ಆದರೆ (i == length) {
                    ಹಿಂತಿರುಗಿ ನಿಜ;
                }
                U16_ಮುಂದಿನ(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

}  // namespace

Unicode_ಗಣ& Unicode_ಗಣ::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen() || isBogus()) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆಯ್ಕೆ (attribute & USET_CASE_ಮುಖವಾಡ) {
    ಸ್ಥಿತಿ 0:
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ ಸುಳ್ಳು);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ USET_ಕೂಡಿಸು_CASE_MAPPINGS:
        close_Over_ಕೂಡಿಸು_Case_Mappings();
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ ನಿಜ);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        // bad option (unreachable)
        ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಶೂನ್ಯ Unicode_ಗಣ::closeOverCaseInsensitive(ತಾರ್ಕಿಕ simple) {
    // Start with input set to guarantee inclusion.
    Unicode_ಗಣ fold_ಗಣ(*ಇದು);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    ಆದರೆ (!simple && fold_ಗಣ.has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        fold_ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು_2->removeAllElements();
    }

    USetAdder sa = {
        fold_ಗಣ.ಗಮ್ಯ_USet(),
        ಗಣ_ಕೂಡಿಸು,
        ಗಣ_ಕೂಡಿಸು_Range,
        ಗಣ_ಕೂಡಿಸು_String,
        ಶೂನ್ಯಸೂಚಕ, // don't need remove()
        ಶೂನ್ಯಸೂಚಕ // don't need removeRange()
    };

    Unicode_ಗಣ subset(0, 0x10ffff);
    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &ಸಂಕೇತ_Points = maybeOnlyCaseSensitive(*ಇದು, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = ಸಂಕೇತ_Points.get_Range_ಎಣಿಕೆ();

    ಗಾಗಿ (int32_t i=0; i<n; ++i) {
        UChar32 ಆರಂಭ = ಸಂಕೇತ_Points.get_Range_ಆರಂಭ(i);
        UChar32 ಕೊನೆ   = ಸಂಕೇತ_Points.get_Range_ಕೊನೆ(i);

        ಆದರೆ (simple) {
            ಗಾಗಿ (UChar32 cp=ಆರಂಭ; cp<=ಕೊನೆ; ++cp) {
                ucase_ಕೂಡಿಸು_Simple_Case_ಆವರಣ(cp, &sa);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಾಗಿ (UChar32 cp=ಆರಂಭ; cp<=ಕೊನೆ; ++cp) {
                ucase_ಕೂಡಿಸು_Case_ಆವರಣ(cp, &sa);
            }
        }
    }
    ಆದರೆ (has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        UnicodeString str;
        ಗಾಗಿ (int32_t j=0; j<ಅಕ್ಷರಸರಣಿಗಳು_2->size(); ++j) {
            ಸ್ಥಿರಾಂಕ UnicodeString* pStr = static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು_2->elementAt(j));
            ಆದರೆ (simple) {
                ಆದರೆ (scfString(*pStr, str)) {
                    fold_ಗಣ.remove(*pStr).ಕೂಡಿಸು(str);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                str = *pStr;
                str.foldCase();
                ಆದರೆ(!ucase_ಕೂಡಿಸು_String_Case_ಆವರಣ(str.get_ಬಫರ್(), str.length(), &sa)) {
                    fold_ಗಣ.ಕೂಡಿಸು(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *ಇದು = fold_ಗಣ;
}

ಶೂನ್ಯ Unicode_ಗಣ::close_Over_ಕೂಡಿಸು_Case_Mappings() {
    // Start with input set to guarantee inclusion.
    Unicode_ಗಣ fold_ಗಣ(*ಇದು);

    Unicode_ಗಣ subset(0, 0x10ffff);
    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &ಸಂಕೇತ_Points = maybeOnlyCaseSensitive(*ಇದು, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = ಸಂಕೇತ_Points.get_Range_ಎಣಿಕೆ();
    UChar32 ಫಲಿತಾಂಶ;
    ಸ್ಥಿರಾಂಕ char16_t *ತುಂಬಿದ;
    UnicodeString str;

    ಗಾಗಿ (int32_t i=0; i<n; ++i) {
        UChar32 ಆರಂಭ = ಸಂಕೇತ_Points.get_Range_ಆರಂಭ(i);
        UChar32 ಕೊನೆ   = ಸಂಕೇತ_Points.get_Range_ಕೊನೆ(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        ಗಾಗಿ (UChar32 cp=ಆರಂಭ; cp<=ಕೊನೆ; ++cp) {
            ಫಲಿತಾಂಶ = ucase_ಗಮ್ಯ_ತುಂಬಿದ_ಅಧೋ(cp, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &ತುಂಬಿದ, UCASE_LOC_ಬೇರು);
            ಕೂಡಿಸು_Case_Mapping(fold_ಗಣ, ಫಲಿತಾಂಶ, ತುಂಬಿದ, str);

            ಫಲಿತಾಂಶ = ucase_ಗಮ್ಯ_ತುಂಬಿದ_Title(cp, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &ತುಂಬಿದ, UCASE_LOC_ಬೇರು);
            ಕೂಡಿಸು_Case_Mapping(fold_ಗಣ, ಫಲಿತಾಂಶ, ತುಂಬಿದ, str);

            ಫಲಿತಾಂಶ = ucase_ಗಮ್ಯ_ತುಂಬಿದ_ಊರ್ಧ್ವ(cp, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &ತುಂಬಿದ, UCASE_LOC_ಬೇರು);
            ಕೂಡಿಸು_Case_Mapping(fold_ಗಣ, ಫಲಿತಾಂಶ, ತುಂಬಿದ, str);

            ಫಲಿತಾಂಶ = ucase_ಗಮ್ಯ_ತುಂಬಿದ_Folding(cp, &ತುಂಬಿದ, 0);
            ಕೂಡಿಸು_Case_Mapping(fold_ಗಣ, ಫಲಿತಾಂಶ, ತುಂಬಿದ, str);
        }
    }
    ಆದರೆ (has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        Locale ಬೇರು("");
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::create_ಶಬ್ದ_Instance(ಬೇರು, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
#ಆದರೆ_ಅಂತ್ಯ
            ಗಾಗಿ (int32_t j=0; j<ಅಕ್ಷರಸರಣಿಗಳು_2->size(); ++j) {
                ಸ್ಥಿರಾಂಕ UnicodeString* pStr = static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು_2->elementAt(j));
                (str = *pStr).ಗಮ್ಯ_ಅಧೋ(ಬೇರು);
                fold_ಗಣ.ಕೂಡಿಸು(str);
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).ಗಮ್ಯ_Title(bi, ಬೇರು);
                fold_ಗಣ.ಕೂಡಿಸು(str);
#ಆದರೆ_ಅಂತ್ಯ
                (str = *pStr).ಗಮ್ಯ_ಊರ್ಧ್ವ(ಬೇರು);
                fold_ಗಣ.ಕೂಡಿಸು(str);
                (str = *pStr).foldCase();
                fold_ಗಣ.ಕೂಡಿಸು(str);
            }
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
        }
        ಅಳಿಸು bi;
#ಆದರೆ_ಅಂತ್ಯ
    }
    *ಇದು = fold_ಗಣ;
}

U_NAMESPACE_ಕೊನೆ
