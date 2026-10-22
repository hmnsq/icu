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
ಧ್ರುವ int32_t ARG_ಅಂಕೆ_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ಧ್ರುವ char16_t SEGMENT_LENGTH_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ಧ್ರುವ int32_t MAX_SEGMENT_LENGTH = SEGMENT_LENGTH_PLACEHOLDER_CHAR - ARG_ಅಂಕೆ_LIMIT;

ಗಣನಾ {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

ಅನ್ತಃಸ್ಥ UBool isInvalidArray(ಧ್ರುವ ಶೂನ್ಯ *array, int32_t length) {
   ಪ್ರತ್ಯಾಗಮ (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::operator=(ಧ್ರುವ SimpleFormatter& other) U_LIFETIME_BOUND {
    ಯದಿ (ಏತತ್ == &other) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    compiledPattern = other.compiledPattern;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        ಧ್ರುವ UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ಧ್ರುವ char16_t *patternBuffer = pattern.getBuffer();
    int32_t patternLength = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t textLength = 0;
    int32_t maxArg = -1;
    UBool inQuote = ಅಸತ್ಯ;
    ಪ್ರತಿ (int32_t i = 0; i < patternLength;) {
        char16_t c = patternBuffer[i++];
        ಯದಿ (c == APOS) {
            ಯದಿ (i < patternLength && (c = patternBuffer[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } ಅನ್ಯಥಾ ಯದಿ (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = ಅಸತ್ಯ;
                ಅನುವರ್ತ;
            } ಅನ್ಯಥಾ ಯದಿ (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = ಸತ್ಯ;
            } ಅನ್ಯಥಾ {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } ಅನ್ಯಥಾ ಯದಿ (!inQuote && c == OPEN_BRACE) {
            ಯದಿ (textLength > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                          static_cast<char16_t>(ARG_ಅಂಕೆ_LIMIT + textLength));
                textLength = 0;
            }
            int32_t arg_ಸಂಖ್ಯೆ;
            ಯದಿ ((i + 1) < patternLength &&
                    0 <= (arg_ಸಂಖ್ಯೆ = patternBuffer[i] - DIGIT_ZERO) && arg_ಸಂಖ್ಯೆ <= 9 &&
                    patternBuffer[i + 1] == CLOSE_BRACE) {
                i += 2;
            } ಅನ್ಯಥಾ {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_ಸಂಖ್ಯೆ = -1;
                ಯದಿ (i < patternLength && DIGIT_ONE <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                    arg_ಸಂಖ್ಯೆ = c - DIGIT_ZERO;
                    ಯಾವತ್ (i < patternLength &&
                            DIGIT_ZERO <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                        arg_ಸಂಖ್ಯೆ = arg_ಸಂಖ್ಯೆ * 10 + (c - DIGIT_ZERO);
                        ಯದಿ (arg_ಸಂಖ್ಯೆ >= ARG_ಅಂಕೆ_LIMIT) {
                            ಭಙ್ಗ;
                        }
                    }
                }
                ಯದಿ (arg_ಸಂಖ್ಯೆ < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            }
            ಯದಿ (arg_ಸಂಖ್ಯೆ > maxArg) {
                maxArg = arg_ಸಂಖ್ಯೆ;
            }
            compiledPattern.append(static_cast<char16_t>(arg_ಸಂಖ್ಯೆ));
            ಅನುವರ್ತ;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        ಯದಿ (textLength == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_LENGTH_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        ಯದಿ (++textLength == MAX_SEGMENT_LENGTH) {
            textLength = 0;
        }
    }
    ಯದಿ (textLength > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                  static_cast<char16_t>(ARG_ಅಂಕೆ_LIMIT + textLength));
    }
    int32_t arg_ಎಣಿಕೆ = maxArg + 1;
    ಯದಿ (arg_ಎಣಿಕೆ < min || max < arg_ಎಣಿಕೆ) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(arg_ಎಣಿಕೆ));
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UnicodeString& SimpleFormatter::format(
        ಧ್ರುವ UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ಧ್ರುವ {
    ಧ್ರುವ UnicodeString *ಮೌಲ್ಯಗಳು[] = { &value0 };
    ಪ್ರತ್ಯಾಗಮ formatAndAppend(ಮೌಲ್ಯಗಳು, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ಧ್ರುವ UnicodeString& value0,
        ಧ್ರುವ UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ಧ್ರುವ {
    ಧ್ರುವ UnicodeString *ಮೌಲ್ಯಗಳು[] = { &value0, &value1 };
    ಪ್ರತ್ಯಾಗಮ formatAndAppend(ಮೌಲ್ಯಗಳು, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ಧ್ರುವ UnicodeString& value0,
        ಧ್ರುವ UnicodeString& value1,
        ಧ್ರುವ UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ಧ್ರುವ {
    ಧ್ರುವ UnicodeString *ಮೌಲ್ಯಗಳು[] = { &value0, &value1, &value2 };
    ಪ್ರತ್ಯಾಗಮ formatAndAppend(ಮೌಲ್ಯಗಳು, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ಧ್ರುವ UnicodeString* ಧ್ರುವ* ಮೌಲ್ಯಗಳು,
        int32_t ಮೌಲ್ಯಗಳು_Length,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ appendTo;
    }
    ಯದಿ (isInvalidArray(ಮೌಲ್ಯಗಳು, ಮೌಲ್ಯಗಳು_Length) || isInvalidArray(offsets, offsetsLength) ||
            ಮೌಲ್ಯಗಳು_Length < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ appendTo;
    }
    ಪ್ರತ್ಯಾಗಮ format(compiledPattern.getBuffer(), compiledPattern.length(), ಮೌಲ್ಯಗಳು,
                  appendTo, nullptr, ಸತ್ಯ,
                  offsets, offsetsLength, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ಧ್ರುವ UnicodeString* ಧ್ರುವ* ಮೌಲ್ಯಗಳು,
        int32_t ಮೌಲ್ಯಗಳು_Length,
        UnicodeString& result U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಯದಿ (isInvalidArray(ಮೌಲ್ಯಗಳು, ಮೌಲ್ಯಗಳು_Length) || isInvalidArray(offsets, offsetsLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಧ್ರುವ char16_t *cp = compiledPattern.getBuffer();
    int32_t cpLength = compiledPattern.length();
    ಯದಿ (ಮೌಲ್ಯಗಳು_Length < getArgumentLimit(cp, cpLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ result;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString resultCopy;
    ಯದಿ (getArgumentLimit(cp, cpLength) > 0) {
        ಪ್ರತಿ (int32_t i = 1; i < cpLength;) {
            int32_t n = cp[i++];
            ಯದಿ (n < ARG_ಅಂಕೆ_LIMIT) {
                ಯದಿ (ಮೌಲ್ಯಗಳು[n] == &result) {
                    ಯದಿ (i == 2) {
                        firstArg = n;
                    } ಅನ್ಯಥಾ ಯದಿ (resultCopy.isEmpty() && !result.isEmpty()) {
                        resultCopy = result;
                    }
                }
            } ಅನ್ಯಥಾ {
                i += n - ARG_ಅಂಕೆ_LIMIT;
            }
        }
    }
    ಯದಿ (firstArg < 0) {
        result.remove();
    }
    ಪ್ರತ್ಯಾಗಮ format(cp, cpLength, ಮೌಲ್ಯಗಳು,
                  result, &resultCopy, ಅಸತ್ಯ,
                  offsets, offsetsLength, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        ಧ್ರುವ char16_t *compiledPattern,
        int32_t compiledPatternLength,
        int32_t* offsets,
        int32_t offsetsLength) {
    ಪ್ರತಿ (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiledPatternLength - 1 -
            getArgumentLimit(compiledPattern, compiledPatternLength);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    ಪ್ರತಿ (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        ಯದಿ (n > ARG_ಅಂಕೆ_LIMIT) {
            n -= ARG_ಅಂಕೆ_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } ಅನ್ಯಥಾ ಯದಿ (n < offsetsLength) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    ಪ್ರತ್ಯಾಗಮ sb;
}

UnicodeString& SimpleFormatter::format(
        ಧ್ರುವ char16_t* compiledPattern,
        int32_t compiledPatternLength,
        ಧ್ರುವ UnicodeString* ಧ್ರುವ* ಮೌಲ್ಯಗಳು,
        UnicodeString& result U_LIFETIME_BOUND,
        ಧ್ರುವ UnicodeString* resultCopy,
        UBool forbid_Result_As_ಮೌಲ್ಯ,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಪ್ರತಿ (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    ಪ್ರತಿ (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        ಯದಿ (n < ARG_ಅಂಕೆ_LIMIT) {
            ಧ್ರುವ UnicodeString *value = ಮೌಲ್ಯಗಳು[n];
            ಯದಿ (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ಪ್ರತ್ಯಾಗಮ result;
            }
            ಯದಿ (value == &result) {
                ಯದಿ (forbid_Result_As_ಮೌಲ್ಯ) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ಪ್ರತ್ಯಾಗಮ result;
                }
                ಯದಿ (i == 2) {
                    // We are appending to result which is also the first value object.
                    ಯದಿ (n < offsetsLength) {
                        offsets[n] = 0;
                    }
                } ಅನ್ಯಥಾ {
                    ಯದಿ (n < offsetsLength) {
                        offsets[n] = result.length();
                    }
                    result.append(*resultCopy);
                }
            } ಅನ್ಯಥಾ {
                ಯದಿ (n < offsetsLength) {
                    offsets[n] = result.length();
                }
                result.append(*value);
            }
        } ಅನ್ಯಥಾ {
            int32_t length = n - ARG_ಅಂಕೆ_LIMIT;
            result.append(compiledPattern + i, length);
            i += length;
        }
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

U_NAMESPACE_END
