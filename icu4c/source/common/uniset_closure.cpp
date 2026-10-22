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

#ضمن "unicode/brkiter.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/parsepos.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "ruleiter.h"
#ضمن "ucase.h"
#ضمن "uprops.h"
#ضمن "util.h"
#ضمن "uvector.h"

U_NAMESPACE_BEGIN

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#تعریف_کرو _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

Unicode_سیٹ::Unicode_سیٹ(ثابت UnicodeString& نمونہ,
                       uint32_t options,
                       ثابت علامت_جدول* symbols,
                       UError_کوڈ& حالت) {
    apply_نمونہ(نمونہ, options, symbols, حالت);
    _dbgct(هذا);
}

Unicode_سیٹ::Unicode_سیٹ(ثابت UnicodeString& نمونہ, ParsePosition& pos,
                       uint32_t options,
                       ثابت علامت_جدول* symbols,
                       UError_کوڈ& حالت) {
    apply_نمونہ(نمونہ, pos, options, symbols, حالت);
    _dbgct(هذا);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_سیٹ& Unicode_سیٹ::apply_نمونہ(ثابت UnicodeString& نمونہ,
                                     uint32_t options,
                                     ثابت علامت_جدول* symbols,
                                     UError_کوڈ& حالت) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    apply_نمونہ(نمونہ, pos, options, symbols, حالت);
    إذا (U_FAILURE(حالت)) أرجع *هذا;

    int32_t i = pos.get_اشاریہ();

    إذا (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(نمونہ, i, سچ);
    }

    إذا (i != نمونہ.length()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
    أرجع *هذا;
}

Unicode_سیٹ& Unicode_سیٹ::apply_نمونہ(ثابت UnicodeString& نمونہ,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ثابت علامت_جدول* symbols,
                                     UError_کوڈ& حالت) U_LIFETIME_BOUND {
    إذا (U_FAILURE(حالت)) {
        أرجع *هذا;
    }
    إذا (isFrozen()) {
        حالت = U_NO_WRITE_PERMISSION;
        أرجع *هذا;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    قاعدہ_Character_Iterator chars(نمونہ, symbols, pos);
    apply_نمونہ(نمونہ, pos, chars, symbols, rebuiltPat, options, &Unicode_سیٹ::closeOver, حالت);
    إذا (U_FAILURE(حالت)) أرجع *هذا;
    إذا (chars.وچ_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        حالت = U_MALFORMED_سیٹ;
        أرجع *هذا;
    }
    سیٹ_نمونہ(rebuiltPat);
    أرجع *هذا;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ساكن فراغ U_CALLCONV
سیٹ_جوڑو(USet *سیٹ, UChar32 c) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(c);
}

ساكن فراغ U_CALLCONV
سیٹ_جوڑو_Range(USet *سیٹ, UChar32 شروع, UChar32 سرا) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(شروع, سرا);
}

ساكن فراغ U_CALLCONV
سیٹ_جوڑو_String(USet *سیٹ, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
ساكن مضمن فراغ
جوڑو_Case_Mapping(Unicode_سیٹ &سیٹ, int32_t نتیجہ, ثابت char16_t *بھریا, UnicodeString &str) {
    إذا(نتیجہ >= 0) {
        إذا(نتیجہ > UCASE_MAX_STRING_لمائی) {
            // add a single-code point case mapping
            سیٹ.جوڑو(نتیجہ);
        } وإلا {
            // add a string case mapping from full with length result
            str.سیٹ_منزل(static_cast<UBool>(خطأ), بھریا, نتیجہ);
            سیٹ.جوڑو(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

نطاق {

/** For case closure on a large set, look only at code points with relevant properties. */
ثابت Unicode_سیٹ &maybeOnlyCaseSensitive(ثابت Unicode_سیٹ &src U_LIFETIME_BOUND,
                                         Unicode_سیٹ &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.شامل_اے(0, 0x10ffff));
    إذا (src.size() < 30) {
        أرجع src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UError_کوڈ error_کوڈ = U_ZERO_ERROR;
    ثابت Unicode_سیٹ *sensitive =
        CharacterProperties::get_ثنائی_Property_سیٹ(UCHAR_CASE_SENSITIVE, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    إذا (src.get_Range_گنتی() > sensitive->get_Range_گنتی()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } وإلا {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    أرجع subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
منطقي scfString(ثابت UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    ثابت char16_t *p = s.get_بفر();
    int32_t length = s.length();
    // Loop while not needing modification.
    لكل (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_اگلا(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        إذا (scfChar != c) {
            // Copy the characters before c.
            scf.سیٹ_منزل(p, i - U16_لمائی(c));
            // Loop over the rest of the string and keep case-folding.
            لكل (;;) {
                scf.append(scfChar);
                إذا (i == length) {
                    أرجع سچ;
                }
                U16_اگلا(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    أرجع خطأ;
}

}  // namespace

Unicode_سیٹ& Unicode_سیٹ::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    بدل (attribute & USET_CASE_نقاب) {
    حالة 0:
        اكسر;
    حالة USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ خطأ);
        اكسر;
    حالة USET_جوڑو_CASE_MAPPINGS:
        close_Over_جوڑو_Case_Mappings();
        اكسر;
    حالة USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ سچ);
        اكسر;
    افتراضي:
        // bad option (unreachable)
        اكسر;
    }
    أرجع *هذا;
}

فراغ Unicode_سیٹ::closeOverCaseInsensitive(منطقي simple) {
    // Start with input set to guarantee inclusion.
    Unicode_سیٹ fold_سیٹ(*هذا);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    إذا (!simple && fold_سیٹ.has_لڑیاں()) {
        fold_سیٹ.لڑیاں_2->removeAllElements();
    }

    USetAdder sa = {
        fold_سیٹ.منزل_USet(),
        سیٹ_جوڑو,
        سیٹ_جوڑو_Range,
        سیٹ_جوڑو_String,
        خالی_سوچک, // don't need remove()
        خالی_سوچک // don't need removeRange()
    };

    Unicode_سیٹ subset(0, 0x10ffff);
    ثابت Unicode_سیٹ &کوڈ_Points = maybeOnlyCaseSensitive(*هذا, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = کوڈ_Points.get_Range_گنتی();

    لكل (int32_t i=0; i<n; ++i) {
        UChar32 شروع = کوڈ_Points.get_Range_شروع(i);
        UChar32 سرا   = کوڈ_Points.get_Range_سرا(i);

        إذا (simple) {
            لكل (UChar32 cp=شروع; cp<=سرا; ++cp) {
                ucase_جوڑو_Simple_Case_گھیرا(cp, &sa);
            }
        } وإلا {
            لكل (UChar32 cp=شروع; cp<=سرا; ++cp) {
                ucase_جوڑو_Case_گھیرا(cp, &sa);
            }
        }
    }
    إذا (has_لڑیاں()) {
        UnicodeString str;
        لكل (int32_t j=0; j<لڑیاں_2->size(); ++j) {
            ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(لڑیاں_2->elementAt(j));
            إذا (simple) {
                إذا (scfString(*pStr, str)) {
                    fold_سیٹ.remove(*pStr).جوڑو(str);
                }
            } وإلا {
                str = *pStr;
                str.foldCase();
                إذا(!ucase_جوڑو_String_Case_گھیرا(str.get_بفر(), str.length(), &sa)) {
                    fold_سیٹ.جوڑو(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *هذا = fold_سیٹ;
}

فراغ Unicode_سیٹ::close_Over_جوڑو_Case_Mappings() {
    // Start with input set to guarantee inclusion.
    Unicode_سیٹ fold_سیٹ(*هذا);

    Unicode_سیٹ subset(0, 0x10ffff);
    ثابت Unicode_سیٹ &کوڈ_Points = maybeOnlyCaseSensitive(*هذا, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = کوڈ_Points.get_Range_گنتی();
    UChar32 نتیجہ;
    ثابت char16_t *بھریا;
    UnicodeString str;

    لكل (int32_t i=0; i<n; ++i) {
        UChar32 شروع = کوڈ_Points.get_Range_شروع(i);
        UChar32 سرا   = کوڈ_Points.get_Range_سرا(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        لكل (UChar32 cp=شروع; cp<=سرا; ++cp) {
            نتیجہ = ucase_منزل_بھریا_ہیٹھلا(cp, خالی_سوچک, خالی_سوچک, &بھریا, UCASE_LOC_جڑ);
            جوڑو_Case_Mapping(fold_سیٹ, نتیجہ, بھریا, str);

            نتیجہ = ucase_منزل_بھریا_Title(cp, خالی_سوچک, خالی_سوچک, &بھریا, UCASE_LOC_جڑ);
            جوڑو_Case_Mapping(fold_سیٹ, نتیجہ, بھریا, str);

            نتیجہ = ucase_منزل_بھریا_اتلا(cp, خالی_سوچک, خالی_سوچک, &بھریا, UCASE_LOC_جڑ);
            جوڑو_Case_Mapping(fold_سیٹ, نتیجہ, بھریا, str);

            نتیجہ = ucase_منزل_بھریا_Folding(cp, &بھریا, 0);
            جوڑو_Case_Mapping(fold_سیٹ, نتیجہ, بھریا, str);
        }
    }
    إذا (has_لڑیاں()) {
        Locale جڑ("");
#إذا !UCONFIG_NO_BREAK_ITERATION
        UError_کوڈ حالت = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::create_لفظ_Instance(جڑ, حالت);
        إذا (U_SUCCESS(حالت)) {
#جے_انت
            لكل (int32_t j=0; j<لڑیاں_2->size(); ++j) {
                ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(لڑیاں_2->elementAt(j));
                (str = *pStr).منزل_ہیٹھلا(جڑ);
                fold_سیٹ.جوڑو(str);
#إذا !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).منزل_Title(bi, جڑ);
                fold_سیٹ.جوڑو(str);
#جے_انت
                (str = *pStr).منزل_اتلا(جڑ);
                fold_سیٹ.جوڑو(str);
                (str = *pStr).foldCase();
                fold_سیٹ.جوڑو(str);
            }
#إذا !UCONFIG_NO_BREAK_ITERATION
        }
        مٹاؤ bi;
#جے_انت
    }
    *هذا = fold_سیٹ;
}

U_NAMESPACE_سرا
