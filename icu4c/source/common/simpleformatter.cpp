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
ثابت int32_t ARG_عدد_حد = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ثابت char16_t SEGMENT_اوږدتیا_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ثابت int32_t MAX_SEGMENT_اوږدتیا = SEGMENT_اوږدتیا_PLACEHOLDER_CHAR - ARG_عدد_حد;

تعداد {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

مضمن UBool isInvalidArray(ثابت فراغ *array, int32_t length) {
   أرجع (length < 0 || (array == تش_اشاره && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::چلوونکی=(ثابت SimpleFormatter& other) U_LIFETIME_BOUND {
    إذا (هذا == &other) {
        أرجع *هذا;
    }
    compiled_نمونه = other.compiled_نمونه;
    أرجع *هذا;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::apply_نمونه_Min_Max_Arguments(
        ثابت UnicodeString &نمونه,
        int32_t min, int32_t max,
        UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع خطأ;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ثابت char16_t *نمونه_بفر = نمونه.get_بفر();
    int32_t نمونه_اوږدتیا = نمونه.length();
    // Reserve the first char for the number of arguments.
    compiled_نمونه.سېټ_منزل(static_cast<char16_t>(0));
    int32_t متن_اوږدتیا = 0;
    int32_t maxArg = -1;
    UBool دننه_Quote = خطأ;
    لكل (int32_t i = 0; i < نمونه_اوږدتیا;) {
        char16_t c = نمونه_بفر[i++];
        إذا (c == APOS) {
            إذا (i < نمونه_اوږدتیا && (c = نمونه_بفر[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } وإلا إذا (دننه_Quote) {
                // skip the quote-ending apostrophe
                دننه_Quote = خطأ;
                استمر;
            } وإلا إذا (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                دننه_Quote = ریښتیا;
            } وإلا {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } وإلا إذا (!دننه_Quote && c == OPEN_BRACE) {
            إذا (متن_اوږدتیا > 0) {
                compiled_نمونه.سېټ_Char_At(compiled_نمونه.length() - متن_اوږدتیا - 1,
                                          static_cast<char16_t>(ARG_عدد_حد + متن_اوږدتیا));
                متن_اوږدتیا = 0;
            }
            int32_t arg_شمېره;
            إذا ((i + 1) < نمونه_اوږدتیا &&
                    0 <= (arg_شمېره = نمونه_بفر[i] - DIGIT_ZERO) && arg_شمېره <= 9 &&
                    نمونه_بفر[i + 1] == CLOSE_BRACE) {
                i += 2;
            } وإلا {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_شمېره = -1;
                إذا (i < نمونه_اوږدتیا && DIGIT_ONE <= (c = نمونه_بفر[i++]) && c <= DIGIT_NINE) {
                    arg_شمېره = c - DIGIT_ZERO;
                    طالما (i < نمونه_اوږدتیا &&
                            DIGIT_ZERO <= (c = نمونه_بفر[i++]) && c <= DIGIT_NINE) {
                        arg_شمېره = arg_شمېره * 10 + (c - DIGIT_ZERO);
                        إذا (arg_شمېره >= ARG_عدد_حد) {
                            اكسر;
                        }
                    }
                }
                إذا (arg_شمېره < 0 || c != CLOSE_BRACE) {
                    error_کوډ = U_ILLEGAL_دلیل_ERROR;
                    أرجع خطأ;
                }
            }
            إذا (arg_شمېره > maxArg) {
                maxArg = arg_شمېره;
            }
            compiled_نمونه.append(static_cast<char16_t>(arg_شمېره));
            استمر;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        إذا (متن_اوږدتیا == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiled_نمونه.append(SEGMENT_اوږدتیا_PLACEHOLDER_CHAR);
        }
        compiled_نمونه.append(c);
        إذا (++متن_اوږدتیا == MAX_SEGMENT_اوږدتیا) {
            متن_اوږدتیا = 0;
        }
    }
    إذا (متن_اوږدتیا > 0) {
        compiled_نمونه.سېټ_Char_At(compiled_نمونه.length() - متن_اوږدتیا - 1,
                                  static_cast<char16_t>(ARG_عدد_حد + متن_اوږدتیا));
    }
    int32_t arg_شمېر = maxArg + 1;
    إذا (arg_شمېر < min || max < arg_شمېر) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    compiled_نمونه.سېټ_Char_At(0, static_cast<char16_t>(arg_شمېر));
    أرجع ریښتیا;
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        UError_کوډ& error_کوډ) ثابت {
    ثابت UnicodeString *ارزښتونه[] = { &value0 };
    أرجع formatAndAppend(ارزښتونه, 1, append_منزل, تش_اشاره, 0, error_کوډ);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        UError_کوډ& error_کوډ) ثابت {
    ثابت UnicodeString *ارزښتونه[] = { &value0, &value1 };
    أرجع formatAndAppend(ارزښتونه, 2, append_منزل, تش_اشاره, 0, error_کوډ);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        ثابت UnicodeString& value2,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        UError_کوډ& error_کوډ) ثابت {
    ثابت UnicodeString *ارزښتونه[] = { &value0, &value1, &value2 };
    أرجع formatAndAppend(ارزښتونه, 3, append_منزل, تش_اشاره, 0, error_کوډ);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ثابت UnicodeString* ثابت* ارزښتونه,
        int32_t ارزښتونه_اوږدتیا,
        UnicodeString& append_منزل U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_اوږدتیا,
        UError_کوډ& error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع append_منزل;
    }
    إذا (isInvalidArray(ارزښتونه, ارزښتونه_اوږدتیا) || isInvalidArray(offsets, offsets_اوږدتیا) ||
            ارزښتونه_اوږدتیا < get_دلیل_حد()) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع append_منزل;
    }
    أرجع format(compiled_نمونه.get_بفر(), compiled_نمونه.length(), ارزښتونه,
                  append_منزل, تش_اشاره, ریښتیا,
                  offsets, offsets_اوږدتیا, error_کوډ);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ثابت UnicodeString* ثابت* ارزښتونه,
        int32_t ارزښتونه_اوږدتیا,
        UnicodeString& پایله U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_اوږدتیا,
        UError_کوډ& error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع پایله;
    }
    إذا (isInvalidArray(ارزښتونه, ارزښتونه_اوږدتیا) || isInvalidArray(offsets, offsets_اوږدتیا)) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع پایله;
    }
    ثابت char16_t *cp = compiled_نمونه.get_بفر();
    int32_t cp_اوږدتیا = compiled_نمونه.length();
    إذا (ارزښتونه_اوږدتیا < get_دلیل_حد(cp, cp_اوږدتیا)) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع پایله;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t لومړی_Arg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString پایله_Copy;
    إذا (get_دلیل_حد(cp, cp_اوږدتیا) > 0) {
        لكل (int32_t i = 1; i < cp_اوږدتیا;) {
            int32_t n = cp[i++];
            إذا (n < ARG_عدد_حد) {
                إذا (ارزښتونه[n] == &پایله) {
                    إذا (i == 2) {
                        لومړی_Arg = n;
                    } وإلا إذا (پایله_Copy.is_تشه() && !پایله.is_تشه()) {
                        پایله_Copy = پایله;
                    }
                }
            } وإلا {
                i += n - ARG_عدد_حد;
            }
        }
    }
    إذا (لومړی_Arg < 0) {
        پایله.remove();
    }
    أرجع format(cp, cp_اوږدتیا, ارزښتونه,
                  پایله, &پایله_Copy, خطأ,
                  offsets, offsets_اوږدتیا, error_کوډ);
}

UnicodeString SimpleFormatter::get_متن_With_No_Arguments(
        ثابت char16_t *compiled_نمونه,
        int32_t compiled_نمونه_اوږدتیا,
        int32_t* offsets,
        int32_t offsets_اوږدتیا) {
    لكل (int32_t i = 0; i < offsets_اوږدتیا; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiled_نمونه_اوږدتیا - 1 -
            get_دلیل_حد(compiled_نمونه, compiled_نمونه_اوږدتیا);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    لكل (int32_t i = 1; i < compiled_نمونه_اوږدتیا;) {
        int32_t n = compiled_نمونه[i++];
        إذا (n > ARG_عدد_حد) {
            n -= ARG_عدد_حد;
            sb.append(compiled_نمونه + i, n);
            i += n;
        } وإلا إذا (n < offsets_اوږدتیا) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    أرجع sb;
}

UnicodeString& SimpleFormatter::format(
        ثابت char16_t* compiled_نمونه,
        int32_t compiled_نمونه_اوږدتیا,
        ثابت UnicodeString* ثابت* ارزښتونه,
        UnicodeString& پایله U_LIFETIME_BOUND,
        ثابت UnicodeString* پایله_Copy,
        UBool forbid_پایله_As_ارزښت,
        int32_t* offsets,
        int32_t offsets_اوږدتیا,
        UError_کوډ& error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع پایله;
    }
    لكل (int32_t i = 0; i < offsets_اوږدتیا; i++) {
        offsets[i] = -1;
    }
    لكل (int32_t i = 1; i < compiled_نمونه_اوږدتیا;) {
        int32_t n = compiled_نمونه[i++];
        إذا (n < ARG_عدد_حد) {
            ثابت UnicodeString *value = ارزښتونه[n];
            إذا (value == تش_اشاره) {
                error_کوډ = U_ILLEGAL_دلیل_ERROR;
                أرجع پایله;
            }
            إذا (value == &پایله) {
                إذا (forbid_پایله_As_ارزښت) {
                    error_کوډ = U_ILLEGAL_دلیل_ERROR;
                    أرجع پایله;
                }
                إذا (i == 2) {
                    // We are appending to result which is also the first value object.
                    إذا (n < offsets_اوږدتیا) {
                        offsets[n] = 0;
                    }
                } وإلا {
                    إذا (n < offsets_اوږدتیا) {
                        offsets[n] = پایله.length();
                    }
                    پایله.append(*پایله_Copy);
                }
            } وإلا {
                إذا (n < offsets_اوږدتیا) {
                    offsets[n] = پایله.length();
                }
                پایله.append(*value);
            }
        } وإلا {
            int32_t length = n - ARG_عدد_حد;
            پایله.append(compiled_نمونه + i, length);
            i += length;
        }
    }
    أرجع پایله;
}

U_NAMESPACE_پای_ټکی
