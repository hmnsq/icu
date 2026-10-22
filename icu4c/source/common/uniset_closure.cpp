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
#تعریف_کرو _dbgct(me)

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

Unicode_سیٹ::Unicode_سیٹ(ثابت UnicodeString& نمونہ,
                       uint32_t options,
                       ثابت علامت_جدول* symbols,
                       UError_رمز& حالت_2) {
    apply_نمونہ(نمونہ, options, symbols, حالت_2);
    _dbgct(یہ);
}

Unicode_سیٹ::Unicode_سیٹ(ثابت UnicodeString& نمونہ, ParsePosition& pos,
                       uint32_t options,
                       ثابت علامت_جدول* symbols,
                       UError_رمز& حالت_2) {
    apply_نمونہ(نمونہ, pos, options, symbols, حالت_2);
    _dbgct(یہ);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_سیٹ& Unicode_سیٹ::apply_نمونہ(ثابت UnicodeString& نمونہ,
                                     uint32_t options,
                                     ثابت علامت_جدول* symbols,
                                     UError_رمز& حالت_2) U_LIFETIME_BOUND {
    ParsePosition pos(0);
    apply_نمونہ(نمونہ, pos, options, symbols, حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس *یہ;

    int32_t i = pos.get_اشاریہ();

    اگر (options & USET_IGNORE_SPACE) {
        // Skip over trailing whitespace
        ICU_Utility::skipWhitespace(نمونہ, i, سچ);
    }

    اگر (i != نمونہ.length()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
    }
    واپس *یہ;
}

Unicode_سیٹ& Unicode_سیٹ::apply_نمونہ(ثابت UnicodeString& نمونہ,
                                     ParsePosition& pos,
                                     uint32_t options,
                                     ثابت علامت_جدول* symbols,
                                     UError_رمز& حالت_2) U_LIFETIME_BOUND {
    اگر (U_FAILURE(حالت_2)) {
        واپس *یہ;
    }
    اگر (isFrozen()) {
        حالت_2 = U_NO_WRITE_PERMISSION;
        واپس *یہ;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    قاعدہ_Character_Iterator chars(نمونہ, symbols, pos);
    apply_نمونہ(نمونہ, pos, chars, symbols, rebuiltPat, options, &Unicode_سیٹ::closeOver, حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس *یہ;
    اگر (chars.اندر_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        حالت_2 = U_MALFORMED_سیٹ;
        واپس *یہ;
    }
    سیٹ_نمونہ(rebuiltPat);
    واپس *یہ;
}

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ساکن خالی U_CALLCONV
سیٹ_جوڑو(USet *سیٹ, UChar32 c) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(c);
}

ساکن خالی U_CALLCONV
سیٹ_جوڑو_Range(USet *سیٹ, UChar32 آغاز, UChar32 انتہا) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(آغاز, انتہا);
}

ساکن خالی U_CALLCONV
سیٹ_جوڑو_String(USet *سیٹ, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(UnicodeString(static_cast<UBool>(length < 0), str, length));
}

//----------------------------------------------------------------
// Case folding API
//----------------------------------------------------------------

// add the result of a full case mapping to the set
// use str as a temporary string to avoid constructing one
ساکن اندرونی خالی
جوڑو_Case_Mapping(Unicode_سیٹ &سیٹ, int32_t نتیجہ, ثابت char16_t *بھرا, UnicodeString &str) {
    اگر(نتیجہ >= 0) {
        اگر(نتیجہ > UCASE_MAX_STRING_لمبائی) {
            // add a single-code point case mapping
            سیٹ.جوڑو(نتیجہ);
        } ورنہ {
            // add a string case mapping from full with length result
            str.سیٹ_مقصد(static_cast<UBool>(جھوٹ), بھرا, نتیجہ);
            سیٹ.جوڑو(str);
        }
    }
    // result < 0: the code point mapped to itself, no need to add it
    // see ucase.h
}

نامخلا {

/** For case closure on a large set, look only at code points with relevant properties. */
ثابت Unicode_سیٹ &maybeOnlyCaseSensitive(ثابت Unicode_سیٹ &src U_LIFETIME_BOUND,
                                         Unicode_سیٹ &subset U_LIFETIME_BOUND) {
    // The subset must have been constructed with all code points,
    // so that the retainAll() intersection effectively copies all single code points from src.
    U_ASSERT(subset.شامل_ہے(0, 0x10ffff));
    اگر (src.size() < 30) {
        واپس src;
    }
    // Return the intersection of the src code points with Case_Sensitive ones.
    UError_رمز error_رمز = U_ZERO_ERROR;
    ثابت Unicode_سیٹ *sensitive =
        CharacterProperties::get_ثنائی_Property_سیٹ(UCHAR_CASE_SENSITIVE, error_رمز);
    اگر (U_FAILURE(error_رمز)) {
        واپس src;
    }
    // Start by copying the "smaller" set.
    // (We "copy" by intersecting all Unicode *code points* with the first set,
    // which omits any strings.)
    اگر (src.get_Range_گنتی() > sensitive->get_Range_گنتی()) {
        subset.retainAll(*sensitive);
        subset.retainAll(src);
    } ورنہ {
        subset.retainAll(src);
        subset.retainAll(*sensitive);
    }
    واپس subset;
}

// Per-character scf = Simple_Case_Folding of a string.
// (Normally when we case-fold a string we use full case foldings.)
منطقی scfString(ثابت UnicodeString &s, UnicodeString &scf) {
    // Iterate over the raw buffer for best performance.
    ثابت char16_t *p = s.get_بفر();
    int32_t length = s.length();
    // Loop while not needing modification.
    برائے (int32_t i = 0; i < length;) {
        UChar32 c;
        U16_اگلا(p, i, length, c);  // post-increments i
        UChar32 scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
        اگر (scfChar != c) {
            // Copy the characters before c.
            scf.سیٹ_مقصد(p, i - U16_لمبائی(c));
            // Loop over the rest of the string and keep case-folding.
            برائے (;;) {
                scf.append(scfChar);
                اگر (i == length) {
                    واپس سچ;
                }
                U16_اگلا(p, i, length, c);  // post-increments i
                scfChar = u_foldCase(c, U_FOLD_CASE_DEFAULT);
            }
        }
    }
    واپس جھوٹ;
}

}  // namespace

Unicode_سیٹ& Unicode_سیٹ::closeOver(int32_t attribute) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        واپس *یہ;
    }
    انتخاب (attribute & USET_CASE_نقاب) {
    حالت 0:
        توڑو;
    حالت USET_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ جھوٹ);
        توڑو;
    حالت USET_جوڑو_CASE_MAPPINGS:
        close_Over_جوڑو_Case_Mappings();
        توڑو;
    حالت USET_SIMPLE_CASE_INSENSITIVE:
        closeOverCaseInsensitive(/* simple= */ سچ);
        توڑو;
    طےشدہ:
        // bad option (unreachable)
        توڑو;
    }
    واپس *یہ;
}

