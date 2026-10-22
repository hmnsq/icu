// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/simpleformatter.h"
#ضمن "unicode/unistr.h"
#ضمن "uassert.h"

U_NAMESPACE_BEGIN

نطاق {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
ثابت int32_t ARG_انگ_حد_بندي = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ثابت char16_t SEGMENT_ڊيگهائي_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ثابت int32_t MAX_SEGMENT_ڊيگهائي = SEGMENT_ڊيگهائي_PLACEHOLDER_CHAR - ARG_انگ_حد_بندي;

تعداد {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

مضمن UBool isInvalidArray(ثابت فراغ *array, int32_t length) {
   أرجع (length < 0 || (array == خالي_اشارو && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::عامل=(ثابت SimpleFormatter& other) U_LIFETIME_BOUND {
    إذا (هذا == &other) {
        أرجع *هذا;
    }
    compiled_نمونو = other.compiled_نمونو;
    أرجع *هذا;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::apply_نمونو_Min_Max_Arguments(
        ثابت UnicodeString &نمونو,
        int32_t min, int32_t max,
        UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع خطأ;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ثابت char16_t *نمونو_بفر = نمونو.get_بفر();
    int32_t نمونو_ڊيگهائي = نمونو.length();
    // Reserve the first char for the number of arguments.
    compiled_نمونو.سيٽ_منزل(static_cast<char16_t>(0));
    int32_t متن_ڊيگهائي = 0;
    int32_t maxArg = -1;
    UBool اندريون_Quote = خطأ;
    لكل (int32_t i = 0; i < نمونو_ڊيگهائي;) {
        char16_t c = نمونو_بفر[i++];
        إذا (c == APOS) {
            إذا (i < نمونو_ڊيگهائي && (c = نمونو_بفر[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } وإلا إذا (اندريون_Quote) {
                // skip the quote-ending apostrophe
                اندريون_Quote = خطأ;
                استمر;
            } وإلا إذا (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                اندريون_Quote = سچ;
            } وإلا {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } وإلا إذا (!اندريون_Quote && c == OPEN_BRACE) {
            إذا (متن_ڊيگهائي > 0) {
                compiled_نمونو.سيٽ_Char_At(compiled_نمونو.length() - متن_ڊيگهائي - 1,
                                          static_cast<char16_t>(ARG_انگ_حد_بندي + متن_ڊيگهائي));
                متن_ڊيگهائي = 0;
            }
            int32_t arg_عدد;
            إذا ((i + 1) < نمونو_ڊيگهائي &&
                    0 <= (arg_عدد = نمونو_بفر[i] - DIGIT_ZERO) && arg_عدد <= 9 &&
                    نمونو_بفر[i + 1] == CLOSE_BRACE) {
                i += 2;
            } وإلا {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_عدد = -1;
                إذا (i < نمونو_ڊيگهائي && DIGIT_ONE <= (c = نمونو_بفر[i++]) && c <= DIGIT_NINE) {
                    arg_عدد = c - DIGIT_ZERO;
                    طالما (i < نمونو_ڊيگهائي &&
                            DIGIT_ZERO <= (c = نمونو_بفر[i++]) && c <= DIGIT_NINE) {
                        arg_عدد = arg_عدد * 10 + (c - DIGIT_ZERO);
                        إذا (arg_عدد >= ARG_انگ_حد_بندي) {
                            اكسر;
                        }
                    }
                }
                إذا (arg_عدد < 0 || c != CLOSE_BRACE) {
                    error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                    أرجع خطأ;
                }
            }
            إذا (arg_عدد > maxArg) {
                maxArg = arg_عدد;
            }
            compiled_نمونو.append(static_cast<char16_t>(arg_عدد));
            استمر;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        إذا (متن_ڊيگهائي == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiled_نمونو.append(SEGMENT_ڊيگهائي_PLACEHOLDER_CHAR);
        }
        compiled_نمونو.append(c);
        إذا (++متن_ڊيگهائي == MAX_SEGMENT_ڊيگهائي) {
            متن_ڊيگهائي = 0;
        }
    }
    إذا (متن_ڊيگهائي > 0) {
        compiled_نمونو.سيٽ_Char_At(compiled_نمونو.length() - متن_ڊيگهائي - 1,
                                  static_cast<char16_t>(ARG_انگ_حد_بندي + متن_ڊيگهائي));
    }
    int32_t arg_ڳڻپ = maxArg + 1;
    إذا (arg_ڳڻپ < min || max < arg_ڳڻپ) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خطأ;
    }
    compiled_نمونو.سيٽ_Char_At(0, static_cast<char16_t>(arg_ڳڻپ));
    أرجع سچ;
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        UError_ڪوڊ& error_ڪوڊ) ثابت {
    ثابت UnicodeString *قدرون[] = { &value0 };
    أرجع formatAndAppend(قدرون, 1, append_منزل, خالي_اشارو, 0, error_ڪوڊ);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        UError_ڪوڊ& error_ڪوڊ) ثابت {
    ثابت UnicodeString *قدرون[] = { &value0, &value1 };
    أرجع formatAndAppend(قدرون, 2, append_منزل, خالي_اشارو, 0, error_ڪوڊ);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        ثابت UnicodeString& value2,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        UError_ڪوڊ& error_ڪوڊ) ثابت {
    ثابت UnicodeString *قدرون[] = { &value0, &value1, &value2 };
    أرجع formatAndAppend(قدرون, 3, append_منزل, خالي_اشارو, 0, error_ڪوڊ);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ثابت UnicodeString* ثابت* قدرون,
        int32_t قدرون_ڊيگهائي,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_ڊيگهائي,
        UError_ڪوڊ& error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع append_منزل;
    }
    إذا (isInvalidArray(قدرون, قدرون_ڊيگهائي) || isInvalidArray(offsets, offsets_ڊيگهائي) ||
            قدرون_ڊيگهائي < get_دليل_حد_بندي()) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع append_منزل;
    }
    أرجع format(compiled_نمونو.get_بفر(), compiled_نمونو.length(), قدرون,
                  append_منزل, خالي_اشارو, سچ,
                  offsets, offsets_ڊيگهائي, error_ڪوڊ);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ثابت UnicodeString* ثابت* قدرون,
        int32_t قدرون_ڊيگهائي,
        UnicodeString& نتيجو U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_ڊيگهائي,
        UError_ڪوڊ& error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع نتيجو;
    }
    إذا (isInvalidArray(قدرون, قدرون_ڊيگهائي) || isInvalidArray(offsets, offsets_ڊيگهائي)) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع نتيجو;
    }
    ثابت char16_t *cp = compiled_نمونو.get_بفر();
    int32_t cp_ڊيگهائي = compiled_نمونو.length();
    إذا (قدرون_ڊيگهائي < get_دليل_حد_بندي(cp, cp_ڊيگهائي)) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع نتيجو;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t پهريون_Arg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString نتيجو_Copy;
    إذا (get_دليل_حد_بندي(cp, cp_ڊيگهائي) > 0) {
        لكل (int32_t i = 1; i < cp_ڊيگهائي;) {
            int32_t n = cp[i++];
            إذا (n < ARG_انگ_حد_بندي) {
                إذا (قدرون[n] == &نتيجو) {
                    إذا (i == 2) {
                        پهريون_Arg = n;
                    } وإلا إذا (نتيجو_Copy.is_خالي_جايون() && !نتيجو.is_خالي_جايون()) {
                        نتيجو_Copy = نتيجو;
                    }
                }
            } وإلا {
                i += n - ARG_انگ_حد_بندي;
            }
        }
    }
    إذا (پهريون_Arg < 0) {
        نتيجو.remove();
    }
    أرجع format(cp, cp_ڊيگهائي, قدرون,
                  نتيجو, &نتيجو_Copy, خطأ,
                  offsets, offsets_ڊيگهائي, error_ڪوڊ);
}

UnicodeString SimpleFormatter::get_متن_With_No_Arguments(
        ثابت char16_t *compiled_نمونو,
        int32_t compiled_نمونو_ڊيگهائي,
        int32_t* offsets,
        int32_t offsets_ڊيگهائي) {
    لكل (int32_t i = 0; i < offsets_ڊيگهائي; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiled_نمونو_ڊيگهائي - 1 -
            get_دليل_حد_بندي(compiled_نمونو, compiled_نمونو_ڊيگهائي);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    لكل (int32_t i = 1; i < compiled_نمونو_ڊيگهائي;) {
        int32_t n = compiled_نمونو[i++];
        إذا (n > ARG_انگ_حد_بندي) {
            n -= ARG_انگ_حد_بندي;
            sb.append(compiled_نمونو + i, n);
            i += n;
        } وإلا إذا (n < offsets_ڊيگهائي) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    أرجع sb;
}

UnicodeString& SimpleFormatter::format(
        ثابت char16_t* compiled_نمونو,
        int32_t compiled_نمونو_ڊيگهائي,
        ثابت UnicodeString* ثابت* قدرون,
        UnicodeString& نتيجو U_LIFETIME_BOUND,
        ثابت UnicodeString* نتيجو_Copy,
        UBool forbid_نتيجو_As_قدر,
        int32_t* offsets,
        int32_t offsets_ڊيگهائي,
        UError_ڪوڊ& error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع نتيجو;
    }
    لكل (int32_t i = 0; i < offsets_ڊيگهائي; i++) {
        offsets[i] = -1;
    }
    لكل (int32_t i = 1; i < compiled_نمونو_ڊيگهائي;) {
        int32_t n = compiled_نمونو[i++];
        إذا (n < ARG_انگ_حد_بندي) {
            ثابت UnicodeString *value = قدرون[n];
            إذا (value == خالي_اشارو) {
                error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                أرجع نتيجو;
            }
            إذا (value == &نتيجو) {
                إذا (forbid_نتيجو_As_قدر) {
                    error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                    أرجع نتيجو;
                }
                إذا (i == 2) {
                    // We are appending to result which is also the first value object.
                    إذا (n < offsets_ڊيگهائي) {
                        offsets[n] = 0;
                    }
                } وإلا {
                    إذا (n < offsets_ڊيگهائي) {
                        offsets[n] = نتيجو.length();
                    }
                    نتيجو.append(*نتيجو_Copy);
                }
            } وإلا {
                إذا (n < offsets_ڊيگهائي) {
                    offsets[n] = نتيجو.length();
                }
                نتيجو.append(*value);
            }
        } وإلا {
            int32_t length = n - ARG_انگ_حد_بندي;
            نتيجو.append(compiled_نمونو + i, length);
            i += length;
        }
    }
    أرجع نتيجو;
}

U_NAMESPACE_پڇاڙي
