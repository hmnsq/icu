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

#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/parsepos.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "ruleiter.h"
#ଅନ୍ତର୍ଭାଵ "ucase.h"
#ଅନ୍ତର୍ଭାଵ "uprops.h"
#ଅନ୍ତର୍ଭାଵ "util.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"

U_NAMESPACE_BEGIN

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#define _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

UnicodeSet::UnicodeSet(ଅଚଳ UnicodeString& pattern,
                       uint32_t options,
                       ଅଚଳ ପ୍ରତୀକ_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, options, symbols, status);
    _dbgct(ଏହି);
}

UnicodeSet::UnicodeSet(ଅଚଳ UnicodeString& pattern, ParsePosition& pos,
                       uint32_t options,
                       ଅଚଳ ପ୍ରତୀକ_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, pos, options, symbols, status);
    _dbgct(ଏହି);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(ଅଚଳ UnicodeString& pattern,
                                     uint32_t options,
                                     ଅଚଳ ପ୍ରତୀକ_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    applyPattern(pattern, pos, options, symbols, status);
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ *ଏହି;

    int32_t i = pos.getIndex();

    ଯଦି (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(pattern, i, ସତ୍ୟ);
    }

    ଯଦି (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

UnicodeSet& UnicodeSet::applyPattern(ଅଚଳ UnicodeString& pattern,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ଅଚଳ ପ୍ରତୀକ_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        ଫେରନ୍ତୁ *ଏହି;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, options, &UnicodeSet::closeOver, status);
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ *ଏହି;
    ଯଦି (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        ଫେରନ୍ତୁ *ଏହି;
    }
    setPattern(rebuiltPat);
    ଫେରନ୍ତୁ *ଏହି;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
_set_addString(USet *set, ଅଚଳ char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
addCaseMapping(UnicodeSet &set, int32_t result, ଅଚଳ char16_t *full, UnicodeString &str) {
    ଯଦି(result >= 0) {
        ଯଦି(result > UCASE_MAX_STRING_LENGTH) {
            // add a single-code point case mapping
            set.add(result);
        } ଅନ୍ୟଥା {
            // add a string case mapping from full with length result
            str.setTo(static_cast<UBool>(ମିଥ୍ୟା), full, result);
            set.add(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

ନାମାକାଶ {

/** For case closure on a large set, look only at code points with relevant properties. */
ଅଚଳ UnicodeSet &maybeOnlyCaseSensitive(ଅଚଳ UnicodeSet &src U_LIFETIME_BOUND,
                                         UnicodeSet &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.contains(0, 0x10ffff));
    ଯଦି (src.size() < 30) {
        ଫେରନ୍ତୁ src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UErrorCode errorCode = U_ZERO_ERROR;
    ଅଚଳ UnicodeSet *sensitive =
        CharacterProperties::get_ଦ୍ୱିମିକ_Property_Set(UCHAR_CASE_SENSITIVE, errorCode);
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    ଯଦି (src.get_Range_ଗଣନା() > sensitive->get_Range_ଗଣନା()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } ଅନ୍ୟଥା {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    ଫେରନ୍ତୁ subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
ତାର୍କିକ scfString(ଅଚଳ UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    ଅଚଳ char16_t *p = s.getBuffer();
    int32_t length = s.length();
    // Loop while not needing modification.
    ପାଇଁ (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_NEXT(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        ଯଦି (scfChar != c) {
            // Copy the characters before c.
            scf.setTo(p, i - U16_LENGTH(c));
            // Loop over the rest of the string and keep case-folding.
            ପାଇଁ (;;) {
                scf.append(scfChar);
                ଯଦି (i == length) {
                    ଫେରନ୍ତୁ ସତ୍ୟ;
                }
                U16_NEXT(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

}  // namespace

UnicodeSet& UnicodeSet::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    ଯଦି (isFrozen() || isBogus()) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ବାଛ (attribute & USET_CASE_MASK) {
    ସ୍ଥିତି 0:
        ଭାଙ୍ଗ;
    ସ୍ଥିତି USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ ମିଥ୍ୟା);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି USET_ADD_CASE_MAPPINGS:
        closeOverAddCaseMappings();
        ଭାଙ୍ଗ;
    ସ୍ଥିତି USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ ସତ୍ୟ);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        // bad option (unreachable)
        ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

ଶୂନ୍ୟ UnicodeSet::closeOverCaseInsensitive(ତାର୍କିକ simple) {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*ଏହି);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    ଯଦି (!simple && foldSet.hasStrings()) {
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
    ଅଚଳ UnicodeSet &codePoints = maybeOnlyCaseSensitive(*ଏହି, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_ଗଣନା();

    ପାଇଁ (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        ଯଦି (simple) {
            ପାଇଁ (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addSimpleCaseClosure(cp, &sa);
            }
        } ଅନ୍ୟଥା {
            ପାଇଁ (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addCaseClosure(cp, &sa);
            }
        }
    }
    ଯଦି (hasStrings()) {
        UnicodeString str;
        ପାଇଁ (int32_t j=0; j<strings_->size(); ++j) {
            ଅଚଳ UnicodeString* pStr = static_cast<ଅଚଳ UnicodeString*>(strings_->elementAt(j));
            ଯଦି (simple) {
                ଯଦି (scfString(*pStr, str)) {
                    foldSet.remove(*pStr).add(str);
                }
            } ଅନ୍ୟଥା {
                str = *pStr;
                str.foldCase();
                ଯଦି(!ucase_addStringCaseClosure(str.getBuffer(), str.length(), &sa)) {
                    foldSet.add(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *ଏହି = foldSet;
}

ଶୂନ୍ୟ UnicodeSet::closeOverAddCaseMappings() {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*ଏହି);

    UnicodeSet subset(0, 0x10ffff);
    ଅଚଳ UnicodeSet &codePoints = maybeOnlyCaseSensitive(*ଏହି, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_ଗଣନା();
    UChar32 result;
    ଅଚଳ char16_t *full;
    UnicodeString str;

    ପାଇଁ (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        ପାଇଁ (UChar32 cp=start; cp<=end; ++cp) {
            result = ucase_toFullLower(cp, nullptr, nullptr, &full, UCASE_LOC_ମୂଳ);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullTitle(cp, nullptr, nullptr, &full, UCASE_LOC_ମୂଳ);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullUpper(cp, nullptr, nullptr, &full, UCASE_LOC_ମୂଳ);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullFolding(cp, &full, 0);
            addCaseMapping(foldSet, result, full, str);
        }
    }
    ଯଦି (hasStrings()) {
        Locale ମୂଳ("");
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
        UErrorCode status = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::createWordInstance(ମୂଳ, status);
        ଯଦି (U_SUCCESS(status)) {
#endif
            ପାଇଁ (int32_t j=0; j<strings_->size(); ++j) {
                ଅଚଳ UnicodeString* pStr = static_cast<ଅଚଳ UnicodeString*>(strings_->elementAt(j));
                (str = *pStr).toLower(ମୂଳ);
                foldSet.add(str);
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).toTitle(bi, ମୂଳ);
                foldSet.add(str);
#endif
                (str = *pStr).toUpper(ମୂଳ);
                foldSet.add(str);
                (str = *pStr).foldCase();
                foldSet.add(str);
            }
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
        }
        delete bi;
#endif
    }
    *ଏହି = foldSet;
}

U_NAMESPACE_END
