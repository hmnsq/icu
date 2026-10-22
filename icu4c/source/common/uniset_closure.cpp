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

#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/parsepos.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ruleiter.h"
#অন্তর্ভাব "ucase.h"
#অন্তর্ভাব "uprops.h"
#অন্তর্ভাব "util.h"
#অন্তর্ভাব "uvector.h"

U_NAMESPACE_BEGIN

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#define _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

UnicodeSet::UnicodeSet(ধ্রুবক UnicodeString& pattern,
                       uint32_t options,
                       ধ্রুবক প্রতীক_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, options, symbols, status);
    _dbgct(এই);
}

UnicodeSet::UnicodeSet(ধ্রুবক UnicodeString& pattern, ParsePosition& pos,
                       uint32_t options,
                       ধ্রুবক প্রতীক_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, pos, options, symbols, status);
    _dbgct(এই);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(ধ্রুবক UnicodeString& pattern,
                                     uint32_t options,
                                     ধ্রুবক প্রতীক_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    applyPattern(pattern, pos, options, symbols, status);
    যদি (U_FAILURE(status)) ফেরত *এই;

    int32_t i = pos.get_সূচক();

    যদি (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(pattern, i, সত্য);
    }

    যদি (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ফেরত *এই;
}

UnicodeSet& UnicodeSet::applyPattern(ধ্রুবক UnicodeString& pattern,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ধ্রুবক প্রতীক_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    যদি (U_FAILURE(status)) {
        ফেরত *এই;
    }
    যদি (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        ফেরত *এই;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, options, &UnicodeSet::closeOver, status);
    যদি (U_FAILURE(status)) ফেরত *এই;
    যদি (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        ফেরত *এই;
    }
    setPattern(rebuiltPat);
    ফেরত *এই;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
স্থির শূন্য U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

স্থির শূন্য U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

স্থির শূন্য U_CALLCONV
_set_addString(USet *set, ধ্রুবক char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
স্থির অন্তঃরেখ শূন্য
addCaseMapping(UnicodeSet &set, int32_t ফলাফল, ধ্রুবক char16_t *full, UnicodeString &str) {
    যদি(ফলাফল >= 0) {
        যদি(ফলাফল > UCASE_MAX_STRING_দৈর্ঘ্য) {
            // add a single-code point case mapping
            set.add(ফলাফল);
        } অন্যথা {
            // add a string case mapping from full with length result
            str.setTo(static_cast<UBool>(মিথ্যা), full, ফলাফল);
            set.add(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

নামাকাশ {

/** For case closure on a large set, look only at code points with relevant properties. */
ধ্রুবক UnicodeSet &maybeOnlyCaseSensitive(ধ্রুবক UnicodeSet &src U_LIFETIME_BOUND,
                                         UnicodeSet &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.contains(0, 0x10ffff));
    যদি (src.size() < 30) {
        ফেরত src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UErrorCode errorCode = U_ZERO_ERROR;
    ধ্রুবক UnicodeSet *sensitive =
        CharacterProperties::get_দ্বিমিক_Property_Set(UCHAR_CASE_SENSITIVE, errorCode);
    যদি (U_FAILURE(errorCode)) {
        ফেরত src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    যদি (src.get_Range_গুনতি() > sensitive->get_Range_গুনতি()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } অন্যথা {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    ফেরত subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
যৌক্তিক scfString(ধ্রুবক UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    ধ্রুবক char16_t *p = s.get_বাফার();
    int32_t length = s.length();
    // Loop while not needing modification.
    জন্য (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_পরবর্তী(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        যদি (scfChar != c) {
            // Copy the characters before c.
            scf.setTo(p, i - U16_দৈর্ঘ্য(c));
            // Loop over the rest of the string and keep case-folding.
            জন্য (;;) {
                scf.append(scfChar);
                যদি (i == length) {
                    ফেরত সত্য;
                }
                U16_পরবর্তী(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    ফেরত মিথ্যা;
}

}  // namespace

UnicodeSet& UnicodeSet::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    যদি (isFrozen() || isBogus()) {
        ফেরত *এই;
    }
    নির্বাচন (attribute & USET_CASE_MASK) {
    ক্ষেত্র 0:
        বিরতি;
    ক্ষেত্র USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ মিথ্যা);
        বিরতি;
    ক্ষেত্র USET_ADD_CASE_MAPPINGS:
        closeOverAddCaseMappings();
        বিরতি;
    ক্ষেত্র USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ সত্য);
        বিরতি;
    পূর্বনির্ধারিত:
        // bad option (unreachable)
        বিরতি;
    }
    ফেরত *এই;
}

শূন্য UnicodeSet::closeOverCaseInsensitive(যৌক্তিক simple) {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*এই);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    যদি (!simple && foldSet.hasStrings()) {
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
    ধ্রুবক UnicodeSet &codePoints = maybeOnlyCaseSensitive(*এই, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_গুনতি();

    জন্য (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        যদি (simple) {
            জন্য (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addSimpleCaseClosure(cp, &sa);
            }
        } অন্যথা {
            জন্য (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addCaseClosure(cp, &sa);
            }
        }
    }
    যদি (hasStrings()) {
        UnicodeString str;
        জন্য (int32_t j=0; j<strings_->size(); ++j) {
            ধ্রুবক UnicodeString* pStr = static_cast<ধ্রুবক UnicodeString*>(strings_->elementAt(j));
            যদি (simple) {
                যদি (scfString(*pStr, str)) {
                    foldSet.remove(*pStr).add(str);
                }
            } অন্যথা {
                str = *pStr;
                str.foldCase();
                যদি(!ucase_addStringCaseClosure(str.get_বাফার(), str.length(), &sa)) {
                    foldSet.add(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *এই = foldSet;
}

শূন্য UnicodeSet::closeOverAddCaseMappings() {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*এই);

    UnicodeSet subset(0, 0x10ffff);
    ধ্রুবক UnicodeSet &codePoints = maybeOnlyCaseSensitive(*এই, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_গুনতি();
    UChar32 ফলাফল;
    ধ্রুবক char16_t *full;
    UnicodeString str;

    জন্য (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        জন্য (UChar32 cp=start; cp<=end; ++cp) {
            ফলাফল = ucase_toFullLower(cp, nullptr, nullptr, &full, UCASE_LOC_মূল);
            addCaseMapping(foldSet, ফলাফল, full, str);

            ফলাফল = ucase_toFullTitle(cp, nullptr, nullptr, &full, UCASE_LOC_মূল);
            addCaseMapping(foldSet, ফলাফল, full, str);

            ফলাফল = ucase_toFullUpper(cp, nullptr, nullptr, &full, UCASE_LOC_মূল);
            addCaseMapping(foldSet, ফলাফল, full, str);

            ফলাফল = ucase_toFullFolding(cp, &full, 0);
            addCaseMapping(foldSet, ফলাফল, full, str);
        }
    }
    যদি (hasStrings()) {
        Locale মূল("");
#যদি !UCONFIG_NO_BREAK_ITERATION
        UErrorCode status = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::createWordInstance(মূল, status);
        যদি (U_SUCCESS(status)) {
#endif
            জন্য (int32_t j=0; j<strings_->size(); ++j) {
                ধ্রুবক UnicodeString* pStr = static_cast<ধ্রুবক UnicodeString*>(strings_->elementAt(j));
                (str = *pStr).toLower(মূল);
                foldSet.add(str);
#যদি !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).toTitle(bi, মূল);
                foldSet.add(str);
#endif
                (str = *pStr).toUpper(মূল);
                foldSet.add(str);
                (str = *pStr).foldCase();
                foldSet.add(str);
            }
#যদি !UCONFIG_NO_BREAK_ITERATION
        }
        delete bi;
#endif
    }
    *এই = foldSet;
}

U_NAMESPACE_END
