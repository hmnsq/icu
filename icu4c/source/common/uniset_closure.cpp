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
#وصف_ڪريو _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

Unicode_سيٽ::Unicode_سيٽ(ثابت UnicodeString& نمونو,
                       uint32_t options,
                       ثابت نشان_جدول* symbols,
                       UError_ڪوڊ& حالت) {
    apply_نمونو(نمونو, options, symbols, حالت);
    _dbgct(هذا);
}

Unicode_سيٽ::Unicode_سيٽ(ثابت UnicodeString& نمونو, ParsePosition& pos,
                       uint32_t options,
                       ثابت نشان_جدول* symbols,
                       UError_ڪوڊ& حالت) {
    apply_نمونو(نمونو, pos, options, symbols, حالت);
    _dbgct(هذا);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_سيٽ& Unicode_سيٽ::apply_نمونو(ثابت UnicodeString& نمونو,
                                     uint32_t options,
                                     ثابت نشان_جدول* symbols,
                                     UError_ڪوڊ& حالت) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    apply_نمونو(نمونو, pos, options, symbols, حالت);
    إذا (U_FAILURE(حالت)) أرجع *هذا;

    int32_t i = pos.get_اشاريو();

    إذا (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(نمونو, i, سچ);
    }

    إذا (i != نمونو.length()) {
        حالت = U_ILLEGAL_دليل_ERROR;
    }
    أرجع *هذا;
}

