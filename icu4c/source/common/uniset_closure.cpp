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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parsepos.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ruleiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

U_NAMESPACE_BEGIN

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

Unicode_ਸੈੱਟ::Unicode_ਸੈੱਟ(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                       uint32_t options,
                       ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ* symbols,
                       UError_ਕੋਡ& ਸਥਿਤੀ) {
    apply_ਨਮੂਨਾ(ਨਮੂਨਾ, options, symbols, ਸਥਿਤੀ);
    _dbgct(ਇਹ);
}

Unicode_ਸੈੱਟ::Unicode_ਸੈੱਟ(ਅਚਲ UnicodeString& ਨਮੂਨਾ, ParsePosition& pos,
                       uint32_t options,
                       ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ* symbols,
                       UError_ਕੋਡ& ਸਥਿਤੀ) {
    apply_ਨਮੂਨਾ(ਨਮੂਨਾ, pos, options, symbols, ਸਥਿਤੀ);
    _dbgct(ਇਹ);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::apply_ਨਮੂਨਾ(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                                     uint32_t options,
                                     ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ* symbols,
                                     UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    apply_ਨਮੂਨਾ(ਨਮੂਨਾ, pos, options, symbols, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ *ਇਹ;

    int32_t i = pos.get_ਸੂਚਕਾਂਕ();

    ਜੇ (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(ਨਮੂਨਾ, i, ਸੱਚ);
    }

    ਜੇ (i != ਨਮੂਨਾ.length()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
    ਵਾਪਸ *ਇਹ;
}

Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::apply_ਨਮੂਨਾ(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ* symbols,
                                     UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (isFrozen()) {
        ਸਥਿਤੀ = U_NO_WRITE_PERMISSION;
        ਵਾਪਸ *ਇਹ;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    ਨਿਯਮ_Character_Iterator chars(ਨਮੂਨਾ, symbols, pos);
    apply_ਨਮੂਨਾ(ਨਮੂਨਾ, pos, chars, symbols, rebuiltPat, options, &Unicode_ਸੈੱਟ::closeOver, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ *ਇਹ;
    ਜੇ (chars.ਵਿੱਚ_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        ਸਥਿਤੀ = U_MALFORMED_ਸੈੱਟ;
        ਵਾਪਸ *ਇਹ;
    }
    ਸੈੱਟ_ਨਮੂਨਾ(rebuiltPat);
    ਵਾਪਸ *ਇਹ;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ਸੈੱਟ_ਜੋੜੋ(USet *ਸੈੱਟ, UChar32 c) {
    reinterpret_cast<Unicode_ਸੈੱਟ*>(ਸੈੱਟ)->ਜੋੜੋ(c);
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ਸੈੱਟ_ਜੋੜੋ_Range(USet *ਸੈੱਟ, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) {
    reinterpret_cast<Unicode_ਸੈੱਟ*>(ਸੈੱਟ)->ਜੋੜੋ(ਸ਼ੁਰੂ, ਸਿਰਾ);
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ਸੈੱਟ_ਜੋੜੋ_String(USet *ਸੈੱਟ, ਅਚਲ char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_ਸੈੱਟ*>(ਸੈੱਟ)->ਜੋੜੋ(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
ਜੋੜੋ_Case_Mapping(Unicode_ਸੈੱਟ &ਸੈੱਟ, int32_t ਨਤੀਜਾ, ਅਚਲ char16_t *ਭਰਿਆ, UnicodeString &str) {
    ਜੇ(ਨਤੀਜਾ >= 0) {
        ਜੇ(ਨਤੀਜਾ > UCASE_MAX_STRING_ਲੰਮਾਈ) {
            // add a single-code point case mapping
            ਸੈੱਟ.ਜੋੜੋ(ਨਤੀਜਾ);
        } ਨਹੀਂਤਾਂ {
            // add a string case mapping from full with length result
            str.ਸੈੱਟ_ਮੰਜ਼ਿਲ(static_cast<UBool>(ਝੂਠ), ਭਰਿਆ, ਨਤੀਜਾ);
            ਸੈੱਟ.ਜੋੜੋ(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

ਨਾਮਾਕਾਸ਼ {

/** For case closure on a large set, look only at code points with relevant properties. */
ਅਚਲ Unicode_ਸੈੱਟ &maybeOnlyCaseSensitive(ਅਚਲ Unicode_ਸੈੱਟ &src U_LIFETIME_BOUND,
                                         Unicode_ਸੈੱਟ &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.ਸ਼ਾਮਲ_ਹੈ(0, 0x10ffff));
    ਜੇ (src.size() < 30) {
        ਵਾਪਸ src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
    ਅਚਲ Unicode_ਸੈੱਟ *sensitive =
        CharacterProperties::get_ਦੁਆਧਾਰੀ_Property_ਸੈੱਟ(UCHAR_CASE_SENSITIVE, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    ਜੇ (src.get_Range_ਗਿਣਤੀ() > sensitive->get_Range_ਗਿਣਤੀ()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } ਨਹੀਂਤਾਂ {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    ਵਾਪਸ subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
ਤਾਰਕਿਕ scfString(ਅਚਲ UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    ਅਚਲ char16_t *p = s.get_ਬਫਰ();
    int32_t length = s.length();
    // Loop while not needing modification.
    ਲਈ (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_ਅਗਲਾ(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        ਜੇ (scfChar != c) {
            // Copy the characters before c.
            scf.ਸੈੱਟ_ਮੰਜ਼ਿਲ(p, i - U16_ਲੰਮਾਈ(c));
            // Loop over the rest of the string and keep case-folding.
            ਲਈ (;;) {
                scf.append(scfChar);
                ਜੇ (i == length) {
                    ਵਾਪਸ ਸੱਚ;
                }
                U16_ਅਗਲਾ(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

}  // namespace

Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    ਜੇ (isFrozen() || isBogus()) {
        ਵਾਪਸ *ਇਹ;
    }
    ਚੋਣ (attribute & USET_CASE_ਨਕਾਬ) {
    ਹਾਲਤ 0:
        ਰੋਕੋ;
    ਹਾਲਤ USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ ਝੂਠ);
        ਰੋਕੋ;
    ਹਾਲਤ USET_ਜੋੜੋ_CASE_MAPPINGS:
        close_Over_ਜੋੜੋ_Case_Mappings();
        ਰੋਕੋ;
    ਹਾਲਤ USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ ਸੱਚ);
        ਰੋਕੋ;
    ਮੂਲ:
        // bad option (unreachable)
        ਰੋਕੋ;
    }
    ਵਾਪਸ *ਇਹ;
}

ਖਾਲੀ Unicode_ਸੈੱਟ::closeOverCaseInsensitive(ਤਾਰਕਿਕ simple) {
    // Start with input set to guarantee inclusion.
    Unicode_ਸੈੱਟ fold_ਸੈੱਟ(*ਇਹ);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    ਜੇ (!simple && fold_ਸੈੱਟ.has_ਲੜੀਆਂ()) {
        fold_ਸੈੱਟ.ਲੜੀਆਂ_2->removeAllElements();
    }

    USetAdder sa = {
        fold_ਸੈੱਟ.ਮੰਜ਼ਿਲ_USet(),
        ਸੈੱਟ_ਜੋੜੋ,
        ਸੈੱਟ_ਜੋੜੋ_Range,
        ਸੈੱਟ_ਜੋੜੋ_String,
        ਖਾਲੀ_ਸੂਚਕ, // don't need remove()
        ਖਾਲੀ_ਸੂਚਕ // don't need removeRange()
    };

    Unicode_ਸੈੱਟ subset(0, 0x10ffff);
    ਅਚਲ Unicode_ਸੈੱਟ &ਕੋਡ_Points = maybeOnlyCaseSensitive(*ਇਹ, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = ਕੋਡ_Points.get_Range_ਗਿਣਤੀ();

    ਲਈ (int32_t i=0; i<n; ++i) {
        UChar32 ਸ਼ੁਰੂ = ਕੋਡ_Points.get_Range_ਸ਼ੁਰੂ(i);
        UChar32 ਸਿਰਾ   = ਕੋਡ_Points.get_Range_ਸਿਰਾ(i);

        ਜੇ (simple) {
            ਲਈ (UChar32 cp=ਸ਼ੁਰੂ; cp<=ਸਿਰਾ; ++cp) {
                ucase_ਜੋੜੋ_Simple_Case_ਘੇਰਾ(cp, &sa);
            }
        } ਨਹੀਂਤਾਂ {
            ਲਈ (UChar32 cp=ਸ਼ੁਰੂ; cp<=ਸਿਰਾ; ++cp) {
                ucase_ਜੋੜੋ_Case_ਘੇਰਾ(cp, &sa);
            }
        }
    }
    ਜੇ (has_ਲੜੀਆਂ()) {
        UnicodeString str;
        ਲਈ (int32_t j=0; j<ਲੜੀਆਂ_2->size(); ++j) {
            ਅਚਲ UnicodeString* pStr = static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ_2->elementAt(j));
            ਜੇ (simple) {
                ਜੇ (scfString(*pStr, str)) {
                    fold_ਸੈੱਟ.remove(*pStr).ਜੋੜੋ(str);
                }
            } ਨਹੀਂਤਾਂ {
                str = *pStr;
                str.foldCase();
                ਜੇ(!ucase_ਜੋੜੋ_String_Case_ਘੇਰਾ(str.get_ਬਫਰ(), str.length(), &sa)) {
                    fold_ਸੈੱਟ.ਜੋੜੋ(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *ਇਹ = fold_ਸੈੱਟ;
}

ਖਾਲੀ Unicode_ਸੈੱਟ::close_Over_ਜੋੜੋ_Case_Mappings() {
    // Start with input set to guarantee inclusion.
    Unicode_ਸੈੱਟ fold_ਸੈੱਟ(*ਇਹ);

    Unicode_ਸੈੱਟ subset(0, 0x10ffff);
    ਅਚਲ Unicode_ਸੈੱਟ &ਕੋਡ_Points = maybeOnlyCaseSensitive(*ਇਹ, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = ਕੋਡ_Points.get_Range_ਗਿਣਤੀ();
    UChar32 ਨਤੀਜਾ;
    ਅਚਲ char16_t *ਭਰਿਆ;
    UnicodeString str;

    ਲਈ (int32_t i=0; i<n; ++i) {
        UChar32 ਸ਼ੁਰੂ = ਕੋਡ_Points.get_Range_ਸ਼ੁਰੂ(i);
        UChar32 ਸਿਰਾ   = ਕੋਡ_Points.get_Range_ਸਿਰਾ(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        ਲਈ (UChar32 cp=ਸ਼ੁਰੂ; cp<=ਸਿਰਾ; ++cp) {
            ਨਤੀਜਾ = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_ਹੇਠਲਾ(cp, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &ਭਰਿਆ, UCASE_LOC_ਜੜ੍ਹ);
            ਜੋੜੋ_Case_Mapping(fold_ਸੈੱਟ, ਨਤੀਜਾ, ਭਰਿਆ, str);

            ਨਤੀਜਾ = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Title(cp, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &ਭਰਿਆ, UCASE_LOC_ਜੜ੍ਹ);
            ਜੋੜੋ_Case_Mapping(fold_ਸੈੱਟ, ਨਤੀਜਾ, ਭਰਿਆ, str);

            ਨਤੀਜਾ = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_ਉੱਤਲਾ(cp, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &ਭਰਿਆ, UCASE_LOC_ਜੜ੍ਹ);
            ਜੋੜੋ_Case_Mapping(fold_ਸੈੱਟ, ਨਤੀਜਾ, ਭਰਿਆ, str);

            ਨਤੀਜਾ = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Folding(cp, &ਭਰਿਆ, 0);
            ਜੋੜੋ_Case_Mapping(fold_ਸੈੱਟ, ਨਤੀਜਾ, ਭਰਿਆ, str);
        }
    }
    ਜੇ (has_ਲੜੀਆਂ()) {
        Locale ਜੜ੍ਹ("");
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::create_ਸ਼ਬਦ_Instance(ਜੜ੍ਹ, ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
#ਜੇ_ਅੰਤ
            ਲਈ (int32_t j=0; j<ਲੜੀਆਂ_2->size(); ++j) {
                ਅਚਲ UnicodeString* pStr = static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ_2->elementAt(j));
                (str = *pStr).ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(ਜੜ੍ਹ);
                fold_ਸੈੱਟ.ਜੋੜੋ(str);
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).ਮੰਜ਼ਿਲ_Title(bi, ਜੜ੍ਹ);
                fold_ਸੈੱਟ.ਜੋੜੋ(str);
#ਜੇ_ਅੰਤ
                (str = *pStr).ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(ਜੜ੍ਹ);
                fold_ਸੈੱਟ.ਜੋੜੋ(str);
                (str = *pStr).foldCase();
                fold_ਸੈੱਟ.ਜੋੜੋ(str);
            }
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
        }
        ਮਿਟਾਓ bi;
#ਜੇ_ਅੰਤ
    }
    *ਇਹ = fold_ਸੈੱਟ;
}

U_NAMESPACE_ਸਿਰਾ
