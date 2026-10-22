// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#شامل "unicode/utypes.h"
#شامل "unicode/simpleformatter.h"
#شامل "unicode/unistr.h"
#شامل "uassert.h"

U_NAMESPACE_BEGIN

نامخلا {

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
ثابت char16_t SEGMENT_لمبائی_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ثابت int32_t MAX_SEGMENT_لمبائی = SEGMENT_لمبائی_PLACEHOLDER_CHAR - ARG_عدد_حد_بندی;

شمار {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

اندرونی UBool isInvalidArray(ثابت خالی *array, int32_t length) {
   واپس (length < 0 || (array == خالی_اشاریہ && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::عامل=(ثابت SimpleFormatter& other) U_LIFETIME_BOUND {
    اگر (یہ == &other) {
        واپس *یہ;
    }
    compiled_نمونہ = other.compiled_نمونہ;
    واپس *یہ;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::apply_نمونہ_Min_Max_Arguments(
        ثابت UnicodeString &نمونہ,
        int32_t min, int32_t max,
        UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) {
        واپس جھوٹ;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ثابت char16_t *نمونہ_بفر = نمونہ.get_بفر();
    int32_t نمونہ_لمبائی = نمونہ.length();
    // Reserve the first char for the number of arguments.
    compiled_نمونہ.سیٹ_مقصد(static_cast<char16_t>(0));
    int32_t متن_لمبائی = 0;
    int32_t maxArg = -1;
    UBool اندر_Quote = جھوٹ;
    برائے (int32_t i = 0; i < نمونہ_لمبائی;) {
        char16_t c = نمونہ_بفر[i++];
        اگر (c == APOS) {
            اگر (i < نمونہ_لمبائی && (c = نمونہ_بفر[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } ورنہ اگر (اندر_Quote) {
                // skip the quote-ending apostrophe
                اندر_Quote = جھوٹ;
                جاری;
            } ورنہ اگر (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                اندر_Quote = سچ;
            } ورنہ {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } ورنہ اگر (!اندر_Quote && c == OPEN_BRACE) {
            اگر (متن_لمبائی > 0) {
                compiled_نمونہ.سیٹ_Char_At(compiled_نمونہ.length() - متن_لمبائی - 1,
                                          static_cast<char16_t>(ARG_عدد_حد_بندی + متن_لمبائی));
                متن_لمبائی = 0;
            }
            int32_t arg_نمبر;
            اگر ((i + 1) < نمونہ_لمبائی &&
                    0 <= (arg_نمبر = نمونہ_بفر[i] - DIGIT_ZERO) && arg_نمبر <= 9 &&
                    نمونہ_بفر[i + 1] == CLOSE_BRACE) {
                i += 2;
            } ورنہ {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_نمبر = -1;
                اگر (i < نمونہ_لمبائی && DIGIT_ONE <= (c = نمونہ_بفر[i++]) && c <= DIGIT_NINE) {
                    arg_نمبر = c - DIGIT_ZERO;
                    جب (i < نمونہ_لمبائی &&
                            DIGIT_ZERO <= (c = نمونہ_بفر[i++]) && c <= DIGIT_NINE) {
                        arg_نمبر = arg_نمبر * 10 + (c - DIGIT_ZERO);
                        اگر (arg_نمبر >= ARG_عدد_حد_بندی) {
                            توڑو;
                        }
                    }
                }
                اگر (arg_نمبر < 0 || c != CLOSE_BRACE) {
                    error_رمز = U_ILLEGAL_دلیل_ERROR;
                    واپس جھوٹ;
                }
            }
            اگر (arg_نمبر > maxArg) {
                maxArg = arg_نمبر;
            }
            compiled_نمونہ.append(static_cast<char16_t>(arg_نمبر));
            جاری;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        اگر (متن_لمبائی == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiled_نمونہ.append(SEGMENT_لمبائی_PLACEHOLDER_CHAR);
        }
        compiled_نمونہ.append(c);
        اگر (++متن_لمبائی == MAX_SEGMENT_لمبائی) {
            متن_لمبائی = 0;
        }
    }
    اگر (متن_لمبائی > 0) {
        compiled_نمونہ.سیٹ_Char_At(compiled_نمونہ.length() - متن_لمبائی - 1,
                                  static_cast<char16_t>(ARG_عدد_حد_بندی + متن_لمبائی));
    }
    int32_t arg_گنتی = maxArg + 1;
    اگر (arg_گنتی < min || max < arg_گنتی) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس جھوٹ;
    }
    compiled_نمونہ.سیٹ_Char_At(0, static_cast<char16_t>(arg_گنتی));
    واپس سچ;
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        UnicodeString& append_مقصد U_LIFETIME_BOUND,
        UError_رمز& error_رمز) ثابت {
    ثابت UnicodeString *اقدار[] = { &value0 };
    واپس formatAndAppend(اقدار, 1, append_مقصد, خالی_اشاریہ, 0, error_رمز);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        UnicodeString& append_مقصد U_LIFETIME_BOUND,
        UError_رمز& error_رمز) ثابت {
    ثابت UnicodeString *اقدار[] = { &value0, &value1 };
    واپس formatAndAppend(اقدار, 2, append_مقصد, خالی_اشاریہ, 0, error_رمز);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        ثابت UnicodeString& value2,
        UnicodeString& append_مقصد U_LIFETIME_BOUND,
        UError_رمز& error_رمز) ثابت {
    ثابت UnicodeString *اقدار[] = { &value0, &value1, &value2 };
    واپس formatAndAppend(اقدار, 3, append_مقصد, خالی_اشاریہ, 0, error_رمز);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ثابت UnicodeString* ثابت* اقدار,
        int32_t اقدار_لمبائی,
        UnicodeString& append_مقصد U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_لمبائی,
        UError_رمز& error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) {
        واپس append_مقصد;
    }
    اگر (isInvalidArray(اقدار, اقدار_لمبائی) || isInvalidArray(offsets, offsets_لمبائی) ||
            اقدار_لمبائی < get_دلیل_حد_بندی()) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس append_مقصد;
    }
    واپس format(compiled_نمونہ.get_بفر(), compiled_نمونہ.length(), اقدار,
                  append_مقصد, خالی_اشاریہ, سچ,
                  offsets, offsets_لمبائی, error_رمز);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ثابت UnicodeString* ثابت* اقدار,
        int32_t اقدار_لمبائی,
        UnicodeString& نتیجہ U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_لمبائی,
        UError_رمز& error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) {
        واپس نتیجہ;
    }
    اگر (isInvalidArray(اقدار, اقدار_لمبائی) || isInvalidArray(offsets, offsets_لمبائی)) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس نتیجہ;
    }
    ثابت char16_t *cp = compiled_نمونہ.get_بفر();
    int32_t cp_لمبائی = compiled_نمونہ.length();
    اگر (اقدار_لمبائی < get_دلیل_حد_بندی(cp, cp_لمبائی)) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس نتیجہ;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t پہلا_Arg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString نتیجہ_Copy;
    اگر (get_دلیل_حد_بندی(cp, cp_لمبائی) > 0) {
        برائے (int32_t i = 1; i < cp_لمبائی;) {
            int32_t n = cp[i++];
            اگر (n < ARG_عدد_حد_بندی) {
                اگر (اقدار[n] == &نتیجہ) {
                    اگر (i == 2) {
                        پہلا_Arg = n;
                    } ورنہ اگر (نتیجہ_Copy.is_خالی_خانے() && !نتیجہ.is_خالی_خانے()) {
                        نتیجہ_Copy = نتیجہ;
                    }
                }
            } ورنہ {
                i += n - ARG_عدد_حد_بندی;
            }
        }
    }
    اگر (پہلا_Arg < 0) {
        نتیجہ.remove();
    }
    واپس format(cp, cp_لمبائی, اقدار,
                  نتیجہ, &نتیجہ_Copy, جھوٹ,
                  offsets, offsets_لمبائی, error_رمز);
}

UnicodeString SimpleFormatter::get_متن_With_No_Arguments(
        ثابت char16_t *compiled_نمونہ,
        int32_t compiled_نمونہ_لمبائی,
        int32_t* offsets,
        int32_t offsets_لمبائی) {
    برائے (int32_t i = 0; i < offsets_لمبائی; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiled_نمونہ_لمبائی - 1 -
            get_دلیل_حد_بندی(compiled_نمونہ, compiled_نمونہ_لمبائی);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    برائے (int32_t i = 1; i < compiled_نمونہ_لمبائی;) {
        int32_t n = compiled_نمونہ[i++];
        اگر (n > ARG_عدد_حد_بندی) {
            n -= ARG_عدد_حد_بندی;
            sb.append(compiled_نمونہ + i, n);
            i += n;
        } ورنہ اگر (n < offsets_لمبائی) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    واپس sb;
}

UnicodeString& SimpleFormatter::format(
        ثابت char16_t* compiled_نمونہ,
        int32_t compiled_نمونہ_لمبائی,
        ثابت UnicodeString* ثابت* اقدار,
        UnicodeString& نتیجہ U_LIFETIME_BOUND,
        ثابت UnicodeString* نتیجہ_Copy,
        UBool forbid_نتیجہ_As_قدر,
        int32_t* offsets,
        int32_t offsets_لمبائی,
        UError_رمز& error_رمز) {
    اگر (U_FAILURE(error_رمز)) {
        واپس نتیجہ;
    }
    برائے (int32_t i = 0; i < offsets_لمبائی; i++) {
        offsets[i] = -1;
    }
    برائے (int32_t i = 1; i < compiled_نمونہ_لمبائی;) {
        int32_t n = compiled_نمونہ[i++];
        اگر (n < ARG_عدد_حد_بندی) {
            ثابت UnicodeString *value = اقدار[n];
            اگر (value == خالی_اشاریہ) {
                error_رمز = U_ILLEGAL_دلیل_ERROR;
                واپس نتیجہ;
            }
            اگر (value == &نتیجہ) {
                اگر (forbid_نتیجہ_As_قدر) {
                    error_رمز = U_ILLEGAL_دلیل_ERROR;
                    واپس نتیجہ;
                }
                اگر (i == 2) {
                    // We are appending to result which is also the first value object.
                    اگر (n < offsets_لمبائی) {
                        offsets[n] = 0;
                    }
                } ورنہ {
                    اگر (n < offsets_لمبائی) {
                        offsets[n] = نتیجہ.length();
                    }
                    نتیجہ.append(*نتیجہ_Copy);
                }
            } ورنہ {
                اگر (n < offsets_لمبائی) {
                    offsets[n] = نتیجہ.length();
                }
                نتیجہ.append(*value);
            }
        } ورنہ {
            int32_t length = n - ARG_عدد_حد_بندی;
            نتیجہ.append(compiled_نمونہ + i, length);
            i += length;
        }
    }
    واپس نتیجہ;
}

U_NAMESPACE_انتہا
