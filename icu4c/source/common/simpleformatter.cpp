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
ثابت int32_t ARG_عدد_حد_بندی = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ثابت char16_t SEGMENT_لمائی_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ثابت int32_t MAX_SEGMENT_لمائی = SEGMENT_لمائی_PLACEHOLDER_CHAR - ARG_عدد_حد_بندی;

تعداد {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

مضمن UBool isInvalidArray(ثابت فراغ *array, int32_t length) {
   أرجع (length < 0 || (array == خالی_سوچک && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::عامل=(ثابت SimpleFormatter& other) U_LIFETIME_BOUND {
    إذا (هذا == &other) {
        أرجع *هذا;
    }
    compiled_نمونہ = other.compiled_نمونہ;
    أرجع *هذا;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::apply_نمونہ_Min_Max_Arguments(
        ثابت UnicodeString &نمونہ,
        int32_t min, int32_t max,
        UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع خطأ;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ثابت char16_t *نمونہ_بفر = نمونہ.get_بفر();
    int32_t نمونہ_لمائی = نمونہ.length();
    // Reserve the first char for the number of arguments.
    compiled_نمونہ.سیٹ_منزل(static_cast<char16_t>(0));
    int32_t متن_لمائی = 0;
    int32_t maxArg = -1;
    UBool وچ_Quote = خطأ;
    لكل (int32_t i = 0; i < نمونہ_لمائی;) {
        char16_t c = نمونہ_بفر[i++];
        إذا (c == APOS) {
            إذا (i < نمونہ_لمائی && (c = نمونہ_بفر[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } وإلا إذا (وچ_Quote) {
                // skip the quote-ending apostrophe
                وچ_Quote = خطأ;
                استمر;
            } وإلا إذا (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                وچ_Quote = سچ;
            } وإلا {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } وإلا إذا (!وچ_Quote && c == OPEN_BRACE) {
            إذا (متن_لمائی > 0) {
                compiled_نمونہ.سیٹ_Char_At(compiled_نمونہ.length() - متن_لمائی - 1,
                                          static_cast<char16_t>(ARG_عدد_حد_بندی + متن_لمائی));
                متن_لمائی = 0;
            }
            int32_t arg_نمبر;
            إذا ((i + 1) < نمونہ_لمائی &&
                    0 <= (arg_نمبر = نمونہ_بفر[i] - DIGIT_ZERO) && arg_نمبر <= 9 &&
                    نمونہ_بفر[i + 1] == CLOSE_BRACE) {
                i += 2;
            } وإلا {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_نمبر = -1;
                إذا (i < نمونہ_لمائی && DIGIT_ONE <= (c = نمونہ_بفر[i++]) && c <= DIGIT_NINE) {
                    arg_نمبر = c - DIGIT_ZERO;
                    طالما (i < نمونہ_لمائی &&
                            DIGIT_ZERO <= (c = نمونہ_بفر[i++]) && c <= DIGIT_NINE) {
                        arg_نمبر = arg_نمبر * 10 + (c - DIGIT_ZERO);
                        إذا (arg_نمبر >= ARG_عدد_حد_بندی) {
                            اكسر;
                        }
                    }
                }
                إذا (arg_نمبر < 0 || c != CLOSE_BRACE) {
                    error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                    أرجع خطأ;
                }
            }
            إذا (arg_نمبر > maxArg) {
                maxArg = arg_نمبر;
            }
            compiled_نمونہ.append(static_cast<char16_t>(arg_نمبر));
            استمر;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        إذا (متن_لمائی == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiled_نمونہ.append(SEGMENT_لمائی_PLACEHOLDER_CHAR);
        }
        compiled_نمونہ.append(c);
        إذا (++متن_لمائی == MAX_SEGMENT_لمائی) {
            متن_لمائی = 0;
        }
    }
    إذا (متن_لمائی > 0) {
        compiled_نمونہ.سیٹ_Char_At(compiled_نمونہ.length() - متن_لمائی - 1,
                                  static_cast<char16_t>(ARG_عدد_حد_بندی + متن_لمائی));
    }
    int32_t arg_گنتی = maxArg + 1;
    إذا (arg_گنتی < min || max < arg_گنتی) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    compiled_نمونہ.سیٹ_Char_At(0, static_cast<char16_t>(arg_گنتی));
    أرجع سچ;
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        UError_کوڈ& error_کوڈ) ثابت {
    ثابت UnicodeString *اقدار[] = { &value0 };
    أرجع formatAndAppend(اقدار, 1, append_منزل, خالی_سوچک, 0, error_کوڈ);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        UError_کوڈ& error_کوڈ) ثابت {
    ثابت UnicodeString *اقدار[] = { &value0, &value1 };
    أرجع formatAndAppend(اقدار, 2, append_منزل, خالی_سوچک, 0, error_کوڈ);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        ثابت UnicodeString& value2,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        UError_کوڈ& error_کوڈ) ثابت {
    ثابت UnicodeString *اقدار[] = { &value0, &value1, &value2 };
    أرجع formatAndAppend(اقدار, 3, append_منزل, خالی_سوچک, 0, error_کوڈ);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ثابت UnicodeString* ثابت* اقدار,
        int32_t اقدار_لمائی,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_لمائی,
        UError_کوڈ& error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع append_منزل;
    }
    إذا (isInvalidArray(اقدار, اقدار_لمائی) || isInvalidArray(offsets, offsets_لمائی) ||
            اقدار_لمائی < get_دلیل_حد_بندی()) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع append_منزل;
    }
    أرجع format(compiled_نمونہ.get_بفر(), compiled_نمونہ.length(), اقدار,
                  append_منزل, خالی_سوچک, سچ,
                  offsets, offsets_لمائی, error_کوڈ);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ثابت UnicodeString* ثابت* اقدار,
        int32_t اقدار_لمائی,
        UnicodeString& نتیجہ U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_لمائی,
        UError_کوڈ& error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع نتیجہ;
    }
    إذا (isInvalidArray(اقدار, اقدار_لمائی) || isInvalidArray(offsets, offsets_لمائی)) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع نتیجہ;
    }
    ثابت char16_t *cp = compiled_نمونہ.get_بفر();
    int32_t cp_لمائی = compiled_نمونہ.length();
    إذا (اقدار_لمائی < get_دلیل_حد_بندی(cp, cp_لمائی)) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع نتیجہ;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t پہلا_Arg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString نتیجہ_Copy;
    إذا (get_دلیل_حد_بندی(cp, cp_لمائی) > 0) {
        لكل (int32_t i = 1; i < cp_لمائی;) {
            int32_t n = cp[i++];
            إذا (n < ARG_عدد_حد_بندی) {
                إذا (اقدار[n] == &نتیجہ) {
                    إذا (i == 2) {
                        پہلا_Arg = n;
                    } وإلا إذا (نتیجہ_Copy.is_خالی_تھاواں() && !نتیجہ.is_خالی_تھاواں()) {
                        نتیجہ_Copy = نتیجہ;
                    }
                }
            } وإلا {
                i += n - ARG_عدد_حد_بندی;
            }
        }
    }
    إذا (پہلا_Arg < 0) {
        نتیجہ.remove();
    }
    أرجع format(cp, cp_لمائی, اقدار,
                  نتیجہ, &نتیجہ_Copy, خطأ,
                  offsets, offsets_لمائی, error_کوڈ);
}

UnicodeString SimpleFormatter::get_متن_With_No_Arguments(
        ثابت char16_t *compiled_نمونہ,
        int32_t compiled_نمونہ_لمائی,
        int32_t* offsets,
        int32_t offsets_لمائی) {
    لكل (int32_t i = 0; i < offsets_لمائی; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiled_نمونہ_لمائی - 1 -
            get_دلیل_حد_بندی(compiled_نمونہ, compiled_نمونہ_لمائی);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    لكل (int32_t i = 1; i < compiled_نمونہ_لمائی;) {
        int32_t n = compiled_نمونہ[i++];
        إذا (n > ARG_عدد_حد_بندی) {
            n -= ARG_عدد_حد_بندی;
            sb.append(compiled_نمونہ + i, n);
            i += n;
        } وإلا إذا (n < offsets_لمائی) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    أرجع sb;
}

UnicodeString& SimpleFormatter::format(
        ثابت char16_t* compiled_نمونہ,
        int32_t compiled_نمونہ_لمائی,
        ثابت UnicodeString* ثابت* اقدار,
        UnicodeString& نتیجہ U_LIFETIME_BOUND,
        ثابت UnicodeString* نتیجہ_Copy,
        UBool forbid_نتیجہ_As_قدر,
        int32_t* offsets,
        int32_t offsets_لمائی,
        UError_کوڈ& error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع نتیجہ;
    }
    لكل (int32_t i = 0; i < offsets_لمائی; i++) {
        offsets[i] = -1;
    }
    لكل (int32_t i = 1; i < compiled_نمونہ_لمائی;) {
        int32_t n = compiled_نمونہ[i++];
        إذا (n < ARG_عدد_حد_بندی) {
            ثابت UnicodeString *value = اقدار[n];
            إذا (value == خالی_سوچک) {
                error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                أرجع نتیجہ;
            }
            إذا (value == &نتیجہ) {
                إذا (forbid_نتیجہ_As_قدر) {
                    error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                    أرجع نتیجہ;
                }
                إذا (i == 2) {
                    // We are appending to result which is also the first value object.
                    إذا (n < offsets_لمائی) {
                        offsets[n] = 0;
                    }
                } وإلا {
                    إذا (n < offsets_لمائی) {
                        offsets[n] = نتیجہ.length();
                    }
                    نتیجہ.append(*نتیجہ_Copy);
                }
            } وإلا {
                إذا (n < offsets_لمائی) {
                    offsets[n] = نتیجہ.length();
                }
                نتیجہ.append(*value);
            }
        } وإلا {
            int32_t length = n - ARG_عدد_حد_بندی;
            نتیجہ.append(compiled_نمونہ + i, length);
            i += length;
        }
    }
    أرجع نتیجہ;
}

U_NAMESPACE_سرا
