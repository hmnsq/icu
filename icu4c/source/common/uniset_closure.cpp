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
#تعریف_کن _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

Unicode_مجموعه::Unicode_مجموعه(ثابت UnicodeString& الگو,
                       uint32_t options,
                       ثابت نماد_جدول* symbols,
                       UError_کد& وضعیت) {
    apply_الگو(الگو, options, symbols, وضعیت);
    _dbgct(هذا);
}

Unicode_مجموعه::Unicode_مجموعه(ثابت UnicodeString& الگو, ParsePosition& pos,
                       uint32_t options,
                       ثابت نماد_جدول* symbols,
                       UError_کد& وضعیت) {
    apply_الگو(الگو, pos, options, symbols, وضعیت);
    _dbgct(هذا);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_مجموعه& Unicode_مجموعه::apply_الگو(ثابت UnicodeString& الگو,
                                     uint32_t options,
                                     ثابت نماد_جدول* symbols,
                                     UError_کد& وضعیت) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    apply_الگو(الگو, pos, options, symbols, وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع *هذا;

    int32_t i = pos.get_شاخص();

    إذا (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(الگو, i, درست);
    }

    إذا (i != الگو.length()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
    }
    أرجع *هذا;
}

Unicode_مجموعه& Unicode_مجموعه::apply_الگو(ثابت UnicodeString& الگو,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ثابت نماد_جدول* symbols,
                                     UError_کد& وضعیت) U_LIFETIME_BOUND {
    إذا (U_FAILURE(وضعیت)) {
        أرجع *هذا;
    }
    إذا (isFrozen()) {
        وضعیت = U_NO_WRITE_PERMISSION;
        أرجع *هذا;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    قاعده_Character_Iterator chars(الگو, symbols, pos);
    apply_الگو(الگو, pos, chars, symbols, rebuiltPat, options, &Unicode_مجموعه::closeOver, وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع *هذا;
    إذا (chars.درون_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        وضعیت = U_MALFORMED_مجموعه;
        أرجع *هذا;
    }
    مجموعه_الگو(rebuiltPat);
    أرجع *هذا;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ساكن فراغ U_CALLCONV
مجموعه_جمع_کن(USet *مجموعه, UChar32 c) {
    reinterpret_cast<Unicode_مجموعه*>(مجموعه)->جمع_کن(c);
}

ساكن فراغ U_CALLCONV
مجموعه_جمع_کن_Range(USet *مجموعه, UChar32 شروع, UChar32 انتها) {
    reinterpret_cast<Unicode_مجموعه*>(مجموعه)->جمع_کن(شروع, انتها);
}

ساكن فراغ U_CALLCONV
مجموعه_جمع_کن_String(USet *مجموعه, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_مجموعه*>(مجموعه)->جمع_کن(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
ساكن مضمن فراغ
جمع_کن_Case_Mapping(Unicode_مجموعه &مجموعه, int32_t نتیجه, ثابت char16_t *پر, UnicodeString &str) {
    إذا(نتیجه >= 0) {
        إذا(نتیجه > UCASE_MAX_STRING_درازی) {
            // add a single-code point case mapping
            مجموعه.جمع_کن(نتیجه);
        } وإلا {
            // add a string case mapping from full with length result
            str.مجموعه_مقصد(static_cast<UBool>(خطأ), پر, نتیجه);
            مجموعه.جمع_کن(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

نطاق {

/** For case closure on a large set, look only at code points with relevant properties. */
ثابت Unicode_مجموعه &maybeOnlyCaseSensitive(ثابت Unicode_مجموعه &src U_LIFETIME_BOUND,
                                         Unicode_مجموعه &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.شامل_است(0, 0x10ffff));
    إذا (src.size() < 30) {
        أرجع src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UError_کد error_کد = U_ZERO_ERROR;
    ثابت Unicode_مجموعه *sensitive =
        CharacterProperties::get_دودویی_Property_مجموعه(UCHAR_CASE_SENSITIVE, error_کد);
    إذا (U_FAILURE(error_کد)) {
        أرجع src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    إذا (src.get_Range_شمار() > sensitive->get_Range_شمار()) {
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
    ثابت char16_t *p = s.get_میانگیر();
    int32_t length = s.length();
    // Loop while not needing modification.
    لكل (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_بعدی(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        إذا (scfChar != c) {
            // Copy the characters before c.
            scf.مجموعه_مقصد(p, i - U16_درازی(c));
            // Loop over the rest of the string and keep case-folding.
            لكل (;;) {
                scf.append(scfChar);
                إذا (i == length) {
                    أرجع درست;
                }
                U16_بعدی(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    أرجع خطأ;
}

}  // namespace

Unicode_مجموعه& Unicode_مجموعه::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    بدل (attribute & USET_CASE_ماسک) {
    حالة 0:
        اكسر;
    حالة USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ خطأ);
        اكسر;
    حالة USET_جمع_کن_CASE_MAPPINGS:
        close_Over_جمع_کن_Case_Mappings();
        اكسر;
    حالة USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ درست);
        اكسر;
    افتراضي:
        // bad option (unreachable)
        اكسر;
    }
    أرجع *هذا;
}

فراغ Unicode_مجموعه::closeOverCaseInsensitive(منطقي simple) {
    // Start with input set to guarantee inclusion.
    Unicode_مجموعه fold_مجموعه(*هذا);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    إذا (!simple && fold_مجموعه.has_رشتهها()) {
        fold_مجموعه.رشتهها_2->removeAllElements();
    }

    USetAdder sa = {
        fold_مجموعه.مقصد_USet(),
        مجموعه_جمع_کن,
        مجموعه_جمع_کن_Range,
        مجموعه_جمع_کن_String,
        اشاره_تهی, // don't need remove()
        اشاره_تهی // don't need removeRange()
    };

    Unicode_مجموعه subset(0, 0x10ffff);
    ثابت Unicode_مجموعه &کد_Points = maybeOnlyCaseSensitive(*هذا, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = کد_Points.get_Range_شمار();

    لكل (int32_t i=0; i<n; ++i) {
        UChar32 شروع = کد_Points.get_Range_شروع(i);
        UChar32 انتها   = کد_Points.get_Range_انتها(i);

        إذا (simple) {
            لكل (UChar32 cp=شروع; cp<=انتها; ++cp) {
                ucase_جمع_کن_Simple_Case_بستگی(cp, &sa);
            }
        } وإلا {
            لكل (UChar32 cp=شروع; cp<=انتها; ++cp) {
                ucase_جمع_کن_Case_بستگی(cp, &sa);
            }
        }
    }
    إذا (has_رشتهها()) {
        UnicodeString str;
        لكل (int32_t j=0; j<رشتهها_2->size(); ++j) {
            ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(رشتهها_2->elementAt(j));
            إذا (simple) {
                إذا (scfString(*pStr, str)) {
                    fold_مجموعه.remove(*pStr).جمع_کن(str);
                }
            } وإلا {
                str = *pStr;
                str.foldCase();
                إذا(!ucase_جمع_کن_String_Case_بستگی(str.get_میانگیر(), str.length(), &sa)) {
                    fold_مجموعه.جمع_کن(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *هذا = fold_مجموعه;
}

فراغ Unicode_مجموعه::close_Over_جمع_کن_Case_Mappings() {
    // Start with input set to guarantee inclusion.
    Unicode_مجموعه fold_مجموعه(*هذا);

    Unicode_مجموعه subset(0, 0x10ffff);
    ثابت Unicode_مجموعه &کد_Points = maybeOnlyCaseSensitive(*هذا, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = کد_Points.get_Range_شمار();
    UChar32 نتیجه;
    ثابت char16_t *پر;
    UnicodeString str;

    لكل (int32_t i=0; i<n; ++i) {
        UChar32 شروع = کد_Points.get_Range_شروع(i);
        UChar32 انتها   = کد_Points.get_Range_انتها(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        لكل (UChar32 cp=شروع; cp<=انتها; ++cp) {
            نتیجه = ucase_مقصد_پر_پایینی(cp, اشاره_تهی, اشاره_تهی, &پر, UCASE_LOC_ریشه);
            جمع_کن_Case_Mapping(fold_مجموعه, نتیجه, پر, str);

            نتیجه = ucase_مقصد_پر_Title(cp, اشاره_تهی, اشاره_تهی, &پر, UCASE_LOC_ریشه);
            جمع_کن_Case_Mapping(fold_مجموعه, نتیجه, پر, str);

            نتیجه = ucase_مقصد_پر_فوقانی(cp, اشاره_تهی, اشاره_تهی, &پر, UCASE_LOC_ریشه);
            جمع_کن_Case_Mapping(fold_مجموعه, نتیجه, پر, str);

            نتیجه = ucase_مقصد_پر_Folding(cp, &پر, 0);
            جمع_کن_Case_Mapping(fold_مجموعه, نتیجه, پر, str);
        }
    }
    إذا (has_رشتهها()) {
        Locale ریشه("");
#إذا !UCONFIG_NO_BREAK_ITERATION
        UError_کد وضعیت = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::create_کلمه_Instance(ریشه, وضعیت);
        إذا (U_SUCCESS(وضعیت)) {
#پایان_اگر
            لكل (int32_t j=0; j<رشتهها_2->size(); ++j) {
                ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(رشتهها_2->elementAt(j));
                (str = *pStr).مقصد_پایینی(ریشه);
                fold_مجموعه.جمع_کن(str);
#إذا !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).مقصد_Title(bi, ریشه);
                fold_مجموعه.جمع_کن(str);
#پایان_اگر
                (str = *pStr).مقصد_فوقانی(ریشه);
                fold_مجموعه.جمع_کن(str);
                (str = *pStr).foldCase();
                fold_مجموعه.جمع_کن(str);
            }
#إذا !UCONFIG_NO_BREAK_ITERATION
        }
        حذف bi;
#پایان_اگر
    }
    *هذا = fold_مجموعه;
}

U_NAMESPACE_انتها
