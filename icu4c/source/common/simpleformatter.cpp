// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/simpleformatter.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
ಸ್ಥಿರಾಂಕ int32_t ARG_ಅಂಕೆ_ಎಲ್ಲೆ = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ಸ್ಥಿರಾಂಕ char16_t SEGMENT_ಉದ್ದಳತೆ_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ಸ್ಥಿರಾಂಕ int32_t MAX_SEGMENT_ಉದ್ದಳತೆ = SEGMENT_ಉದ್ದಳತೆ_PLACEHOLDER_CHAR - ARG_ಅಂಕೆ_ಎಲ್ಲೆ;

ಎಣಿಕೆ {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

ಸಾಲೊಳಗೆ UBool isInvalidArray(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *array, int32_t length) {
   ಹಿಂತಿರುಗಿ (length < 0 || (array == ಶೂನ್ಯಸೂಚಕ && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ SimpleFormatter& other) U_LIFETIME_BOUND {
    ಆದರೆ (ಇದು == &other) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    compiled_ನಮೂನೆ = other.compiled_ನಮೂನೆ;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::apply_ನಮೂನೆ_Min_Max_Arguments(
        ಸ್ಥಿರಾಂಕ UnicodeString &ನಮೂನೆ,
        int32_t min, int32_t max,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ಸ್ಥಿರಾಂಕ char16_t *ನಮೂನೆ_ಬಫರ್ = ನಮೂನೆ.get_ಬಫರ್();
    int32_t ನಮೂನೆ_ಉದ್ದಳತೆ = ನಮೂನೆ.length();
    // Reserve the first char for the number of arguments.
    compiled_ನಮೂನೆ.ಗಣ_ಗಮ್ಯ(static_cast<char16_t>(0));
    int32_t ಪಠ್ಯ_ಉದ್ದಳತೆ = 0;
    int32_t maxArg = -1;
    UBool ಒಳ_Quote = ಸುಳ್ಳು;
    ಗಾಗಿ (int32_t i = 0; i < ನಮೂನೆ_ಉದ್ದಳತೆ;) {
        char16_t c = ನಮೂನೆ_ಬಫರ್[i++];
        ಆದರೆ (c == APOS) {
            ಆದರೆ (i < ನಮೂನೆ_ಉದ್ದಳತೆ && (c = ನಮೂನೆ_ಬಫರ್[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_Quote) {
                // skip the quote-ending apostrophe
                ಒಳ_Quote = ಸುಳ್ಳು;
                ಮುಂದುವರಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                ಒಳ_Quote = ನಿಜ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಒಳ_Quote && c == OPEN_BRACE) {
            ಆದರೆ (ಪಠ್ಯ_ಉದ್ದಳತೆ > 0) {
                compiled_ನಮೂನೆ.ಗಣ_Char_At(compiled_ನಮೂನೆ.length() - ಪಠ್ಯ_ಉದ್ದಳತೆ - 1,
                                          static_cast<char16_t>(ARG_ಅಂಕೆ_ಎಲ್ಲೆ + ಪಠ್ಯ_ಉದ್ದಳತೆ));
                ಪಠ್ಯ_ಉದ್ದಳತೆ = 0;
            }
            int32_t arg_ಸಂಖ್ಯೆ;
            ಆದರೆ ((i + 1) < ನಮೂನೆ_ಉದ್ದಳತೆ &&
                    0 <= (arg_ಸಂಖ್ಯೆ = ನಮೂನೆ_ಬಫರ್[i] - DIGIT_ZERO) && arg_ಸಂಖ್ಯೆ <= 9 &&
                    ನಮೂನೆ_ಬಫರ್[i + 1] == CLOSE_BRACE) {
                i += 2;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_ಸಂಖ್ಯೆ = -1;
                ಆದರೆ (i < ನಮೂನೆ_ಉದ್ದಳತೆ && DIGIT_ONE <= (c = ನಮೂನೆ_ಬಫರ್[i++]) && c <= DIGIT_NINE) {
                    arg_ಸಂಖ್ಯೆ = c - DIGIT_ZERO;
                    ಆಗ (i < ನಮೂನೆ_ಉದ್ದಳತೆ &&
                            DIGIT_ZERO <= (c = ನಮೂನೆ_ಬಫರ್[i++]) && c <= DIGIT_NINE) {
                        arg_ಸಂಖ್ಯೆ = arg_ಸಂಖ್ಯೆ * 10 + (c - DIGIT_ZERO);
                        ಆದರೆ (arg_ಸಂಖ್ಯೆ >= ARG_ಅಂಕೆ_ಎಲ್ಲೆ) {
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                }
                ಆದರೆ (arg_ಸಂಖ್ಯೆ < 0 || c != CLOSE_BRACE) {
                    error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
            ಆದರೆ (arg_ಸಂಖ್ಯೆ > maxArg) {
                maxArg = arg_ಸಂಖ್ಯೆ;
            }
            compiled_ನಮೂನೆ.append(static_cast<char16_t>(arg_ಸಂಖ್ಯೆ));
            ಮುಂದುವರಿ;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        ಆದರೆ (ಪಠ್ಯ_ಉದ್ದಳತೆ == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiled_ನಮೂನೆ.append(SEGMENT_ಉದ್ದಳತೆ_PLACEHOLDER_CHAR);
        }
        compiled_ನಮೂನೆ.append(c);
        ಆದರೆ (++ಪಠ್ಯ_ಉದ್ದಳತೆ == MAX_SEGMENT_ಉದ್ದಳತೆ) {
            ಪಠ್ಯ_ಉದ್ದಳತೆ = 0;
        }
    }
    ಆದರೆ (ಪಠ್ಯ_ಉದ್ದಳತೆ > 0) {
        compiled_ನಮೂನೆ.ಗಣ_Char_At(compiled_ನಮೂನೆ.length() - ಪಠ್ಯ_ಉದ್ದಳತೆ - 1,
                                  static_cast<char16_t>(ARG_ಅಂಕೆ_ಎಲ್ಲೆ + ಪಠ್ಯ_ಉದ್ದಳತೆ));
    }
    int32_t arg_ಎಣಿಕೆ = maxArg + 1;
    ಆದರೆ (arg_ಎಣಿಕೆ < min || max < arg_ಎಣಿಕೆ) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    compiled_ನಮೂನೆ.ಗಣ_Char_At(0, static_cast<char16_t>(arg_ಎಣಿಕೆ));
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UnicodeString& SimpleFormatter::format(
        ಸ್ಥಿರಾಂಕ UnicodeString& value0,
        UnicodeString& append_ಗಮ್ಯ U_LIFETIME_BOUND,
        UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ UnicodeString *ಮೌಲ್ಯಗಳು[] = { &value0 };
    ಹಿಂತಿರುಗಿ formatAndAppend(ಮೌಲ್ಯಗಳು, 1, append_ಗಮ್ಯ, ಶೂನ್ಯಸೂಚಕ, 0, error_ಸಂಕೇತ);
}

UnicodeString& SimpleFormatter::format(
        ಸ್ಥಿರಾಂಕ UnicodeString& value0,
        ಸ್ಥಿರಾಂಕ UnicodeString& value1,
        UnicodeString& append_ಗಮ್ಯ U_LIFETIME_BOUND,
        UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ UnicodeString *ಮೌಲ್ಯಗಳು[] = { &value0, &value1 };
    ಹಿಂತಿರುಗಿ formatAndAppend(ಮೌಲ್ಯಗಳು, 2, append_ಗಮ್ಯ, ಶೂನ್ಯಸೂಚಕ, 0, error_ಸಂಕೇತ);
}

UnicodeString& SimpleFormatter::format(
        ಸ್ಥಿರಾಂಕ UnicodeString& value0,
        ಸ್ಥಿರಾಂಕ UnicodeString& value1,
        ಸ್ಥಿರಾಂಕ UnicodeString& value2,
        UnicodeString& append_ಗಮ್ಯ U_LIFETIME_BOUND,
        UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ UnicodeString *ಮೌಲ್ಯಗಳು[] = { &value0, &value1, &value2 };
    ಹಿಂತಿರುಗಿ formatAndAppend(ಮೌಲ್ಯಗಳು, 3, append_ಗಮ್ಯ, ಶೂನ್ಯಸೂಚಕ, 0, error_ಸಂಕೇತ);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ಸ್ಥಿರಾಂಕ UnicodeString* ಸ್ಥಿರಾಂಕ* ಮೌಲ್ಯಗಳು,
        int32_t ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ,
        UnicodeString& append_ಗಮ್ಯ U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_ಉದ್ದಳತೆ,
        UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ append_ಗಮ್ಯ;
    }
    ಆದರೆ (isInvalidArray(ಮೌಲ್ಯಗಳು, ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ) || isInvalidArray(offsets, offsets_ಉದ್ದಳತೆ) ||
            ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ < get_ವಾದ_ಎಲ್ಲೆ()) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ append_ಗಮ್ಯ;
    }
    ಹಿಂತಿರುಗಿ format(compiled_ನಮೂನೆ.get_ಬಫರ್(), compiled_ನಮೂನೆ.length(), ಮೌಲ್ಯಗಳು,
                  append_ಗಮ್ಯ, ಶೂನ್ಯಸೂಚಕ, ನಿಜ,
                  offsets, offsets_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ಸ್ಥಿರಾಂಕ UnicodeString* ಸ್ಥಿರಾಂಕ* ಮೌಲ್ಯಗಳು,
        int32_t ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ,
        UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_ಉದ್ದಳತೆ,
        UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಆದರೆ (isInvalidArray(ಮೌಲ್ಯಗಳು, ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ) || isInvalidArray(offsets, offsets_ಉದ್ದಳತೆ)) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಸ್ಥಿರಾಂಕ char16_t *cp = compiled_ನಮೂನೆ.get_ಬಫರ್();
    int32_t cp_ಉದ್ದಳತೆ = compiled_ನಮೂನೆ.length();
    ಆದರೆ (ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ < get_ವಾದ_ಎಲ್ಲೆ(cp, cp_ಉದ್ದಳತೆ)) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t ಮೊದಲ_Arg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString ಫಲಿತಾಂಶ_Copy;
    ಆದರೆ (get_ವಾದ_ಎಲ್ಲೆ(cp, cp_ಉದ್ದಳತೆ) > 0) {
        ಗಾಗಿ (int32_t i = 1; i < cp_ಉದ್ದಳತೆ;) {
            int32_t n = cp[i++];
            ಆದರೆ (n < ARG_ಅಂಕೆ_ಎಲ್ಲೆ) {
                ಆದರೆ (ಮೌಲ್ಯಗಳು[n] == &ಫಲಿತಾಂಶ) {
                    ಆದರೆ (i == 2) {
                        ಮೊದಲ_Arg = n;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ_Copy.is_ಖಾಲಿಗಳು() && !ಫಲಿತಾಂಶ.is_ಖಾಲಿಗಳು()) {
                        ಫಲಿತಾಂಶ_Copy = ಫಲಿತಾಂಶ;
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                i += n - ARG_ಅಂಕೆ_ಎಲ್ಲೆ;
            }
        }
    }
    ಆದರೆ (ಮೊದಲ_Arg < 0) {
        ಫಲಿತಾಂಶ.remove();
    }
    ಹಿಂತಿರುಗಿ format(cp, cp_ಉದ್ದಳತೆ, ಮೌಲ್ಯಗಳು,
                  ಫಲಿತಾಂಶ, &ಫಲಿತಾಂಶ_Copy, ಸುಳ್ಳು,
                  offsets, offsets_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
}

UnicodeString SimpleFormatter::get_ಪಠ್ಯ_With_No_Arguments(
        ಸ್ಥಿರಾಂಕ char16_t *compiled_ನಮೂನೆ,
        int32_t compiled_ನಮೂನೆ_ಉದ್ದಳತೆ,
        int32_t* offsets,
        int32_t offsets_ಉದ್ದಳತೆ) {
    ಗಾಗಿ (int32_t i = 0; i < offsets_ಉದ್ದಳತೆ; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiled_ನಮೂನೆ_ಉದ್ದಳತೆ - 1 -
            get_ವಾದ_ಎಲ್ಲೆ(compiled_ನಮೂನೆ, compiled_ನಮೂನೆ_ಉದ್ದಳತೆ);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    ಗಾಗಿ (int32_t i = 1; i < compiled_ನಮೂನೆ_ಉದ್ದಳತೆ;) {
        int32_t n = compiled_ನಮೂನೆ[i++];
        ಆದರೆ (n > ARG_ಅಂಕೆ_ಎಲ್ಲೆ) {
            n -= ARG_ಅಂಕೆ_ಎಲ್ಲೆ;
            sb.append(compiled_ನಮೂನೆ + i, n);
            i += n;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n < offsets_ಉದ್ದಳತೆ) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    ಹಿಂತಿರುಗಿ sb;
}

UnicodeString& SimpleFormatter::format(
        ಸ್ಥಿರಾಂಕ char16_t* compiled_ನಮೂನೆ,
        int32_t compiled_ನಮೂನೆ_ಉದ್ದಳತೆ,
        ಸ್ಥಿರಾಂಕ UnicodeString* ಸ್ಥಿರಾಂಕ* ಮೌಲ್ಯಗಳು,
        UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
        ಸ್ಥಿರಾಂಕ UnicodeString* ಫಲಿತಾಂಶ_Copy,
        UBool forbid_ಫಲಿತಾಂಶ_As_ಮೌಲ್ಯ,
        int32_t* offsets,
        int32_t offsets_ಉದ್ದಳತೆ,
        UError_ಸಂಕೇತ& error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಗಾಗಿ (int32_t i = 0; i < offsets_ಉದ್ದಳತೆ; i++) {
        offsets[i] = -1;
    }
    ಗಾಗಿ (int32_t i = 1; i < compiled_ನಮೂನೆ_ಉದ್ದಳತೆ;) {
        int32_t n = compiled_ನಮೂನೆ[i++];
        ಆದರೆ (n < ARG_ಅಂಕೆ_ಎಲ್ಲೆ) {
            ಸ್ಥಿರಾಂಕ UnicodeString *value = ಮೌಲ್ಯಗಳು[n];
            ಆದರೆ (value == ಶೂನ್ಯಸೂಚಕ) {
                error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
            ಆದರೆ (value == &ಫಲಿತಾಂಶ) {
                ಆದರೆ (forbid_ಫಲಿತಾಂಶ_As_ಮೌಲ್ಯ) {
                    error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
                }
                ಆದರೆ (i == 2) {
                    // We are appending to result which is also the first value object.
                    ಆದರೆ (n < offsets_ಉದ್ದಳತೆ) {
                        offsets[n] = 0;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ (n < offsets_ಉದ್ದಳತೆ) {
                        offsets[n] = ಫಲಿತಾಂಶ.length();
                    }
                    ಫಲಿತಾಂಶ.append(*ಫಲಿತಾಂಶ_Copy);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (n < offsets_ಉದ್ದಳತೆ) {
                    offsets[n] = ಫಲಿತಾಂಶ.length();
                }
                ಫಲಿತಾಂಶ.append(*value);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            int32_t length = n - ARG_ಅಂಕೆ_ಎಲ್ಲೆ;
            ಫಲಿತಾಂಶ.append(compiled_ನಮೂನೆ + i, length);
            i += length;
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_NAMESPACE_ಕೊನೆ
