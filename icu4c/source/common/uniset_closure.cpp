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

#समावेश "unicode/brkiter.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/parsepos.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "ruleiter.h"
#समावेश "ucase.h"
#समावेश "uprops.h"
#समावेश "util.h"
#समावेश "uvector.h"

U_NAMESPACE_BEGIN

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#घोषणा _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

UnicodeSet::UnicodeSet(स्थिर UnicodeString& pattern,
                       uint32_t options,
                       स्थिर प्रतीक_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, options, symbols, status);
    _dbgct(यह);
}

UnicodeSet::UnicodeSet(स्थिर UnicodeString& pattern, ParsePosition& pos,
                       uint32_t options,
                       स्थिर प्रतीक_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, pos, options, symbols, status);
    _dbgct(यह);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(स्थिर UnicodeString& pattern,
                                     uint32_t options,
                                     स्थिर प्रतीक_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    applyPattern(pattern, pos, options, symbols, status);
    अगर (U_FAILURE(status)) वापस *यह;

    int32_t i = pos.get_सूचकांक();

    अगर (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(pattern, i, true);
    }

    अगर (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    वापस *यह;
}

UnicodeSet& UnicodeSet::applyPattern(स्थिर UnicodeString& pattern,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     स्थिर प्रतीक_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    अगर (U_FAILURE(status)) {
        वापस *यह;
    }
    अगर (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        वापस *यह;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, options, &UnicodeSet::closeOver, status);
    अगर (U_FAILURE(status)) वापस *यह;
    अगर (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        वापस *यह;
    }
    setPattern(rebuiltPat);
    वापस *यह;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
अटल व्योम U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

अटल व्योम U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

अटल व्योम U_CALLCONV
_set_addString(USet *set, स्थिर char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
अटल अंतरभूत व्योम
addCaseMapping(UnicodeSet &set, int32_t परिणाम, स्थिर char16_t *full, UnicodeString &str) {
    अगर(परिणाम >= 0) {
        अगर(परिणाम > UCASE_MAX_STRING_लंबाई) {
            // add a single-code point case mapping
            set.add(परिणाम);
        } अन्यथा {
            // add a string case mapping from full with length result
            str.setTo(static_cast<UBool>(false), full, परिणाम);
            set.add(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

namespace {

/** For case closure on a large set, look only at code points with relevant properties. */
स्थिर UnicodeSet &maybeOnlyCaseSensitive(स्थिर UnicodeSet &src U_LIFETIME_BOUND,
                                         UnicodeSet &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.contains(0, 0x10ffff));
    अगर (src.size() < 30) {
        वापस src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UErrorCode errorCode = U_ZERO_ERROR;
    स्थिर UnicodeSet *sensitive =
        CharacterProperties::get_द्विआधारी_Property_Set(UCHAR_CASE_SENSITIVE, errorCode);
    अगर (U_FAILURE(errorCode)) {
        वापस src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    अगर (src.get_Range_गिनती() > sensitive->get_Range_गिनती()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } अन्यथा {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    वापस subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
bool scfString(स्थिर UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    स्थिर char16_t *p = s.get_बफर();
    int32_t length = s.length();
    // Loop while not needing modification.
    क्रम (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_अग्र(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        अगर (scfChar != c) {
            // Copy the characters before c.
            scf.setTo(p, i - U16_लंबाई(c));
            // Loop over the rest of the string and keep case-folding.
            क्रम (;;) {
                scf.append(scfChar);
                अगर (i == length) {
                    वापस true;
                }
                U16_अग्र(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    वापस false;
}

}  // namespace

UnicodeSet& UnicodeSet::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    अगर (isFrozen() || isBogus()) {
        वापस *यह;
    }
    चयन (attribute & USET_CASE_MASK) {
    हाल 0:
        अवरोध;
    हाल USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ false);
        अवरोध;
    हाल USET_ADD_CASE_MAPPINGS:
        closeOverAddCaseMappings();
        अवरोध;
    हाल USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ true);
        अवरोध;
    शेष:
        // bad option (unreachable)
        अवरोध;
    }
    वापस *यह;
}

व्योम UnicodeSet::closeOverCaseInsensitive(bool simple) {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*यह);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    अगर (!simple && foldSet.hasStrings()) {
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
    स्थिर UnicodeSet &codePoints = maybeOnlyCaseSensitive(*यह, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_गिनती();

    क्रम (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        अगर (simple) {
            क्रम (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addSimpleCaseClosure(cp, &sa);
            }
        } अन्यथा {
            क्रम (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addCaseClosure(cp, &sa);
            }
        }
    }
    अगर (hasStrings()) {
        UnicodeString str;
        क्रम (int32_t j=0; j<strings_->size(); ++j) {
            स्थिर UnicodeString* pStr = static_cast<स्थिर UnicodeString*>(strings_->elementAt(j));
            अगर (simple) {
                अगर (scfString(*pStr, str)) {
                    foldSet.हटाओ(*pStr).add(str);
                }
            } अन्यथा {
                str = *pStr;
                str.foldCase();
                अगर(!ucase_addStringCaseClosure(str.get_बफर(), str.length(), &sa)) {
                    foldSet.add(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *यह = foldSet;
}

व्योम UnicodeSet::closeOverAddCaseMappings() {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*यह);

    UnicodeSet subset(0, 0x10ffff);
    स्थिर UnicodeSet &codePoints = maybeOnlyCaseSensitive(*यह, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_गिनती();
    UChar32 परिणाम;
    स्थिर char16_t *full;
    UnicodeString str;

    क्रम (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        क्रम (UChar32 cp=start; cp<=end; ++cp) {
            परिणाम = ucase_toFullLower(cp, nullptr, nullptr, &full, UCASE_LOC_मूल);
            addCaseMapping(foldSet, परिणाम, full, str);

            परिणाम = ucase_toFullTitle(cp, nullptr, nullptr, &full, UCASE_LOC_मूल);
            addCaseMapping(foldSet, परिणाम, full, str);

            परिणाम = ucase_toFullUpper(cp, nullptr, nullptr, &full, UCASE_LOC_मूल);
            addCaseMapping(foldSet, परिणाम, full, str);

            परिणाम = ucase_toFullFolding(cp, &full, 0);
            addCaseMapping(foldSet, परिणाम, full, str);
        }
    }
    अगर (hasStrings()) {
        Locale मूल("");
#अगर !UCONFIG_NO_BREAK_ITERATION
        UErrorCode status = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::createWordInstance(मूल, status);
        अगर (U_SUCCESS(status)) {
#पूर्ण_अगर
            क्रम (int32_t j=0; j<strings_->size(); ++j) {
                स्थिर UnicodeString* pStr = static_cast<स्थिर UnicodeString*>(strings_->elementAt(j));
                (str = *pStr).toLower(मूल);
                foldSet.add(str);
#अगर !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).toTitle(bi, मूल);
                foldSet.add(str);
#पूर्ण_अगर
                (str = *pStr).toUpper(मूल);
                foldSet.add(str);
                (str = *pStr).foldCase();
                foldSet.add(str);
            }
#अगर !UCONFIG_NO_BREAK_ITERATION
        }
        मिटाओ bi;
#पूर्ण_अगर
    }
    *यह = foldSet;
}

U_NAMESPACE_END
