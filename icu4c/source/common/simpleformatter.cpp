// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/simpleformatter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

U_NAMESPACE_BEGIN

ਨਾਮਾਕਾਸ਼ {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
ਅਚਲ int32_t ARG_ਅੰਕ_ਹੱਦਬੰਦੀ = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ਅਚਲ char16_t SEGMENT_ਲੰਮਾਈ_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ਅਚਲ int32_t MAX_SEGMENT_ਲੰਮਾਈ = SEGMENT_ਲੰਮਾਈ_PLACEHOLDER_CHAR - ARG_ਅੰਕ_ਹੱਦਬੰਦੀ;

ਗਿਣਤੀ {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

ਸਤਰਅੰਦਰ UBool isInvalidArray(ਅਚਲ ਖਾਲੀ *array, int32_t length) {
   ਵਾਪਸ (length < 0 || (array == ਖਾਲੀ_ਸੂਚਕ && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::ਸੰਚਾਲਕ=(ਅਚਲ SimpleFormatter& other) U_LIFETIME_BOUND {
    ਜੇ (ਇਹ == &other) {
        ਵਾਪਸ *ਇਹ;
    }
    compiled_ਨਮੂਨਾ = other.compiled_ਨਮੂਨਾ;
    ਵਾਪਸ *ਇਹ;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::apply_ਨਮੂਨਾ_Min_Max_Arguments(
        ਅਚਲ UnicodeString &ਨਮੂਨਾ,
        int32_t min, int32_t max,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ਅਚਲ char16_t *ਨਮੂਨਾ_ਬਫਰ = ਨਮੂਨਾ.get_ਬਫਰ();
    int32_t ਨਮੂਨਾ_ਲੰਮਾਈ = ਨਮੂਨਾ.length();
    // Reserve the first char for the number of arguments.
    compiled_ਨਮੂਨਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(static_cast<char16_t>(0));
    int32_t ਪਾਠ_ਲੰਮਾਈ = 0;
    int32_t maxArg = -1;
    UBool ਵਿੱਚ_Quote = ਝੂਠ;
    ਲਈ (int32_t i = 0; i < ਨਮੂਨਾ_ਲੰਮਾਈ;) {
        char16_t c = ਨਮੂਨਾ_ਬਫਰ[i++];
        ਜੇ (c == APOS) {
            ਜੇ (i < ਨਮੂਨਾ_ਲੰਮਾਈ && (c = ਨਮੂਨਾ_ਬਫਰ[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_Quote) {
                // skip the quote-ending apostrophe
                ਵਿੱਚ_Quote = ਝੂਠ;
                ਜਾਰੀ;
            } ਨਹੀਂਤਾਂ ਜੇ (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                ਵਿੱਚ_Quote = ਸੱਚ;
            } ਨਹੀਂਤਾਂ {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } ਨਹੀਂਤਾਂ ਜੇ (!ਵਿੱਚ_Quote && c == OPEN_BRACE) {
            ਜੇ (ਪਾਠ_ਲੰਮਾਈ > 0) {
                compiled_ਨਮੂਨਾ.ਸੈੱਟ_Char_At(compiled_ਨਮੂਨਾ.length() - ਪਾਠ_ਲੰਮਾਈ - 1,
                                          static_cast<char16_t>(ARG_ਅੰਕ_ਹੱਦਬੰਦੀ + ਪਾਠ_ਲੰਮਾਈ));
                ਪਾਠ_ਲੰਮਾਈ = 0;
            }
            int32_t arg_ਸੰਖਿਆ;
            ਜੇ ((i + 1) < ਨਮੂਨਾ_ਲੰਮਾਈ &&
                    0 <= (arg_ਸੰਖਿਆ = ਨਮੂਨਾ_ਬਫਰ[i] - DIGIT_ZERO) && arg_ਸੰਖਿਆ <= 9 &&
                    ਨਮੂਨਾ_ਬਫਰ[i + 1] == CLOSE_BRACE) {
                i += 2;
            } ਨਹੀਂਤਾਂ {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_ਸੰਖਿਆ = -1;
                ਜੇ (i < ਨਮੂਨਾ_ਲੰਮਾਈ && DIGIT_ONE <= (c = ਨਮੂਨਾ_ਬਫਰ[i++]) && c <= DIGIT_NINE) {
                    arg_ਸੰਖਿਆ = c - DIGIT_ZERO;
                    ਜਦੋਂ (i < ਨਮੂਨਾ_ਲੰਮਾਈ &&
                            DIGIT_ZERO <= (c = ਨਮੂਨਾ_ਬਫਰ[i++]) && c <= DIGIT_NINE) {
                        arg_ਸੰਖਿਆ = arg_ਸੰਖਿਆ * 10 + (c - DIGIT_ZERO);
                        ਜੇ (arg_ਸੰਖਿਆ >= ARG_ਅੰਕ_ਹੱਦਬੰਦੀ) {
                            ਰੋਕੋ;
                        }
                    }
                }
                ਜੇ (arg_ਸੰਖਿਆ < 0 || c != CLOSE_BRACE) {
                    error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                    ਵਾਪਸ ਝੂਠ;
                }
            }
            ਜੇ (arg_ਸੰਖਿਆ > maxArg) {
                maxArg = arg_ਸੰਖਿਆ;
            }
            compiled_ਨਮੂਨਾ.append(static_cast<char16_t>(arg_ਸੰਖਿਆ));
            ਜਾਰੀ;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        ਜੇ (ਪਾਠ_ਲੰਮਾਈ == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiled_ਨਮੂਨਾ.append(SEGMENT_ਲੰਮਾਈ_PLACEHOLDER_CHAR);
        }
        compiled_ਨਮੂਨਾ.append(c);
        ਜੇ (++ਪਾਠ_ਲੰਮਾਈ == MAX_SEGMENT_ਲੰਮਾਈ) {
            ਪਾਠ_ਲੰਮਾਈ = 0;
        }
    }
    ਜੇ (ਪਾਠ_ਲੰਮਾਈ > 0) {
        compiled_ਨਮੂਨਾ.ਸੈੱਟ_Char_At(compiled_ਨਮੂਨਾ.length() - ਪਾਠ_ਲੰਮਾਈ - 1,
                                  static_cast<char16_t>(ARG_ਅੰਕ_ਹੱਦਬੰਦੀ + ਪਾਠ_ਲੰਮਾਈ));
    }
    int32_t arg_ਗਿਣਤੀ = maxArg + 1;
    ਜੇ (arg_ਗਿਣਤੀ < min || max < arg_ਗਿਣਤੀ) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    compiled_ਨਮੂਨਾ.ਸੈੱਟ_Char_At(0, static_cast<char16_t>(arg_ਗਿਣਤੀ));
    ਵਾਪਸ ਸੱਚ;
}

UnicodeString& SimpleFormatter::format(
        ਅਚਲ UnicodeString& value0,
        UnicodeString& append_ਮੰਜ਼ਿਲ U_LIFETIME_BOUND,
        UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਅਚਲ UnicodeString *ਮੁੱਲਾਂ[] = { &value0 };
    ਵਾਪਸ formatAndAppend(ਮੁੱਲਾਂ, 1, append_ਮੰਜ਼ਿਲ, ਖਾਲੀ_ਸੂਚਕ, 0, error_ਕੋਡ);
}

UnicodeString& SimpleFormatter::format(
        ਅਚਲ UnicodeString& value0,
        ਅਚਲ UnicodeString& value1,
        UnicodeString& append_ਮੰਜ਼ਿਲ U_LIFETIME_BOUND,
        UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਅਚਲ UnicodeString *ਮੁੱਲਾਂ[] = { &value0, &value1 };
    ਵਾਪਸ formatAndAppend(ਮੁੱਲਾਂ, 2, append_ਮੰਜ਼ਿਲ, ਖਾਲੀ_ਸੂਚਕ, 0, error_ਕੋਡ);
}

UnicodeString& SimpleFormatter::format(
        ਅਚਲ UnicodeString& value0,
        ਅਚਲ UnicodeString& value1,
        ਅਚਲ UnicodeString& value2,
        UnicodeString& append_ਮੰਜ਼ਿਲ U_LIFETIME_BOUND,
        UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਅਚਲ UnicodeString *ਮੁੱਲਾਂ[] = { &value0, &value1, &value2 };
    ਵਾਪਸ formatAndAppend(ਮੁੱਲਾਂ, 3, append_ਮੰਜ਼ਿਲ, ਖਾਲੀ_ਸੂਚਕ, 0, error_ਕੋਡ);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ਅਚਲ UnicodeString* ਅਚਲ* ਮੁੱਲਾਂ,
        int32_t ਮੁੱਲਾਂ_ਲੰਮਾਈ,
        UnicodeString& append_ਮੰਜ਼ਿਲ U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_ਲੰਮਾਈ,
        UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ append_ਮੰਜ਼ਿਲ;
    }
    ਜੇ (isInvalidArray(ਮੁੱਲਾਂ, ਮੁੱਲਾਂ_ਲੰਮਾਈ) || isInvalidArray(offsets, offsets_ਲੰਮਾਈ) ||
            ਮੁੱਲਾਂ_ਲੰਮਾਈ < get_ਦਲੀਲ_ਹੱਦਬੰਦੀ()) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ append_ਮੰਜ਼ਿਲ;
    }
    ਵਾਪਸ format(compiled_ਨਮੂਨਾ.get_ਬਫਰ(), compiled_ਨਮੂਨਾ.length(), ਮੁੱਲਾਂ,
                  append_ਮੰਜ਼ਿਲ, ਖਾਲੀ_ਸੂਚਕ, ਸੱਚ,
                  offsets, offsets_ਲੰਮਾਈ, error_ਕੋਡ);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ਅਚਲ UnicodeString* ਅਚਲ* ਮੁੱਲਾਂ,
        int32_t ਮੁੱਲਾਂ_ਲੰਮਾਈ,
        UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_ਲੰਮਾਈ,
        UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਜੇ (isInvalidArray(ਮੁੱਲਾਂ, ਮੁੱਲਾਂ_ਲੰਮਾਈ) || isInvalidArray(offsets, offsets_ਲੰਮਾਈ)) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਅਚਲ char16_t *cp = compiled_ਨਮੂਨਾ.get_ਬਫਰ();
    int32_t cp_ਲੰਮਾਈ = compiled_ਨਮੂਨਾ.length();
    ਜੇ (ਮੁੱਲਾਂ_ਲੰਮਾਈ < get_ਦਲੀਲ_ਹੱਦਬੰਦੀ(cp, cp_ਲੰਮਾਈ)) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t ਪਹਿਲਾ_Arg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString ਨਤੀਜਾ_Copy;
    ਜੇ (get_ਦਲੀਲ_ਹੱਦਬੰਦੀ(cp, cp_ਲੰਮਾਈ) > 0) {
        ਲਈ (int32_t i = 1; i < cp_ਲੰਮਾਈ;) {
            int32_t n = cp[i++];
            ਜੇ (n < ARG_ਅੰਕ_ਹੱਦਬੰਦੀ) {
                ਜੇ (ਮੁੱਲਾਂ[n] == &ਨਤੀਜਾ) {
                    ਜੇ (i == 2) {
                        ਪਹਿਲਾ_Arg = n;
                    } ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ_Copy.is_ਖਾਲੀ_ਥਾਵਾਂ() && !ਨਤੀਜਾ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                        ਨਤੀਜਾ_Copy = ਨਤੀਜਾ;
                    }
                }
            } ਨਹੀਂਤਾਂ {
                i += n - ARG_ਅੰਕ_ਹੱਦਬੰਦੀ;
            }
        }
    }
    ਜੇ (ਪਹਿਲਾ_Arg < 0) {
        ਨਤੀਜਾ.remove();
    }
    ਵਾਪਸ format(cp, cp_ਲੰਮਾਈ, ਮੁੱਲਾਂ,
                  ਨਤੀਜਾ, &ਨਤੀਜਾ_Copy, ਝੂਠ,
                  offsets, offsets_ਲੰਮਾਈ, error_ਕੋਡ);
}

UnicodeString SimpleFormatter::get_ਪਾਠ_With_No_Arguments(
        ਅਚਲ char16_t *compiled_ਨਮੂਨਾ,
        int32_t compiled_ਨਮੂਨਾ_ਲੰਮਾਈ,
        int32_t* offsets,
        int32_t offsets_ਲੰਮਾਈ) {
    ਲਈ (int32_t i = 0; i < offsets_ਲੰਮਾਈ; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiled_ਨਮੂਨਾ_ਲੰਮਾਈ - 1 -
            get_ਦਲੀਲ_ਹੱਦਬੰਦੀ(compiled_ਨਮੂਨਾ, compiled_ਨਮੂਨਾ_ਲੰਮਾਈ);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    ਲਈ (int32_t i = 1; i < compiled_ਨਮੂਨਾ_ਲੰਮਾਈ;) {
        int32_t n = compiled_ਨਮੂਨਾ[i++];
        ਜੇ (n > ARG_ਅੰਕ_ਹੱਦਬੰਦੀ) {
            n -= ARG_ਅੰਕ_ਹੱਦਬੰਦੀ;
            sb.append(compiled_ਨਮੂਨਾ + i, n);
            i += n;
        } ਨਹੀਂਤਾਂ ਜੇ (n < offsets_ਲੰਮਾਈ) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    ਵਾਪਸ sb;
}

UnicodeString& SimpleFormatter::format(
        ਅਚਲ char16_t* compiled_ਨਮੂਨਾ,
        int32_t compiled_ਨਮੂਨਾ_ਲੰਮਾਈ,
        ਅਚਲ UnicodeString* ਅਚਲ* ਮੁੱਲਾਂ,
        UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
        ਅਚਲ UnicodeString* ਨਤੀਜਾ_Copy,
        UBool forbid_ਨਤੀਜਾ_As_ਮੁੱਲ,
        int32_t* offsets,
        int32_t offsets_ਲੰਮਾਈ,
        UError_ਕੋਡ& error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਲਈ (int32_t i = 0; i < offsets_ਲੰਮਾਈ; i++) {
        offsets[i] = -1;
    }
    ਲਈ (int32_t i = 1; i < compiled_ਨਮੂਨਾ_ਲੰਮਾਈ;) {
        int32_t n = compiled_ਨਮੂਨਾ[i++];
        ਜੇ (n < ARG_ਅੰਕ_ਹੱਦਬੰਦੀ) {
            ਅਚਲ UnicodeString *value = ਮੁੱਲਾਂ[n];
            ਜੇ (value == ਖਾਲੀ_ਸੂਚਕ) {
                error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                ਵਾਪਸ ਨਤੀਜਾ;
            }
            ਜੇ (value == &ਨਤੀਜਾ) {
                ਜੇ (forbid_ਨਤੀਜਾ_As_ਮੁੱਲ) {
                    error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                    ਵਾਪਸ ਨਤੀਜਾ;
                }
                ਜੇ (i == 2) {
                    // We are appending to result which is also the first value object.
                    ਜੇ (n < offsets_ਲੰਮਾਈ) {
                        offsets[n] = 0;
                    }
                } ਨਹੀਂਤਾਂ {
                    ਜੇ (n < offsets_ਲੰਮਾਈ) {
                        offsets[n] = ਨਤੀਜਾ.length();
                    }
                    ਨਤੀਜਾ.append(*ਨਤੀਜਾ_Copy);
                }
            } ਨਹੀਂਤਾਂ {
                ਜੇ (n < offsets_ਲੰਮਾਈ) {
                    offsets[n] = ਨਤੀਜਾ.length();
                }
                ਨਤੀਜਾ.append(*value);
            }
        } ਨਹੀਂਤਾਂ {
            int32_t length = n - ARG_ਅੰਕ_ਹੱਦਬੰਦੀ;
            ਨਤੀਜਾ.append(compiled_ਨਮੂਨਾ + i, length);
            i += length;
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

U_NAMESPACE_ਸਿਰਾ