خالی Unicode_سیٹ::closeOverCaseInsensitive(منطقی simple) {
    // Start with input set to guarantee inclusion.
    Unicode_سیٹ fold_سیٹ(*یہ);
    // Full case mappings closure:
    // Remove strings because the strings will actually be reduced (folded);
    // therefore, start with no strings and add only those needed.
    // Do this before processing code points, because they may add strings.
    اگر (!simple && fold_سیٹ.has_لڑیاں()) {
        fold_سیٹ.لڑیاں_2->removeAllElements();
    }

    USetAdder sa = {
        fold_سیٹ.مقصد_USet(),
        سیٹ_جوڑو,
        سیٹ_جوڑو_Range,
        سیٹ_جوڑو_String,
        خالی_اشاریہ, // don't need remove()
        خالی_اشاریہ // don't need removeRange()
    };

    Unicode_سیٹ subset(0, 0x10ffff);
    ثابت Unicode_سیٹ &رمز_Points = maybeOnlyCaseSensitive(*یہ, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = رمز_Points.get_Range_گنتی();

    برائے (int32_t i=0; i<n; ++i) {
        UChar32 آغاز = رمز_Points.get_Range_آغاز(i);
        UChar32 انتہا   = رمز_Points.get_Range_انتہا(i);

        اگر (simple) {
            برائے (UChar32 cp=آغاز; cp<=انتہا; ++cp) {
                ucase_جوڑو_Simple_Case_احاطہ(cp, &sa);
            }
        } ورنہ {
            برائے (UChar32 cp=آغاز; cp<=انتہا; ++cp) {
                ucase_جوڑو_Case_احاطہ(cp, &sa);
            }
        }
    }
    اگر (has_لڑیاں()) {
        UnicodeString str;
        برائے (int32_t j=0; j<لڑیاں_2->size(); ++j) {
            ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(لڑیاں_2->elementAt(j));
            اگر (simple) {
                اگر (scfString(*pStr, str)) {
                    fold_سیٹ.remove(*pStr).جوڑو(str);
                }
            } ورنہ {
                str = *pStr;
                str.foldCase();
                اگر(!ucase_جوڑو_String_Case_احاطہ(str.get_بفر(), str.length(), &sa)) {
                    fold_سیٹ.جوڑو(str); // does not map to code points: add the folded string itself
                }
            }
        }
    }
    *یہ = fold_سیٹ;
}

خالی Unicode_سیٹ::close_Over_جوڑو_Case_Mappings() {
    // Start with input set to guarantee inclusion.
    Unicode_سیٹ fold_سیٹ(*یہ);

    Unicode_سیٹ subset(0, 0x10ffff);
    ثابت Unicode_سیٹ &رمز_Points = maybeOnlyCaseSensitive(*یہ, subset);

    // Iterate over the ranges of single code points. Nested loop for each code point.
    int32_t n = رمز_Points.get_Range_گنتی();
    UChar32 نتیجہ;
    ثابت char16_t *بھرا;
    UnicodeString str;

    برائے (int32_t i=0; i<n; ++i) {
        UChar32 آغاز = رمز_Points.get_Range_آغاز(i);
        UChar32 انتہا   = رمز_Points.get_Range_انتہا(i);

        // add case mappings
        // (does not add long s for regular s, or Kelvin for k, for example)
        برائے (UChar32 cp=آغاز; cp<=انتہا; ++cp) {
            نتیجہ = ucase_مقصد_بھرا_زیریں(cp, خالی_اشاریہ, خالی_اشاریہ, &بھرا, UCASE_LOC_جڑ);
            جوڑو_Case_Mapping(fold_سیٹ, نتیجہ, بھرا, str);

            نتیجہ = ucase_مقصد_بھرا_Title(cp, خالی_اشاریہ, خالی_اشاریہ, &بھرا, UCASE_LOC_جڑ);
            جوڑو_Case_Mapping(fold_سیٹ, نتیجہ, بھرا, str);

            نتیجہ = ucase_مقصد_بھرا_بالائی(cp, خالی_اشاریہ, خالی_اشاریہ, &بھرا, UCASE_LOC_جڑ);
            جوڑو_Case_Mapping(fold_سیٹ, نتیجہ, بھرا, str);

            نتیجہ = ucase_مقصد_بھرا_Folding(cp, &بھرا, 0);
            جوڑو_Case_Mapping(fold_سیٹ, نتیجہ, بھرا, str);
        }
    }
    اگر (has_لڑیاں()) {
        Locale جڑ("");
#اگر !UCONFIG_NO_BREAK_ITERATION
        UError_رمز حالت_2 = U_ZERO_ERROR;
        BreakIterator *bi = BreakIterator::create_لفظ_Instance(جڑ, حالت_2);
        اگر (U_SUCCESS(حالت_2)) {
#اگر_ختم
            برائے (int32_t j=0; j<لڑیاں_2->size(); ++j) {
                ثابت UnicodeString* pStr = static_cast<ثابت UnicodeString*>(لڑیاں_2->elementAt(j));
                (str = *pStr).مقصد_زیریں(جڑ);
                fold_سیٹ.جوڑو(str);
#اگر !UCONFIG_NO_BREAK_ITERATION
                (str = *pStr).مقصد_Title(bi, جڑ);
                fold_سیٹ.جوڑو(str);
#اگر_ختم
                (str = *pStr).مقصد_بالائی(جڑ);
                fold_سیٹ.جوڑو(str);
                (str = *pStr).foldCase();
                fold_سیٹ.جوڑو(str);
            }
#اگر !UCONFIG_NO_BREAK_ITERATION
        }
        حذف bi;
#اگر_ختم
    }
    *یہ = fold_سیٹ;
}

U_NAMESPACE_انتہا
