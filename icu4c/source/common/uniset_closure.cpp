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

#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/parsepos.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ruleiter.h"
#அந்தர்பாவ "ucase.h"
#அந்தர்பாவ "uprops.h"
#அந்தர்பாவ "util.h"
#அந்தர்பாவ "uvector.h"

U_NAMESPACE_BEGIN

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#define _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

UnicodeSet::UnicodeSet(மாறிலி UnicodeString& pattern,
                       uint32_t options,
                       மாறிலி குறியீடு_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, options, symbols, status);
    _dbgct(இது);
}

UnicodeSet::UnicodeSet(மாறிலி UnicodeString& pattern, ParsePosition& pos,
                       uint32_t options,
                       மாறிலி குறியீடு_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, pos, options, symbols, status);
    _dbgct(இது);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(மாறிலி UnicodeString& pattern,
                                     uint32_t options,
                                     மாறிலி குறியீடு_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    applyPattern(pattern, pos, options, symbols, status);
    எனில் (U_FAILURE(status)) திரும்பு *இது;

    int32_t i = pos.getIndex();

    எனில் (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(pattern, i, உண்மை);
    }

    எனில் (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    திரும்பு *இது;
}

UnicodeSet& UnicodeSet::applyPattern(மாறிலி UnicodeString& pattern,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     மாறிலி குறியீடு_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    எனில் (U_FAILURE(status)) {
        திரும்பு *இது;
    }
    எனில் (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        திரும்பு *இது;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, options, &UnicodeSet::closeOver, status);
    எனில் (U_FAILURE(status)) திரும்பு *இது;
    எனில் (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        திரும்பு *இது;
    }
    setPattern(rebuiltPat);
    திரும்பு *இது;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
நிலையான வெறுமை U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

நிலையான வெறுமை U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

நிலையான வெறுமை U_CALLCONV
_set_addString(USet *set, மாறிலி char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
நிலையான உள்வரி வெறுமை
addCaseMapping(UnicodeSet &set, int32_t result, மாறிலி char16_t *full, UnicodeString &str) {
    எனில்(result >= 0) {
        எனில்(result > UCASE_MAX_STRING_LENGTH) {
            // add a single-code point case mapping
            set.add(result);
        } இல்லையெனில் {
            // add a string case mapping from full with length result
            str.setTo(static_cast<UBool>(பொய்), full, result);
            set.add(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

நாமாகாஶ {

/** For case closure on a large set, look only at code points with relevant properties. */
மாறிலி UnicodeSet &maybeOnlyCaseSensitive(மாறிலி UnicodeSet &src U_LIFETIME_BOUND,
                                         UnicodeSet &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.contains(0, 0x10ffff));
    எனில் (src.size() < 30) {
        திரும்பு src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UErrorCode errorCode = U_ZERO_ERROR;
    மாறிலி UnicodeSet *sensitive =
        CharacterProperties::get_இருமம்_Property_Set(UCHAR_CASE_SENSITIVE, errorCode);
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    எனில் (src.get_Range_மொத்தம்() > sensitive->get_Range_மொத்தம்()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } இல்லையெனில் {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    திரும்பு subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
தருக்க scfString(மாறிலி UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    மாறிலி char16_t *p = s.getBuffer();
    int32_t length = s.length();
    // Loop while not needing modification.
    க்கு (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_NEXT(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        எனில் (scfChar != c) {
            // Copy the characters before c.
            scf.setTo(p, i - U16_LENGTH(c));
            // Loop over the rest of the string and keep case-folding.
            க்கு (;;) {
                scf.append(scfChar);
                எனில் (i == length) {
                    திரும்பு உண்மை;
                }
                U16_NEXT(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    திரும்பு பொய்;
}

}  // namespace

UnicodeSet& UnicodeSet::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    எனில் (isFrozen() || isBogus()) {
        திரும்பு *இது;
    }
    தேர்வு (attribute & USET_CASE_MASK) {
    நிலை 0:
        நிறுத்து;
    நிலை USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ பொய்);
        நிறுத்து;
    நிலை USET_ADD_CASE_MAPPINGS:
        closeOverAddCaseMappings();
        நிறுத்து;
    நிலை USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ உண்மை);
        நிறுத்து;
    இயல்பு:
        // bad option (unreachable)
        நிறுத்து;
    }
    திரும்பு *இது;
}

வெறுமை UnicodeSet::closeOverCaseInsensitive(தருக்க simple) {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*இது);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    எனில் (!simple && foldSet.hasStrings()) {
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
    மாறிலி UnicodeSet &codePoints = maybeOnlyCaseSensitive(*இது, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_மொத்தம்();

    க்கு (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        எனில் (simple) {
            க்கு (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addSimpleCaseClosure(cp, &sa);
            }
        } இல்லையெனில் {
            க்கு (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addCaseClosure(cp, &sa);
            }
        }
    }
    எனில் (hasStrings()) {
        UnicodeString str;
        க்கு (int32_t j=0; j<strings_->size(); ++j) {
            மாறிலி UnicodeString* pStr = static_cast<மாறிலி UnicodeString*>(strings_->elementAt(j));
            எனில் (simple) {
                எனில் (scfString(*pStr, str)) {
                    foldSet.remove(*pStr).add(str);
                }
            } இல்லையெனில் {
                str = *pStr;
                str.foldCase();
                எனில்(!ucase_addStringCaseClosure(str.getBuffer(), str.length(), &sa)) {
                    foldSet.add(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *இது = foldSet;
}

வெறுமை UnicodeSet::closeOverAddCaseMappings() {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*இது);

    UnicodeSet subset(0, 0x10ffff);
    மாறிலி UnicodeSet &codePoints = maybeOnlyCaseSensitive(*இது, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_மொத்தம்();
    UChar32 result;
    மாறிலி char16_t *full;
    UnicodeString str;

    க்கு (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        க்கு (UChar32 cp=start; cp<=end; ++cp) {
            result = ucase_toFullLower(cp, nullptr, nullptr, &full, UCASE_LOC_வேர்);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullTitle(cp, nullptr, nullptr, &full, UCASE_LOC_வேர்);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullUpper(cp, nullptr, nullptr, &full, UCASE_LOC_வேர்);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullFolding(cp, &full, 0);
            addCaseMapping(foldSet, result, full, str);
        }
    }
    எனில் (hasStrings()) {
        Locale வேர்("");
#எனில் !UCONFIG_NO_BREAK_ITERATION
        UErrorCode status = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::createWordInstance(வேர், status);
        எனில் (U_SUCCESS(status)) {
#endif
            க்கு (int32_t j=0; j<strings_->size(); ++j) {
                மாறிலி UnicodeString* pStr = static_cast<மாறிலி UnicodeString*>(strings_->elementAt(j));
                (str = *pStr).toLower(வேர்);
                foldSet.add(str);
#எனில் !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).toTitle(bi, வேர்);
                foldSet.add(str);
#endif
                (str = *pStr).toUpper(வேர்);
                foldSet.add(str);
                (str = *pStr).foldCase();
                foldSet.add(str);
            }
#எனில் !UCONFIG_NO_BREAK_ITERATION
        }
        delete bi;
#endif
    }
    *இது = foldSet;
}

U_NAMESPACE_END
