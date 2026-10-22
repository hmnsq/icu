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

#شامل "unicode/brkiter.h"
#شامل "unicode/locid.h"
#شامل "unicode/parsepos.h"
#شامل "unicode/uniset.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "ruleiter.h"
#شامل "ucase.h"
#شامل "uprops.h"
#شامل "util.h"
#شامل "uvector.h"

U_NAMESPACE_BEGIN

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#define _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

UnicodeSet::UnicodeSet(ثابت UnicodeString& pattern,
                       uint32_t options,
                       ثابت نماد_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, options, symbols, status);
    _dbgct(این);
}

UnicodeSet::UnicodeSet(ثابت UnicodeString& pattern, ParsePosition& pos,
                       uint32_t options,
                       ثابت نماد_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, pos, options, symbols, status);
    _dbgct(این);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(ثابت UnicodeString& pattern,
                                     uint32_t options,
                                     ثابت نماد_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    applyPattern(pattern, pos, options, symbols, status);
    اگر (U_FAILURE(status)) بازگشت *این;

    int32_t i = pos.getIndex();

    اگر (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(pattern, i, درست);
    }

    اگر (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    بازگشت *این;
}

UnicodeSet& UnicodeSet::applyPattern(ثابت UnicodeString& pattern,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ثابت نماد_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    اگر (U_FAILURE(status)) {
        بازگشت *این;
    }
    اگر (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        بازگشت *این;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, options, &UnicodeSet::closeOver, status);
    اگر (U_FAILURE(status)) بازگشت *این;
    اگر (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        بازگشت *این;
    }
    setPattern(rebuiltPat);
    بازگشت *این;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ایستا تهی U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

ایستا تهی U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

ایستا تهی U_CALLCONV
_set_addString(USet *set, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
ایستا درونخطی تهی
addCaseMapping(UnicodeSet &set, int32_t result, ثابت char16_t *full, UnicodeString &str) {
    اگر(result >= 0) {
        اگر(result > UCASE_MAX_STRING_LENGTH) {
            // add a single-code point case mapping
            set.add(result);
        } وگرنه {
            // add a string case mapping from full with length result
            str.setTo(static_cast<UBool>(نادرست), full, result);
            set.add(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

فضاینام {

/** For case closure on a large set, look only at code points with relevant properties. */
ثابت UnicodeSet &maybeOnlyCaseSensitive(ثابت UnicodeSet &src U_LIFETIME_BOUND,
                                         UnicodeSet &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.contains(0, 0x10ffff));
    اگر (src.size() < 30) {
        بازگشت src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UErrorCode errorCode = U_ZERO_ERROR;
    ثابت UnicodeSet *sensitive =
        CharacterProperties::get_دودویی_Property_Set(UCHAR_CASE_SENSITIVE, errorCode);
    اگر (U_FAILURE(errorCode)) {
        بازگشت src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    اگر (src.get_Range_شمار() > sensitive->get_Range_شمار()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } وگرنه {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    بازگشت subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
منطقی scfString(ثابت UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    ثابت char16_t *p = s.getBuffer();
    int32_t length = s.length();
    // Loop while not needing modification.
    برای (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_NEXT(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        اگر (scfChar != c) {
            // Copy the characters before c.
            scf.setTo(p, i - U16_LENGTH(c));
            // Loop over the rest of the string and keep case-folding.
            برای (;;) {
                scf.append(scfChar);
                اگر (i == length) {
                    بازگشت درست;
                }
                U16_NEXT(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    بازگشت نادرست;
}

}  // namespace

UnicodeSet& UnicodeSet::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        بازگشت *این;
    }
    انتخاب (attribute & USET_CASE_MASK) {
    حالت 0:
        بشکن;
    حالت USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ نادرست);
        بشکن;
    حالت USET_ADD_CASE_MAPPINGS:
        closeOverAddCaseMappings();
        بشکن;
    حالت USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ درست);
        بشکن;
    پیشفرض:
        // bad option (unreachable)
        بشکن;
    }
    بازگشت *این;
}

تهی UnicodeSet::closeOverCaseInsensitive(منطقی simple) {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*این);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    اگر (!simple && foldSet.hasStrings()) {
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
    ثابت UnicodeSet &codePoints = maybeOnlyCaseSensitive(*این, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_شمار();

    برای (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        اگر (simple) {
            برای (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addSimpleCaseClosure(cp, &sa);
            }
        } وگرنه {
            برای (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addCaseClosure(cp, &sa);
            }
        }
    }
    اگر (hasStrings()) {
        UnicodeString str;
        برای (int32_t j=0; j<strings_->size(); ++j) {
            ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(strings_->elementAt(j));
            اگر (simple) {
                اگر (scfString(*pStr, str)) {
                    foldSet.remove(*pStr).add(str);
                }
            } وگرنه {
                str = *pStr;
                str.foldCase();
                اگر(!ucase_addStringCaseClosure(str.getBuffer(), str.length(), &sa)) {
                    foldSet.add(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *این = foldSet;
}

تهی UnicodeSet::closeOverAddCaseMappings() {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*این);

    UnicodeSet subset(0, 0x10ffff);
    ثابت UnicodeSet &codePoints = maybeOnlyCaseSensitive(*این, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_شمار();
    UChar32 result;
    ثابت char16_t *full;
    UnicodeString str;

    برای (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        برای (UChar32 cp=start; cp<=end; ++cp) {
            result = ucase_toFullLower(cp, nullptr, nullptr, &full, UCASE_LOC_ریشه);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullTitle(cp, nullptr, nullptr, &full, UCASE_LOC_ریشه);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullUpper(cp, nullptr, nullptr, &full, UCASE_LOC_ریشه);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullFolding(cp, &full, 0);
            addCaseMapping(foldSet, result, full, str);
        }
    }
    اگر (hasStrings()) {
        Locale ریشه("");
#اگر !UCONFIG_NO_BREAK_ITERATION
        UErrorCode status = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::createWordInstance(ریشه, status);
        اگر (U_SUCCESS(status)) {
#endif
            برای (int32_t j=0; j<strings_->size(); ++j) {
                ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(strings_->elementAt(j));
                (str = *pStr).toLower(ریشه);
                foldSet.add(str);
#اگر !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).toTitle(bi, ریشه);
                foldSet.add(str);
#endif
                (str = *pStr).toUpper(ریشه);
                foldSet.add(str);
                (str = *pStr).foldCase();
                foldSet.add(str);
            }
#اگر !UCONFIG_NO_BREAK_ITERATION
        }
        delete bi;
#endif
    }
    *این = foldSet;
}

U_NAMESPACE_END
