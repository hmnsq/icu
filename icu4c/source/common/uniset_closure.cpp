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
#परिभाषय _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

Unicode_समुच्चयः::Unicode_समुच्चयः(ध्रुव UnicodeString& आदर्शः,
                       uint32_t options,
                       ध्रुव चिह्नम्_सारणी* symbols,
                       UError_संहिता& स्थितिः) {
    apply_आदर्शः(आदर्शः, options, symbols, स्थितिः);
    _dbgct(एतत्);
}

Unicode_समुच्चयः::Unicode_समुच्चयः(ध्रुव UnicodeString& आदर्शः, ParsePosition& pos,
                       uint32_t options,
                       ध्रुव चिह्नम्_सारणी* symbols,
                       UError_संहिता& स्थितिः) {
    apply_आदर्शः(आदर्शः, pos, options, symbols, स्थितिः);
    _dbgct(एतत्);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_समुच्चयः& Unicode_समुच्चयः::apply_आदर्शः(ध्रुव UnicodeString& आदर्शः,
                                     uint32_t options,
                                     ध्रुव चिह्नम्_सारणी* symbols,
                                     UError_संहिता& स्थितिः) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    apply_आदर्शः(आदर्शः, pos, options, symbols, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम *एतत्;

    int32_t i = pos.get_सूचकांक();

    यदि (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(आदर्शः, i, सत्य);
    }

    यदि (i != आदर्शः.length()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    }
    प्रत्यागम *एतत्;
}

Unicode_समुच्चयः& Unicode_समुच्चयः::apply_आदर्शः(ध्रुव UnicodeString& आदर्शः,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ध्रुव चिह्नम्_सारणी* symbols,
                                     UError_संहिता& स्थितिः) U_LIFETIME_BOUND {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम *एतत्;
    }
    यदि (isFrozen()) {
        स्थितिः = U_NO_WRITE_PERMISSION;
        प्रत्यागम *एतत्;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    नियमः_Character_Iterator chars(आदर्शः, symbols, pos);
    apply_आदर्शः(आदर्शः, pos, chars, symbols, rebuiltPat, options, &Unicode_समुच्चयः::closeOver, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम *एतत्;
    यदि (chars.अन्तर्_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        स्थितिः = U_MALFORMED_समुच्चयः;
        प्रत्यागम *एतत्;
    }
    समुच्चयः_आदर्शः(rebuiltPat);
    प्रत्यागम *एतत्;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
स्थिर शून्य U_CALLCONV
समुच्चयः_सङ्कलय(USet *समुच्चयः, UChar32 c) {
    reinterpret_cast<Unicode_समुच्चयः*>(समुच्चयः)->सङ्कलय(c);
}

स्थिर शून्य U_CALLCONV
समुच्चयः_सङ्कलय_Range(USet *समुच्चयः, UChar32 start, UChar32 अन्तः) {
    reinterpret_cast<Unicode_समुच्चयः*>(समुच्चयः)->सङ्कलय(start, अन्तः);
}

स्थिर शून्य U_CALLCONV
समुच्चयः_सङ्कलय_String(USet *समुच्चयः, ध्रुव char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_समुच्चयः*>(समुच्चयः)->सङ्कलय(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
स्थिर अन्तःस्थ शून्य
सङ्कलय_Case_Mapping(Unicode_समुच्चयः &समुच्चयः, int32_t परिणाम, ध्रुव char16_t *पूर्णम्, UnicodeString &str) {
    यदि(परिणाम >= 0) {
        यदि(परिणाम > UCASE_MAX_STRING_दीर्घता) {
            // add a single-code point case mapping
            समुच्चयः.सङ्कलय(परिणाम);
        } अन्यथा {
            // add a string case mapping from full with length result
            str.समुच्चयः_गन्तव्यम्(static_cast<UBool>(असत्य), पूर्णम्, परिणाम);
            समुच्चयः.सङ्कलय(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

नामाकाश {

/** For case closure on a large set, look only at code points with relevant properties. */
ध्रुव Unicode_समुच्चयः &maybeOnlyCaseSensitive(ध्रुव Unicode_समुच्चयः &src U_LIFETIME_BOUND,
                                         Unicode_समुच्चयः &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.अन्तर्भवति(0, 0x10ffff));
    यदि (src.size() < 30) {
        प्रत्यागम src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UError_संहिता error_संहिता = U_ZERO_ERROR;
    ध्रुव Unicode_समुच्चयः *sensitive =
        CharacterProperties::get_द्विमान_Property_समुच्चयः(UCHAR_CASE_SENSITIVE, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    यदि (src.get_Range_गणनम्() > sensitive->get_Range_गणनम्()) {
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
    ध्रुव char16_t *p = s.get_बफर();
    int32_t length = s.length();
    // Loop while not needing modification.
    प्रति (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_अग्र(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        यदि (scfChar != c) {
            // Copy the characters before c.
            scf.समुच्चयः_गन्तव्यम्(p, i - U16_दीर्घता(c));
            // Loop over the rest of the string and keep case-folding.
            प्रति (;;) {
                scf.append(scfChar);
                यदि (i == length) {
                    प्रत्यागम सत्य;
                }
                U16_अग्र(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    प्रत्यागम असत्य;
}

}  // namespace

Unicode_समुच्चयः& Unicode_समुच्चयः::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        प्रत्यागम *एतत्;
    }
    विकल्प (attribute & USET_CASE_आवरणम्) {
    पक्ष 0:
        भङ्ग;
    पक्ष USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ असत्य);
        भङ्ग;
    पक्ष USET_सङ्कलय_CASE_MAPPINGS:
        close_Over_सङ्कलय_Case_Mappings();
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

शून्य Unicode_समुच्चयः::closeOverCaseInsensitive(तार्किक simple) {
    // Start with input set to guarantee inclusion.
    Unicode_समुच्चयः fold_समुच्चयः(*एतत्);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    यदि (!simple && fold_समुच्चयः.has_वाक्यानि()) {
        fold_समुच्चयः.वाक्यानि_2->removeAllElements();
    }

    USetAdder sa = {
        fold_समुच्चयः.गन्तव्यम्_USet(),
        समुच्चयः_सङ्कलय,
        समुच्चयः_सङ्कलय_Range,
        समुच्चयः_सङ्कलय_String,
        शून्यसूचकः, // don't need remove()
        शून्यसूचकः // don't need removeRange()
    };

    Unicode_समुच्चयः subset(0, 0x10ffff);
    ध्रुव Unicode_समुच्चयः &संहिता_Points = maybeOnlyCaseSensitive(*एतत्, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = संहिता_Points.get_Range_गणनम्();

    प्रति (int32_t i=0; i<n; ++i) {
        UChar32 start = संहिता_Points.getRangeStart(i);
        UChar32 अन्तः   = संहिता_Points.get_Range_अन्तः(i);

        यदि (simple) {
            प्रति (UChar32 cp=start; cp<=अन्तः; ++cp) {
                ucase_सङ्कलय_Simple_Case_संवृतिः(cp, &sa);
            }
        } अन्यथा {
            प्रति (UChar32 cp=start; cp<=अन्तः; ++cp) {
                ucase_सङ्कलय_Case_संवृतिः(cp, &sa);
            }
        }
    }
    यदि (has_वाक्यानि()) {
        UnicodeString str;
        प्रति (int32_t j=0; j<वाक्यानि_2->size(); ++j) {
            ध्रुव UnicodeString* pStr = static_cast<ध्रुव UnicodeString*>(वाक्यानि_2->elementAt(j));
            यदि (simple) {
                यदि (scfString(*pStr, str)) {
                    fold_समुच्चयः.remove(*pStr).सङ्कलय(str);
                }
            } अन्यथा {
                str = *pStr;
                str.foldCase();
                यदि(!ucase_सङ्कलय_String_Case_संवृतिः(str.get_बफर(), str.length(), &sa)) {
                    fold_समुच्चयः.सङ्कलय(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *एतत् = fold_समुच्चयः;
}

शून्य Unicode_समुच्चयः::close_Over_सङ्कलय_Case_Mappings() {
    // Start with input set to guarantee inclusion.
    Unicode_समुच्चयः fold_समुच्चयः(*एतत्);

    Unicode_समुच्चयः subset(0, 0x10ffff);
    ध्रुव Unicode_समुच्चयः &संहिता_Points = maybeOnlyCaseSensitive(*एतत्, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = संहिता_Points.get_Range_गणनम्();
    UChar32 परिणाम;
    ध्रुव char16_t *पूर्णम्;
    UnicodeString str;

    प्रति (int32_t i=0; i<n; ++i) {
        UChar32 start = संहिता_Points.getRangeStart(i);
        UChar32 अन्तः   = संहिता_Points.get_Range_अन्तः(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        प्रति (UChar32 cp=start; cp<=अन्तः; ++cp) {
            परिणाम = ucase_गन्तव्यम्_पूर्णम्_अधस्तनम्(cp, शून्यसूचकः, शून्यसूचकः, &पूर्णम्, UCASE_LOC_मूलम्);
            सङ्कलय_Case_Mapping(fold_समुच्चयः, परिणाम, पूर्णम्, str);

            परिणाम = ucase_गन्तव्यम्_पूर्णम्_Title(cp, शून्यसूचकः, शून्यसूचकः, &पूर्णम्, UCASE_LOC_मूलम्);
            सङ्कलय_Case_Mapping(fold_समुच्चयः, परिणाम, पूर्णम्, str);

            परिणाम = ucase_गन्तव्यम्_पूर्णम्_ऊर्ध्वस्थम्(cp, शून्यसूचकः, शून्यसूचकः, &पूर्णम्, UCASE_LOC_मूलम्);
            सङ्कलय_Case_Mapping(fold_समुच्चयः, परिणाम, पूर्णम्, str);

            परिणाम = ucase_गन्तव्यम्_पूर्णम्_Folding(cp, &पूर्णम्, 0);
            सङ्कलय_Case_Mapping(fold_समुच्चयः, परिणाम, पूर्णम्, str);
        }
    }
    यदि (has_वाक्यानि()) {
        Locale मूलम्("");
#यदि !UCONFIG_NO_BREAK_ITERATION
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::createWordInstance(मूलम्, स्थितिः);
        यदि (U_SUCCESS(स्थितिः)) {
#यदि_अन्तः
            प्रति (int32_t j=0; j<वाक्यानि_2->size(); ++j) {
                ध्रुव UnicodeString* pStr = static_cast<ध्रुव UnicodeString*>(वाक्यानि_2->elementAt(j));
                (str = *pStr).गन्तव्यम्_अधस्तनम्(मूलम्);
                fold_समुच्चयः.सङ्कलय(str);
#यदि !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).गन्तव्यम्_Title(bi, मूलम्);
                fold_समुच्चयः.सङ्कलय(str);
#यदि_अन्तः
                (str = *pStr).गन्तव्यम्_ऊर्ध्वस्थम्(मूलम्);
                fold_समुच्चयः.सङ्कलय(str);
                (str = *pStr).foldCase();
                fold_समुच्चयः.सङ्कलय(str);
            }
#यदि !UCONFIG_NO_BREAK_ITERATION
        }
        विलोपय bi;
#यदि_अन्तः
    }
    *एतत् = fold_समुच्चयः;
}

U_NAMESPACE_अन्तः
