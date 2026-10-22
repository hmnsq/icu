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
ثابت int32_t ARG_رقم_حد = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ثابت char16_t SEGMENT_درازی_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ثابت int32_t MAX_SEGMENT_درازی = SEGMENT_درازی_PLACEHOLDER_CHAR - ARG_رقم_حد;

تعداد {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

مضمن UBool isInvalidArray(ثابت فراغ *array, int32_t length) {
   أرجع (length < 0 || (array == اشاره_تهی && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::عملگر=(ثابت SimpleFormatter& other) U_LIFETIME_BOUND {
    إذا (هذا == &other) {
        أرجع *هذا;
    }
    compiled_الگو = other.compiled_الگو;
    أرجع *هذا;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::apply_الگو_Min_Max_Arguments(
        ثابت UnicodeString &الگو,
        int32_t min, int32_t max,
        UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) {
        أرجع خطأ;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ثابت char16_t *الگو_میانگیر = الگو.get_میانگیر();
    int32_t الگو_درازی = الگو.length();
    // Reserve the first char for the number of arguments.
    compiled_الگو.مجموعه_مقصد(static_cast<char16_t>(0));
    int32_t متن_درازی = 0;
    int32_t maxArg = -1;
    UBool درون_Quote = خطأ;
    لكل (int32_t i = 0; i < الگو_درازی;) {
        char16_t c = الگو_میانگیر[i++];
        إذا (c == APOS) {
            إذا (i < الگو_درازی && (c = الگو_میانگیر[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } وإلا إذا (درون_Quote) {
                // skip the quote-ending apostrophe
                درون_Quote = خطأ;
                استمر;
            } وإلا إذا (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                درون_Quote = درست;
            } وإلا {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } وإلا إذا (!درون_Quote && c == OPEN_BRACE) {
            إذا (متن_درازی > 0) {
                compiled_الگو.مجموعه_Char_At(compiled_الگو.length() - متن_درازی - 1,
                                          static_cast<char16_t>(ARG_رقم_حد + متن_درازی));
                متن_درازی = 0;
            }
            int32_t arg_عدد;
            إذا ((i + 1) < الگو_درازی &&
                    0 <= (arg_عدد = الگو_میانگیر[i] - DIGIT_ZERO) && arg_عدد <= 9 &&
                    الگو_میانگیر[i + 1] == CLOSE_BRACE) {
                i += 2;
            } وإلا {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_عدد = -1;
                إذا (i < الگو_درازی && DIGIT_ONE <= (c = الگو_میانگیر[i++]) && c <= DIGIT_NINE) {
                    arg_عدد = c - DIGIT_ZERO;
                    طالما (i < الگو_درازی &&
                            DIGIT_ZERO <= (c = الگو_میانگیر[i++]) && c <= DIGIT_NINE) {
                        arg_عدد = arg_عدد * 10 + (c - DIGIT_ZERO);
                        إذا (arg_عدد >= ARG_رقم_حد) {
                            اكسر;
                        }
                    }
                }
                إذا (arg_عدد < 0 || c != CLOSE_BRACE) {
                    error_کد = U_ILLEGAL_آرگومان_ERROR;
                    أرجع خطأ;
                }
            }
            إذا (arg_عدد > maxArg) {
                maxArg = arg_عدد;
            }
            compiled_الگو.append(static_cast<char16_t>(arg_عدد));
            استمر;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        إذا (متن_درازی == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiled_الگو.append(SEGMENT_درازی_PLACEHOLDER_CHAR);
        }
        compiled_الگو.append(c);
        إذا (++متن_درازی == MAX_SEGMENT_درازی) {
            متن_درازی = 0;
        }
    }
    إذا (متن_درازی > 0) {
        compiled_الگو.مجموعه_Char_At(compiled_الگو.length() - متن_درازی - 1,
                                  static_cast<char16_t>(ARG_رقم_حد + متن_درازی));
    }
    int32_t arg_شمار = maxArg + 1;
    إذا (arg_شمار < min || max < arg_شمار) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع خطأ;
    }
    compiled_الگو.مجموعه_Char_At(0, static_cast<char16_t>(arg_شمار));
    أرجع درست;
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        UnicodeString& append_مقصد U_LIFETIME_BOUND,
        UError_کد& error_کد) ثابت {
    ثابت UnicodeString *مقادیر[] = { &value0 };
    أرجع formatAndAppend(مقادیر, 1, append_مقصد, اشاره_تهی, 0, error_کد);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        UnicodeString& append_مقصد U_LIFETIME_BOUND,
        UError_کد& error_کد) ثابت {
    ثابت UnicodeString *مقادیر[] = { &value0, &value1 };
    أرجع formatAndAppend(مقادیر, 2, append_مقصد, اشاره_تهی, 0, error_کد);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        ثابت UnicodeString& value2,
        UnicodeString& append_مقصد U_LIFETIME_BOUND,
        UError_کد& error_کد) ثابت {
    ثابت UnicodeString *مقادیر[] = { &value0, &value1, &value2 };
    أرجع formatAndAppend(مقادیر, 3, append_مقصد, اشاره_تهی, 0, error_کد);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ثابت UnicodeString* ثابت* مقادیر,
        int32_t مقادیر_درازی,
        UnicodeString& append_مقصد U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_درازی,
        UError_کد& error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) {
        أرجع append_مقصد;
    }
    إذا (isInvalidArray(مقادیر, مقادیر_درازی) || isInvalidArray(offsets, offsets_درازی) ||
            مقادیر_درازی < get_آرگومان_حد()) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع append_مقصد;
    }
    أرجع format(compiled_الگو.get_میانگیر(), compiled_الگو.length(), مقادیر,
                  append_مقصد, اشاره_تهی, درست,
                  offsets, offsets_درازی, error_کد);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ثابت UnicodeString* ثابت* مقادیر,
        int32_t مقادیر_درازی,
        UnicodeString& نتیجه U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_درازی,
        UError_کد& error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) {
        أرجع نتیجه;
    }
    إذا (isInvalidArray(مقادیر, مقادیر_درازی) || isInvalidArray(offsets, offsets_درازی)) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع نتیجه;
    }
    ثابت char16_t *cp = compiled_الگو.get_میانگیر();
    int32_t cp_درازی = compiled_الگو.length();
    إذا (مقادیر_درازی < get_آرگومان_حد(cp, cp_درازی)) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع نتیجه;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t اولی_Arg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString نتیجه_Copy;
    إذا (get_آرگومان_حد(cp, cp_درازی) > 0) {
        لكل (int32_t i = 1; i < cp_درازی;) {
            int32_t n = cp[i++];
            إذا (n < ARG_رقم_حد) {
                إذا (مقادیر[n] == &نتیجه) {
                    إذا (i == 2) {
                        اولی_Arg = n;
                    } وإلا إذا (نتیجه_Copy.is_خالی() && !نتیجه.is_خالی()) {
                        نتیجه_Copy = نتیجه;
                    }
                }
            } وإلا {
                i += n - ARG_رقم_حد;
            }
        }
    }
    إذا (اولی_Arg < 0) {
        نتیجه.remove();
    }
    أرجع format(cp, cp_درازی, مقادیر,
                  نتیجه, &نتیجه_Copy, خطأ,
                  offsets, offsets_درازی, error_کد);
}

UnicodeString SimpleFormatter::get_متن_With_No_Arguments(
        ثابت char16_t *compiled_الگو,
        int32_t compiled_الگو_درازی,
        int32_t* offsets,
        int32_t offsets_درازی) {
    لكل (int32_t i = 0; i < offsets_درازی; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiled_الگو_درازی - 1 -
            get_آرگومان_حد(compiled_الگو, compiled_الگو_درازی);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    لكل (int32_t i = 1; i < compiled_الگو_درازی;) {
        int32_t n = compiled_الگو[i++];
        إذا (n > ARG_رقم_حد) {
            n -= ARG_رقم_حد;
            sb.append(compiled_الگو + i, n);
            i += n;
        } وإلا إذا (n < offsets_درازی) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    أرجع sb;
}

UnicodeString& SimpleFormatter::format(
        ثابت char16_t* compiled_الگو,
        int32_t compiled_الگو_درازی,
        ثابت UnicodeString* ثابت* مقادیر,
        UnicodeString& نتیجه U_LIFETIME_BOUND,
        ثابت UnicodeString* نتیجه_Copy,
        UBool forbid_نتیجه_As_مقدار,
        int32_t* offsets,
        int32_t offsets_درازی,
        UError_کد& error_کد) {
    إذا (U_FAILURE(error_کد)) {
        أرجع نتیجه;
    }
    لكل (int32_t i = 0; i < offsets_درازی; i++) {
        offsets[i] = -1;
    }
    لكل (int32_t i = 1; i < compiled_الگو_درازی;) {
        int32_t n = compiled_الگو[i++];
        إذا (n < ARG_رقم_حد) {
            ثابت UnicodeString *value = مقادیر[n];
            إذا (value == اشاره_تهی) {
                error_کد = U_ILLEGAL_آرگومان_ERROR;
                أرجع نتیجه;
            }
            إذا (value == &نتیجه) {
                إذا (forbid_نتیجه_As_مقدار) {
                    error_کد = U_ILLEGAL_آرگومان_ERROR;
                    أرجع نتیجه;
                }
                إذا (i == 2) {
                    // We are appending to result which is also the first value object.
                    إذا (n < offsets_درازی) {
                        offsets[n] = 0;
                    }
                } وإلا {
                    إذا (n < offsets_درازی) {
                        offsets[n] = نتیجه.length();
                    }
                    نتیجه.append(*نتیجه_Copy);
                }
            } وإلا {
                إذا (n < offsets_درازی) {
                    offsets[n] = نتیجه.length();
                }
                نتیجه.append(*value);
            }
        } وإلا {
            int32_t length = n - ARG_رقم_حد;
            نتیجه.append(compiled_الگو + i, length);
            i += length;
        }
    }
    أرجع نتیجه;
}

U_NAMESPACE_انتها
