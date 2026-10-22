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

فضاینام {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
ثابت int32_t ARG_رقم_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ثابت char16_t SEGMENT_LENGTH_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ثابت int32_t MAX_SEGMENT_LENGTH = SEGMENT_LENGTH_PLACEHOLDER_CHAR - ARG_رقم_LIMIT;

شمارش {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

درونخطی UBool isInvalidArray(ثابت تهی *array, int32_t length) {
   بازگشت (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::operator=(ثابت SimpleFormatter& other) U_LIFETIME_BOUND {
    اگر (این == &other) {
        بازگشت *این;
    }
    compiledPattern = other.compiledPattern;
    بازگشت *این;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        ثابت UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) {
        بازگشت نادرست;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ثابت char16_t *patternBuffer = pattern.getBuffer();
    int32_t patternLength = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t textLength = 0;
    int32_t maxArg = -1;
    UBool inQuote = نادرست;
    برای (int32_t i = 0; i < patternLength;) {
        char16_t c = patternBuffer[i++];
        اگر (c == APOS) {
            اگر (i < patternLength && (c = patternBuffer[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } وگرنه اگر (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = نادرست;
                ادامه;
            } وگرنه اگر (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = درست;
            } وگرنه {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } وگرنه اگر (!inQuote && c == OPEN_BRACE) {
            اگر (textLength > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                          static_cast<char16_t>(ARG_رقم_LIMIT + textLength));
                textLength = 0;
            }
            int32_t arg_عدد;
            اگر ((i + 1) < patternLength &&
                    0 <= (arg_عدد = patternBuffer[i] - DIGIT_ZERO) && arg_عدد <= 9 &&
                    patternBuffer[i + 1] == CLOSE_BRACE) {
                i += 2;
            } وگرنه {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_عدد = -1;
                اگر (i < patternLength && DIGIT_ONE <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                    arg_عدد = c - DIGIT_ZERO;
                    وقتی (i < patternLength &&
                            DIGIT_ZERO <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                        arg_عدد = arg_عدد * 10 + (c - DIGIT_ZERO);
                        اگر (arg_عدد >= ARG_رقم_LIMIT) {
                            بشکن;
                        }
                    }
                }
                اگر (arg_عدد < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    بازگشت نادرست;
                }
            }
            اگر (arg_عدد > maxArg) {
                maxArg = arg_عدد;
            }
            compiledPattern.append(static_cast<char16_t>(arg_عدد));
            ادامه;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        اگر (textLength == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_LENGTH_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        اگر (++textLength == MAX_SEGMENT_LENGTH) {
            textLength = 0;
        }
    }
    اگر (textLength > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                  static_cast<char16_t>(ARG_رقم_LIMIT + textLength));
    }
    int32_t arg_شمار = maxArg + 1;
    اگر (arg_شمار < min || max < arg_شمار) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت نادرست;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(arg_شمار));
    بازگشت درست;
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ثابت {
    ثابت UnicodeString *مقادیر[] = { &value0 };
    بازگشت formatAndAppend(مقادیر, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ثابت {
    ثابت UnicodeString *مقادیر[] = { &value0, &value1 };
    بازگشت formatAndAppend(مقادیر, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        ثابت UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ثابت {
    ثابت UnicodeString *مقادیر[] = { &value0, &value1, &value2 };
    بازگشت formatAndAppend(مقادیر, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ثابت UnicodeString* ثابت* مقادیر,
        int32_t مقادیر_Length,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) {
        بازگشت appendTo;
    }
    اگر (isInvalidArray(مقادیر, مقادیر_Length) || isInvalidArray(offsets, offsetsLength) ||
            مقادیر_Length < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت appendTo;
    }
    بازگشت format(compiledPattern.getBuffer(), compiledPattern.length(), مقادیر,
                  appendTo, nullptr, درست,
                  offsets, offsetsLength, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ثابت UnicodeString* ثابت* مقادیر,
        int32_t مقادیر_Length,
        UnicodeString& result U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) {
        بازگشت result;
    }
    اگر (isInvalidArray(مقادیر, مقادیر_Length) || isInvalidArray(offsets, offsetsLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت result;
    }
    ثابت char16_t *cp = compiledPattern.getBuffer();
    int32_t cpLength = compiledPattern.length();
    اگر (مقادیر_Length < getArgumentLimit(cp, cpLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت result;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString resultCopy;
    اگر (getArgumentLimit(cp, cpLength) > 0) {
        برای (int32_t i = 1; i < cpLength;) {
            int32_t n = cp[i++];
            اگر (n < ARG_رقم_LIMIT) {
                اگر (مقادیر[n] == &result) {
                    اگر (i == 2) {
                        firstArg = n;
                    } وگرنه اگر (resultCopy.isEmpty() && !result.isEmpty()) {
                        resultCopy = result;
                    }
                }
            } وگرنه {
                i += n - ARG_رقم_LIMIT;
            }
        }
    }
    اگر (firstArg < 0) {
        result.remove();
    }
    بازگشت format(cp, cpLength, مقادیر,
                  result, &resultCopy, نادرست,
                  offsets, offsetsLength, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        ثابت char16_t *compiledPattern,
        int32_t compiledPatternLength,
        int32_t* offsets,
        int32_t offsetsLength) {
    برای (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiledPatternLength - 1 -
            getArgumentLimit(compiledPattern, compiledPatternLength);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    برای (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        اگر (n > ARG_رقم_LIMIT) {
            n -= ARG_رقم_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } وگرنه اگر (n < offsetsLength) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    بازگشت sb;
}

UnicodeString& SimpleFormatter::format(
        ثابت char16_t* compiledPattern,
        int32_t compiledPatternLength,
        ثابت UnicodeString* ثابت* مقادیر,
        UnicodeString& result U_LIFETIME_BOUND,
        ثابت UnicodeString* resultCopy,
        UBool forbid_Result_As_مقدار,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) {
    اگر (U_FAILURE(errorCode)) {
        بازگشت result;
    }
    برای (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    برای (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        اگر (n < ARG_رقم_LIMIT) {
            ثابت UnicodeString *value = مقادیر[n];
            اگر (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                بازگشت result;
            }
            اگر (value == &result) {
                اگر (forbid_Result_As_مقدار) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    بازگشت result;
                }
                اگر (i == 2) {
                    // We are appending to result which is also the first value object.
                    اگر (n < offsetsLength) {
                        offsets[n] = 0;
                    }
                } وگرنه {
                    اگر (n < offsetsLength) {
                        offsets[n] = result.length();
                    }
                    result.append(*resultCopy);
                }
            } وگرنه {
                اگر (n < offsetsLength) {
                    offsets[n] = result.length();
                }
                result.append(*value);
            }
        } وگرنه {
            int32_t length = n - ARG_رقم_LIMIT;
            result.append(compiledPattern + i, length);
            i += length;
        }
    }
    بازگشت result;
}

U_NAMESPACE_END
