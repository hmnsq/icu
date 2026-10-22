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
#define _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

UnicodeSet::UnicodeSet(ಧ್ರುವ UnicodeString& pattern,
                       uint32_t options,
                       ಧ್ರುವ ಚಿಹ್ನೆ_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, options, symbols, status);
    _dbgct(ಏತತ್);
}

UnicodeSet::UnicodeSet(ಧ್ರುವ UnicodeString& pattern, ParsePosition& pos,
                       uint32_t options,
                       ಧ್ರುವ ಚಿಹ್ನೆ_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, pos, options, symbols, status);
    _dbgct(ಏತತ್);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(ಧ್ರುವ UnicodeString& pattern,
                                     uint32_t options,
                                     ಧ್ರುವ ಚಿಹ್ನೆ_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    applyPattern(pattern, pos, options, symbols, status);
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ *ಏತತ್;

    int32_t i = pos.getIndex();

    ಯದಿ (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(pattern, i, ಸತ್ಯ);
    }

    ಯದಿ (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

UnicodeSet& UnicodeSet::applyPattern(ಧ್ರುವ UnicodeString& pattern,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ಧ್ರುವ ಚಿಹ್ನೆ_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, options, &UnicodeSet::closeOver, status);
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    ಯದಿ (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    setPattern(rebuiltPat);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_set_addString(USet *set, ಧ್ರುವ char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
addCaseMapping(UnicodeSet &set, int32_t result, ಧ್ರುವ char16_t *full, UnicodeString &str) {
    ಯದಿ(result >= 0) {
        ಯದಿ(result > UCASE_MAX_STRING_LENGTH) {
            // add a single-code point case mapping
            set.add(result);
        } ಅನ್ಯಥಾ {
            // add a string case mapping from full with length result
            str.setTo(static_cast<UBool>(ಅಸತ್ಯ), full, result);
            set.add(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

ನಾಮಾಕಾಶ {

/** For case closure on a large set, look only at code points with relevant properties. */
ಧ್ರುವ UnicodeSet &maybeOnlyCaseSensitive(ಧ್ರುವ UnicodeSet &src U_LIFETIME_BOUND,
                                         UnicodeSet &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.contains(0, 0x10ffff));
    ಯದಿ (src.size() < 30) {
        ಪ್ರತ್ಯಾಗಮ src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UErrorCode errorCode = U_ZERO_ERROR;
    ಧ್ರುವ UnicodeSet *sensitive =
        CharacterProperties::get_ದ್ವಿಮಾನ_Property_Set(UCHAR_CASE_SENSITIVE, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    ಯದಿ (src.get_Range_ಎಣಿಕೆ() > sensitive->get_Range_ಎಣಿಕೆ()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } ಅನ್ಯಥಾ {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    ಪ್ರತ್ಯಾಗಮ subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
ತಾರ್ಕಿಕ scfString(ಧ್ರುವ UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    ಧ್ರುವ char16_t *p = s.getBuffer();
    int32_t length = s.length();
    // Loop while not needing modification.
    ಪ್ರತಿ (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_NEXT(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        ಯದಿ (scfChar != c) {
            // Copy the characters before c.
            scf.setTo(p, i - U16_LENGTH(c));
            // Loop over the rest of the string and keep case-folding.
            ಪ್ರತಿ (;;) {
                scf.append(scfChar);
                ಯದಿ (i == length) {
                    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
                }
                U16_NEXT(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

}  // namespace

UnicodeSet& UnicodeSet::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    ಯದಿ (isFrozen() || isBogus()) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ವಿಕಲ್ಪ (attribute & USET_CASE_MASK) {
    ಪಕ್ಷ 0:
        ಭಙ್ಗ;
    ಪಕ್ಷ USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ ಅಸತ್ಯ);
        ಭಙ್ಗ;
    ಪಕ್ಷ USET_ADD_CASE_MAPPINGS:
        closeOverAddCaseMappings();
        ಭಙ್ಗ;
    ಪಕ್ಷ USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ ಸತ್ಯ);
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        // bad option (unreachable)
        ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ಶೂನ್ಯ UnicodeSet::closeOverCaseInsensitive(ತಾರ್ಕಿಕ simple) {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*ಏತತ್);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    ಯದಿ (!simple && foldSet.hasStrings()) {
        foldSet.strings_->removeAllElements();
    }

    USetAdder sa = {
        foldSet.toUSet(),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    UnicodeSet subset(0, 0x10ffff);
    ಧ್ರುವ UnicodeSet &codePoints = maybeOnlyCaseSensitive(*ಏತತ್, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_ಎಣಿಕೆ();

    ಪ್ರತಿ (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        ಯದಿ (simple) {
            ಪ್ರತಿ (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addSimpleCaseClosure(cp, &sa);
            }
        } ಅನ್ಯಥಾ {
            ಪ್ರತಿ (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addCaseClosure(cp, &sa);
            }
        }
    }
    ಯದಿ (hasStrings()) {
        UnicodeString str;
        ಪ್ರತಿ (int32_t j=0; j<strings_->size(); ++j) {
            ಧ್ರುವ UnicodeString* pStr = static_cast<ಧ್ರುವ UnicodeString*>(strings_->elementAt(j));
            ಯದಿ (simple) {
                ಯದಿ (scfString(*pStr, str)) {
                    foldSet.remove(*pStr).add(str);
                }
            } ಅನ್ಯಥಾ {
                str = *pStr;
                str.foldCase();
                ಯದಿ(!ucase_addStringCaseClosure(str.getBuffer(), str.length(), &sa)) {
                    foldSet.add(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *ಏತತ್ = foldSet;
}

ಶೂನ್ಯ UnicodeSet::closeOverAddCaseMappings() {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*ಏತತ್);

    UnicodeSet subset(0, 0x10ffff);
    ಧ್ರುವ UnicodeSet &codePoints = maybeOnlyCaseSensitive(*ಏತತ್, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_ಎಣಿಕೆ();
    UChar32 result;
    ಧ್ರುವ char16_t *full;
    UnicodeString str;

    ಪ್ರತಿ (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        ಪ್ರತಿ (UChar32 cp=start; cp<=end; ++cp) {
            result = ucase_toFullLower(cp, nullptr, nullptr, &full, UCASE_LOC_ಬೇರು);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullTitle(cp, nullptr, nullptr, &full, UCASE_LOC_ಬೇರು);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullUpper(cp, nullptr, nullptr, &full, UCASE_LOC_ಬೇರು);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullFolding(cp, &full, 0);
            addCaseMapping(foldSet, result, full, str);
        }
    }
    ಯದಿ (hasStrings()) {
        Locale ಬೇರು("");
#ಯದಿ !UCONFIG_NO_BREAK_ITERATION
        UErrorCode status = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::createWordInstance(ಬೇರು, status);
        ಯದಿ (U_SUCCESS(status)) {
#endif
            ಪ್ರತಿ (int32_t j=0; j<strings_->size(); ++j) {
                ಧ್ರುವ UnicodeString* pStr = static_cast<ಧ್ರುವ UnicodeString*>(strings_->elementAt(j));
                (str = *pStr).toLower(ಬೇರು);
                foldSet.add(str);
#ಯದಿ !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).toTitle(bi, ಬೇರು);
                foldSet.add(str);
#endif
                (str = *pStr).toUpper(ಬೇರು);
                foldSet.add(str);
                (str = *pStr).foldCase();
                foldSet.add(str);
            }
#ಯದಿ !UCONFIG_NO_BREAK_ITERATION
        }
        delete bi;
#endif
    }
    *ಏತತ್ = foldSet;
}

U_NAMESPACE_END