Unicode_سيٽ& Unicode_سيٽ::apply_نمونو(ثابت UnicodeString& نمونو,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ثابت نشان_جدول* symbols,
                                     UError_ڪوڊ& حالت) U_LIFETIME_BOUND {
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
    قاعدو_Character_Iterator chars(نمونو, symbols, pos);
    apply_نمونو(نمونو, pos, chars, symbols, rebuiltPat, options, &Unicode_سيٽ::closeOver, حالت);
    إذا (U_FAILURE(حالت)) أرجع *هذا;
    إذا (chars.اندريون_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        حالت = U_MALFORMED_سيٽ;
        أرجع *هذا;
    }
    سيٽ_نمونو(rebuiltPat);
    أرجع *هذا;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ساكن فراغ U_CALLCONV
سيٽ_جوڙيو(USet *سيٽ, UChar32 c) {
    reinterpret_cast<Unicode_سيٽ*>(سيٽ)->جوڙيو(c);
}

ساكن فراغ U_CALLCONV
سيٽ_جوڙيو_Range(USet *سيٽ, UChar32 شروعات, UChar32 پڇاڙي) {
    reinterpret_cast<Unicode_سيٽ*>(سيٽ)->جوڙيو(شروعات, پڇاڙي);
}

ساكن فراغ U_CALLCONV
سيٽ_جوڙيو_String(USet *سيٽ, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_سيٽ*>(سيٽ)->جوڙيو(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
ساكن مضمن فراغ
جوڙيو_Case_Mapping(Unicode_سيٽ &سيٽ, int32_t نتيجو, ثابت char16_t *ڀريل, UnicodeString &str) {
    إذا(نتيجو >= 0) {
        إذا(نتيجو > UCASE_MAX_STRING_ڊيگهائي) {
            // add a single-code point case mapping
            سيٽ.جوڙيو(نتيجو);
        } وإلا {
            // add a string case mapping from full with length result
            str.سيٽ_منزل(static_cast<UBool>(خطأ), ڀريل, نتيجو);
            سيٽ.جوڙيو(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

نطاق {

/** For case closure on a large set, look only at code points with relevant properties. */
ثابت Unicode_سيٽ &maybeOnlyCaseSensitive(ثابت Unicode_سيٽ &src U_LIFETIME_BOUND,
                                         Unicode_سيٽ &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.شامل_آهي(0, 0x10ffff));
    إذا (src.size() < 30) {
        أرجع src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
    ثابت Unicode_سيٽ *sensitive =
        CharacterProperties::get_ٻٽو_Property_سيٽ(UCHAR_CASE_SENSITIVE, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    إذا (src.get_Range_ڳڻپ() > sensitive->get_Range_ڳڻپ()) {
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
        U16_ايندڙ(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        إذا (scfChar != c) {
            // Copy the characters before c.
            scf.سيٽ_منزل(p, i - U16_ڊيگهائي(c));
            // Loop over the rest of the string and keep case-folding.
            لكل (;;) {
                scf.append(scfChar);
                إذا (i == length) {
                    أرجع سچ;
                }
                U16_ايندڙ(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    أرجع خطأ;
}

}  // namespace

Unicode_سيٽ& Unicode_سيٽ::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    بدل (attribute & USET_CASE_نقاب) {
    حالة 0:
        اكسر;
    حالة USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ خطأ);
        اكسر;
    حالة USET_جوڙيو_CASE_MAPPINGS:
        close_Over_جوڙيو_Case_Mappings();
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

فراغ Unicode_سيٽ::closeOverCaseInsensitive(منطقي simple) {
    // Start with input set to guarantee inclusion.
    Unicode_سيٽ fold_سيٽ(*هذا);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    إذا (!simple && fold_سيٽ.has_لڙيون()) {
        fold_سيٽ.لڙيون_2->removeAllElements();
    }

    USetAdder sa = {
        fold_سيٽ.منزل_USet(),
        سيٽ_جوڙيو,
        سيٽ_جوڙيو_Range,
        سيٽ_جوڙيو_String,
        خالي_اشارو, // don't need remove()
        خالي_اشارو // don't need removeRange()
    };

    Unicode_سيٽ subset(0, 0x10ffff);
    ثابت Unicode_سيٽ &ڪوڊ_Points = maybeOnlyCaseSensitive(*هذا, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = ڪوڊ_Points.get_Range_ڳڻپ();

    لكل (int32_t i=0; i<n; ++i) {
        UChar32 شروعات = ڪوڊ_Points.get_Range_شروعات(i);
        UChar32 پڇاڙي   = ڪوڊ_Points.get_Range_پڇاڙي(i);

        إذا (simple) {
            لكل (UChar32 cp=شروعات; cp<=پڇاڙي; ++cp) {
                ucase_جوڙيو_Simple_Case_گهيرو(cp, &sa);
            }
        } وإلا {
            لكل (UChar32 cp=شروعات; cp<=پڇاڙي; ++cp) {
                ucase_جوڙيو_Case_گهيرو(cp, &sa);
            }
        }
    }
    إذا (has_لڙيون()) {
        UnicodeString str;
        لكل (int32_t j=0; j<لڙيون_2->size(); ++j) {
            ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(لڙيون_2->elementAt(j));
            إذا (simple) {
                إذا (scfString(*pStr, str)) {
                    fold_سيٽ.remove(*pStr).جوڙيو(str);
                }
            } وإلا {
                str = *pStr;
                str.foldCase();
                إذا(!ucase_جوڙيو_String_Case_گهيرو(str.get_بفر(), str.length(), &sa)) {
                    fold_سيٽ.جوڙيو(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *هذا = fold_سيٽ;
}

فراغ Unicode_سيٽ::close_Over_جوڙيو_Case_Mappings() {
    // Start with input set to guarantee inclusion.
    Unicode_سيٽ fold_سيٽ(*هذا);

    Unicode_سيٽ subset(0, 0x10ffff);
    ثابت Unicode_سيٽ &ڪوڊ_Points = maybeOnlyCaseSensitive(*هذا, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = ڪوڊ_Points.get_Range_ڳڻپ();
    UChar32 نتيجو;
    ثابت char16_t *ڀريل;
    UnicodeString str;

    لكل (int32_t i=0; i<n; ++i) {
        UChar32 شروعات = ڪوڊ_Points.get_Range_شروعات(i);
        UChar32 پڇاڙي   = ڪوڊ_Points.get_Range_پڇاڙي(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        لكل (UChar32 cp=شروعات; cp<=پڇاڙي; ++cp) {
            نتيجو = ucase_منزل_ڀريل_هيٺاهون(cp, خالي_اشارو, خالي_اشارو, &ڀريل, UCASE_LOC_پاڙ);
            جوڙيو_Case_Mapping(fold_سيٽ, نتيجو, ڀريل, str);

            نتيجو = ucase_منزل_ڀريل_Title(cp, خالي_اشارو, خالي_اشارو, &ڀريل, UCASE_LOC_پاڙ);
            جوڙيو_Case_Mapping(fold_سيٽ, نتيجو, ڀريل, str);

            نتيجو = ucase_منزل_ڀريل_مٿانهون(cp, خالي_اشارو, خالي_اشارو, &ڀريل, UCASE_LOC_پاڙ);
            جوڙيو_Case_Mapping(fold_سيٽ, نتيجو, ڀريل, str);

            نتيجو = ucase_منزل_ڀريل_Folding(cp, &ڀريل, 0);
            جوڙيو_Case_Mapping(fold_سيٽ, نتيجو, ڀريل, str);
        }
    }
    إذا (has_لڙيون()) {
        Locale پاڙ("");
#إذا !UCONFIG_NO_BREAK_ITERATION
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::create_لفظ_Instance(پاڙ, حالت);
        إذا (U_SUCCESS(حالت)) {
#جيڪڏهن_پورو
            لكل (int32_t j=0; j<لڙيون_2->size(); ++j) {
                ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(لڙيون_2->elementAt(j));
                (str = *pStr).منزل_هيٺاهون(پاڙ);
                fold_سيٽ.جوڙيو(str);
#إذا !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).منزل_Title(bi, پاڙ);
                fold_سيٽ.جوڙيو(str);
#جيڪڏهن_پورو
                (str = *pStr).منزل_مٿانهون(پاڙ);
                fold_سيٽ.جوڙيو(str);
                (str = *pStr).foldCase();
                fold_سيٽ.جوڙيو(str);
            }
#إذا !UCONFIG_NO_BREAK_ITERATION
        }
        ميٽيو bi;
#جيڪڏهن_پورو
    }
    *هذا = fold_سيٽ;
}

U_NAMESPACE_پڇاڙي
