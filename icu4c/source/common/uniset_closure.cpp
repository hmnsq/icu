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
#تعریف_کړه _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

Unicode_سېټ::Unicode_سېټ(ثابت UnicodeString& نمونه,
                       uint32_t options,
                       ثابت نښه_جدول* symbols,
                       UError_کوډ& حالت) {
    apply_نمونه(نمونه, options, symbols, حالت);
    _dbgct(هذا);
}

Unicode_سېټ::Unicode_سېټ(ثابت UnicodeString& نمونه, ParsePosition& pos,
                       uint32_t options,
                       ثابت نښه_جدول* symbols,
                       UError_کوډ& حالت) {
    apply_نمونه(نمونه, pos, options, symbols, حالت);
    _dbgct(هذا);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_سېټ& Unicode_سېټ::apply_نمونه(ثابت UnicodeString& نمونه,
                                     uint32_t options,
                                     ثابت نښه_جدول* symbols,
                                     UError_کوډ& حالت) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    apply_نمونه(نمونه, pos, options, symbols, حالت);
    إذا (U_FAILURE(حالت)) أرجع *هذا;

    int32_t i = pos.get_شاخص();

    إذا (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(نمونه, i, ریښتیا);
    }

    إذا (i != نمونه.length()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
    أرجع *هذا;
}

Unicode_سېټ& Unicode_سېټ::apply_نمونه(ثابت UnicodeString& نمونه,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ثابت نښه_جدول* symbols,
                                     UError_کوډ& حالت) U_LIFETIME_BOUND {
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
    قاعده_Character_Iterator chars(نمونه, symbols, pos);
    apply_نمونه(نمونه, pos, chars, symbols, rebuiltPat, options, &Unicode_سېټ::closeOver, حالت);
    إذا (U_FAILURE(حالت)) أرجع *هذا;
    إذا (chars.دننه_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        حالت = U_MALFORMED_سېټ;
        أرجع *هذا;
    }
    سېټ_نمونه(rebuiltPat);
    أرجع *هذا;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ساكن فراغ U_CALLCONV
سېټ_جمع_کړه(USet *سېټ, UChar32 c) {
    reinterpret_cast<Unicode_سېټ*>(سېټ)->جمع_کړه(c);
}

ساكن فراغ U_CALLCONV
سېټ_جمع_کړه_Range(USet *سېټ, UChar32 پیل, UChar32 پای_ټکی) {
    reinterpret_cast<Unicode_سېټ*>(سېټ)->جمع_کړه(پیل, پای_ټکی);
}

ساكن فراغ U_CALLCONV
سېټ_جمع_کړه_String(USet *سېټ, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_سېټ*>(سېټ)->جمع_کړه(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
ساكن مضمن فراغ
جمع_کړه_Case_Mapping(Unicode_سېټ &سېټ, int32_t پایله, ثابت char16_t *ډک, UnicodeString &str) {
    إذا(پایله >= 0) {
        إذا(پایله > UCASE_MAX_STRING_اوږدتیا) {
            // add a single-code point case mapping
            سېټ.جمع_کړه(پایله);
        } وإلا {
            // add a string case mapping from full with length result
            str.سېټ_منزل(static_cast<UBool>(خطأ), ډک, پایله);
            سېټ.جمع_کړه(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

نطاق {

/** For case closure on a large set, look only at code points with relevant properties. */
ثابت Unicode_سېټ &maybeOnlyCaseSensitive(ثابت Unicode_سېټ &src U_LIFETIME_BOUND,
                                         Unicode_سېټ &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.لري(0, 0x10ffff));
    إذا (src.size() < 30) {
        أرجع src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UError_کوډ error_کوډ = U_ZERO_ERROR;
    ثابت Unicode_سېټ *sensitive =
        CharacterProperties::get_دوګونی_Property_سېټ(UCHAR_CASE_SENSITIVE, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) {
        أرجع src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    إذا (src.get_Range_شمېر() > sensitive->get_Range_شمېر()) {
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
        U16_بل(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        إذا (scfChar != c) {
            // Copy the characters before c.
            scf.سېټ_منزل(p, i - U16_اوږدتیا(c));
            // Loop over the rest of the string and keep case-folding.
            لكل (;;) {
                scf.append(scfChar);
                إذا (i == length) {
                    أرجع ریښتیا;
                }
                U16_بل(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    أرجع خطأ;
}

}  // namespace

Unicode_سېټ& Unicode_سېټ::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    بدل (attribute & USET_CASE_ماسک) {
    حالة 0:
        اكسر;
    حالة USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ خطأ);
        اكسر;
    حالة USET_جمع_کړه_CASE_MAPPINGS:
        close_Over_جمع_کړه_Case_Mappings();
        اكسر;
    حالة USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ ریښتیا);
        اكسر;
    افتراضي:
        // bad option (unreachable)
        اكسر;
    }
    أرجع *هذا;
}

فراغ Unicode_سېټ::closeOverCaseInsensitive(منطقي simple) {
    // Start with input set to guarantee inclusion.
    Unicode_سېټ fold_سېټ(*هذا);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    إذا (!simple && fold_سېټ.has_مزي_ګانې()) {
        fold_سېټ.مزي_ګانې_2->removeAllElements();
    }

    USetAdder sa = {
        fold_سېټ.منزل_USet(),
        سېټ_جمع_کړه,
        سېټ_جمع_کړه_Range,
        سېټ_جمع_کړه_String,
        تش_اشاره, // don't need remove()
        تش_اشاره // don't need removeRange()
    };

    Unicode_سېټ subset(0, 0x10ffff);
    ثابت Unicode_سېټ &کوډ_Points = maybeOnlyCaseSensitive(*هذا, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = کوډ_Points.get_Range_شمېر();

    لكل (int32_t i=0; i<n; ++i) {
        UChar32 پیل = کوډ_Points.get_Range_پیل(i);
        UChar32 پای_ټکی   = کوډ_Points.get_Range_پای_ټکی(i);

        إذا (simple) {
            لكل (UChar32 cp=پیل; cp<=پای_ټکی; ++cp) {
                ucase_جمع_کړه_Simple_Case_تړون(cp, &sa);
            }
        } وإلا {
            لكل (UChar32 cp=پیل; cp<=پای_ټکی; ++cp) {
                ucase_جمع_کړه_Case_تړون(cp, &sa);
            }
        }
    }
    إذا (has_مزي_ګانې()) {
        UnicodeString str;
        لكل (int32_t j=0; j<مزي_ګانې_2->size(); ++j) {
            ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(مزي_ګانې_2->elementAt(j));
            إذا (simple) {
                إذا (scfString(*pStr, str)) {
                    fold_سېټ.remove(*pStr).جمع_کړه(str);
                }
            } وإلا {
                str = *pStr;
                str.foldCase();
                إذا(!ucase_جمع_کړه_String_Case_تړون(str.get_بفر(), str.length(), &sa)) {
                    fold_سېټ.جمع_کړه(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *هذا = fold_سېټ;
}

فراغ Unicode_سېټ::close_Over_جمع_کړه_Case_Mappings() {
    // Start with input set to guarantee inclusion.
    Unicode_سېټ fold_سېټ(*هذا);

    Unicode_سېټ subset(0, 0x10ffff);
    ثابت Unicode_سېټ &کوډ_Points = maybeOnlyCaseSensitive(*هذا, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = کوډ_Points.get_Range_شمېر();
    UChar32 پایله;
    ثابت char16_t *ډک;
    UnicodeString str;

    لكل (int32_t i=0; i<n; ++i) {
        UChar32 پیل = کوډ_Points.get_Range_پیل(i);
        UChar32 پای_ټکی   = کوډ_Points.get_Range_پای_ټکی(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        لكل (UChar32 cp=پیل; cp<=پای_ټکی; ++cp) {
            پایله = ucase_منزل_ډک_ښکتنی(cp, تش_اشاره, تش_اشاره, &ډک, UCASE_LOC_ریښه);
            جمع_کړه_Case_Mapping(fold_سېټ, پایله, ډک, str);

            پایله = ucase_منزل_ډک_Title(cp, تش_اشاره, تش_اشاره, &ډک, UCASE_LOC_ریښه);
            جمع_کړه_Case_Mapping(fold_سېټ, پایله, ډک, str);

            پایله = ucase_منزل_ډک_پاسنی(cp, تش_اشاره, تش_اشاره, &ډک, UCASE_LOC_ریښه);
            جمع_کړه_Case_Mapping(fold_سېټ, پایله, ډک, str);

            پایله = ucase_منزل_ډک_Folding(cp, &ډک, 0);
            جمع_کړه_Case_Mapping(fold_سېټ, پایله, ډک, str);
        }
    }
    إذا (has_مزي_ګانې()) {
        Locale ریښه("");
#إذا !UCONFIG_NO_BREAK_ITERATION
        UError_کوډ حالت = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::create_کلمه_Instance(ریښه, حالت);
        إذا (U_SUCCESS(حالت)) {
#د_که_پای
            لكل (int32_t j=0; j<مزي_ګانې_2->size(); ++j) {
                ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(مزي_ګانې_2->elementAt(j));
                (str = *pStr).منزل_ښکتنی(ریښه);
                fold_سېټ.جمع_کړه(str);
#إذا !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).منزل_Title(bi, ریښه);
                fold_سېټ.جمع_کړه(str);
#د_که_پای
                (str = *pStr).منزل_پاسنی(ریښه);
                fold_سېټ.جمع_کړه(str);
                (str = *pStr).foldCase();
                fold_سېټ.جمع_کړه(str);
            }
#إذا !UCONFIG_NO_BREAK_ITERATION
        }
        ړنګول bi;
#د_که_پای
    }
    *هذا = fold_سېټ;
}

U_NAMESPACE_پای_ټکی
