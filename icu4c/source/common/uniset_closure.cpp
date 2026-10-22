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

#અન્તર્ભાવ "unicode/brkiter.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/parsepos.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "ruleiter.h"
#અન્તર્ભાવ "ucase.h"
#અન્તર્ભાવ "uprops.h"
#અન્તર્ભાવ "util.h"
#અન્તર્ભાવ "uvector.h"

U_NAMESPACE_BEGIN

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#define _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

UnicodeSet::UnicodeSet(અચલ UnicodeString& pattern,
                       uint32_t options,
                       અચલ પ્રતીક_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, options, symbols, status);
    _dbgct(આ);
}

UnicodeSet::UnicodeSet(અચલ UnicodeString& pattern, ParsePosition& pos,
                       uint32_t options,
                       અચલ પ્રતીક_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, pos, options, symbols, status);
    _dbgct(આ);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(અચલ UnicodeString& pattern,
                                     uint32_t options,
                                     અચલ પ્રતીક_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    applyPattern(pattern, pos, options, symbols, status);
    જો (U_FAILURE(status)) પાછા *આ;

    int32_t i = pos.getIndex();

    જો (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(pattern, i, સાચું);
    }

    જો (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    પાછા *આ;
}

UnicodeSet& UnicodeSet::applyPattern(અચલ UnicodeString& pattern,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     અચલ પ્રતીક_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    જો (U_FAILURE(status)) {
        પાછા *આ;
    }
    જો (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        પાછા *આ;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, options, &UnicodeSet::closeOver, status);
    જો (U_FAILURE(status)) પાછા *આ;
    જો (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        પાછા *આ;
    }
    setPattern(rebuiltPat);
    પાછા *આ;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
સ્થિર શૂન્ય U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

સ્થિર શૂન્ય U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

સ્થિર શૂન્ય U_CALLCONV
_set_addString(USet *set, અચલ char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
સ્થિર અંતઃરેખ શૂન્ય
addCaseMapping(UnicodeSet &set, int32_t result, અચલ char16_t *full, UnicodeString &str) {
    જો(result >= 0) {
        જો(result > UCASE_MAX_STRING_LENGTH) {
            // add a single-code point case mapping
            set.add(result);
        } નહીંતર {
            // add a string case mapping from full with length result
            str.setTo(static_cast<UBool>(ખોટું), full, result);
            set.add(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

નામાકાશ {

/** For case closure on a large set, look only at code points with relevant properties. */
અચલ UnicodeSet &maybeOnlyCaseSensitive(અચલ UnicodeSet &src U_LIFETIME_BOUND,
                                         UnicodeSet &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.contains(0, 0x10ffff));
    જો (src.size() < 30) {
        પાછા src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UErrorCode errorCode = U_ZERO_ERROR;
    અચલ UnicodeSet *sensitive =
        CharacterProperties::get_દ્વિઅંકી_Property_Set(UCHAR_CASE_SENSITIVE, errorCode);
    જો (U_FAILURE(errorCode)) {
        પાછા src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    જો (src.get_Range_ગણતરી() > sensitive->get_Range_ગણતરી()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } નહીંતર {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    પાછા subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
તાર્કિક scfString(અચલ UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    અચલ char16_t *p = s.getBuffer();
    int32_t length = s.length();
    // Loop while not needing modification.
    માટે (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_NEXT(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        જો (scfChar != c) {
            // Copy the characters before c.
            scf.setTo(p, i - U16_LENGTH(c));
            // Loop over the rest of the string and keep case-folding.
            માટે (;;) {
                scf.append(scfChar);
                જો (i == length) {
                    પાછા સાચું;
                }
                U16_NEXT(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    પાછા ખોટું;
}

}  // namespace

UnicodeSet& UnicodeSet::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    જો (isFrozen() || isBogus()) {
        પાછા *આ;
    }
    પસંદગી (attribute & USET_CASE_MASK) {
    સ્થિતિ 0:
        તોડો;
    સ્થિતિ USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ ખોટું);
        તોડો;
    સ્થિતિ USET_ADD_CASE_MAPPINGS:
        closeOverAddCaseMappings();
        તોડો;
    સ્થિતિ USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ સાચું);
        તોડો;
    મૂળભૂત:
        // bad option (unreachable)
        તોડો;
    }
    પાછા *આ;
}

શૂન્ય UnicodeSet::closeOverCaseInsensitive(તાર્કિક simple) {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*આ);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    જો (!simple && foldSet.hasStrings()) {
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
    અચલ UnicodeSet &codePoints = maybeOnlyCaseSensitive(*આ, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_ગણતરી();

    માટે (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        જો (simple) {
            માટે (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addSimpleCaseClosure(cp, &sa);
            }
        } નહીંતર {
            માટે (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addCaseClosure(cp, &sa);
            }
        }
    }
    જો (hasStrings()) {
        UnicodeString str;
        માટે (int32_t j=0; j<strings_->size(); ++j) {
            અચલ UnicodeString* pStr = static_cast<અચલ UnicodeString*>(strings_->elementAt(j));
            જો (simple) {
                જો (scfString(*pStr, str)) {
                    foldSet.remove(*pStr).add(str);
                }
            } નહીંતર {
                str = *pStr;
                str.foldCase();
                જો(!ucase_addStringCaseClosure(str.getBuffer(), str.length(), &sa)) {
                    foldSet.add(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *આ = foldSet;
}

શૂન્ય UnicodeSet::closeOverAddCaseMappings() {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*આ);

    UnicodeSet subset(0, 0x10ffff);
    અચલ UnicodeSet &codePoints = maybeOnlyCaseSensitive(*આ, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_ગણતરી();
    UChar32 result;
    અચલ char16_t *full;
    UnicodeString str;

    માટે (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        માટે (UChar32 cp=start; cp<=end; ++cp) {
            result = ucase_toFullLower(cp, nullptr, nullptr, &full, UCASE_LOC_મૂળ);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullTitle(cp, nullptr, nullptr, &full, UCASE_LOC_મૂળ);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullUpper(cp, nullptr, nullptr, &full, UCASE_LOC_મૂળ);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullFolding(cp, &full, 0);
            addCaseMapping(foldSet, result, full, str);
        }
    }
    જો (hasStrings()) {
        Locale મૂળ("");
#જો !UCONFIG_NO_BREAK_ITERATION
        UErrorCode status = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::createWordInstance(મૂળ, status);
        જો (U_SUCCESS(status)) {
#endif
            માટે (int32_t j=0; j<strings_->size(); ++j) {
                અચલ UnicodeString* pStr = static_cast<અચલ UnicodeString*>(strings_->elementAt(j));
                (str = *pStr).toLower(મૂળ);
                foldSet.add(str);
#જો !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).toTitle(bi, મૂળ);
                foldSet.add(str);
#endif
                (str = *pStr).toUpper(મૂળ);
                foldSet.add(str);
                (str = *pStr).foldCase();
                foldSet.add(str);
            }
#જો !UCONFIG_NO_BREAK_ITERATION
        }
        delete bi;
#endif
    }
    *આ = foldSet;
}

U_NAMESPACE_END
