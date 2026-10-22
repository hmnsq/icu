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

#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/parsepos.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ruleiter.h"
#अन्तर्भाव "ucase.h"
#अन्तर्भाव "uprops.h"
#अन्तर्भाव "util.h"
#अन्तर्भाव "uvector.h"

U_NAMESPACE_BEGIN

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#define _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

UnicodeSet::UnicodeSet(ध्रुव UnicodeString& pattern,
                       uint32_t options,
                       ध्रुव प्रतीक_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, options, symbols, status);
    _dbgct(एतत्);
}

UnicodeSet::UnicodeSet(ध्रुव UnicodeString& pattern, ParsePosition& pos,
                       uint32_t options,
                       ध्रुव प्रतीक_Table* symbols,
                       UErrorCode& status) {
    applyPattern(pattern, pos, options, symbols, status);
    _dbgct(एतत्);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(ध्रुव UnicodeString& pattern,
                                     uint32_t options,
                                     ध्रुव प्रतीक_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    applyPattern(pattern, pos, options, symbols, status);
    यदि (U_FAILURE(status)) प्रत्यागम *एतत्;

    int32_t i = pos.getIndex();

    यदि (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(pattern, i, सत्य);
    }

    यदि (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    प्रत्यागम *एतत्;
}

UnicodeSet& UnicodeSet::applyPattern(ध्रुव UnicodeString& pattern,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ध्रुव प्रतीक_Table* symbols,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    यदि (U_FAILURE(status)) {
        प्रत्यागम *एतत्;
    }
    यदि (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        प्रत्यागम *एतत्;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, options, &UnicodeSet::closeOver, status);
    यदि (U_FAILURE(status)) प्रत्यागम *एतत्;
    यदि (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        प्रत्यागम *एतत्;
    }
    setPattern(rebuiltPat);
    प्रत्यागम *एतत्;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
स्थिर शून्य U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

स्थिर शून्य U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

स्थिर शून्य U_CALLCONV
_set_addString(USet *set, ध्रुव char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
स्थिर अन्तःस्थ शून्य
addCaseMapping(UnicodeSet &set, int32_t result, ध्रुव char16_t *full, UnicodeString &str) {
    यदि(result >= 0) {
        यदि(result > UCASE_MAX_STRING_LENGTH) {
            // add a single-code point case mapping
            set.add(result);
        } अन्यथा {
            // add a string case mapping from full with length result
            str.setTo(static_cast<UBool>(असत्य), full, result);
            set.add(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

नामाकाश {

/** For case closure on a large set, look only at code points with relevant properties. */
ध्रुव UnicodeSet &maybeOnlyCaseSensitive(ध्रुव UnicodeSet &src U_LIFETIME_BOUND,
                                         UnicodeSet &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.contains(0, 0x10ffff));
    यदि (src.size() < 30) {
        प्रत्यागम src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UErrorCode errorCode = U_ZERO_ERROR;
    ध्रुव UnicodeSet *sensitive =
        CharacterProperties::get_द्विआधारी_Property_Set(UCHAR_CASE_SENSITIVE, errorCode);
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    यदि (src.get_Range_गिनती() > sensitive->get_Range_गिनती()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } अन्यथा {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    प्रत्यागम subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
तार्किक scfString(ध्रुव UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    ध्रुव char16_t *p = s.getBuffer();
    int32_t length = s.length();
    // Loop while not needing modification.
    प्रति (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_NEXT(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        यदि (scfChar != c) {
            // Copy the characters before c.
            scf.setTo(p, i - U16_LENGTH(c));
            // Loop over the rest of the string and keep case-folding.
            प्रति (;;) {
                scf.append(scfChar);
                यदि (i == length) {
                    प्रत्यागम सत्य;
                }
                U16_NEXT(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    प्रत्यागम असत्य;
}

}  // namespace

UnicodeSet& UnicodeSet::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        प्रत्यागम *एतत्;
    }
    विकल्प (attribute & USET_CASE_MASK) {
    पक्ष 0:
        भङ्ग;
    पक्ष USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ असत्य);
        भङ्ग;
    पक्ष USET_ADD_CASE_MAPPINGS:
        closeOverAddCaseMappings();
        भङ्ग;
    पक्ष USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ सत्य);
        भङ्ग;
    स्वभाव:
        // bad option (unreachable)
        भङ्ग;
    }
    प्रत्यागम *एतत्;
}

शून्य UnicodeSet::closeOverCaseInsensitive(तार्किक simple) {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*एतत्);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    यदि (!simple && foldSet.hasStrings()) {
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
    ध्रुव UnicodeSet &codePoints = maybeOnlyCaseSensitive(*एतत्, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_गिनती();

    प्रति (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        यदि (simple) {
            प्रति (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addSimpleCaseClosure(cp, &sa);
            }
        } अन्यथा {
            प्रति (UChar32 cp=start; cp<=end; ++cp) {
                ucase_addCaseClosure(cp, &sa);
            }
        }
    }
    यदि (hasStrings()) {
        UnicodeString str;
        प्रति (int32_t j=0; j<strings_->size(); ++j) {
            ध्रुव UnicodeString* pStr = static_cast<ध्रुव UnicodeString*>(strings_->elementAt(j));
            यदि (simple) {
                यदि (scfString(*pStr, str)) {
                    foldSet.remove(*pStr).add(str);
                }
            } अन्यथा {
                str = *pStr;
                str.foldCase();
                यदि(!ucase_addStringCaseClosure(str.getBuffer(), str.length(), &sa)) {
                    foldSet.add(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *एतत् = foldSet;
}

शून्य UnicodeSet::closeOverAddCaseMappings() {
    // Start with input set to guarantee inclusion.
    UnicodeSet foldSet(*एतत्);

    UnicodeSet subset(0, 0x10ffff);
    ध्रुव UnicodeSet &codePoints = maybeOnlyCaseSensitive(*एतत्, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = codePoints.get_Range_गिनती();
    UChar32 result;
    ध्रुव char16_t *full;
    UnicodeString str;

    प्रति (int32_t i=0; i<n; ++i) {
        UChar32 start = codePoints.getRangeStart(i);
        UChar32 end   = codePoints.getRangeEnd(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        प्रति (UChar32 cp=start; cp<=end; ++cp) {
            result = ucase_toFullLower(cp, nullptr, nullptr, &full, UCASE_LOC_मूल);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullTitle(cp, nullptr, nullptr, &full, UCASE_LOC_मूल);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullUpper(cp, nullptr, nullptr, &full, UCASE_LOC_मूल);
            addCaseMapping(foldSet, result, full, str);

            result = ucase_toFullFolding(cp, &full, 0);
            addCaseMapping(foldSet, result, full, str);
        }
    }
    यदि (hasStrings()) {
        Locale मूल("");
#यदि !UCONFIG_NO_BREAK_ITERATION
        UErrorCode status = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::createWordInstance(मूल, status);
        यदि (U_SUCCESS(status)) {
#endif
            प्रति (int32_t j=0; j<strings_->size(); ++j) {
                ध्रुव UnicodeString* pStr = static_cast<ध्रुव UnicodeString*>(strings_->elementAt(j));
                (str = *pStr).toLower(मूल);
                foldSet.add(str);
#यदि !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).toTitle(bi, मूल);
                foldSet.add(str);
#endif
                (str = *pStr).toUpper(मूल);
                foldSet.add(str);
                (str = *pStr).foldCase();
                foldSet.add(str);
            }
#यदि !UCONFIG_NO_BREAK_ITERATION
        }
        delete bi;
#endif
    }
    *एतत् = foldSet;
}

U_NAMESPACE_END
